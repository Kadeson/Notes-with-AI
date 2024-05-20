cmake_minimum_required(VERSION 3.10)
project(NoteApp)

set(CMAKE_CXX_STANDARD 17)

# Путь к библиотекам и заголовочным файлам GTK
set(GTK_DIR "C:/msys64/mingw64")
set(CMAKE_PREFIX_PATH ${GTK_DIR})

# Найдите GTK3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

# Включите заголовочные файлы GTK
include_directories(${GTK_INCLUDE_DIRS})

# Укажите линковочные библиотеки
link_directories(${GTK_LIBRARY_DIRS})
link_libraries(${GTK_LIBRARIES})

add_executable(NoteApp main.cpp)

# Линковка с GTK
target_link_libraries(NoteApp ${GTK_LIBRARIES})
