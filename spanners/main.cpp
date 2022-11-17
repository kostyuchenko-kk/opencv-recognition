#include "opencv2\imgproc.hpp"
#include "opencv2\highgui.hpp" 
#include "opencv2\opencv.hpp"


using namespace std;
using namespace cv;


int main()
{
	string window = "original";

	Scalar right_gk(0, 255, 0);//цвет обводки ключа
	Scalar wrong_gk(0, 0, 255);



	namedWindow(window, CV_WINDOW_FULLSCREEN);



	Mat img = imread("gk.jpg");
	Mat img2 = imread("gk_tmplt.jpg");

	cvtColor(img, img, CV_BGR2GRAY);
	cvtColor(img2, img2, CV_BGR2GRAY);
	
	Mat img1;
	Mat img3;
	

	threshold(img,img1,245,255,THRESH_BINARY_INV);
	threshold(img2, img3, 245, 255, THRESH_BINARY);
	
	
	vector<vector<Point>> cnts;
	vector<vector<Point>> cnts_gk;
	

	findContours(img1, cnts, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	findContours(img3, cnts_gk, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	
	
	for (int i = 0; i < cnts.size(); i++)//чем matchShapes значение этой функции, тем более похожи ключи
	{
		double diff = matchShapes(cnts[i], cnts_gk[0], ShapeMatchModes::CV_CONTOURS_MATCH_I2, 0);
		cout << diff << endl;
	}

	cvtColor(img, img, CV_GRAY2BGR);
	
	for (int i = 0; i < cnts.size(); i++)//обводка с порогом по форме
	{
		if (contourArea(cnts[i], false)>128 & matchShapes(cnts[i], cnts_gk[0], ShapeMatchModes::CV_CONTOURS_MATCH_I2, 0)<0.2)
		{
			polylines(img, cnts[i], false, right_gk, 3);
		}

		if (contourArea(cnts[i], false)>128 & matchShapes(cnts[i], cnts_gk[0], ShapeMatchModes::CV_CONTOURS_MATCH_I2, 0)>0.2)// а если значение ф-ии матшейпс больше 0.2(ключи разные) то обводим эти контура красным цветом.
		{
			polylines(img, cnts[i], false, wrong_gk, 3);
		}

	}
	
	
	
	imshow(window, img);

	waitKey(0);

	destroyAllWindows();

	return 0;


}
