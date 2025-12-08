#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <mutex>
#include <future>   // 得到异步结果
#include <condition_variable>
#include <utility>
#include <stdexcept>

class ThreadPool
{
public:
    ThreadPool(size_t threadNum);
    ~ThreadPool();

    // 返回此时的线程池的运行状态
    bool isStopped() const {    return _stop;   }

private:
    void workerThread();    // 工作线程应该执行的函数

    std::vector<std::thread> _workers;      // 工作线程
    std::queue<std::function<void()>> _taskQueue;   // 任务队列
    std::mutex _queue_mtx;      // 线程安全地访问任务队列
    std::condition_variable _condition;     // 条件变量，同步进行生产者消费者模型
    std::atomic<bool> _stop{false};    // 线程池是否停止标志位
};

#endif