cmake_minimum_required(VERSION 3.10)

project(NoteApp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})

add_definitions(${GTK_CFLAGS_OTHER})

add_executable(NoteApp main.cpp noteapp.cpp
        noteapp.cpp)

target_link_libraries(NoteApp ${GTK_LIBRARIES})
