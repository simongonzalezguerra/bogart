include_directories(../)

file(GLOB SERVICE_SOURCES "*.cpp")
add_library(service ${SERVICE_SOURCES})
