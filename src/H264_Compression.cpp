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
    namedWindow("diff", CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);


     cv::createTrackbar("QP:", "H264", &val, 40);
   //cv::createButton("button1", H264_Compression::playBtn,NULL);


    int n=0;
    Mat image;
    while(cap.read(image) && n < 100)
    {
        assert(image.rows < 1500 && image.cols < 1500);

        image.copyTo(images[n]);
        n++;

    }

    cap.release();

    Frame::ChangeQP(val);

    printf("%d frames.\n", n);

    for(int i=0; i<n; i+=10)
    {
        Frame *frame[10];
        frame[0] = new IFrame();
        frame[0]->SetImage(images[i+0]);
        frame[0]->EncodeDecode();;

        frame[9] = new IFrame();
        frame[9]->SetImage(images[i+9]);
        frame[9]->EncodeDecode();;

        frame[3] = new PFrame();
        frame[3]->SetImage(images[i+3]);
        ((PFrame*)frame[3])->refFrame = frame[0];
        frame[3]->EncodeDecode();;

        frame[6] = new PFrame();
        frame[6]->SetImage(images[i+6]);
        ((PFrame*)frame[6])->refFrame = frame[3];
        frame[6]->EncodeDecode();;

        frame[1] = new BFrame();
        frame[1]->SetImage(images[i+1]);
        ((BFrame*)frame[1])->refFrame1 = frame[0];
        ((BFrame*)frame[1])->refFrame2 = frame[3];
        frame[1]->EncodeDecode();;

        frame[2] = new BFrame();
        frame[2]->SetImage(images[i+2]);
        ((BFrame*)frame[2])->refFrame1 = frame[0];
        ((BFrame*)frame[2])->refFrame2 = frame[3];
        frame[2]->EncodeDecode();;

        frame[4] = new BFrame();
        frame[4]->SetImage(images[i+4]);
        ((BFrame*)frame[4])->refFrame1 = frame[3];
        ((BFrame*)frame[4])->refFrame2 = frame[6];
        frame[4]->EncodeDecode();;

        frame[5] = new BFrame();
        frame[5]->SetImage(images[i+5]);
        ((BFrame*)frame[5])->refFrame1 = frame[3];
        ((BFrame*)frame[5])->refFrame2 = frame[6];
        frame[5]->EncodeDecode();;

        frame[7] = new BFrame();
        frame[7]->SetImage(images[i+7]);
        ((BFrame*)frame[7])->refFrame1 = frame[6];
        ((BFrame*)frame[7])->refFrame2 = frame[9];
        frame[7]->EncodeDecode();;

        frame[8] = new BFrame();
        frame[8]->SetImage(images[i+8]);
        ((BFrame*)frame[8])->refFrame1 = frame[6];
        ((BFrame*)frame[8])->refFrame2 = frame[9];
        frame[8]->EncodeDecode();;

        //for(int j=0; j<10; j++)
        //    frames.push_back(frame[j]);

        for(int ii=0; ii<10; ii++)
        {

            printf("\t\t\t%d out of %d \n", i+ii, n);

            imshow("H264", frame[ii]->img);
            imshow("raw", images[i+ii]);
            Mat diff;
            cv::absdiff(frame[ii]->img.rowRange(0,images[i+ii].rows).colRange(0,images[i+ii].cols), images[i+ii], diff);
            imshow("diff", diff);
            if(waitKey(1000) >= 0) break;
        }
    }





    printf("bye\n");
}



