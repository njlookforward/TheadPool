#include "ThreadPool.h"
#include <iostream>

// constructor：应该是用来构造线程池的数量
ThreadPool::ThreadPool(size_t threadnum)
{
    std::cout << "ThreadPool的构造函数被调用，threadNum = " << threadnum << std::endl;
}

// destructor：应该是终止线程池的运行，然后回收各种所有工作线程
ThreadPool::~ThreadPool()
{
    std::cout << "ThreadPool的析构函数被调用" << std::endl;
}