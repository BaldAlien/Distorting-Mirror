#include<opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<iostream>
#include<conio.h>
#include<fstream>

using namespace cv;
using namespace std;

Mat ConvexMirror(Mat img);
Mat ConcaveMirror(Mat img);
void ModeSelect();
void SetDroidCamUrl();

VideoCapture cap;
string camUrl;


Mat ConvexMirror(Mat img) {						//凸面镜（放大）
	Mat srcImage;
	img.copyTo(srcImage);

	int radius = 250;							//定义凸面镜半径
	int height = img.rows;
	int width = img.cols;
	Point2d center;
	center.x = height / 1.5;
	center.y = width / 2.5;						//凸面镜中心点
	int chns = img.channels();					//获取原图片的通道数

	int newX, newY;
	int real_radius = int(radius / 2.0);

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			int tX = (int)x - center.x;
			int tY = (int)y - center.y;

			int distance = tX * tX + tY * tY;	//与凸面镜中心点距离
			if (distance < radius*radius) {
				newX = int(tX / 2.0);
				newY = int(tY / 2.0);

				newX = int(newX * (sqrt(distance) / real_radius));
				newY = int(newY * (sqrt(distance) / real_radius));

				newX = int(newX + center.x);
				newY = int(newY + center.y);	//算出映射点的坐标
				if (newX < width && newY < height) {
					for (int c = 0; c < chns; c++)
        			{
						srcImage.at<Vec3b>(y, x)[c] = img.at<Vec3b>(newY, newX)[c];	//映射
        			}
				}
			}
			else {
				for (int c = 0; c < chns; c++)
        		{
					srcImage.at<Vec3b>(y, x)[c] = img.at<Vec3b>(y, x)[c];			//映射
        		}
			}
		}
	return srcImage;
}

Mat ConcaveMirror(Mat img) {					//凹面镜（挤压）
	Mat srcImage;
	img.copyTo(srcImage);
	int radius = 250;							//定义凹面镜半径

	int height = img.rows;
	int width = img.cols;
	Point2d center;
	center.x = height / 1.5;
	center.y = width / 2.5;						//凹面镜中心点
	int chns = img.channels();					//获取原图片的通道数

	int newX, newY;

	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			double tX = x - center.x;
			double tY = y - center.y;

			double theta = atan2(tY, tX);		//算出原点至中点（处理点与凹面镜中心点的中点）的方位角
			radius = sqrt((tX * tX) + (tY * tY));

			int newR = sqrt(radius) * 6;
			newX = int(center.x + (newR * cos(theta)));
			newY = int(center.y + (newR * sin(theta)));		//算出映射点的坐标

			if (newX<0 && newX>width)
				newX = 0;
			if (newY<0 && newY>height)
				newY = 0;

			if (newX < width && newY < height) {
				for (int c = 0; c < chns; c++)
        		{
					srcImage.at<Vec3b>(y, x)[c] = img.at<Vec3b>(newY, newX * 3 + c)[c];
				}
			}
			else {
				for (int c = 0; c < chns; c++)
        		{
					srcImage.at<Vec3b>(y, x)[c] = img.at<Vec3b>(y, x)[c];	//映射
        		}
			}
		}
	return srcImage;
}

Mat DistortMirror(Mat img){						//扭曲镜

	Mat srcImage;
	img.copyTo(srcImage);

	int height = img.rows;
	int width = img.cols;
	int degree = 80;							//扭曲深度
	int pi = 3.14;								//定义Π
	int T = 3;									//周期
	int X;
	int chns = img.channels();					//获取原图片的通道数

	for (int j = 0; j < width; j++)
	{
		double temp =  degree * sin(1.0 * j / width  *  pi * T );
		temp = degree + temp;
		for (int i = int(temp + 0.5); i < height + temp - 2 * degree; i++)
		{
			X = (int)((i - temp) *  (height) / (height - degree));
			if (X >= img.rows)
				X = img.rows - 1;
			if (X < 0)
				X = 0;								//算出映射点的坐标

			for (int c = 0; c < chns; c++)
			{
				srcImage.at<Vec3b>(i, j)[c] = img.at<Vec3b>(X, j)[c];	//映射
			}
		}
	}
	return srcImage;
}

void ModeSelect()
{
	for (;;)
	{
		int mode;
		bool esc = false;
		cout << "mode 1:ConvexMirror\n" << "mode 2:ConcaveMirror\n"<<"mode 3:DistortMirror\n";
		mode = getch();

		Mat img;
		system("cls");
		switch (mode)
		{
		case (int)'1':
			for (;;) {
				cap.read(img);
				img = ConvexMirror(img);
				imshow("Image", img);

				if (waitKey(1) == 27) {
					destroyWindow("Image");
					break;
				}
			}
			break;
		case (int)'2':
			for (;;) {
				cap.read(img);
				img = ConcaveMirror(img);
				imshow("Image", img);

				if (waitKey(1) == 27) {
					destroyWindow("Image");
					break;
				}
			}
			break;
		case (int)'3':
			for (;;) {
				cap.read(img);
				img = DistortMirror(img);
				imshow("Image", img);

				if (waitKey(1) == 27) {
					destroyWindow("Image");
					break;
				}
			}
			break;
		case 27: esc = true; break;
		default:
			break;
		}
		if (esc) {
			system("cls");
			break;
		}
	}
}

void SetDroidCamUrl()
{
	string url;
	cin >> url;
	camUrl = url;

	ofstream OutFile("CamUrl.txt");
	OutFile << camUrl;
	OutFile.close();
}

int main()
{
	ifstream readFile("CamUrl.txt");	//读取手机live地址
	readFile >> camUrl;
	readFile.close();

	for (;;)
	{
		int Camera;
		bool esc = false;
		cout << "Camera 1:ComputerCam\n" << "Camera 2:DroidCam\n" << "Select 3:Set DroidCam url\n";
		Camera = getch();

		switch (Camera)																//选择摄像头，然后自动进入模式选择函数
		{
			case (int)'1': cap.open(1); system("cls"); ModeSelect(); break;			//打开电脑默认摄像头
			case (int)'2': cap.open(camUrl); system("cls"); ModeSelect(); break;	//打开手机live
			case (int)'3': system("cls"); SetDroidCamUrl(); break;					//修改手机live地址
			case 27: esc = true; break;
			default:
				break;
		}
		system("cls");

		if (esc)
			break;
	}
	return 0;
}
