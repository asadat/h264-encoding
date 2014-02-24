#include "opencv2/opencv.hpp"
#include "TooN/TooN.h"

using namespace TooN;
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

    void IntegerTransform(int n, int i, int j, int QP);
    void IntegerTransformInverse(int n, int i, int j, int QP);

    void SetImage(Mat &image);
    Mat * getImage(){return &img;}

    static void ChangeQP(int qp);
    int sumAbs(Matrix<4,4,int> &m);
    void printfMat(Mat m, bool ischar=false);
private:

    static Matrix<4,4,float> Mf;
    static Matrix<4,4,float> Vf;
    Mat img;
    IFrame* nextIF;
    IFrame* previousIF;
    Mat yuv[3];
    Matrix<1000,1000,int> pmyuv[3];
    Mat intraPred[3];
    static int m_QP;

};
