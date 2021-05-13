#include <cv.h>
#include <highgui.h>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#include <wiringPi.h>

#include <pthread.h>

#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>

using namespace std;
using namespace cv;


#define comPortName "/dev/ttyUSB0"
#define comPortSpeed B115200

#define width  320
#define height 240

#define right_black 185
#define left_black  200

int F_ID = -1;
int res;

int Xc = 0,
    Yc = 0;

int dir = 5;

bool blackline = false;

int  openPort(const char *COM_name, speed_t speed)
{
	F_ID = open(COM_name, O_RDWR | O_NOCTTY);
	if (F_ID == -1)
	{
		char *errmsg = strerror(errno);
		//printf("%s\n", errmsg);
		return 0;
	}
	struct termios options;
	tcgetattr(F_ID, &options);
	cfsetispeed(&options, speed);
	cfsetospeed(&options, speed);
	options.c_cc[VTIME] = 20;
	options.c_cc[VMIN] = 0;
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CS8;
	options.c_lflag = 0;
	options.c_oflag &= ~OPOST;
	tcsetattr(F_ID, TCSANOW, &options);
	return 1;
}

int readData(unsigned char *buff, int size)
{
	int n = read(F_ID, buff, size);
	if(n == -1)
	{
		char *errmsg = strerror(errno);
		//printf("%s\n", errmsg);
	}
	return 0;
}

int sendData(unsigned char* buff, int len)
{
	int n = write(F_ID, buff, len);
	if(n == -1)
	{
		char *errmsg = strerror(errno);
		//printf("%s\n", errmsg);
	}
	return n;
}

void closeCom(void)
{
	close(F_ID);
	F_ID = -1;
	return;
}

void setRTS()
{
	int status;
	ioctl(F_ID, TIOCMGET, &status);
	status |= TIOCM_RTS;
	ioctl(F_ID, TIOCMSET, &status);
}

void clrRTS()
{
	int status;
	ioctl(F_ID, TIOCMGET, &status);
	status &= ~TIOCM_RTS;
	ioctl(F_ID, TIOCMSET, &status);
}

void* Cam(void* data)
{
	CvMoments M;
	Mat m;
	double m00, m01, m10;
	int H_min[] = {0,164,188},
	    H_max[] = {11,256,256};

	CvCapture* capture = cvCaptureFromCAM(CV_CAP_ANY);
	assert(capture);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,height);
	IplImage* image = cvQueryFrame(capture);

	IplImage *hsv = 0,
		 *dst = 0,
		 tmp;

	image = cvQueryFrame(capture);

	hsv = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
	dst = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 1 );

	cvNamedWindow("original",CV_WINDOW_AUTOSIZE);
	cvNamedWindow("hsv",CV_WINDOW_AUTOSIZE);

	while(true){
		image = cvQueryFrame(capture);
		m = cvarrToMat(image);
		medianBlur(m,m,7);
		tmp = m;
		cvCopy(&tmp,image);
		cvCvtColor(image, hsv, CV_BGR2HSV);
		cvInRangeS(hsv, cvScalar(H_min[0],H_min[1],H_min[2]), cvScalar(H_max[0],H_max[1],H_max[2]), dst);
		m = cvarrToMat(dst);
		medianBlur(m,m,7);
		tmp = m;
		cvCopy(&tmp,dst);
		cvMoments(dst, &M);
		m00 = cvGetSpatialMoment(&M,0,0);
		m10 = cvGetSpatialMoment(&M,1,0);
		m01 = cvGetSpatialMoment(&M,0,1);
		if(m00 > 100)
		{
			Xc = int(m10/m00);
			Yc = int(m01/m00);
			Yc = 240 - Yc;
			//printf("x-%d \t y-%d \n",Xc,Yc);
			cvCircle(image,cvPoint(Xc,240-Yc),5,CV_RGB(255,255,255),2,8);
		}
		else
		{
			Xc = 0;
			Yc = 0;
		}
		cvShowImage( "hsv", dst);
		cvShowImage("original",image);
		cvWaitKey(1);
        }

        // освобождаем ресурсы
	//cvReleaseImage(m);
	//cvReleaseImage(tmp);
	cvReleaseImage(&image);
	cvReleaseImage(&hsv);
	cvReleaseImage(&dst);
        // удаляем окна
        cvDestroyAllWindows();
}

void* comPort(void* data)
{
	unsigned char send[1] = {};
	unsigned char read[2] = {};
	int left = 0,
	    right = 0;
	while(1)
	{
		send[0] = (char) dir;
		res = openPort(comPortName, comPortSpeed);
		sendData(send, 1);
		readData(read, 2);
		closeCom();
		left = (int)read[0];
		right = (int)read[1];
		//printf("left - %d \t right - %d \t balckline - %d \n", left, right, blackline);
		if(right < right_black || left < left_black)
		{
			blackline = true;
		}
	}
}

void* movement(void* data)
{
	int dif = 20;
	while(1)
	{
		printf("x-%d \t y-%d \n",Xc,Yc);
		if(blackline)
		{
			switch(dir)
			{
				case 8:
					dir = 2;
				break;
				case 6:
					dir = 4;
				break;
				case 2:
					dir = 8;
				break;
				case 4:
					dir = 6;
				break;
			}
			//delay(200);
			blackline = false;
		}
		else if(!blackline && Yc == 0 && Xc == 0)
		{
			dir = 5;
		}
		else if(!blackline && Xc > 90 + dif)
		{
			dir = 6;
		}
		else if(!blackline && Xc < 90 - dif)
		{
			dir = 4;
		}
		else if (!blackline)
		{
			dir = 8;
		}
	}
}

int main(int argc, char* argv[])
{

	//wiringPiSetup();
	pthread_t pth_Cam;
	pthread_t pth_movement;
	pthread_t pth_comPort;
	void* data = NULL;
	pthread_create(&pth_movement,NULL,movement,data);
	pthread_create(&pth_comPort,NULL,comPort,data);
	pthread_create(&pth_Cam,NULL,Cam,data);
	pthread_join(pth_Cam,NULL);
	pthread_join(pth_comPort,NULL);
	pthread_join(pth_movement,NULL);
	while(1)
	{

	}

        return 0;
}
