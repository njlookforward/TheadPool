cmake_minimum_required(VERSION 3.10)

# 我想要试一下，变量是与当前CMakeLists.txt是保持一致的
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 定义一个函数来添加测试
function(add_pool_test test_name test_src)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE threadpool)
    # 难道不应该指定这个动态库的位置吗？
    # 不需要，这里的threadpool是一个target，CMake会按照依赖顺序自动构建项目
endfunction()

# 添加测试，执行上面的函数
add_pool_test(test_day1_basic ${SRC})
