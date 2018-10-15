cmake_minimum_required(VERSION 3.0)

set(SRC ListHIDDevice.c)

add_executable(ListHIDDevice ${SRC})


target_link_libraries(ListHIDDevice ${HIDAPI_LIBRARY})
