 #include <iostream>
 #include <fstream>
 #include <windows.h>
 #include "opencv2/core/core.hpp"
 #include "opencv2/objdetect/objdetect.hpp"
 #include "opencv2/highgui/highgui.hpp"
 #include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

 const int   projectorWidth= 1920,
            projectorHeight= 1080,   

                  cameraNum= 0,      
               hitCameraNum= 1,      

                   screenUp= 50,
                 screenDown= 260,
                 screenLeft= 80,
                screenRight= 450,    

                  ballColor= 1,     
                 ballDifNum= 10,    
                  ballThres= 8,
                    ballTtl= 5,     

             motionTreshold= 80,     
               motionDifNum= 10,     
                                     
                   startTtl= 30,     
                detectDelay= 4;      

const string pointsFileName= "C:/points.txt";   

int main()
{
    Mat frame,      
        frameHSV,
        frameBitmap,

        frameCurrent,   
        framePriv,      

        mask;           

    vector<Mat> imgs;   
    Mat timg;

    int ttl=startTtl,ttl2=0,ttl3=0,explTtl=0;

    ofstream out(pointsFileName);

    int x=0,y=0,px=0,py=0,n,hx=0,hy=0;

    short nx,ny;

    int clr;
    char key;

    CvCapture* capture = cvCaptureFromCAM(cameraNum);
    assert(capture);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);

    CvCapture* captureHit = cvCaptureFromCAM(hitCameraNum);
    assert(captureHit);
    cvSetCaptureProperty(captureHit, CV_CAP_PROP_FRAME_WIDTH, 640);
    cvSetCaptureProperty(captureHit, CV_CAP_PROP_FRAME_HEIGHT, 480);

//    cvNamedWindow("win", CV_WINDOW_AUTOSIZE);  

        frame=cvQueryFrame(captureHit);
        cvtColor(frame, frame, CV_RGB2GRAY );   // èç öâåòíîãî â ñåðîå
        GaussianBlur(frame,framePriv,cv::Size( 3, 3 ), -1);  // óáèðàåì øóìû

    while(true)
    {
        frame=cvQueryFrame(captureHit);
        if (ttl<=0)
        {
            cvtColor(frame, frame, CV_RGB2GRAY );   
            GaussianBlur(frame,frameCurrent,cv::Size( 3, 3 ), -1);  
            absdiff(frameCurrent,framePriv,mask);  
            framePriv=frameCurrent.clone(); 
            threshold(mask,mask,motionTreshold,255,cv::THRESH_BINARY);
            if ((countNonZero(mask)>motionDifNum) && (ttl2<ballTtl) && (nx!=0) && (ny!=0))
            {
                 ttl3=detectDelay;
                 ttl=startTtl;
            }
        } else
        {
            ttl--;
        }

        if (ttl3>0) ttl3--;

        if (ttl3==1)
        {// Ïîïàäàíèå

                 hx=nx;
                 hy=ny;
                 cout << "Hit:" << nx << ":" << ny << "\n";
                 out << "Hit:" << nx << ":" << ny << "\n";
                 out.flush();
                 PlaySound("C:\\sounds\\02.wav", NULL, SND_ASYNC);
                 explTtl=7;
        }

 //       imshow("win", frame);

//        cvWaitKey(33);

        clr=ballColor;
        frame=cvQueryFrame(capture);
        cvtColor(frame,frameHSV,CV_BGR2HSV);

        inRange(frameHSV,Scalar(clr-ballThres,120,120),Scalar(clr+ballThres,255,255),frameBitmap);
        medianBlur(frameBitmap,frameBitmap,5);

//        imshow("win", frameBitmap);

        x=y=n=0;

        char cl;

        for(int i = screenLeft; i < screenRight; i++)
        {
            for(int j = screenUp; j < screenDown; j++)
            {
                cl=frameBitmap.at<char>(j,i);
                if (cl!=0)
                {
                    x+=i;
                    y+=j;
                    n++;
                }
            }
        }

        if (n>ballDifNum)
        {
            x/=n;
            y/=n;
            px=x;
            py=y;
            ttl2=0;
        } else
        {
            ttl2++;
            x=px;
            y=py;
        }

  //      if (ttl2<ballTtl) cout << x << ":" << y << "\n";
  //                        else cout << "nothing\n";

        if (ttl2>10) { nx=0;ny=0;x=0;y=0;}
        cout << x << ":" << y << "\n";
        out << nx << ":" << ny << "\n";
        out.flush();

       Mat frameResult=Mat::zeros(Size(projectorWidth,projectorHeight), CV_8UC3);

       nx=screenLeft-x;
       nx*=1.0*projectorWidth/(screenLeft-screenRight);

       ny=screenDown-y;
       ny*=1.0*projectorHeight/(screenDown-screenUp);

       timg=imgs[0].clone();
//       drawImage(frameResult,timg,tx-timg.cols/2,projectorHeight-ty-timg.rows/2);

//       circle(frameResult,Point(tx,projectorHeight-ty),10,cvScalar(0,0,255));

/*
       if (explTtl>0)
        {
            timg=imgs[9-explTtl].clone();
            drawImage(frameResult,timg,hx-timg.cols/2,pro
			jectorHeight-hy-timg.rows/2);
            explTtl--;
        }
*/
       circle(frameResult,Point(nx,projectorHeight-ny),70,cvScalar(0,0,255));

       circle(frameResult,Point(hx,projectorHeight-hy),70,cvScalar(0,255,0));

       imshow("win", frameResult);

       cvWaitKey(1);

    }
    out.close();
    return 0;
}
