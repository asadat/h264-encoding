cmake_minimum_required(VERSION 2.4.6 FATAL_ERROR)

SET(ProjName h264)

include_directories("/usr/include")
include_directories("/usr/local/include")
include_directories("./src")

link_directories("/usr/lib")
link_directories("/usr/local/lib")

set(h264_LIBRARIES opencv_core opencv_video opencv_highgui opencv_imgproc opencv_contrib)

#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_CGAL_COMPILE_FLAGS}" )

add_executable(${ProjName}
src/h264.cpp
src/H264_Compression.cpp
src/H264_Compression.h
src/IFrame.cpp
src/IFrame.h
)
target_link_libraries(${ProjName} ${h264_LIBRARIES})

