include_directories (../../Math)
include_directories (../../Client)
include_directories (../../Server)
include_directories (../../)

file(GLOB CLIENT_MENU_SOURCES "*.cpp")
add_library(ClientMenu ${CLIENT_MENU_SOURCES})
# Even though we are building a library and not an executable, CMake requires that we declare a link dependency
# to the Client library which we are using here. Otherwise we will get link errors when building
target_link_libraries(ClientMenu Client)
