#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int rmin = 50;
const int rmax = 150;
int th = 170;

void houghCerchi(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    Mat imgCanny;
    Canny(blur, imgCanny, 100, 200);
    imshow("Canny", imgCanny);

    //size e mat voti
    int size[] = {imgCanny.rows, imgCanny.cols, (rmax - rmin) + 1};
    Mat votes = Mat::zeros( 3, size, CV_8UC1);

    double rad = CV_PI/180;

    for (int i = 0; i < imgCanny.rows; i++)
    {
        for (int j = 0; j < imgCanny.cols; j++)
        {
            if( imgCanny.at<uchar>(i,j) == 255){
                for (int r = rmin; r < rmax; r++)
                {
                    for (int t = 0; t < 360; t++)
                    {
                        int a = cvRound( i - r * sin(t * rad));
                        int b = cvRound( j - r * cos(t * rad));
                        

                        if( a >= 0 && a < imgCanny.rows && b >= 0 && b < imgCanny.cols)
                            votes.at<uchar>(a, b, r - rmin) ++;
                    }
                }
            }
        }
    }
    
    src.copyTo(dst);

    for (int r = rmin; r < rmax; r++)
    {
        for (int i = 0; i < imgCanny.rows; i++)
        {
            for (int j = 0; j < imgCanny.cols; j++)
            {
                if(votes.at<uchar>(i,j, r - rmin) > th){
                    circle(dst, Point(j, i), 3, Scalar(127, 127, 0), 2, 8, 0);
                    circle(dst, Point(j, i), r, Scalar(127, 127, 0), 2, 8, 0);
                }
            }
            
        }
    }


}


int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_COLOR);
    Mat dst;

    if(src.empty())
        return -1;

    imshow("Original", src);

    houghCerchi(src,dst);

    imshow("Hough",dst);
    waitKey(0);

    return 0;
}