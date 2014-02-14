#include "opencv2/opencv.hpp"

using namespace cv;


class H264_Compression
{
public:
    H264_Compression(const char* file);
    ~H264_Compression();
private:
    void Init(const char* file);

    Mat frames[1000];
    VideoCapture cap;
};
