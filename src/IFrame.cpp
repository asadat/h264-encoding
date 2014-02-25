#include "IFrame.h"
#include <stdio.h>
#include "TooN/TooN.h"

using namespace TooN;


IFrame::IFrame():Frame()
{
}

IFrame::~IFrame()
{

}

void IFrame::EncodeDecode()
{
    printf("I Frame....\n");
    Convert2YUV();
    Intra4x4Prediction();
    Intra4x4PredictionInverse();
    Convert2RGB();
}

void IFrame::Intra4x4Prediction()
{
//    Mat yuv_[3];
//    yuv[0].convertTo(yuv_[0], CV_32FC1);
//    yuv[1].convertTo(yuv_[1], CV_32FC1);
//    yuv[2].convertTo(yuv_[2], CV_32FC1);

    Matrix<1500,1500,int> myuv[3];


    for(int i=0; i< 3; i++)
    {
        //Matrix<1000,1,int> tmp(yuv[i].rows, yuv[i].cols);
        for(int ii=0; ii<yuv[i].rows; ii++)
        {
            for(int jj=0; jj<yuv[i].cols; jj++)
            {
                myuv[i][ii][jj] = (int)yuv[i].at<uchar>(ii,jj);
                //printf("(%d,%f)\t", tmp[ii][jj], yuv_[i].at<float>(ii,jj));
            }
           //printf("\n");
        }
        //myuv[i] = tmp;
    }

//    printfMat(yuv[2].colRange(4,8).rowRange(4,8), true);
//    printf("***** \n\n");
//    printfMat(yuv_[2].colRange(4,8).rowRange(4,8));
//    exit(0);
    int bs = 4;
    intraPred[0] = Mat::zeros(yuv[0].rows/bs, yuv[0].cols/bs, CV_8UC1);
    intraPred[1] = Mat::zeros(yuv[1].rows/bs, yuv[1].cols/bs, CV_8UC1);
    intraPred[2] = Mat::zeros(yuv[2].rows/bs, yuv[2].cols/bs, CV_8UC1);

    for(int n =0; n<3; n++)
        for(int i=0; i< yuv[n].rows; i+=bs)
            for(int j=0; j< yuv[n].cols; j+=bs)
            {
                bool hor=false, ver=false, dc = false;

                if(i>0 && j>0)
                    dc = true;
                if(i>0)
                    ver = true;
                if(j>0)
                    hor=true;

                int v_dc = 0;
                int v_hor = 0;
                int v_ver = 0;

                int min=0;


                Matrix<4,4,int> mdc,mver,mhor;

                int mean =0;
                //printf("1\n");
                if(dc)
                {

                    // printf("\n Mean start: ");
                    for(int ii=0; ii<bs; ii++)
                    {
//                        int b = 15;
//                        if(i/bs == b & j/bs == b && n==0)
//                            printf("%d\t%d\t",myuv[n][i-1][j+ii], myuv[n][i+ii][j-1]);
                        //printf("%d\t%d\t",myuv[n][i-1][j+ii],myuv[n][i+ii][j-1]);
                        mean += myuv[n][i-1][j+ii];
                        mean += myuv[n][i+ii][j-1];
                    }

                    mean = mean/(2*bs);
                   // printf("\nmean: %d\t\n", mean);
                    Matrix<4,4,int> meanmat = mean * TooN::Ones(bs,bs);
                    //printf("MeanMat:");
                    //printfMat(meanmat);

                   // printf("here1");
                    mdc = myuv[n].slice(i,j,bs,bs) - meanmat;
                   // printf("here2");

                    //Scalar sv_dc =  cv::sum(cv::abs( mdc ));
                    v_dc = sumAbs(mdc);

                   min = 1;
                }
                //printf("2\n");
                if(hor)
                {
                   Matrix<4,4,int> m;
                    for(int ii=0; ii<bs; ii++)
                        for(int jj=0; jj<bs; jj++)
                            m[ii][jj] = myuv[n][i+ii][j-1];

                    mhor = myuv[n].slice(i,j,bs,bs) - m;
                    v_hor = sumAbs(mhor);

                    if(min ==1 && v_dc > v_hor )
                    {
                        min = 2;
                    }
                    else if(min ==0)
                    {
                        min = 2;
                    }
                }

                //printf("3\n");
                if(ver)
                {
                   // Mat m = Mat::zeros(4,4,CV_8UC1);
                    Matrix<4,4,int> m;
                    for(int ii=0; ii<bs; ii++)
                        for(int jj=0; jj<bs; jj++)
                            m[ii][jj] = myuv[n][i-1][j+jj];

                    mver = myuv[n].slice(i,j,bs,bs) - m;
                    v_ver = sumAbs(mver);

                    if(min == 1 && v_dc > v_ver)
                    {
                        min = 3;
                    }
                    else if(min == 2 && v_hor > v_ver)
                    {
                        min = 3;
                    }
                    else if(min == 0)
                    {
                        min = 3;
                    }
                }

                intraPred[n].at<uchar>(i/bs,j/bs) = min;

                if(min == 0)
                {
//                    for(int iii=0; iii<4; iii++)
//                    {
//                        for(int jjj=0; jjj<4; jjj++)
//                        {
//                            pmyuv[n][i+iii][j+jjj] = myuv[n][i+iii][j+jjj];
//                        }
//                    }

                    yuv_m[n].slice(i,j,bs,bs) = myuv[n].slice(i,j,bs,bs);
                }
                else if(min == 1)
                {
                    //printf("31\n");


                    //printf("DC:");
                    //printfMat(mdc);

                   // printf("Before Frame:\n");
                    //printfMat(yuv_[n].colRange(j,j+bs).rowRange(i,i+bs));
//                    int b = 15;
//                    if(i/bs == b & j/bs == b && n==0)
//                        printf("**B 5 : mean:%d \n", mean);

//                    for(int iii=0; iii<4; iii++)
//                    {
//                        for(int jjj=0; jjj<4; jjj++)
//                        {
//                            pmyuv[n][i+iii][j+jjj] = mdc[iii][jjj];
//                        }
//                    }
                    yuv_m[n].slice(i,j,bs,bs) = mdc;
                   // yuv_[n].colRange(j,j+bs).rowRange(i,i+bs) = mdc;
                    //yuv_[n].colRange(j,j+bs).rowRange(i,i+bs)= mdc;//Mat::zeros(bs,bs, CV_8UC1);

                    //printf("After Frame:\n");
                    //printfMat(yuv_[n].colRange(j,j+bs).rowRange(i,i+bs));
                }
                else if(min == 2)
                {
                    //printf("HO:");
                    yuv_m[n].slice(i,j,bs,bs) = mhor;

                    //yuv_[n].colRange(j,j+bs).rowRange(i,i+bs) = mhor;
                }
                else if(min == 3)
                {
                   // printf("VE:");
                    //printf("33\n");
                    yuv_m[n].slice(i,j,bs,bs) = mver;
                    //yuv_[n].colRange(j,j+bs).rowRange(i,i+bs) = mver;
                }
                //printf("34\n");

                if(n==0 && i==40 && j==40)
                {
                    for(int gi=0; gi<4; gi++)
                    {
                        for(int gj=0; gj<4; gj++)
                            printf("\t%d\t",yuv_m[n][i+gi][j+gj]);
                        printf("\n");
                    }
                }

                IntegerTransform(n,i,j);
            }

//    for(int i=0; i< 3; i++)
//    {
//        for(int ii=0; ii<yuv[i].rows; ii++)
//        {
//            for(int jj=0; jj<yuv[i].cols; jj++)
//            {
//                yuv_[i].at<float>(ii,jj)  = pmyuv[i][ii][jj];
//                //printf("(%d,%f)\t", tmp[ii][jj], yuv_[i].at<float>(ii,jj));
//            }
//           //printf("\n");
//        }

//    }

//    yuv_[0].convertTo(yuv[0], CV_8UC1);
//    yuv_[1].convertTo(yuv[1], CV_8UC1);
//    yuv_[2].convertTo(yuv[2], CV_8UC1);
    //printfMat(intraPred[0], true);
}

