#include "ThreadPool.h"
#include <iostream>

// constructor：应该是用来构造线程池的数量
ThreadPool::ThreadPool(size_t threadnum)
{
    std::cout << "ThreadPool的构造函数被调用，threadNum = " << threadnum << std::endl;

    // 创建对应数量的线程池
    for (size_t i = 0; i < threadnum; ++i)
    {
        _workers.emplace_back(
            [this]
            { this->workerThread(); } // 可以添加lambda表达式作为线程的执行函数
        );

        // 多种不同的方法可以创建worker的工作线程
        // _workers.emplace_back(
        //     &ThreadPool::workerThread, this
        // );
    }
}

// destructor：应该是终止线程池的运行，然后回收各种所有工作线程
ThreadPool::~ThreadPool()
{
    std::cout << "ThreadPool的析构函数被调用" << std::endl;

    // threadpool销毁前，需要将所有的工作线程回收，否则会直接terminate
    // 线程池执行结束，应该置为true
    _stop = true;

    _condition.notify_all();

    // 增强for循环将所有的工作线程回收
    for (auto &worker : _workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::workerThread()
{
    // 工作线程就是作为任务队列的消费者的角色

    while (true)
    {
        // 不停地创建新任务，轮询得到新的任务
        std::function<void()> task;

        // 互斥获取新的任务
        {
            std::unique_lock<std::mutex> lock(_queue_mtx);

            _condition.wait(lock, [this]
                            { return this->_stop || !this->_taskQueue.empty(); });

            // 需要判断是停止线程池，还是能够获取新的任务
            if (_stop && _taskQueue.empty())
            {
                // 线程池终止
                return;
            }

            task = std::move(_taskQueue.front());
            _taskQueue.pop();
        }

        // 得到了新的任务
        try
        {
            task();
        }
        catch (const std::exception &e)
        {
            std::cerr << "任务执行异常" << std::endl;
        }
        catch (...)
        {
            std::cerr << "任务执行过程中出现了未知异常" << std::endl;
        }
    }
}

// 获得可调用函数后，因为需要得到异步回调的执行结果，所以需要使用package_task来包裹可执行函数
// 但是函数对象，可能是不同的返回类型，因此需要使用lambda表达式进行包裹
template <class F, class... Args>
auto ThreadPool::enqueue(F &&func, Args&&...args)
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    // 判断func必须是有效的
    if(!func)
    {
        // 这是空函数，没有任何意义
        throw std::runtime_error("必须传入有效的函数对象");
    }

    using resultType = typename std::invoke_result<F, Args...>::type;

    // 使用package_task修饰要被执行的函数，因为也是一个对象，需要被跨线程共享
    // 因此申请在堆空间，然后使用智能指针包裹
    std::shared_ptr<std::packaged_task<resultType()>> task = 
            std::make_shared<std::packaged_task<resultType()>>(
                // BUG: 这里是不对的，被包裹的函数对象必须是与模板类型一样的，因此需要使用std::bind绑定
                std::bind(std::forward<F>(func), std::forward<Args>(args)...)
            );
    std::future<resultType> result = task->get_future();

    // 接下来就是正常的生产者流程了，但是需要先判断——stop的状态
    if(_stop)
    {
        throw std::runtime_error("此时线程池已经结束了，不应该再投放任务");
    }

    std::unique_lock<std::mutex> lock(_queue_mtx);
    _taskQueue.emplace(
        [task] {    (*task)();  }   // lambda表达式太强大了，直接修改了函数的返回值
    );
    _condition.notify_one();
    lock.unlock();

    return result;
} 