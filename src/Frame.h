#ifndef _FRAME_
#define _FRAME_

#include "opencv2/opencv.hpp"
#include "TooN/TooN.h"

using namespace TooN;
using namespace cv;

class Frame
{
public:
    Frame();
    ~Frame();

    void Convert2YUV();
    void Convert2RGB();

    void IntegerTransform(int n, int i, int j);
    void IntegerTransformInverse(int n, int i, int j);

    void SetImage(Mat &image);
    Mat * getImage(){return &img;}

    int sumAbs(Matrix<4,4,int> &m);
    int sumAbs8x8(Matrix<8,8,int> &m);

    virtual void EncodeDecode()=0;

    static void ChangeQP(int qp);

protected:

    static Matrix<4,4,float> Mf;
    static Matrix<4,4,float> Vf;

    Mat img;
    Frame* nextF;
    Frame* previousF;
    Mat yuv[3];
    Matrix<1500,1500,int> yuv_m[3];
    static int m_QP;

    friend class PFrame;
};

#endif
