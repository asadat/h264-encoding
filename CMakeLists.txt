cmake_minimum_required(VERSION 2.4.6 FATAL_ERROR)

SET(ProjName h264)

include_directories("/usr/local/include")
link_directories("/usr/local/lib")

set(h264_LIBRARIES opencv_core opencv_video opencv_highgui opencv_imgproc)

#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_CGAL_COMPILE_FLAGS}" )

add_executable(${ProjName} src/h264.cpp)
target_link_libraries(${ProjName} ${h264_LIBRARIES})

