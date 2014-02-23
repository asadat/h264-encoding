#include "IFrame.h"
#include <stdio.h>

IFrame::IFrame(): nextIF(NULL), previousIF(NULL)
{

}

IFrame::~IFrame()
{

}

void IFrame::Convert2RGB()
{
    double chrs = 2;
    Mat yuv1 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat yuv2 = Mat::zeros(img.rows, img.cols, CV_8UC1);

    resize(yuv[1], yuv1, Size(yuv1.cols, yuv1.rows), chrs, chrs, INTER_LINEAR);
    resize(yuv[2], yuv2, Size(yuv2.cols, yuv2.rows), chrs, chrs, INTER_LINEAR);


    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++)
        {
            img.at<Vec3b>(i,j)[0] = yuv[0].at<uchar>(i,j);
            img.at<Vec3b>(i,j)[1] = yuv1.at<uchar>(i,j);
            img.at<Vec3b>(i,j)[2] = yuv2.at<uchar>(i,j);
        }

    cvtColor(img, img, CV_YUV2RGB);
}

void IFrame::Convert2YUV()
{
    double chrs = 2;
    yuv[0] = Mat::zeros(img.rows, img.cols, CV_8UC1);
    yuv[1] = Mat::zeros(img.rows/chrs, img.cols/chrs, CV_8UC1);
    yuv[2] = Mat::zeros(img.rows/chrs, img.cols/chrs, CV_8UC1);

    Mat yuv1 = Mat::zeros(img.rows, img.cols, CV_8UC1);
    Mat yuv2 = Mat::zeros(img.rows, img.cols, CV_8UC1);

    cvtColor(img, img, CV_RGB2YUV);

    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++)
        {
            yuv[0].at<uchar>(i,j) = img.at<Vec3b>(i,j)[0];
            yuv1.at<uchar>(i,j) = img.at<Vec3b>(i,j)[1];
            yuv2.at<uchar>(i,j) = img.at<Vec3b>(i,j)[2];
        }

    resize(yuv1, yuv[1], Size(yuv[1].cols, yuv[1].rows), 1/chrs, 1/chrs, INTER_LINEAR);
    resize(yuv2, yuv[2], Size(yuv[2].cols, yuv[2].rows), 1/chrs, 1/chrs, INTER_LINEAR);
}

void IFrame::SetImage(Mat &image)
{
    double newc, newr;
    newc = image.cols + 8-image.cols%8;
    newr = image.rows + 8-image.rows%8;

    resize(image, img, Size(newc, newr), newc/image.cols, newr/image.rows, INTER_NEAREST);
}

