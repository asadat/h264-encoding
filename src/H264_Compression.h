#include "opencv2/opencv.hpp"
#include "IFrame.h"
#include "PFrame.h"
#include "BFrame.h"

#include <vector>

//using namespace cv;


class H264_Compression
{
public:
    H264_Compression(const char* file);
    ~H264_Compression();

    static void playBtn(int a, void * data);

private:
    void Init(const char* file);

    Mat images[1000];
    std::vector<Frame*> frames;
    VideoCapture cap;

};
