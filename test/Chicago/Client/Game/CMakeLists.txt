include_directories (../../Math)
include_directories (../../Server)
include_directories (../../Client)
# Awful hack to workaround unnecessary dependency to MenuController.hpp through Application.hpp
include_directories (../../Client/Menu)
include_directories (../../)

file(GLOB CLIENT_GAME_SOURCES "*.cpp")
add_library(ClientGame ${CLIENT_GAME_SOURCES})
# Even though we are building a library and not an executable, CMake requires that we declare a link dependency
# to the Client library which we are using here. Otherwise we will get link errors when building
target_link_libraries(ClientGame Client)

