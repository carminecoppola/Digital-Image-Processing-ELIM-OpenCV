#include <iostream>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

int Rmin = 20;
int Rmax = 50;
int th = 180;

void myHoughCircle(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src, blur, Size(5,5),0,0);

    Mat canny;
    Canny(blur, canny, 100,200);
    imshow("canny", canny);


    int size[] = {canny.rows, canny.cols, (Rmax - Rmin) + 1};

    Mat votes = Mat::zeros(3, size, CV_8UC1);

    double rad = CV_PI/180;

    for (int i = 0; i < canny.rows; i++)
    {
        for (int j = 0; j < canny.cols; j++)
        {
            if (canny.at<uchar>(i,j) == 255)
            {
                for (int r = Rmin; r < Rmax; r++)
                {
                    for (int t = 0; t < 360; t++)
                    {
                        
                        int a = cvRound(i - r * cos(t * rad));
                        int b = cvRound(j - r * sin(t * rad));

                        if (a >=0 && a < canny.rows && b >=0 && b < canny.cols)
                        {
                            votes.at<uchar>(a,b, Rmin - r)++;
                        }
                    }
                }
            }
        }
    }

    src.copyTo(dst);


    for (int r = Rmin; r < Rmax; r++)
    {
        for (int i = 0; i < canny.rows; i++)
        {
            for (int j = 0; j < canny.cols; j++)
            {
                if (votes.at<uchar>(i,j, Rmin - r) > th)
                {
                    circle(dst, Point(j,i) , 2, Scalar(127,127,0), 2, 8 ,0);
                    circle(dst, Point(j,i) , r, Scalar(127,127,0), 2, 8 ,0);
                }
            }

        }
    }

}


int main(int argc, char** argv){

    Mat src = imread(argv[1],IMREAD_COLOR);
    Mat dst;

    if (src.empty())
        return -1;

    imshow("Original", src);
    waitKey(0);

    myHoughCircle(src,dst);
    imshow("HoughC", dst);
    waitKey(0);

    return 0;
}
