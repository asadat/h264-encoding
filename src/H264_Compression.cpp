#include "H264_Compression.h"

H264_Compression::H264_Compression(const char* file)
{
    Init(file);
}

H264_Compression::~H264_Compression()
{

}

void H264_Compression::Init(const char* file)
{
    cap.open(file);

    if(!cap.isOpened())
        return;

    namedWindow("H264",1);

    Mat frame;
    while(cap.read(frame))
    {
        imshow("H264", frame);
        if(waitKey(30) >= 0) break;
    }


    cap.release();
}



