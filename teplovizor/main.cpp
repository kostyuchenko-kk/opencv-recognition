#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp" 
#include "opencv2\opencv.hpp" 

using namespace std;
using namespace cv;



int main()
{
	string window = "original";
	string window1 = "filtred";

	Scalar aimColor(0, 0, 255);// цвет мишени RGB

	namedWindow(window, CV_WINDOW_FULLSCREEN);
	namedWindow(window1, CV_WINDOW_FULLSCREEN);



	Mat img = imread("ig_1.jpg");
	
	cvtColor(img, img, CV_BGR2GRAY);//перевод в одноканальное
	
	Mat img1 = img.clone();


	threshold(img, img1, 220, 250, THRESH_BINARY);//пороговая фильтрация
		
	
	vector<vector<Point>> cnts;//вектор контуров
	findContours(img1, cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//находит контуры
	
	int Max = 0;// индекс самого большого контура
	for (int i = 0; i < cnts.size(); i++) // поиск самого большого по площади контура
	{
		if (contourArea(cnts[i], false) > contourArea(cnts[Max], false))
		{
			Max = i;
		}

	}
	
	
	polylines(img, cnts, false,1,2);// обводка
	Moments mnts = moments(cnts[Max]);//центр масс
	
	double m00 = mnts.m00;
	double m10 = mnts.m10;
	double m01 = mnts.m01;
	double xmass = m10 / m00;
	double ymass = m01 / m00;

	ellipse(img, Point(xmass, ymass), Size(2,2), 0, 0, 360, aimColor, -1);//в центре масс эллипс

	cvtColor(img, img, CV_GRAY2BGR);//обратно в RGB

	
	imshow(window, img);
	imshow(window1, img1);


	waitKey(0);

	destroyAllWindows();

	return 0;


}