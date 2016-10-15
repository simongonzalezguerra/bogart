add_subdirectory (async)
add_subdirectory (log)
add_subdirectory (service)

file(GLOB ROOT_SOURCES "*.cpp")
add_executable(bogart ${ROOT_SOURCES})

FIND_LIBRARY(X11_LIBRARY X11)
target_link_libraries(bogart async log service libHorde3D.so libHorde3DUtils.so libglfw.so ${X11_LIBRARY} GL pthread)
