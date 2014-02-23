#include "IFrame.h"

IFrame::IFrame(): nextIF(NULL), previousIF(NULL)
{

}

IFrame::~IFrame()
{

}

void IFrame::Convert2RGB()
{
    double chrs = 2;
    Mat yuv1 = Mat::zeros(img.rows, img.cols, CV_8U);
    Mat yuv2 = Mat::zeros(img.rows, img.cols, CV_8U);

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
    yuv[0] = Mat::zeros(img.rows, img.cols, CV_8U);
    yuv[1] = Mat::zeros(img.rows/chrs, img.cols/chrs, CV_8U);
    yuv[2] = Mat::zeros(img.rows/chrs, img.cols/chrs, CV_8U);

    Mat yuv1 = Mat::zeros(img.rows, img.cols, CV_8U);
    Mat yuv2 = Mat::zeros(img.rows, img.cols, CV_8U);

    cvtColor(img, img, CV_RGB2YUV);

    for(int i=0; i<img.rows; i++)
        for(int j=0; j<img.cols; j++)
        {
            yuv[0].at<uchar>(i,j) = img.at<Vec3b>(i,j)[0];
            yuv1.at<uchar>(i,j) = img.at<Vec3b>(i,j)[1];
            yuv2.at<uchar>(i,j) = img.at<Vec3b>(i,j)[2];
        }

   // printf("%d %d %d %d \n", yuv1.rows, yuv1.cols, yuv[1].rows, yuv[1].cols);
   // printf("%d %d %d %d \n", yuv2.rows, yuv2.cols, yuv[2].rows, yuv[2].cols);

   // pyrDown(yuv1, yuv[1], Size(yuv[1].rows, yuv[1].cols));
   // pyrDown(yuv2, yuv[2], Size(yuv[2].rows, yuv[2].cols));
    resize(yuv1, yuv[1], Size(yuv[1].cols, yuv[1].rows), 1/chrs, 1/chrs, INTER_LINEAR);
    resize(yuv2, yuv[2], Size(yuv[2].cols, yuv[2].rows), 1/chrs, 1/chrs, INTER_LINEAR);
}
