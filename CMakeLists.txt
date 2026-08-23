cmake_minimum_required(VERSION 3.15)

project(GamerOverlay VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES src/main.cpp)
set(RESOURCES res/Resource.rc)

add_executable(Overlay WIN32 ${SOURCES} ${RESOURCES})

target_include_directories(Overlay PRIVATE include res)

target_link_libraries(Overlay PRIVATE 
    user32 
    gdi32 
    comdlg32 
    gdiplus 
    comctl32
)