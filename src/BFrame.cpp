#include "BFrame.h"
#include <stdio.h>

BFrame::BFrame():Frame()
{

}

BFrame::~BFrame()
{

}

void BFrame::EncodeDecode()
{
    printf("B Frame....\n");
    Convert2YUV();
    printf("B Frame diff ....\n");
    ComputeMC_Diff_IntegerTransform();
    printf("B Frame diff inverse....\n");
    ComputeMC_Diff_IntegerTransformInverse();
    printf("B Frame 2rgb....\n");
    Convert2RGB();
    printf("B Frame done....\n");
}

void BFrame::ComputeMC_Diff_IntegerTransform()
{
    int bs = 8;
    for(int n=0; n<3; n++)
        for(int i=0; i+7<yuv[n].rows; i+= bs)
            for(int j=0; j+7<yuv[n].cols; j+= bs)
            {
                for(int ni=0; ni< 8; ni++)
                    for(int nj=0; nj<8; nj++)
                        yuv_m[n][i+ni][j+nj] = (int)yuv[n].at<uchar>(i+ni,j+nj);

                //printf("here3 %d %d %d\n", n, i, j);
                BlockMC_Diff(n,i,j);
                //printf("here3.5 \n");
                IntegerTransform(n, i,      j);
                IntegerTransform(n, i+4,    j);
                IntegerTransform(n, i+4,    j+4);
                IntegerTransform(n, i,      j+4);

//                for(int ii=0;ii<2;ii++)
//                    for(int jj=0;jj<2;jj++)
//                        IntegerTransform(n, i+ii*4, j+jj*4);
                //printf(" here4\n");
            }

    //printf("here4");

}

void BFrame::ComputeMC_Diff_IntegerTransformInverse()
{
    int bs = 8;
    for(int n=0; n<3; n++)
        for(int i=0; i+7<yuv[n].rows; i+= bs)
            for(int j=0; j+7<yuv[n].cols; j+= bs)
            {
//                for(int ii=0;ii<2;ii++)
//                    for(int jj=0;jj<2;jj++)
//                        IntegerTransformInverse(n, i+ii*4, j+jj*4);

                IntegerTransformInverse(n, i,      j);
                IntegerTransformInverse(n, i+4,    j);
                IntegerTransformInverse(n, i+4,    j+4);
                IntegerTransformInverse(n, i,      j+4);

                BlockMC_Diff_Inverse(n,i,j);

                for(int ni=0; ni< 8; ni++)
                    for(int nj=0; nj<8; nj++)
                    {
                        int f = yuv_m[n][i+ni][j+nj];
                        f = (f<0)?0:((f>255)?255:f);
                        yuv[n].at<uchar>(i+ni,j+nj)  = static_cast<unsigned char>(f);
                    }
            }
}


void BFrame::BlockMC_Diff_Inverse(int n, int i, int j)
{
    int refi1 = mv1_x[n][i/8][j/8];
    int refj1 = mv1_y[n][i/8][j/8];
    int refi2 = mv2_x[n][i/8][j/8];
    int refj2 = mv2_y[n][i/8][j/8];

    Matrix<8,8,int> sum = yuv_m[n].slice(i,j,8,8) + 0.5*(refFrame1->yuv_m[n].slice(refi1,refj1,8,8)+refFrame2->yuv_m[n].slice(refi2,refj2,8,8));
    yuv_m[n].slice(i,j,8,8) = sum;
}

void BFrame::BlockMC_Diff(int n, int i, int j)
{
    int bs = 8;
    int p = 8*4;
    int minI1 = i;
    int minJ1 = j;

    int minI2 = i;
    int minJ2 = j;

    for(;p>=1; p=ceil(p/2))
    {
        //printf("11\n");
        double minMAD1 = 99999999999;
        int mini1=-1;
        int minj1=-1;

        double minMAD2 = 99999999999;
        int mini2=-1;
        int minj2=-1;

        for(int ii=-1; ii<=1; ii++)
            for(int jj=-1; jj<=1; jj++)
            {
                int refi1 = minI1+ii*ceil(p/2);
                int refj1 = minJ1+jj*ceil(p/2);

                int refi2 = minI2+ii*ceil(p/2);
                int refj2 = minJ2+jj*ceil(p/2);

                if(refi1 >=0 && refi1+7 < yuv[n].rows && refj1 >=0 && refj1+7 < yuv[n].cols)
                {
                    double MAD = GetMAD(1,n, i, j, refi1, refj1);
                    if(MAD < minMAD1)
                    {
                        minMAD1 = MAD;
                        mini1 = refi1;
                        minj1 = refj1;
                    }
                }

                if(refi2 >=0 && refi2+7 < yuv[n].rows && refj2 >=0 && refj2+7 < yuv[n].cols)
                {
                    double MAD = GetMAD(2,n, i, j, refi2, refj2);
                    if(MAD < minMAD2)
                    {
                        minMAD2 = MAD;
                        mini2 = refi2;
                        minj2 = refj2;
                    }
                }
            }

        assert(mini1 >= 0 && mini1 < yuv[n].rows);
        assert(minj1 >= 0 && minj1 < yuv[n].cols);
        assert(mini2 >= 0 && mini2 < yuv[n].rows);
        assert(minj2 >= 0 && minj2 < yuv[n].cols);

        minI1 = mini1;
        minJ1 = minj1;
        minI2 = mini2;
        minJ2 = minj2;

        //printf("22 minMAD: %f %d %d\n", minMAD, minI, minJ);
    }

    int mi = i/8;
    int mj = j/8;
    mv1_x[n][mi][mj] = minI1;
    mv1_y[n][mi][mj] = minJ1;

    mv2_x[n][mi][mj] = minI2;
    mv2_y[n][mi][mj] = minJ2;

    Matrix<8,8,int> dif = yuv_m[n].slice(i,j,bs,bs) - 0.5*(refFrame1->yuv_m[n].slice(minI1,minJ1,bs,bs)+refFrame2->yuv_m[n].slice(minI2,minJ2,bs,bs));
    yuv_m[n].slice(i,j,bs,bs) = dif;



}

double BFrame::GetMAD(int refn, int n, int i, int j, int refi, int refj)
{
    //printf("GetMad %d %d %d %d %d ", n, i, j, refi, refj);

    Matrix<8,8,int> dif;
    if(refn ==1)
        dif = yuv_m[n].slice(i,j,8,8) - refFrame1->yuv_m[n].slice(refi,refj,8,8);
    else if(refn == 2)
        dif = yuv_m[n].slice(i,j,8,8) - refFrame2->yuv_m[n].slice(refi,refj,8,8);

    double sum = sumAbs8x8(dif);

    //printf(" done\n");

    return sum/64;
}

