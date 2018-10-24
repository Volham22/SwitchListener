cmake_minimum_required(VERSION 3.0)

set(LISTHID_SRC ListHIDDevice.c)
set(RULESCHECKER_SRC RulesChecker.c)

add_executable(ListHIDDevice ${LISTHID_SRC})
add_executable(RulesChecker ${RULESCHECKER_SRC})

target_link_libraries(ListHIDDevice ${HIDAPI_LIBRARY})