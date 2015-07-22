#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define bluemin Scalar(80,96,101)
#define bluemax Scalar(125,255,255)

#define redmin Scalar(166,154,100)
#define redmax Scalar(179,255,255)

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	VideoCapture cap(0); //capture the video from webcam
	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	int iLastX = -1;
	int iLastY = -1;

	int iLastXb = -1;
	int iLastYb = -1;

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp);

	//Create a black image with the size as the camera output
	Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);
	Mat board = Mat::zeros(imgTmp.size(), CV_8UC3);

	cout << imgTmp.size();

	while (true)
	{
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal); // read a new frame from video



		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		Mat imgHSV;

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;
		Mat imgAzul;

		//comprueba que la imagen se encuentre entre las escalas
		//inrange(imagen,scala1,scala2,imgdestino)
		inRange(imgHSV, redmin, redmax, imgThresholded); //Threshold the image
		inRange(imgHSV, bluemin, bluemax, imgAzul);

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);
		Moments Blue = moments(imgAzul);

		//m10,m01 centroides
		//m00 area
		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		double c2 = Blue.m01;
		double c1 = Blue.m10;
		double area = Blue.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		//size of image xD
		if (dArea > 20000 || area>20000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;
			int posBlueX = c1 / area;
			int posBlueY = c2 / area;

			if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
			{
				posX = 640 - posX;
				line(imgLines,Point(posX,posY),Point(iLastX,iLastY),Scalar(0,0,255),3);
			}
			if (iLastXb >= 0 && iLastYb >= 0 && posBlueX >= 0 && posBlueY >= 0)
			{
				posBlueX = 640 - posBlueX;
				line(imgLines, Point(posBlueX, posBlueY), Point(iLastXb, iLastYb), Scalar(255,0,0), 3);
			}
			iLastX = posX;
			iLastY = posY;
			iLastXb = posBlueX;
			iLastYb = posBlueY;

		}

		//imshow("Thresholded Image", imgThresholded); //show the thresholded image
		//imshow("Thresholded Image", imgAzul);

		board = board + imgLines;
		imshow("blabla", board);
		//
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}