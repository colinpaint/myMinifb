cmake_minimum_required(VERSION 3.7)
set (PROJECT_NAME MiniFB)
project (${PROJECT_NAME})
message(STATUS "Processing " ${PROJECT_NAME})

# Sources
set (SrcLib
     include/MiniFB.h include/MiniFB_cpp.h include/MiniFB_enums.h
     src/MiniFB_internal.h src/WindowData.h
     src/MiniFB_common.c src/MiniFB_cpp.cpp src/MiniFB_internal.c src/MiniFB_timer.c)

set (SrcGL src/gl/MiniFB_GL.h src/gl/MiniFB_GL.c )
set (SrcWindows src/windows/WinMiniFB.c src/windows/WindowData_Win.h )
set (SrcWayland src/wayland/WaylandMiniFB.c src/wayland/WindowData_Way.h src/MiniFB_linux.c )
set (SrcX11 src/x11/X11MiniFB.c src/x11/WindowData_X11.h src/MiniFB_linux.c
)

# Avoid RelWithDebInfo and MinSizeRel
set (CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Define Release by default
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
  message (STATUS "Build type not specified: Use Release by default")
endif (NOT CMAKE_BUILD_TYPE)

# Set features
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# Code generation options
option (MINIFB_BUILD_EXAMPLES "Build minifb example programs" TRUE)
option (MINIFB_AVOID_CPP_HEADERS "Avoid including C++ Headers" FALSE)

if (UNIX)
  option (USE_WAYLAND_API "Build the project using wayland API code" OFF)
  if (NOT USE_WAYLAND_API)
    option (USE_OPENGL_API "Build the project using OpenGL API code" ON)
  endif()
elseif (WIN32)
  option (USE_OPENGL_API "Build the project using OpenGL API code" ON)
endif()

# Set GCC/Clang flags
if(NOT MSVC)
  # Avoid default flag values
  set (CMAKE_C_FLAGS "")
  set (CMAKE_C_FLAGS_DEBUG "" )
  set (CMAKE_C_FLAGS_RELEASE "")

  set (CMAKE_CXX_FLAGS "")
  set (CMAKE_CXX_FLAGS_DEBUG "")
  set (CMAKE_CXX_FLAGS_RELEASE "")

  set (CMAKE_OBJC_FLAGS "")
  set (CMAKE_OBJC_FLAGS_DEBUG "")
  set (CMAKE_OBJC_FLAGS_RELEASE "")

  set (CMAKE_OBJCXX_FLAGS "")
  set (CMAKE_OBJCXX_FLAGS_DEBUG "")
  set (CMAKE_OBJCXX_FLAGS_RELEASE "")

  # Set our flags
  add_compile_options ("$<$<CONFIG:Debug>:-g>")
  add_compile_options ("$<IF:$<CONFIG:Debug>,-O0,-O2>")
  add_compile_options (-Wall -Wextra)
  add_compile_options (-Wno-switch -Wno-unused-function -Wno-unused-parameter -Wno-implicit-fallthrough)

  add_compile_options (-Wno-cast-function-type)

else()
  # Security check
  add_compile_options (/GS)

  # Function level linking
  add_compile_options (/Gy)

  # Exceptions
  add_compile_options (/EHsc)
  if(MSVC_VERSION GREATER_EQUAL 1900)
    # SDL checks 2015+
    add_compile_options (/sdl)
  endif()
  if(MSVC_VERSION LESS_EQUAL 1920)
    # Enable Minimal Rebuild (required for Edit and Continue) (deprecated)
    add_compile_options (/Gm)
  endif()
  add_compile_options (/fp:fast)

  # Runtime library
  add_compile_options ("$<IF:$<CONFIG:Debug>,/MDd,/MD>")

  # Program database for edit and continue
  add_compile_options ("$<IF:$<CONFIG:Debug>,/ZI,/Zi>")

  # Optimizations
  add_compile_options ("$<IF:$<CONFIG:Debug>,/Od,/O2>")

  # Inline function expansion
  add_compile_options ("$<IF:$<CONFIG:Debug>,/Ob0,/Ob2>")

  # Basic runtime checks
  add_compile_options ("$<$<CONFIG:Debug>:/RTC1>")

  # Force Visual Studio to actualize __cplusplus version macro
  add_compile_options (/Zc:__cplusplus)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions (-D_DEBUG)
  add_definitions (-DDEBUG)
endif()

# Set compiler/platform specific flags and dependencies
if(WIN32)
  if(MSVC)
    add_definitions (-D_CRT_SECURE_NO_WARNINGS)
  endif()
  add_definitions (-D_WIN32_WINNT=0x0601)  # Windows 7 (we are in 2020)

  if(USE_OPENGL_API)
    list (APPEND SrcLib ${SrcGL})
    add_definitions (-DUSE_OPENGL_API)
  endif()

  list (APPEND SrcLib ${SrcWindows})
elseif(UNIX)
  if (USE_WAYLAND_API)
    list (APPEND SrcLib ${SrcWayland})
  else()
    if (USE_OPENGL_API)
      list (APPEND SrcLib ${SrcGL})
      add_definitions (-DUSE_OPENGL_API)
    endif()
  list (APPEND SrcLib ${SrcX11})
  endif()
endif()

# Library
add_library (minifb STATIC ${SrcLib} )

# Link
if (UNIX)
  if (USE_WAYLAND_API)
    target_link_libraries (minifb "-lwayland-client" "-lwayland-cursor")
  else()
    target_link_libraries (minifb "-lX11")
    #"-lxkbcommon"
    #"-lXrandr" DPI NOT WORKING
    if (USE_OPENGL_API)
      target_link_libraries (minifb "-lGL" )
    endif()
  endif()
elseif(WIN32)
  if(USE_OPENGL_API)
    target_link_libraries (minifb "Opengl32.lib" )
  endif()
  target_link_libraries (minifb "winmm.lib" )
endif()

# For all projects
target_include_directories (minifb PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories (minifb PRIVATE src)
link_libraries (minifb)

# Examples
if(MINIFB_BUILD_EXAMPLES)
  add_executable (noise tests/noise.c )
  add_executable (input_events tests/input_events.c )
  add_executable (input_events_cpp tests/input_events_cpp.cpp )
  add_executable (multiple_windows tests/multiple_windows.c )
  add_executable (hidpi tests/hidpi.c )
  add_executable (fullscreen tests/fullscreen.c )
endif()

# Organize IDE Folders
set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (TARGET minifb PROPERTY FOLDER "Libs")

if(MINIFB_BUILD_EXAMPLES)
  set_property (TARGET noise PROPERTY FOLDER "Tests")
  set_property (TARGET input_events PROPERTY FOLDER "Tests")
  set_property (TARGET input_events_cpp PROPERTY FOLDER "Tests")
  set_property (TARGET multiple_windows PROPERTY FOLDER "Tests")
  set_property (TARGET hidpi PROPERTY FOLDER "Tests")
endif()

message (STATUS "Done " ${PROJECT_NAME})
