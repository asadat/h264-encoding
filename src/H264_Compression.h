#include "opencv2/opencv.hpp"

using namespace cv;


class H264_Compression
{
public:
    H264_Compression(const char* file);
    ~H264_Compression();
private:
    void Init(const char* file);

    VideoCapture cap;
};
