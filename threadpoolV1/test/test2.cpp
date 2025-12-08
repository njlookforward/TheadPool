#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <chrono>

// 需要定义一个每个工作线程的执行函数
void sleepFor(size_t seconds)
{
    // 必须要打好日志，证明每个函数都成功执行了
    std::cout << "\n=== thread " << std::this_thread::get_id() << " 开始执行工作 ===" << std::endl;
    // 调用chrono的函数的确不知道 -- 还是之前的那个，不要想复杂，就用自己熟悉的函数
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    std::cout << "=== thread " << std::this_thread::get_id() << " 完成工作 ===" << std::endl;
} 

int main()
{
    try
    {
        // 被创建的线程的数量是有一个函数的，可以得到当前处理器的核心数
        auto threadCount = std::thread::hardware_concurrency();
        std::cout << "=== 当前运行的机器CPU的核心数是 " << threadCount << " ===" << std::endl;

        // 为了方便观察
        unsigned int threadNum = std::min(threadCount, (unsigned int)4);

        ThreadPool pool(threadNum);

        std::cout << "线程池成功创建了" << std::endl;

        // 让主线程睡一会，模拟工作线程的工作过程，工作了3秒
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    catch(const std::exception & e)
    {
        std::cerr << "出现了执行错误：" << e.what() << std::endl; 
        return 1;
    }
    
    std::cout << "=== 此时执行结束，线程池销毁了" << std::endl;
    
    return 0;
}