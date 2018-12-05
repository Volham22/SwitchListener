cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -DDEBUG")

if(DEBUG_VERBOSE)
    add_definitions(-DDEBUG_VERBOSE)
endif()

# SwitchListener Project
project(SwitchListener)

set(SRC
    src/Interface.cpp
    src/AnswerReader.cpp
    src/Controller.cpp
    src/HidComm.cpp
    src/HidScanner.cpp
    src/ConnectionsManager.cpp
    src/main.cpp)

set(HEADERS
    src/Interface.h
    src/AnswerReader.h
    src/Controller.h
    src/HidComm.h
    src/HidScanner.h
    src/ConnectionsManager.h)

add_executable(SwitchListener ${SRC} ${HEADERS})

find_package(HIDAPI)
if(HIDAPI_FOUND)
    target_include_directories(SwitchListener PUBLIC ${HIDAPI_INCLUDE_DIR})
    target_link_libraries(SwitchListener ${HIDAPI_LIBRARY})
else()
    message(FATAL_ERROR "hidapi not found !")
endif()

find_package(udev)
if(UDEV_FOUND)
    target_include_directories(SwitchListener PUBLIC ${UDEV_INCLUDE_DIR})
    target_link_libraries(SwitchListener ${UDEV_LIBRARY})
else()
    message(FATAL_ERROR "udev not found !")
endif()

add_subdirectory(tools)