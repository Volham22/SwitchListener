cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -DDEBUG -DDEBUG_VERBOSE")

# SwitchListener Project
project(SwitchListener)

set(SRC
    src/AnswerReader.cpp
    src/Controller.cpp
    src/HidComm.cpp
    src/HidScanner.cpp
    src/ConnectionsManager.cpp
    src/main.cpp)

set(HEADERS
    src/AnswerReader.h
    src/Controller.h
    src/HidComm.h
    src/HidScanner.h
    src/ConnectionsManager.h)

add_subdirectory(libs/)

add_executable(SwitchListener ${SRC} ${HEADERS})
target_link_libraries(SwitchListener hidapi)


#find_package(udev)
#if(UDEV_FOUND)
#    target_include_directories(SwitchListener PUBLIC ${UDEV_INCLUDE_DIR})
#    target_link_libraries(SwitchListener ${UDEV_LIBRARY})
#else()
#    message(FATAL_ERROR "udev not found !")
#endif()

add_subdirectory(tools)