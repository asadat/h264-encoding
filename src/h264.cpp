#include "opencv2/opencv.hpp"
#include "H264_Compression.h"


int main(int argc, char** argv)
{
    H264_Compression h264(argv[1], atoi(argv[2]));
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
