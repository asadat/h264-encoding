#include "opencv2/opencv.hpp"
#include "TooN/TooN.h"
//#include "Frame.h"
#include "IFrame.h"

using namespace TooN;

class BFrame: public Frame
{
public:
    BFrame();
    ~BFrame();

    char TypeOfFrame(){return 'B';}

    void SetReferenceFrames(Frame* refframe1, Frame* refframe2){refFrame1 = refframe1;refFrame2 = refframe2;}
    void EncodeDecode();
    Frame *refFrame1;
    Frame *refFrame2;


private:
    double GetMAD(int refn, int n, int i, int j, int refi, int refj);

    void BlockMC_Diff(int n, int i, int j);
    void BlockMC_Diff_Inverse(int n, int i, int j);

    void ComputeMC_Diff_IntegerTransform();
    void ComputeMC_Diff_IntegerTransformInverse();

    Matrix<1500,1500,int> mv1_x[3];
    Matrix<1500,1500,int> mv1_y[3];
    Matrix<1500,1500,int> mv2_x[3];
    Matrix<1500,1500,int> mv2_y[3];
};
