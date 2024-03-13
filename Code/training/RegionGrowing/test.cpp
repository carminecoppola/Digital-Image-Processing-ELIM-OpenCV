#include<iostream>
#include<opencv2/opencv.hpp>
#include <stack>


using namespace std;
using namespace cv;

int maxRegion = 50;
float minAreaFactor = 0.001;
int th = 30;

const Point pointShifting[8] = {

    Point(-1,-1),Point(-1, 0),Point(-1, 1),
    Point( 0,-1),             Point( 0, 1),
    Point( 1,-1),Point( 1, 0),Point( 1, 1),
};


void grow(const Mat& src, Mat& dst,  Mat& mask, Point seed){

    stack<Point> pointStack;
    pointStack.push(seed);

    while(!pointStack.empty()){

        Point center = pointStack.top();
        mask.at<uchar>(center) = 1;
        pointStack.pop();

        for (int i = 0; i < 8; i++)
        {
            Point estimatingPoint = center + pointShifting[i];


            if (estimatingPoint.x < 0 && estimatingPoint.y < 0 && estimatingPoint.x > src.cols - 1 && estimatingPoint.y > src.rows - 1)
            {
                continue;
            }
            else{
                int delta = (int) sqrt(pow(src.at<uchar>(seed) - src.at<uchar>(estimatingPoint), 2));

                if (dst.at<uchar>(estimatingPoint) == 0 && mask.at<uchar>(estimatingPoint) == 0 && delta < th)
                {
                    pointStack.push(estimatingPoint);
                }
            }
        }
    }

}


void myRG(const Mat& src, Mat& dst){

    int maxRegionA = (int)(src.rows * src.cols * minAreaFactor);
    dst = Mat::zeros(src.size(),CV_8UC1);
    Mat mask = Mat::zeros(src.size(),CV_8UC1);

    uchar label = 1;

    for (int i = 0; i < src.cols; i++)
    {
        for (int j = 0; j < src.rows; j++)
        {
            if (dst.at<uchar>(Point(i,j)) == 0)
            {
                grow(src, dst, mask, Point(i,j));

                int areaMask = (int) sum(mask).val[0];

                if (areaMask > maxRegionA)
                {
                    dst = dst + mask * label;

                    imshow("mask", mask*255);
                    waitKey(0);


                    if (++label > maxRegion)
                    {
                        cout<<"out of region"<<endl;
                    }
                }
                else{
                    dst = dst + mask * 255;
                }
                mask -= mask;
            }
        }
    }

}

int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst;

    if (src.empty())
        return -1;

    imshow("Original",src);
    waitKey(0);

    myRG(src,dst);
    imshow("RG",dst);
    waitKey(0);


    return 0;
}

