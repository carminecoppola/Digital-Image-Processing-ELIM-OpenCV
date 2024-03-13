#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int th = 180;

void myHarris(const Mat& src, Mat& dst){

    
    Mat dx, dy;
    Sobel(src, dx, CV_32FC1, 1, 0);
    Sobel(src, dy, CV_32FC1, 0, 1);

    Mat dx2, dy2, dxy;
    pow(dx,2,dx2);
    pow(dy,2,dy2);
    multiply(dx,dy,dxy);

    Mat C00, C11, C10, C01;
    GaussianBlur(dx2, C00, Size(3,3), 0, 0);
    GaussianBlur(dy2, C11, Size(3,3), 0, 0);
    GaussianBlur(dxy, C10, Size(3,3), 0, 0);
    C10.copyTo(C01);

    Mat C1, C2;
    multiply(C00, C11, C1);
    multiply(C10, C01, C2);

    Mat R, traccia, det;
    double k = 0.05;


    traccia = C00 + C11;
    det = C1 - C2;
    pow(traccia, 2, traccia);
    traccia = k * traccia;
    R = det - traccia;

    normalize(R, dst, 0, 255, NORM_MINMAX);

    Mat dst_scale;
    convertScaleAbs(dst, dst_scale);

    for (int i = 0; i < dst.rows; i++)
    {
        for (int j = 0; j < dst.cols; j++)
        {
            if (dst.at<float>(i,j) > th)
            {
                circle(dst_scale, Point(j,i), 5, Scalar(0,0,255), 2, 8, 0);
            }
        }
    }

    dst_scale.copyTo(dst);

}


int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst;

    if (src.empty())
        return -1;

    imshow("Original", src);
    waitKey(0);


    myHarris(src, dst);
    imshow("Harris", dst);
    waitKey(0);


    return 0;
}