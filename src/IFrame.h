#include "opencv2/opencv.hpp"

using namespace cv;

class IFrame
{
public:
    IFrame();
    ~IFrame();

    void Convert2YUV();
    void Convert2RGB();

    void Intra4x4Prediction();
    void Intra4x4PredictionInverse();

    void SetImage(Mat &image);
    Mat * getImage(){return &img;}


        Mat img;
        IFrame* nextIF;
        IFrame* previousIF;
        Mat yuv[3];

        Mat intraPred[3];

private:
};
