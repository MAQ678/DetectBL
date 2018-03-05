#include<opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>  
#include <queue>
#include <vector>

using namespace  std;
using namespace  cv;



double rgb2v(int R, int G, int B)
{
	//cout <<"RGB: "<< R << " " << G << " " << B << endl;
	int M, m, c;
	M = max(R, max(G, B));
	return (double)M / 255.0;
}

void printIt(int i, int j, Mat img)
{
	cout << "I : " << i << " J: " << j << endl;
	Mat hsvImg;
	cvtColor(img, hsvImg, CV_BGR2HSV);

	//cout << setprecision(5) << endl;
	Vec3b pixel = img.at<Vec3b>(i, j);
	cout << (int)pixel[2] << " " << (int)pixel[1] << " " << (int)pixel[0] << endl;
	cout << "My function: " << rgb2v((int)pixel[2], (int)pixel[1], (int)pixel[0]) << endl;

	Vec3b p1 = hsvImg.at<Vec3b>(i, j);
	cout << "OpenCV: " << p1[0] << " " << p1[1] << " " << p1[2] << endl;

}


//float tM[] = {.10, .15, .20, .25, .3}

vector<pair<int, int> >blackPixels1;
float finalThreashold = 0.0;
bool vis[999][2025] = { 0 }, bfsVis[999][2025] = { 0 };

bool setTreasholdVvale(Mat& img)
{
	int count = 0, i, j;
	//uchar* grayDataPtr = grayImg.data;
	Vec3b pixel;
	long long totalNoOfPixels = img.rows * img.cols;
	double vValue, ratio;
	float threashold = 0.10;
	finalThreashold = 0.10;
	int loop = 7;
	while (loop--)
	{
		cout << threashold << " ";
		count = 0;
		blackPixels1.clear();
		for (i = 0; i < img.rows; i++)
		{
			for (j = 0; j < img.cols; j++)
			{
				pixel = img.at<Vec3b>(i, j);
				vValue = rgb2v((int)pixel[2], (int)pixel[1], (int)pixel[0]);

				if (vValue <= threashold)
				{
					blackPixels1.push_back(make_pair(i, j));
					count++;
				}

			}
		}
		ratio = (count * 100.0) / totalNoOfPixels;
		cout << ratio << endl;
		finalThreashold = threashold;
		if (ratio >= 20.0)
		{
			//if (ratio >= 40.0)
			//{
			//	if (finalThreashold == 0.10)
			//		finalThreashold = threashold;
			break;
			//	}
		}
		if (fabs(ratio - 20.0) <= 2.0)
			threashold += 0.02;
		else
			threashold += 0.05;

	}
	return blackPixels1.size() ? 1 : 0;

}



int dx[] = { 0, -1, 0, 1 };
int dy[] = { -1, 0, 1, 0 };

bool isChecked(int x, int y, int &row, int &col)
{
	if (x >= 0 && y >= 0 && x<row && y<col)
	{
		///if (~(vis[x][y] | bfsVis[x][y])) // is not taken as black pixel
		if (!vis[x][y] && !bfsVis[x][y])
		{
			return true;
		}
	}
	return false;

}

bool isClosest(pair<int, int> &Pixel1, pair<int, int> Pixel2, Mat &img)
{
	/*
	const int hi = i*img.cols * 3 + j * 3;
	int rgb1, rgb2;
	for (int i = 1; i <= 3;i++)
	{
	rgb1 = Pixel1.first * img.cols *i + Pixel1.second*i;
	rgb2 = Pixel2.first * img.cols *i + Pixel2.second*i;

	if ()
	}
	*/
	Vec3b p1, p2;
	p1 = img.at<Vec3b>(Pixel1.first, Pixel1.second);
	p2 = img.at<Vec3b>(Pixel2.first, Pixel2.second);

	//cout << (int)p1[0] << " " << (int)p1[1] << " " << (int)p1[2] << "      and       "; cout << (int)p2[0] << " " << (int)p2[1] << " " << (int)p2[2] << endl;


	if (abs((int)p1[0] - (int)p2[0]) <= 3 && abs((int)p1[1] - (int)p2[1]) <= 3 && abs((int)p1[2] - (int)p2[2]) <= 3)
	{
		if (((int)p2[2] - (int)p2[1] <= 5) && ((int)p2[2] - (int)p2[0] <= 7))
			return true;
	}

	return false;
}

bool checkPixel(pair<int, int> &Pixel, Mat &img)
{
	int x, y;
	for (int i = 0; i < 4; i++)
	{
		x = Pixel.first + dx[i];
		y = Pixel.second + dy[i];
		if (isChecked(x, y, img.rows, img.cols) && isClosest(Pixel, { x, y }, img))
			return true;
	}
	return false;
}


Mat img2, imgTemp;