void IFrame::Intra4x4Prediction()
{
    intraPred[0] = Mat::zeros(yuv[0].rows/4, yuv[0].cols/4, CV_8UC1);
    intraPred[1] = Mat::zeros(yuv[1].rows/4, yuv[1].cols/4, CV_8UC1);
    intraPred[2] = Mat::zeros(yuv[2].rows/4, yuv[2].cols/4, CV_8UC1);

    for(int n =0; n<1; n++)
        for(int i=0; i< yuv[n].rows; i+=4)
            for(int j=0; j< yuv[n].cols; j+=4)
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

                Mat mdc,mver,mhor;
                //printf("1\n");
                if(dc)
                {
//                     printf("\n");
//                    for(int ii=0; ii<4; ii++)
//                    {
//                        int mm = 0;
//                        for(int jj=0; jj<4; jj++)
//                        {
//                            mm += (int)yuv[n].at<uchar>(i+ii,j+jj);
//                            printf("%u \t", yuv[n].at<uchar>(i+ii,j+jj));
//                        }
//                        printf(" \t%d\n", mm);
//                    }
                    int mean =0;
                    for(int ii=0; ii<4; ii++)
                    {
                        printf("%u\t%u\t[%u]\t",yuv[n].at<uchar>(i-1,j+ii),yuv[n].at<uchar>(i+ii,j-1),mean);
                        mean += (unsigned int)yuv[n].at<uchar>(i-1,j+ii);
                        mean += (unsigned int)yuv[n].at<uchar>(i+ii,j-1);
                    }


                    //printf("\n%f\t\n", mean);
                    mdc = mean * Mat::ones(4,4,CV_8UC1) - yuv[n].rowRange(i,i+4).colRange(j,j+4);
                   //Scalar sv_dc =  cv::sum(cv::abs( mdc ));
                   //v_dc = sv_dc[0];

                   min = 1;
                }
                //printf("2\n");
                if(hor)
                {
                    Mat m = Mat::zeros(4,4,CV_8UC1);
                    for(int ii=0; ii<4; ii++)
                        for(int jj=0; jj<4; jj++)
                            m.at<uchar>(ii,jj) = yuv[n].at<uchar>(i+ii,j-1);

                    //m.at<uchar>(0,0) = m.at<uchar>(0,1) = m.at<uchar>(0,2) = m.at<uchar>(0,3) = yuv[n].at<uchar>(i,j-1);
                    //m.at<uchar>(1,0) = m.at<uchar>(1,1) = m.at<uchar>(1,2) = m.at<uchar>(1,3) = yuv[n].at<uchar>(i+1,j-1);
                    //m.at<uchar>(2,0) = m.at<uchar>(2,1) = m.at<uchar>(2,2) = m.at<uchar>(2,3) = yuv[n].at<uchar>(i+2,j-1);
                    //m.at<uchar>(3,0) = m.at<uchar>(3,1) = m.at<uchar>(3,2) = m.at<uchar>(3,3) = yuv[n].at<uchar>(i+3,j-1);
                    //printf("21\n");
                    //printf("%d %d %d %d \n", m.rows, m.cols, yuv[n].rowRange(i,i+3).colRange(j,j+3).rows, yuv[n].rowRange(i,i+3).colRange(j,j+3).cols);
                    mhor = m - yuv[n].rowRange(i,i+4).colRange(j,j+4);
                    //printf("22\n");

                    Scalar sv_hor = cv::sum(cv::abs( mhor ));
                    //printf("23\n");

                    v_hor = sv_hor[0];
                    //printf("24\n");

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
                    Mat m = Mat::zeros(4,4,CV_8UC1);
                    for(int ii=0; ii<4; ii++)
                        for(int jj=0; jj<4; jj++)
                            m.at<uchar>(ii,jj) = yuv[n].at<uchar>(i-1,j+jj);

                    //m.at<uchar>(0,0) = m.at<uchar>(1,0) = m.at<uchar>(2,0) = m.at<uchar>(3,0) = yuv[n].at<uchar>(i-1,j);
                    //m.at<uchar>(0,1) = m.at<uchar>(1,1) = m.at<uchar>(2,1) = m.at<uchar>(3,1) = yuv[n].at<uchar>(i-1,j+1);
                    //m.at<uchar>(0,2) = m.at<uchar>(1,2) = m.at<uchar>(2,2) = m.at<uchar>(3,2) = yuv[n].at<uchar>(i-1,j+2);
                    //m.at<uchar>(0,3) = m.at<uchar>(1,3) = m.at<uchar>(2,3) = m.at<uchar>(3,3) = yuv[n].at<uchar>(i-1,j+3);
                    mver = m - yuv[n].rowRange(i,i+4).colRange(j,j+4);
                    Scalar sv_ver = cv::sum(cv::abs( mver ));
                    v_ver = sv_ver[0];

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

                intraPred[n].at<uchar>(i/4,j/4) = min;
/*
                if(min == 1)
                {
                    //printf("31\n");

                    mdc.copyTo(yuv[n].colRange(j,j+4).rowRange(i,i+4) );
                }
                else if(min == 2)
                {
                   // printf("32\n");

                    mhor.copyTo(yuv[n].colRange(j,j+4).rowRange(i,i+4));
                }
                else if(min == 3)
                {
                    //printf("33\n");

                    mver.copyTo(yuv[n].colRange(j,j+4).rowRange(i,i+4));
                }
  */              //printf("34\n");

            }

    printf("I predictionending....\n");
}
