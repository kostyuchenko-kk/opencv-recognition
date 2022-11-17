#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp" 
#include "opencv2\opencv.hpp"
#include <math.h>

#include <sstream>
#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>


using namespace std;
using namespace cv;

void on_low_h_thresh_trackbar(int, void *); //трекбары дл€ отладки
void on_high_h_thresh_trackbar(int, void *);
void on_low_s_thresh_trackbar(int, void *);
void on_high_s_thresh_trackbar(int, void *);
void on_low_v_thresh_trackbar(int, void *);
void on_high_v_thresh_trackbar(int, void *);

int low_h = 0, low_s = 0, low_v = 0;
int high_h = 179, high_s = 255, high_v = 255;


int main()
{
	string window = "original";
	string window1 = "before";
	string window2 = "after";

	Scalar lampColor(255, 0, 161);//цвет индикатора лампы
	Scalar redColor(0, 0, 255);//цвет каждого робота
	Scalar greenColor(0, 255, 0);
	Scalar blueColor(255, 0, 0);

	namedWindow(window, CV_WINDOW_FULLSCREEN);


	/*createTrackbar("Low H", "original", &low_h, 179, on_low_h_thresh_trackbar);
	createTrackbar("High H", "original", &high_h, 179, on_high_h_thresh_trackbar);
	createTrackbar("Low S", "original", &low_s, 255, on_low_s_thresh_trackbar);
	createTrackbar("High S", "original", &high_s, 255, on_high_s_thresh_trackbar);
	createTrackbar("Low V", "original", &low_v, 255, on_low_v_thresh_trackbar);
	createTrackbar("High V", "original", &high_v, 255, on_high_v_thresh_trackbar);*/



	Mat img = imread("roi_robotov_2.jpg");
	
	cvtColor(img, img, CV_BGR2HSV);
	
	//Mat img1;
	Mat red_low; //дл€ отфильтрованных по цветам
	Mat red_up;
	Mat red_full;
	Mat lamp;
	Mat thresh_green;
	Mat thresh_blue;

	/*while ((char)waitKey(1) != 32)
	{
		inRange(img, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img1);
		imshow(window1, img1);
	}*/
	inRange(img, Scalar(0, 48, 0), Scalar(9, 255, 255), red_low);
	inRange(img, Scalar(170, 0, 0), Scalar(179, 255, 250), red_up);
	addWeighted(red_low, 1.0, red_up, 1.0, 0.0, red_full);//красный

	inRange(img, Scalar(0, 0, 248), Scalar(179, 12, 255), lamp);//лампа
	
	inRange(img, Scalar(67, 33, 139), Scalar(80, 255, 255), thresh_green);//зеленый

	inRange(img, Scalar(82, 0, 0), Scalar(110, 255, 255), thresh_blue);//синий



	Rect rp = Rect(600, 0, 80, 720);//roi, чтобы определ€лись и под палкой на изображении 
	Mat roi = thresh_green(rp); 
	
	dilate(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(20, 20)));//расширение и сужение позвол€ют(от палки)
	erode(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));

	roi = thresh_blue(rp);
	
	dilate(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(16, 11)));// дл€ каждого цвета
	erode(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(16, 11)));

	roi = red_full(rp);

	dilate(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(20, 10)));
	erode(roi, roi, getStructuringElement(MORPH_ELLIPSE, Size(20, 10)));


	
	vector<vector<Point>> cnts_red;//контуры
	vector<vector<Point>> cnts_lamp;
	vector<vector<Point>> cnts_green;
	vector<vector<Point>> cnts_blue;

	findContours(red_full, cnts_red, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(lamp, cnts_lamp, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(thresh_green, cnts_green, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(thresh_blue, cnts_blue, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	

	cvtColor(img, img, CV_HSV2BGR);

	int Max_lamp = 0;

	for (int i = 0; i < cnts_lamp.size() ; i++)//сама€ больша€ лампа
	{
		if (contourArea(cnts_lamp[i], false)>contourArea(cnts_lamp[Max_lamp], false))
		{
			Max_lamp=i;
		}

	}

	for (int i = 0; i < cnts_red.size(); i++)//порог площади роботов с учетом плафона
	{
		if (contourArea(cnts_red[i], false)>70 && contourArea(cnts_red[i], false)<1600)
		{
			polylines(img, cnts_red[i], false, redColor,2);
		}
		
	}

	for (int i = 0; i < cnts_green.size(); i++)//порог зеленых
	{
		if (contourArea(cnts_green[i], false)>100)
		{
			polylines(img, cnts_green[i], false, greenColor, 2);
		}

	}

	for (int i = 0; i < cnts_blue.size(); i++)
	{
		if (contourArea(cnts_blue[i], false)>128)
		{
			polylines(img, cnts_blue[i], false, blueColor, 2);
		}

	}
	
	
	Moments mnts_lamp = moments(cnts_lamp[Max_lamp]);//центр масс лампы 

	double m00_lamp = mnts_lamp.m00;
	double m10_lamp = mnts_lamp.m10;
	double m01_lamp = mnts_lamp.m01;
	double xmass_lamp = m10_lamp / m00_lamp;
	double ymass_lamp = m01_lamp / m00_lamp;

	ellipse(img, Point(xmass_lamp, ymass_lamp), Size(3, 6), 0, 0, 360, lampColor, -1);


	double minimum_red = 0;
	Moments m_r_red;
	Point2f min_red;
	Point2f mc_lamp = Point2f(xmass_lamp, ymass_lamp);

	for (int i = 0; i < cnts_red.size(); i++)//минимальное рассто€ние между лампой и роботом
	{
		if (contourArea(cnts_red[i], false) < 1600 && contourArea(cnts_red[i], false)>70)
		{

			m_r_red = moments(cnts_red[i], false);
			Point2f mc_r_red;

			mc_r_red = Point2f(m_r_red.m10 / m_r_red.m00, m_r_red.m01 / m_r_red.m00);
			double dist = norm(mc_lamp - mc_r_red); 
			if (minimum_red == 0) minimum_red = dist;
			if (dist < minimum_red && minimum_red != 0)
			{
				minimum_red = dist;
				min_red = mc_r_red;

			}
		}
	
	}
	ellipse(img, min_red, Size(3, 6), 0, 0, 360, (0,0,0), -1);


	double minimum_green = 0;
	Moments m_r_green;
	Point2f min_green;
	for (int i = 0; i < cnts_green.size(); i++)
	{

		if (contourArea(cnts_green[i], false) < 1700 && contourArea(cnts_green[i], false)>70)
		{

			m_r_green = moments(cnts_green[i], false);
			Point2f mc_r_green;

			mc_r_green = Point2f(m_r_green.m10 / m_r_green.m00, m_r_green.m01 / m_r_green.m00);
			double dist = norm(mc_lamp - mc_r_green);
			if (minimum_green == 0) minimum_green = dist;
			if (dist < minimum_green && minimum_green != 0)
			{
				minimum_green = dist;
				min_green = mc_r_green;

			}
		}



	}
	ellipse(img, min_green, Size(3, 6), 0, 0, 360, (0, 0, 0), -1);


	double minimum_blue = 0;
	Moments m_r_blue;
	Point2f min_blue;
	for (int i = 0; i < cnts_blue.size(); i++)
	{


		if (contourArea(cnts_blue[i], false) < 1700 && contourArea(cnts_blue[i], false)>100)// и тут дл€ синего
		{

			m_r_blue = moments(cnts_blue[i], false);
			Point2f mc_r_blue;

			mc_r_blue = Point2f(m_r_blue.m10 / m_r_blue.m00, m_r_blue.m01 / m_r_blue.m00);
			double dist = norm(mc_lamp - mc_r_blue);
			if (minimum_blue == 0) minimum_blue = dist;
			if (dist < minimum_blue && minimum_blue != 0)
			{
				minimum_blue = dist;
				min_blue = mc_r_blue;

			}
		}



	}
	ellipse(img, min_blue, Size(3, 6), 0, 0, 360, (0, 0, 0), -1);


	imshow(window, img);

	waitKey(0);

	destroyAllWindows();

	return 0;


}

void on_low_h_thresh_trackbar(int, void *)// не используетс€
{
	low_h = min(high_h - 1, low_h);
	setTrackbarPos("Low H", "original", low_h);
}
void on_high_h_thresh_trackbar(int, void *)
{
	high_h = max(high_h, low_h + 1);
	setTrackbarPos("High H", "original", high_h);
}
void on_low_s_thresh_trackbar(int, void *)
{
	low_s = min(high_s - 1, low_s);
	setTrackbarPos("Low S", "original", low_s);
}
void on_high_s_thresh_trackbar(int, void *)
{
	high_s = max(high_s, low_s + 1);
	setTrackbarPos("High S", "original", high_s);
}
void on_low_v_thresh_trackbar(int, void *)
{
	low_v = min(high_v - 1, low_v);
	setTrackbarPos("Low V", "original", low_v);
}
void on_high_v_thresh_trackbar(int, void *)
{
	high_v = max(high_v, low_v + 1);
	setTrackbarPos("High V", "original", high_v);
}
