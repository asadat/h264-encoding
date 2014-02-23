#include "opencv2/opencv.hpp"

using namespace cv;

class IFrame
{
public:
    IFrame();
    ~IFrame();

    IFrame* nextIF;
    IFrame* previousIF;
    Mat img;
    Mat yuv[3];


    void Convert2YUV();
    void Convert2RGB();


};
