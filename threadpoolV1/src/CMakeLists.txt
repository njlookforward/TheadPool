# 在子目录的CMakeLists.txt中就不需要添加cmake_minimum_required

# 检索到源文件，这里写错了，CMAKE_CURRENT_SOURCE_DIR就是当前cmakelists.txt所在的目录
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 如果我不添加SHARED，看看默认会是什么库
# 不添加任何修饰词，默认生成静态库
add_library(threadpool SHARED ${SRC})

# 链接动态库文件：线程库
find_package(Threads REQUIRED)  # 查找线程库
target_link_libraries(threadpool PRIVATE Threads::Threads)  # 链接动态库文件：线程库

# 设置安装位置
# 安装库，将生成的库文件安装到系统的默认lib文件下，LIBRARY是动态库，ARCHIVE是静态库
install(TARGETS threadpool
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)   