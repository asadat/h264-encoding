#include "H264_Compression.h"
#include <stdio.h>
H264_Compression::H264_Compression(const char* file)
{
    Init(file);
}

H264_Compression::~H264_Compression()
{
    while(!frames.empty())
    {
        Frame* ifr = frames.back();
        frames.pop_back();

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

   cv::createTrackbar("QP:", "H264", &val, 40);
   //cv::createButton("button1", H264_Compression::playBtn,NULL);


    int n=0;
    Mat image;
    while(cap.read(image) && n < 100)
    {
        Frame *frame;
        if(n%2 == 0)
        {
            frame = new IFrame();
        }
        else
        {
            frame = new PFrame();
            ((PFrame*)frame)->refFrame = frames.back();
        }

        if(n <1000)
        {
            image.copyTo(images[n]);
            n++;
        }

        frame->SetImage(image);
        frames.push_back(frame);
    }


    printf("%d %d frames.\n", frames.size(), n);

    for(int i=0; i<frames.size(); i+=1)
    {
        IFrame::ChangeQP(val);
        frames[i]->EncodeDecode();

        imshow("H264", *frames[i]->getImage());

        if(i<1000)
            imshow("raw", images[i]);

        if(waitKey(30) >= 0) break;
    }


    cap.release();

}