void IFrame::Intra4x4PredictionInverse()
{
    int bs = 4;

//    Mat yuv_[3];
//    yuv[0].convertTo(yuv_[0], CV_32FC1);
//    yuv[1].convertTo(yuv_[1], CV_32FC1);
//    yuv[2].convertTo(yuv_[2], CV_32FC1);

//    Matrix<1000,1000,int> myuv[3];
//    for(int i=0; i< 3; i++)
//    {
//        //Matrix<1000,1,int> tmp(yuv[i].rows, yuv[i].cols);
//        for(int ii=0; ii<yuv[i].rows; ii++)
//        {
//            for(int jj=0; jj<yuv[i].cols; jj++)
//            {
//                myuv[i][ii][jj] = (int)yuv_[i].at<float>(ii,jj);
//                //printf("(%d,%f)\t", tmp[ii][jj], yuv_[i].at<float>(ii,jj));
//            }
//           //printf("\n");
//        }
//        //myuv[i] = tmp;
//    }

    for(int n=0; n<3;n++)
        for(int i=0; i<yuv[n].rows; i+=bs)
            for(int j=0; j<yuv[n].cols; j+=bs)
            {
                IntegerTransformInverse(n, i , j);

                if(intraPred[n].at<uchar>(i/bs,j/bs) == 0)
                {
//                    printf("\n Raw:\n");
//                    for(int ii=0; ii<bs; ii++)
//                    {
//                        for(int jj=0; jj<bs; jj++)
//                        {
//                            printf("%d\t", pmyuv[n][i+ii][j+jj]);
//                        }
//                        printf("\n");
//                    }
                }
                else if(intraPred[n].at<uchar>(i/bs,j/bs) == 1)
                {
                    assert(intraPred[n].at<uchar>(i/bs-1,j/bs)==5);
                    assert(intraPred[n].at<uchar>(i/bs,j/bs-1)==5);

                    int mean =0;
                    for(int ii=0; ii<bs; ii++)
                    {
//                        int b = 15;
//                        if(i/bs == b & j/bs == b && n==0)
//                            printf("%d\t%d\t",pmyuv[n][i-1][j+ii], pmyuv[n][i+ii][j-1]);
                        mean += yuv_m[n][i-1][j+ii];
                        mean += yuv_m[n][i+ii][j-1];
                    }

                    mean = mean / (2*bs);

//                    int b = 15;
//                    if(i/bs == b & j/bs == b && n==0)
//                        printf("\n**decoding B 5 : mean:%d \n", mean);

                    for(int ii=0; ii<bs; ii++)
                        for(int jj=0; jj<bs; jj++)
                            yuv_m[n][i+ii][j+jj] += mean;

                }
                else if(intraPred[n].at<uchar>(i/bs,j/bs) == 2)
                {
                    for(int ii=0; ii<bs; ii++)
                        for(int jj=0; jj<bs; jj++)
                            yuv_m[n][i+ii][j+jj] += yuv_m[n][i+ii][j-1];
                }
                else if(intraPred[n].at<uchar>(i/bs,j/bs) == 3)
                {
                    for(int ii=0; ii<bs; ii++)
                        for(int jj=0; jj<bs; jj++)
                            yuv_m[n][i+ii][j+jj] += yuv_m[n][i-1][j+jj];
                }

                intraPred[n].at<uchar>(i/bs,j/bs) = 5;
            }


    for(int i=0; i< 3; i++)
    {
        for(int ii=0; ii<yuv[i].rows; ii++)
        {
            for(int jj=0; jj<yuv[i].cols; jj++)
            {
                int f = yuv_m[i][ii][jj];
                f = (f<0)?0:((f>255)?255:f);
                yuv_m[i][ii][jj] = f;
                yuv[i].at<uchar>(ii,jj)  = static_cast<unsigned char>(f);
                //printf("(%d,%f)\t", tmp[ii][jj], yuv_[i].at<float>(ii,jj));
            }
           //printf("\n");
        }

    }

//    yuv_[0].convertTo(yuv[0], CV_8UC1);
//    yuv_[1].convertTo(yuv[1], CV_8UC1);
//    yuv_[2].convertTo(yuv[2], CV_8UC1);
}

void IFrame::printfMat(Mat m, bool ischar)
{
    printf("\n Size : %d %d\n", m.rows, m.cols);
    for(int i=0; i<m.rows; i++)
    {
        for(int j=0; j< m.cols; j++)
            if(ischar)
                printf("%u\t", m.at<uchar>(i,j));
            else
                printf("%f\t", m.at<float>(i,j));
        printf("\n");
    }
}
