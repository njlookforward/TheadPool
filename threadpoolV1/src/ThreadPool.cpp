#include "ThreadPool.h"
#include <iostream>

// constructor：应该是用来构造线程池的数量
ThreadPool::ThreadPool(size_t threadnum)
{
    std::cout << "ThreadPool的构造函数被调用，threadNum = " << threadnum << std::endl;

    // 创建对应数量的线程池
    for(size_t i = 0; i < threadnum; ++i)
    {
        _workers.emplace_back(
            [this] {    this->workerThread();   }   // 可以添加lambda表达式作为线程的执行函数
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
    for(auto &worker : _workers)
    {
        if(worker.joinable())
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

            _condition.wait(lock, [this] {
                return this->_stop || !this->_taskQueue.empty();
            });

            // 需要判断是停止线程池，还是能够获取新的任务
            if(_stop && _taskQueue.empty())
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
        catch(const std::exception & e)
        {
            std::cerr << "任务执行异常" << std::endl;
        }
        catch(...)
        {
            std::cerr << "任务执行过程中出现了未知异常" << std::endl;
        }
    }
}