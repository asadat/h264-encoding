#include "opencv2/opencv.hpp"
#include "TooN/TooN.h"
//#include "Frame.h"
#include "IFrame.h"

using namespace TooN;
using namespace cv;

class PFrame: public Frame
{
public:
    PFrame();
    ~PFrame();

    void SetReferenceFrame(IFrame* refframe){refFrame = refframe;}
    void EncodeDecode();
    Frame *refFrame;

private:
    double GetMAD(int n, int i, int j, int refi, int refj);

    void BlockMC_Diff(int n, int i, int j);
    void BlockMC_Diff_Inverse(int n, int i, int j);

    void ComputeMC_Diff_IntegerTransform();
    void ComputeMC_Diff_IntegerTransformInverse();

    Matrix<1000,1000,int> mv_x[3];
    Matrix<1000,1000,int> mv_y[3];
};
