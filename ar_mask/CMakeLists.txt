cmake_minimum_required(VERSION 3.5)
project(ar_mask)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_MODULE_PATH /home/xuan/下载/aruco-2.0.19/build )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
find_package( OpenCV REQUIRED)
#find_package( aruco REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})


set(SOURCE_FILES main.cpp ARCapturer.cpp ARCapturer.h HandDetector.cpp HandDetector.h OnPaper.cpp OnPaper.h Painter.cpp Painter.h  LayerManager.h GestureJudge.cpp GestureJudge.h cvutils.h PaperFun.cpp PaperFun.h json.hpp)
add_executable(ar_mask ${SOURCE_FILES})


target_link_libraries(ar_mask ${OpenCV_LIBS} ${aruco_LIBS} "/usr/lib/libaruco.so")