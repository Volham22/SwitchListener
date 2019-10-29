cmake_minimum_required(VERSION 3.0)

# List HID device tool
set(LISTHID_SRC ListHIDDevice.c)
add_executable(ListHIDDevice ${LISTHID_SRC})

# Rules checker is an linux only tool no need under windows
if(UNIX)
set(RULESCHECKER_SRC RulesChecker.c)
    add_executable(RulesChecker ${RULESCHECKER_SRC})
endif(WIN32)

target_link_libraries(ListHIDDevice hidapi)