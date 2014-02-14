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
    int n=0;
    while(cap.read(frames[n++]));
    n--;

    printf("%d frames.\n", n);

    for(int i=0; i<n; i++)
    {
        imshow("H264", frames[i]);
        if(waitKey(30) >= 0) break;
    }

    cap.release();

}



