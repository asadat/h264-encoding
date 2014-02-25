#include "PFrame.h"

PFrame::PFrame():Frame()
{

}

PFrame::~PFrame()
{

}

void PFrame::EncodeDecode()
{
    Convert2YUV();
    ComputeMC_Diff_IntegerTransform();
    ComputeMC_Diff_IntegerTransformInverse();
    Convert2RGB();
}

void PFrame::ComputeMC_Diff_IntegerTransform()
{
    int bs = 8;
    for(int n=0; n<3; n++)
        for(int i=0; i<yuv[n].rows; i+= bs)
            for(int j=0; j<yuv[n].cols; j+= bs)
            {
                printf("here3 %d %d %d\n", n, i, j);
                BlockMC_Diff(n,i,j);
                //printf("here3.5 \n");
                for(int ii=0;ii<2;ii++)
                    for(int jj=0;jj<2;jj++)
                        IntegerTransform(n, i+ii*4, j+jj*4, m_QP);
                //printf(" here4\n");
            }

    //printf("here4");

}

void PFrame::ComputeMC_Diff_IntegerTransformInverse()
{
    int bs = 8;
    for(int n=0; n<3; n++)
        for(int i=0; i<yuv[n].rows; i+= bs)
            for(int j=0; j<yuv[n].cols; j+= bs)
            {
                for(int ii=0;ii<2;ii++)
                    for(int jj=0;jj<2;jj++)
                        IntegerTransformInverse(n, i+ii*4, j+jj*4, m_QP);

                BlockMC_Diff_Inverse(n,i,j);
            }
}


void PFrame::BlockMC_Diff_Inverse(int n, int i, int j)
{
    int refi = mv[n][0][i/8][j/8];
    int refj = mv[n][1][i/8][j/8];

    yuv_m[n].slice(i,j,8,8) += refFrame->yuv_m[n].slice(refj,refj,8,8);
}

void PFrame::BlockMC_Diff(int n, int i, int j)
{
    int bs = 8;
    int p = 8*2;
    int minI = i;
    int minJ = j;

    for(;p>=1; p=ceil(p/2))
    {
        double minMAD = 99999999999;
        int mini=-1;
        int minj=-1;
        for(int ii=-1; ii<=1; ii++)
            for(int jj=-1; jj<=1; jj++)
            {
                int refi = minI+ii*ceil(p/2);
                int refj = minJ+jj*ceil(p/2);

                if(refi >=0 && refi < img.rows && refj >=0 && refj < img.cols)
                {
                    double MAD = GetMAD(n, i, j, refi, refj);
                    if(MAD < minMAD)
                    {
                        minMAD = MAD;
                        mini = refi;
                        minj = refj;
                    }
                }
            }
        assert(mini >= 0);
        assert(minj >= 0);

        minI = mini;
        minJ = minj;
    }

    //printf("1");
    mv[n][0][i/8][j/8] = minI;
    mv[n][0][i/8][j/8] = minJ;
    yuv_m[n].slice(i,i,bs,bs) = yuv_m[n].slice(i,i,bs,bs) - refFrame->yuv_m[n].slice(minI,minJ,bs,bs);
    //printf(" 2\n");

}

double PFrame::GetMAD(int n, int i, int j, int refi, int refj)
{
    Matrix<8,8,int> dif = yuv_m[n].slice(i,j,8,8) - refFrame->yuv_m[n].slice(refi,refj,8,8);
    double sum = sumAbs8x8(dif);
    return sum/64;
}
