#include "H264_Compression.h"
#include <stdio.h>
H264_Compression::H264_Compression(const char* file)
{
    Init(file);
}

H264_Compression::~H264_Compression()
{
    while(!iframes.empty())
    {
        IFrame* ifr = iframes.back();
        iframes.pop_back();

        delete ifr;
    }
}

void H264_Compression::playBtn(int a, void * data)
{

}

int val=0;
void H264_Compression::Init(const char* file)
{

    cap.open(file);

    if(!cap.isOpened())
        return;

    namedWindow("H264", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
    namedWindow("raw", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

//    cv::createTrackbar("track1", "H264", &val, 100);
//    cv::createButton("button1", H264_Compression::playBtn,NULL,CV_CHECKBOX,0);

    IFrame * ifr = new IFrame();
    int n=0;
    Mat frame;
    while(cap.read(frame) && n < 100)
    {

        if(n <1000)
        {
            frame.copyTo(frames[n]);
            n++;
        }

        ifr->SetImage(frame);
        iframes.push_back(ifr);
        ifr = new IFrame();
    }

    delete ifr;


    printf("%d %d frames.\n", iframes.size(), n);

    for(int i=0; i<iframes.size(); i++)
    {
        iframes[i]->Convert2YUV();
        iframes[i]->Intra4x4Prediction();
        iframes[i]->Intra4x4PredictionInverse();
        iframes[i]->Convert2RGB();

        imshow("H264", iframes[i]->img);
//        for(int ii=0; ii<4;ii++)
//        {
//            for(int jj=0; jj<4;jj++)
//                printf("%d\t", iframes[i]->yuv[0].at<uchar>(4+ii,4+jj));

//            printf("\n");
//        }

        if(i<1000)
            imshow("raw", frames[i]);

        if(waitKey(30) >= 0) break;
    }


    cap.release();

}



