include_directories(../Math)

file(GLOB SERVER_SOURCES "*.cpp")
add_library(Server ${SERVER_SOURCES})
