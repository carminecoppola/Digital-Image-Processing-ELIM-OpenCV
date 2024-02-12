#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const int Rmin = 50;
const int Rmax = 150;
const int ht = 170;

void houghCircle(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    Mat imgCanny;
    Canny(blur, imgCanny, 100,200);
    imshow("Canny",imgCanny);

    //Dimensione matrice dei voti
    int size[] = {imgCanny.rows, imgCanny.cols, (Rmax - Rmin) + 1};
    Mat votes = Mat::zeros(3, size, CV_8UC1);

    //Calcolo dei voti

    double rad = CV_PI/180;

    for (int i = 0; i < imgCanny.rows; i++)
    {
        for (int j = 0; j < imgCanny.cols; j++)
        {
            if(imgCanny.at<uchar>(i,j) == 255){

                for (int r = Rmin; r < Rmax; r++)
                {
                    for (int t = 0; t < 360; t++)
                    {
                        int a = cvRound( i - r * sin( t * rad));
                        int b = cvRound( j - r * cos( t * rad));

                        //Incremento voti
                        if(a >= 0 && a < imgCanny.rows && b >= 0 && b < imgCanny.cols){
                            votes.at<uchar>(a,b,r - Rmin)++;
                        }
                    }  
                }
            }
        }
    }
    

    src.copyTo(dst);

    for (int r = Rmin; r < Rmax; r++)
    {
        for (int i = 0; i < imgCanny.rows; i++)
        {
            for (int j = 0; j < imgCanny.cols; j++)
            {
                if(votes.at<uchar>(i, j, r - Rmin) > ht){
                    circle(dst, Point(j,i), 3, Scalar(127,127,0), 2, 8, 0);
                    circle(dst, Point(j,i), r, Scalar(127,127,0), 2, 8, 0);

                }
            }   
        }
    }


}


int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_COLOR);

    if( src.empty() )
        return -1;

    imshow("Original img", src);

    houghCircle(src, dst);
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}
