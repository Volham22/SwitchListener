cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH cmake/)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -02 -Wall")

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

add_executable(SwitchListener ${SRC} ${HEADERS})

find_package(HIDAPI)
if(HIDAPI_FOUND)
    target_include_directories(SwitchListener PUBLIC ${HIDAPI_INCLUDE_DIR})
    target_link_libraries(SwitchListener ${HIDAPI_LIBRARY})
else()
    message(FATAL_ERROR "hidapi not found !")
endif()

add_subdirectory(tools)