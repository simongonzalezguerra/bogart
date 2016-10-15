include_directories(../MVC)
include_directories(../Math)
include_directories(../Server)
include_directories(../Log)
include_directories(../)

file(GLOB CLIENT_SOURCES "*.cpp")
add_library(Client ${CLIENT_SOURCES})
target_link_libraries(Client ClientGame ClientMenu)
