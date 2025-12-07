#include "ThreadPool.h"
#include <thread>
#include <iostream>

int main()
{
    // 工业级代码，应该有完整的异常处理逻辑
    try
    {
        // 创建一个拥有4个线程threadpool
        ThreadPool pool(4);

        // 添加打印信息
        std::cout << "线程池创建成功" << std::endl
                  << "线程池中有4个工作线程！" << std::endl;

        // 通过sleep函数模拟线程池的执行
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    catch (const std::exception & e)
    {
        std::cout << e.what() << std::endl;
        return 1;
    }

    std::cout << "主函数结束，线程池被销毁" << std::endl;

    return 0;
}