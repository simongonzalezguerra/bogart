include_directories(../)

file(GLOB LOG_SOURCES "*.cpp")
add_library(log ${LOG_SOURCES})
