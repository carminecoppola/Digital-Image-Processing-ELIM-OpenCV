#include <opencv4/opencv2/opencv.hpp>
#include <stdlib.h>
#include <vector>

using namespace cv;
using namespace std;

int cth1 = 20, cth2 = 24;

vector<double> createNormalizedHist(const Mat src){

	Mat imgBlur, imgCanny;
	GaussianBlur(src, imgBlur, Size(5,5), 0, 0);
	Canny(imgBlur, imgCanny, cth1, cth2);
	imshow("canny", imgCanny);

	int nxm = 0; 

	vector<double> normalizedHist(256, 0.0f);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if(imgCanny.at<uchar>(i,j) == 255){
				normalizedHist.at(src.at<uchar>(i,j))++;
				nxm ++;
			}
		}	
	}

	for(auto& prob : normalizedHist)
		prob /= nxm;

	return normalizedHist;
}


vector<int> otsuEdge(const Mat src){

	vector<double> normalizedHist = createNormalizedHist(src);
	vector<int> T (3, 0);

	vector<double> cumSum(3, 0.0f), cumAvg(3, 0.0f);
	double mg = 0.0f, iv = 0.0f, interclassVariance = 0.0f;

	cumSum.at(0) = normalizedHist.at(0);
	cumAvg.at(0) = normalizedHist.at(0);

	mg = normalizedHist.at(0);


	for (int i = 1; i < 256; i++)
		mg += i * normalizedHist.at(i);


	for (int i = 1; i < 256 - 2; i++)
	{
		cumSum.at(0) += normalizedHist.at(i);
		cumAvg.at(0) += i * normalizedHist.at(i);

		for (int j = i + 1; j < 256 ; j++)
		{
			cumSum.at(1) += normalizedHist.at(j);
			cumAvg.at(1) += j * normalizedHist.at(j);


			for (int k = j + 1; k < 256 ; k++)
			{
				cumSum.at(2) += normalizedHist.at(k);
				cumAvg.at(2) += k * normalizedHist.at(k);


				for (int z = 0; z < 3; z++)
					iv += cumSum.at(z) * pow( cumAvg.at(z) / cumSum.at(z) - mg , 2);

				if (iv > interclassVariance)
			  	{
			  		interclassVariance = iv;
			  		T.at(0) = i;
			  		T.at(1) = j;
			  	} 
			  	iv = 0.0f; 
			}

			cumSum.at(2) = cumAvg.at(2) = 0.0f;
		}

		cumSum.at(1) = cumAvg.at(1) = 0.0f;
	}


	return T;
}


void myTh(const Mat src,  Mat& dst, int th1, int th2){


	src.copyTo(dst);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i,j) >= th2)
			{
				dst.at<uchar>(i,j) = 255;
			}
			else if (src.at<uchar>(i,j) >= th1)
			{
				dst.at<uchar>(i,j) = 127;
			}
			else
				dst.at<uchar>(i,j) = 0;
			
		}
	}
}


int main( int argc, char** argv ) {
	Mat src = imread( argv[1], IMREAD_GRAYSCALE );
	Mat dst;

	if(src.empty()) 
		return -1;

	imshow("src", src);
	waitKey(0);
	
	vector<int> T = otsuEdge(src);
	myTh( src, dst, T.at(0), T.at(1) );

	
	imshow("dst", dst);
	waitKey(0);
	return 0;
}






