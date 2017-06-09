#include<opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <opencv/cxcore.hpp>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <wiringPi.h>
#include<softPwm.h>
#include"iostream"
using namespace cv;
using namespace std;
CvCapture* pCapture = NULL;
IplImage* rowImage = NULL;
int lp = 90;
int rp = 90;
bool mode = 0;
void car(bool left,int leftpower,bool right, int rightpower)
{
	digitalWrite(2,!left);
	digitalWrite(3,left);
	digitalWrite(4,right);
	digitalWrite(5,!right);
	softPwmWrite(0,leftpower);
	softPwmWrite(1,rightpower);
	digitalWrite(24,!left);
	digitalWrite(27,left);
	digitalWrite(25,!right);
	digitalWrite(28,right);
	softPwmWrite(22,leftpower);
	softPwmWrite(26,rightpower);
}
void carinit()
{
	wiringPiSetup();
	pinMode(0,OUTPUT);
	pinMode(1,OUTPUT);
	pinMode(2,OUTPUT);
	pinMode(3,OUTPUT);
	pinMode(4,OUTPUT);
	pinMode(5,OUTPUT);
	pinMode(22,OUTPUT);
	pinMode(26,OUTPUT);
	pinMode(24,OUTPUT);
	pinMode(27,OUTPUT);
	pinMode(25,OUTPUT);
	pinMode(28,OUTPUT);
	softPwmCreate(0,0,400);
	softPwmCreate(1,0,400);
	softPwmCreate(22,0,400);
	softPwmCreate(26,0,400);
}
void trace()
{
	int addnumber = 0;
	rowImage = cvQueryFrame( pCapture );
		cvShowImage("row",rowImage);
		IplImage* grey = cvCreateImage(cvGetSize(rowImage), IPL_DEPTH_8U, 1);
		cvCvtColor(rowImage,grey,CV_BGR2GRAY);
		cvSmooth(grey, grey, CV_MEDIAN, 5);
		cvNamedWindow("binary",CV_WINDOW_AUTOSIZE);
		cvThreshold(grey,grey, 70.0, 255.0, CV_THRESH_BINARY);
    {
        IplConvKernel *element = cvCreateStructuringElementEx(10, 10, 0, 0, CV_SHAPE_ELLIPSE);
        cvMorphologyEx(grey, grey, NULL, element, CV_MOP_OPEN);
        cvReleaseStructuringElement(&element);
    }
		cvShowImage("binary",grey);
		    {
        int color = 254;
        CvSize sz = cvGetSize(grey);
        int w, h;
        for (w = 0; w < sz.width; w++)
        {
            for (h = 0; h < sz.height; h++)
            {
                if (color > 0)
                {
                    if (CV_IMAGE_ELEM(grey, unsigned char, h, w) == 0)
                    {
                        cvFloodFill(grey, cvPoint(w, h), CV_RGB(color,color,color));
                        color--;
                    }
                }
            }
        }
        int colorsum[255] = {0};
        for (w = 0; w < sz.width; w++)
        {
            for (h=0; h<sz.height; h++)
            {
                if (CV_IMAGE_ELEM(grey, unsigned char, h, w) > 0)
                {
                    colorsum[CV_IMAGE_ELEM(grey, unsigned char, h, w)]++;
                }
            }
        }
        std::vector<int> v1(colorsum, colorsum + 255);
        int maxcolorsum = max_element(v1.begin(), v1.end()) - v1.begin();
		//cout<<"max = "<<maxcolorsum<<endl;
		int maxarea=0;
        for (w = 0; w < sz.width; w++)
        {
            for (h = 0; h < sz.height; h++)
            {
                if (CV_IMAGE_ELEM(grey, unsigned char, h, w) == maxcolorsum)
                {
                    CV_IMAGE_ELEM(grey, unsigned char, h, w) = 0;
					maxarea++;
                }
                else
                {
                    CV_IMAGE_ELEM(grey, unsigned char, h, w) = 255;
                }
            }
        }
		//cout<<"maxarea = "<<maxarea<<endl;
		/*int linecolor[sz.height]= {0};
		int linecolorright[sz.height]={sz.width};
		if(maxarea<60000&&maxarea>3000)
		{
			for (h = 0; h < sz.height; h++)
			{
				for (w = 0; w < sz.width; w++)
				{
					if (CV_IMAGE_ELEM(grey, unsigned char, h, w) == 255)
					{
						linecolor[h]++;
					}
					else
					{
						break;
					}
				}
				for (w = sz.width; w >= 0; w--)
				{
					if (CV_IMAGE_ELEM(grey, unsigned char, h, w) == 255)
					{
						linecolorright[h]--;
					}
					else
					{
						linecolor[h]= (linecolor[h]+linecolorright[h])/2;
						break;
					}
				}
				addnumber += (10+0.25*h)*linecolor[h];
			}
			addnumber=addnumber/9600;
			cout<<"position = "<<addnumber<<endl;
		}*/
		int linecolor[3]={0};
		int linecolorleft[3]= {0};
		int linecolorright[3]={sz.width,sz.width,sz.width};
		int hei[3]={20,100,210};
		for(h=0;h<3;h++)
		{
			cvLine(grey,cvPoint(0,hei[h]+2),cvPoint(320,hei[h]+2),cvScalarAll(2));
				for (w = 0; w < sz.width; w++)
				{
					if (CV_IMAGE_ELEM(grey, unsigned char,hei[h], w) == 255)
					{
						linecolorleft[h]++;
					}
					else
					{
						break;
					}
				}
				for (w = sz.width-1; w > 0; w--)
				{
					if (CV_IMAGE_ELEM(grey, unsigned char, hei[h], w) == 255)
					{
						linecolorright[h]--;
					}
					else
					{
						break;
					}
				}
				linecolor[h]= (linecolorleft[h]+linecolorright[h])/2-160;
		}
			cout<<"position="<<linecolor[0]<<"	"<<linecolor[1]<<"	 "<<linecolor[2]<<endl;
			char key=cvWaitKey(10);
			if(key == 23)
			{
				mode = !mode;
			}
			if(mode == 0){
				if(key == 119)
				{
					car(1,lp,1,rp);
				}
				else if(key == 115)
				{
					car(0,lp,0,rp);
				}
				else if(key == 100)
				{
					car(1,lp+50,0,rp);
				}
				else if(key == 97)
				{
					car(0,lp,1,rp+50);
				}
				else{
					car(0,0,0,0);
				}
			}
			if(mode == 1){
				int linenumber = 1;
				if(linecolor[linenumber]>-50&&linecolor[linenumber]<50)
					car(1,lp,1,rp);
				else if(linecolor[linenumber]>-80&&linecolor[linenumber]<=-50)
					car(1,lp-30,1,rp);
				else if(linecolor[linenumber]>=50&&linecolor[linenumber]<=80)
					car(1,lp,1,rp-30);
				else if(linecolor[linenumber]>-110&&linecolor[linenumber]<=-80)
					car(1,lp-50,1,rp);
				else if(linecolor[linenumber]>80&&linecolor[linenumber]<=110)
					car(1,lp,1,rp-50);
				else if(linecolor[linenumber]>-200&&linecolor[linenumber]<=-110)
					car(0,lp-40,1,rp-10);
				else if(linecolor[linenumber]>110&&linecolor[linenumber]<=200)
					car(1,lp-10,0,rp-40);
				else
					car(0,0,0,0);
				/*addnumber += (10+0.25*h)*linecolor[h];
				addnumber=addnumber/9600;
				cout<<"position = "<<addnumber<<endl;*/
			}
        cvNamedWindow("gray");
        cvShowImage("gray", grey);
			}
	//return addnumber;
}
int main()
{
	pCapture = cvCreateCameraCapture(0);
	cvSetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(pCapture, CV_CAP_PROP_FRAME_HEIGHT, 240);
	cvNamedWindow("row",CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("control",CV_WINDOW_AUTOSIZE);
	carinit();
	while(1)
	{
		trace();
		char c = cvWaitKey(5);
		if(c == 27)
		{
			car(0,0,0,0);
			break;
		}
	}
	car(0,0,0,0);
	cvReleaseCapture(&pCapture);
	cvDestroyWindow("row");
	return 0;
}
