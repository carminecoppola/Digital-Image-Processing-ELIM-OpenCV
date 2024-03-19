#include<iostream>
#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;


int main(int argc, char** argv){

	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	//Mat src = imread(argv[1], IMREAD_COLOR);
    Mat dst;
	

	if(src.empty())
		return -1;

    imshow("Original", src);
    waitKey(0);


    myCanny(src, dst);
    imshow("Cany", dst);
    waitKey(0);


	return 0;
}