cmake_minimum_required(VERSION 3.5)
project(handmask)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
find_package( OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})



set(SOURCE_FILES main.cpp HandDetector.cpp HandDetector.h)
add_executable(handmask ${SOURCE_FILES})


target_link_libraries(handmask ${OpenCV_LIBS})
