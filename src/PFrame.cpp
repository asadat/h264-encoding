#include "PFrame.h"
#include <stdio.h>

PFrame::PFrame():Frame()
{

}

PFrame::~PFrame()
{

}

void PFrame::EncodeDecode()
{
    printf("P Frame....\n");
    Convert2YUV();
    printf("P Frame diff ....\n");
    ComputeMC_Diff_IntegerTransform();
    printf("P Frame diff inverse....\n");
    ComputeMC_Diff_IntegerTransformInverse();
    printf("P Frame 2rgb....\n");
    Convert2RGB();
    printf("P Frame done....\n");
}

void PFrame::ComputeMC_Diff_IntegerTransform()
{
    int bs = 8;
    for(int n=0; n<3; n++)
        for(int i=0; i+7<yuv[n].rows; i+= bs)
            for(int j=0; j+7<yuv[n].cols; j+= bs)
            {
                for(int ni=0; ni< 8; ni++)
                    for(int nj=0; nj<8; nj++)
                        yuv_m[n][i+ni][j+nj] = (int)yuv[n].at<uchar>(i+ni,j+nj);

                BlockMC_Diff(n,i,j);

                if(n==1 && i == 64 && j == 64)
                {
                    printf("Before INT:\n");
                    for(int ni=0; ni< 4; ni++)
                    {
                        for(int nj=0; nj<4; nj++)
                            printf("%d\t", yuv_m[n][i+ni][j+nj]);
                        printf("\n");
                    }
                }

                IntegerTransform(n, i,      j);
                IntegerTransform(n, i+4,    j);
                IntegerTransform(n, i+4,    j+4);
                IntegerTransform(n, i,      j+4);

                if(n==1 && i == 64 && j == 64)
                {
                    printf("After INT:\n");
                    for(int ni=0; ni< 4; ni++)
                    {
                        for(int nj=0; nj<4; nj++)
                            printf("%d\t", yuv_m[n][i+ni][j+nj]);
                        printf("\n");
                    }
                }

            }

}

void PFrame::ComputeMC_Diff_IntegerTransformInverse()
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

                if(n==1 && i == 64 && j == 64)
                {
                    printf("After INT-Inverse:\n");
                    for(int ni=0; ni< 4; ni++)
                    {
                        for(int nj=0; nj<4; nj++)
                            printf("%d\t", yuv_m[n][i+ni][j+nj]);
                        printf("\n");
                    }
                }

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


void PFrame::BlockMC_Diff_Inverse(int n, int i, int j)
{
    int refi = mv_x[n][i/8][j/8];
    int refj = mv_y[n][i/8][j/8];
    if(refi >=0 && refj >=0)
    {
        Matrix<8,8,int> sum = yuv_m[n].slice(i,j,8,8) + refFrame->yuv_m[n].slice(refi,refj,8,8);
        yuv_m[n].slice(i,j,8,8) = sum;
    }
}

void PFrame::BlockMC_Diff(int n, int i, int j)
{
    int bs = 8;
    int p = 8;
    int minI = i;
    int minJ = j;
    double minMAD = 0;

    int mi = i/8;
    int mj = j/8;

    if(n==0) //luminance
    {
        for(;p>=1; p=ceil(p/2))
        {
            //printf("11\n");
            minMAD = 99999999999;
            int mini=-1;
            int minj=-1;
            for(int ii=-1; ii<=1; ii++)
                for(int jj=-1; jj<=1; jj++)
                {
                    int refi = minI+ii*ceil(p/2);
                    int refj = minJ+jj*ceil(p/2);

                    if(refi >=0 && refi+7 < img.rows && refj >=0 && refj+7 < img.cols)
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
            //printf("22 minMAD: %f %d %d\n", minMAD, minI, minJ);
        }
    }
    else
    {
        minI = ceil(mv_x[0][mi][mj]/2);
        minJ = ceil(mv_y[0][mi][mj]/2);

        if(minI < 0) minI = 0;
        if(minI+7 >= yuv[n].rows) minI = yuv[n].rows-8;

        if(minJ < 0) minJ = 0;
        if(minJ+7 >= yuv[n].cols) minJ = yuv[n].cols-8;
    }

    //if(n>1) printf("33: %d %d %d %d %d %d %d\n", n, i, j, minI, minJ, mi, mj);
    if(false && i>0 && j>0 && minMAD > 4)
    {
        mv_x[n][mi][mj] = -1;
        mv_y[n][mi][mj] = -1;
        //Matrix<8,8,int> dif = yuv_m[n].slice(i,j,bs,bs) - refFrame->yuv_m[n].slice(minI,minJ,bs,bs);
    }
    else
    {
        mv_x[n][mi][mj] = minI;
        mv_y[n][mi][mj] = minJ;
        Matrix<8,8,int> dif = yuv_m[n].slice(i,j,bs,bs) - refFrame->yuv_m[n].slice(minI,minJ,bs,bs);
        yuv_m[n].slice(i,j,bs,bs) = dif;
    }
}

double PFrame::GetMAD(int n, int i, int j, int refi, int refj)
{
    Matrix<8,8,int> dif = yuv_m[n].slice(i,j,8,8) - refFrame->yuv_m[n].slice(refi,refj,8,8);
    double sum = sumAbs8x8(dif);
    return sum;
}

