#include "Frame.h"
#include <stdio.h>
#include "TooN/TooN.h"

using namespace TooN;

int Frame::m_QP;


Matrix<4,4,float> Frame::Mf;
Matrix<4,4,float> Frame::Vf;


Matrix<6,3,double> V = Data(10, 16, 13,
                         11, 18, 14,
                         13, 20, 16,
                         14, 23, 18,
                         16, 25, 20,
                         18, 29, 23);

Matrix<6,3,double> M = Data(13107, 5243, 8066,
                         11916, 4660, 7490,
                         10082, 4194, 6554,
                         9363 , 3647, 5825,
                         8192 , 3355, 5243,
                         7282 , 2893, 4559);

Frame::Frame(): nextF(NULL), previousF(NULL)
{
    m_QP =5;
    ChangeQP(m_QP);

}

Frame::~Frame()
{

}

void Frame::Convert2RGB()
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

void Frame::Convert2YUV()
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

void Frame::SetImage(Mat &image)
{
    double newc, newr;
    newc = image.cols + 8-image.cols%8;
    newr = image.rows + 8-image.rows%8;

    resize(image, img, Size(newc, newr), newc/image.cols, newr/image.rows, INTER_NEAREST);
}

int Frame::sumAbs(Matrix<4,4,int> &m)
{
    int sum=0;
    for(int i=0; i<m.num_rows(); i++)
        for(int j=0; j<m.num_cols(); j++)
            sum += abs(m[i][j]);

    return sum;
}

int Frame::sumAbs8x8(Matrix<8,8,int> &m)
{
    int sum=0;
    for(int i=0; i<m.num_rows(); i++)
        for(int j=0; j<m.num_cols(); j++)
            sum += abs(m[i][j]);

    return sum;
}

void Frame::ChangeQP(int qp)
{
    int QP = qp;
    m_QP = qp;
    Mf = Data(
                M[QP%6][0]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][0]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)),
                M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][1]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][1]/pow(2,floor(QP/6)),
                M[QP%6][0]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][0]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)),
                M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][1]/pow(2,floor(QP/6)), M[QP%6][2]/pow(2,floor(QP/6)), M[QP%6][1]/pow(2,floor(QP/6))
                );

   Vf = Data(
                V[QP%6][0]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][0]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)),
                V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][1]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][1]*pow(2,floor(QP/6)),
                V[QP%6][0]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][0]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)),
                V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][1]*pow(2,floor(QP/6)), V[QP%6][2]*pow(2,floor(QP/6)), V[QP%6][1]*pow(2,floor(QP/6))
                );

}

void Frame::IntegerTransform(int n, int i, int j, int QP)
{
    Matrix<4,4,int> c = Data(
                1,  1,  1,  1,
                2,  1, -1, -2,
                1,  -1, -1, 1,
                1,  -2, 2,  -1
                );




    Matrix<4,4, int> tmp = (c*(yuv_m[n].slice(i,j,4,4)*c.T()));

    for(int ii=0; ii<tmp.num_rows(); ii++)
       for(int jj=0; jj<tmp.num_cols(); jj++)
       {
           int val = tmp[ii][jj]*Mf[ii][jj];
           val = val >> 15;
           tmp[ii][jj] = val;
       }



    if(false && n==1 && i/4==15 && j/4==15)
    {
        printf("\n Raw:\n");
        for(int ii=0; ii<tmp.num_rows(); ii++)
        {
            for(int jj=0; jj<tmp.num_cols(); jj++)
                printf("%d\t", yuv_m[n][i+ii][j+jj]);
            printf("\n");
        }

        printf("\n Encoded:\n");
        for(int ii=0; ii<tmp.num_rows(); ii++)
        {
            for(int jj=0; jj<tmp.num_cols(); jj++)
                printf("%d\t", tmp[ii][jj]);
            printf("\n");
        }
    }

      yuv_m[n].slice(i,j,4,4)= tmp;
}

void Frame::IntegerTransformInverse(int n, int i, int j, int QP)
{
    Matrix<4,4,double> c = Data(
                1,      1,      1,      1,
                1,      0.5,    -0.5,   -1,
                1,      -1,     -1,     1,
                0.5,    -1,     1,      -0.5
                );


    Matrix<4,4, float> tmp1;
    for(int ii=0; ii<4; ii++)
       for(int jj=0; jj<4; jj++)
       {
           tmp1[ii][jj] = yuv_m[n][i+ii][j+jj]*Vf[ii][jj];
       }

    Matrix<4,4, int> tmp = (1.0/64.0)*(c.T()*(tmp1*c));


    yuv_m[n].slice(i,j,4,4) = tmp;

    if(false && n==1 && i/4==15 && j/4==15)
    {
        printf("\n Decoded:\n");
        for(int ii=0; ii<tmp.num_rows(); ii++)
        {
            for(int jj=0; jj<tmp.num_cols(); jj++)
                printf("%d\t", tmp[ii][jj]);
            printf("\n");
        }

        printf("\n Vf:\n");
        for(int ii=0; ii<tmp.num_rows(); ii++)
        {
            for(int jj=0; jj<tmp.num_cols(); jj++)
                printf("%f\t", Vf[ii][jj]);
            printf("\n");
        }
    }
}
