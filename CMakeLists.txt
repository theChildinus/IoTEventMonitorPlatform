cmake_minimum_required(VERSION 3.5)
project(IoTEventMonitorPlatform)

set(CMAKE_CXX_STANDARD 11)

# 打开QT的MOC功能功能
set(CMAKE_AUTOMOC ON)

# 查找 QT的库
if (WIN32)
    #set(Qt5Widgets_DIR D:\\Qt\\5.9.2\\mingw53_32\\lib\\cmake\\Qt5Widgets)

    #link_directories(D:\\Qt\\Tools\\mingw530_32\\i686-w64-mingw32\\lib)
    link_directories(D:\\Users\\chenkuan\\ClionProjects\\tinylog\\lib)
    link_directories(D:\\Users\\chenkuan\\ClionProjects\\tinyxml2\\lib)
    link_directories(D:\\Users\\chenkuan\\ClionProjects\\runtime-verification-system\\lib)

    include_directories(D:\\Users\\chenkuan\\ClionProjects\\tinylog\\headers)
    include_directories(D:\\Users\\chenkuan\\ClionProjects\\tinyxml2\\headers)
    include_directories(D:\\Users\\chenkuan\\ClionProjects\\runtime-verification-system\\headers\\DFA)
    include_directories(D:\\software\\compilerandalgorithm\\z3\\z3-4.6.0-x86-win\\include)
elseif(UNIX)
    set(Qt5Widgets_DIR /home/kong/Qt/5.9.3/gcc_64/lib/cmake/Qt5Widgets)

    link_directories(/home/kong/CLionProjects/tinylog/lib)
    link_directories(/home/kong/CLionProjects/tinyxml2/lib)
    link_directories(/home/kong/CLionProjects/runtime-verification-system/lib)

    include_directories(/home/kong/CLionProjects/tinylog/headers)
    include_directories(/home/kong/CLionProjects/tinyxml2/headers)
    include_directories(/home/kong/CLionProjects/runtime-verification-system/headers/DFA)
endif()
find_package(Qt5Widgets)

# 依赖头文件
include_directories(headers/GUI)
include_directories(headers/manager)
include_directories(headers/memoryClient)
include_directories(headers/netfilterClient)
include_directories(headers/serialPortClient)

# 代码文件
set(SRC_MAIN src/main.cpp)
aux_source_directory(src/GUI SRC_GUI)
aux_source_directory(src/manager SRC_MANAGER)
aux_source_directory(src/memoryClient SRC_MEMORY_CLIENT)
aux_source_directory(src/netfilterClient SRC_NETFILTER_CLIENT)
aux_source_directory(src/serialPortClient SRC_SERIAL_PORT_CLIENT)

add_executable(IoTEventMonitorPlatform ${SRC_MAIN} ${SRC_GUI} ${SRC_MANAGER} ${SRC_NETFILTER_CLIENT} ${SRC_SERIAL_PORT_CLIENT} ${SRC_MEMORY_CLIENT})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 链接QT库
target_link_libraries(IoTEventMonitorPlatform Qt5::Widgets)

# 链接tinylog
target_link_libraries(IoTEventMonitorPlatform tinylog)

# 链接tinyxml2
target_link_libraries(IoTEventMonitorPlatform tinyxml2)

# 链接状态机
target_link_libraries(IoTEventMonitorPlatform rvsDFA)
