include_directories(../)

file(GLOB ASYNC_SOURCES "*.cpp")
add_library(async ${ASYNC_SOURCES})
