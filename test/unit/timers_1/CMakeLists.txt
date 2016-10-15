file(GLOB TIMERS_1_SOURCES "*.cpp")
add_executable(timers_1 ${TIMERS_1_SOURCES})

target_link_libraries(timers_1 async pthread log)
