cmake_minimum_required(VERSION 3.15)
project(GamerOverlay VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(HEADERS 
    include/AppState.h
    include/ControlUI.h
    include/CoreLogic.h
    include/ExtraKeysUI.h
    include/InputManager.h
    include/OverlayUI.h
    include/Win32Utils.h
)

set(SOURCES src/main.cpp)
set(RESOURCES res/Resource.rc)

add_executable(Overlay WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(Overlay PRIVATE include res)

target_link_libraries(Overlay PRIVATE user32 gdi32 comdlg32 gdiplus comctl32 shell32)

if(MINGW)
    target_link_options(Overlay PRIVATE -static -static-libgcc -static-libstdc++)
endif()