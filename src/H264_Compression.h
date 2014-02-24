#include "opencv2/opencv.hpp"
#include "IFrame.h"
using namespace cv;


class H264_Compression
{
public:
    H264_Compression(const char* file);
    ~H264_Compression();

    static void playBtn(int a, void * data);

private:
    void Init(const char* file);

    Mat frames[1000];
    cv::Vector<IFrame*> iframes;
    VideoCapture cap;

};
