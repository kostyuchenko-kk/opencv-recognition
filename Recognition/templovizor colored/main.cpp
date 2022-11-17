#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp" 
#include "opencv2\opencv.hpp"


using namespace std;
using namespace cv;

void on_low_h_thresh_trackbar(int, void *);//трекбары для отладки
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
	string window1 = "filtred";
	double sum;


	Scalar aimColor(0, 255, 21);//цвет мишени

	namedWindow(window, CV_WINDOW_FULLSCREEN);
	namedWindow(window1, CV_WINDOW_FULLSCREEN);



	/*createTrackbar("Low H", "original", &low_h, 179, on_low_h_thresh_trackbar);
	createTrackbar("High H", "original", &high_h, 179, on_high_h_thresh_trackbar);
	createTrackbar("Low S", "original", &low_s, 255, on_low_s_thresh_trackbar);
	createTrackbar("High S", "original", &high_s, 255, on_high_s_thresh_trackbar);
	createTrackbar("Low V", "original", &low_v, 255, on_low_v_thresh_trackbar);
	createTrackbar("High V", "original", &high_v, 255, on_high_v_thresh_trackbar);*/



	Mat img = imread("teplo.jpg"); //в проекте несколько примеров изображений, менять индекс "teplo1" etc.

	blur(img, img, Size(9,9));//блюр, шумы убрать
	
	cvtColor(img, img, CV_BGR2HSV);//в hsv
	

	
	Mat img1;//переменные там ниже будут участвовать
	Mat full;
	Mat red_up;

	/*while ((char)waitKey(1) != 32) //для трекбаров
	{
		inRange(img, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img1);
		imshow(window1, img1);
	}*/

	inRange(img, Scalar(0, 120, 0), Scalar(30, 255, 255), img1);

	inRange(img, Scalar(170, 0, 0), Scalar(179, 255, 255), red_up);
	addWeighted(img1, 1.0, red_up, 1.0, 0.0, full);
	//(в палитре hsv значение h(hue- оттенок) для красного цвета лежит в двух областях (от 0 до 30 и от 170 до 179),
	//поэтому красный цвет нужно находить дважды и потом складывать
	
	
	vector<vector<Point>> cnts;//контуры
	findContours(full, cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	

	cvtColor(img, img, CV_HSV2BGR);


	for (int i = 0; i < cnts.size(); i++) //отрисовка
	{
		
		if (contourArea(cnts[i], false)>=5)
		{
			polylines(img, cnts[i], false, 1, 2);
			Moments mnts = moments(cnts[i]);

			double m00 = mnts.m00;
			double m10 = mnts.m10;
			double m01 = mnts.m01;
			double xmass = m10 / m00;
			double ymass = m01 / m00;
			ellipse(img, Point(xmass, ymass), Size(3, 3), 0, 0, 360, aimColor, -1);
		}
		

	
	}


	imshow(window, img);
	imshow(window1, img1);


	waitKey(0);

	destroyAllWindows();

	return 0;


}

void on_low_h_thresh_trackbar(int, void *)
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