void changeImg2(pair<int, int> Pixel)
{
	/*
	*uchar* img2DataPtr = img2.data;
	int id;
	for (int i = 1; i <= 3; i++)
	{
	id = Pixel.first *img2.cols * i + Pixel.second * i;
	img2DataPtr[id] = (i == 3) ? 255 : 0;
	}
	*/

	Vec3b &color = img2.at<Vec3b>(Point(Pixel.second, Pixel.first));
	color[2] = 255;
	color[1] = 0;
	color[0] = 0;

}

void bfs(pair<int, int> &Pixel, Mat &img)
{
	queue<pair<int, int> >q;
	q.push(Pixel);
	bfsVis[Pixel.first][Pixel.second] = 1;

	pair<int, int> u;
	int i, x, y;
	int cou = 0;
	while (!q.empty())
	{
		u = q.front();
		q.pop();
		for (i = 0; i < 4; i++)
		{
			x = u.first + dx[i];
			y = u.second + dy[i];
			if (isChecked(x, y, img.rows, img.cols) && isClosest(u, { x, y }, img))
			{
				//	cout << cou++ << endl;
				bfsVis[x][y] = 1;
				q.push({ x, y });

				changeImg2({ x, y });
			}
		}
	}
}


void changeImgTemp(pair<int, int> Pixel)
{
	Vec3b &color = imgTemp.at<Vec3b>(Point(Pixel.second, Pixel.first));
	color[2] = 255;
	color[1] = 0;
	color[0] = 0;
}


void DoBfs(Mat &img)
{
	imgTemp = img.clone();

	for (pair<int, int> Pixel : blackPixels1)
	{
		vis[Pixel.first][Pixel.second] = 1;

		changeImg2(Pixel);

		changeImgTemp(Pixel);

	}
	imshow("v value", imgTemp);
	imshow("RGB + v value", img2);

	for (pair<int, int> Pixel : blackPixels1)
	{
		if (checkPixel(Pixel, img)) //Now use flood fill
		{
			cout << "Now BFS on: " << Pixel.first << " " << Pixel.second << endl;
			bfs(Pixel, img);
		}
	}
}


bool checkForRGBthreashold(Vec3b color)
{
	int mn = min((int)color[0], min((int)color[1], (int)color[2]));
	int mx = max((int)color[0], max((int)color[1], (int)color[2]));
	if (mx <= 90 && abs(mn - mx) <= 10)
		return true;
	return false;
}

void DoWithRGB(Mat &img)
{
	Mat img3 = img.clone();
	Vec3b pixel;
	int i, j;

	for (i = 0; i < img.rows; i++)
	{
		for (j = 0; j < img.cols; j++)
		{
			pixel = img3.at<Vec3b>(i, j);
			if (checkForRGBthreashold(pixel))
			{
				Vec3b &color = img2.at<Vec3b>(Point(j, i));
				color[2] = 255;
				color[1] = 0;
				color[0] = 0;
			}
		}
	}
	imshow("By using RGB", img2);
}

int main()
{


	int i = 100, j = 100;
	Mat img = imread("H:\\black test\\test1\\01.jpg"); // BGR format
	//cout << img.rows << " " << img.cols << endl;

	/*
	Mat  grayImg;
	cvtColor(img, grayImg, CV_BGR2GRAY);

	*/
	img2 = img.clone();
	DoWithRGB(img);





	//imshow("image1", img2);
	//cout << "Ok1" << endl;

	if (setTreasholdVvale(img))
	{
		//	cout << "Ok2" << endl;
		cout << blackPixels1.size() << endl;

		cout << finalThreashold << endl;

		//Now use bfs
		DoBfs(img);
		//getchar();
		imshow("Using bfs + v value", img2);
		//	cout << "Ok3" << endl;
	}
	else
		imshow("image", img);


	//return 0;

	//cout << count << endl;


	/*printIt(0, 0, img);
	printIt(768, 0, img);
	printIt(768, 1024, img);
	printIt(0, 1024, img);*/

	//getchar();
	/*
	Mat hsvImg, grayImg ;
	cvtColor(img, grayImg, CV_BGR2GRAY);
	cvtColor(img, hsvImg, CV_BGR2HSV);



	uchar* grayDataPtr = grayImg.data;
	uchar* hsvDataPtr = hsvImg.data;

	for (int i = 0; i < img.rows;i++)
	{
	for (int j = 0; j < img.cols; j++)
	{
	const int hi = i*img.cols * 3 + j * 3,
	gi = i*img.cols + j;
	//if (fabs(hsvDataPtr[hi + 2] - threashold) > 1e-4)
	grayDataPtr[gi] = hsvDataPtr[hi + 2];
	//else
	//grayDataPtr[gi] = 0.0;
	}
	}
	//namedWindow("image", WINDOW_NORMAL);
	//imshow("V-channel", grayImg);





	cout << hsvImg << endl;
	//namedWindow("image", WINDOW_NORMAL);
	//imshow("image", img);
	*/
	waitKey(0);
}
