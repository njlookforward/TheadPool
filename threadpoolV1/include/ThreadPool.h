#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <mutex>
#include <future> // 得到异步结果
#include <condition_variable>
#include <utility>
#include <stdexcept>

class ThreadPool
{
public:
    ThreadPool(size_t threadNum);
    ~ThreadPool();

    // 线程池不应该被拷贝，是线程唯一的
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    // 添加任务的接口：会返回异步执行函数所返回的结果，所以需要使用std::package_task与std::future与lambda表达式
    // 这个任务队列更加强大，能够添加任何类型的函数，并异步返回其结果
    template <typename F, typename... Args>
    auto enqueue(F &&func, Args &&...args)
        -> std::future<typename std::invoke_result<F, Args...>::type>;

    // 返回此时的线程池的运行状态
    bool isStopped() const { return _stop; }

private:
    void workerThread(); // 工作线程应该执行的函数

    std::vector<std::thread> _workers;            // 工作线程
    std::queue<std::function<void()>> _taskQueue; // 任务队列
    std::mutex _queue_mtx;                        // 线程安全地访问任务队列
    std::condition_variable _condition;           // 条件变量，同步进行生产者消费者模型
    std::atomic<bool> _stop{false};               // 线程池是否停止标志位
};

#endif