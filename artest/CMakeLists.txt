cmake_minimum_required(VERSION 3.5)
project(artest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH /Users/Heranort/Downloads/aruco-2.0.19/build)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
find_package( OpenCV REQUIRED)
find_package( aruco REQUIRED)

message("system: ${CMAKE_SYSTEM_PREFIX_PATH}")
find_library(POPPLER_LIBRARY poppler)

message("ppl: ${POPPLER_LIBRARY}")

include_directories(${OpenCV_INCLUDE_DIRS})



set(SOURCE_FILES main.cpp ar_capturer.cpp ar_capturer.h)
add_executable(artest ${SOURCE_FILES})

target_link_libraries(artest /usr/local/Cellar/poppler/0.54.0/lib/libpoppler-cpp.0.dylib)
target_link_libraries(artest ${OpenCV_LIBS} ${aruco_LIBS} ${POPPLER_LIBRARY} ${POPPLER_LIBS})