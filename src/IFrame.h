#ifndef _IFRAME_
#define _IFRAME_

#include "opencv2/opencv.hpp"
#include "TooN/TooN.h"
#include "Frame.h"

using namespace TooN;
using namespace cv;

class IFrame: public Frame
{
public:
    IFrame();
    ~IFrame();

    void Intra4x4Prediction();
    void Intra4x4PredictionInverse(int n, int i, int j);

    void EncodeDecode();
    char TypeOfFrame(){return 'I';}
    void printfMat(Mat m, bool ischar=false);
private:

    Mat intraPred[3];

};

#endif
