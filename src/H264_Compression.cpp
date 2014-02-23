#include "H264_Compression.h"

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

//    cv::createTrackbar("track1", "H264", &val, 100);
//    cv::createButton("button1", H264_Compression::playBtn,NULL,CV_CHECKBOX,0);

    IFrame * ifr = new IFrame();


    while(cap.read(ifr->img))
    {
        iframes.push_back(ifr);
        ifr = new IFrame();
    }

    delete ifr;


    printf("%d frames.\n", iframes.size());

    for(int i=0; i<iframes.size(); i++)
    {
        iframes[i]->Convert2YUV();
        iframes[i]->Convert2RGB();

        imshow("H264", iframes[i]->img);
        if(waitKey(30) >= 0) break;
    }


    cap.release();

}



