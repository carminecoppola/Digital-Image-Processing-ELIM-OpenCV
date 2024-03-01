#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int th = 175;

void convertPolar(double rho, double theta, Point& p1, Point& p2){

    int x0 = cvRound(rho * cos(theta));
    int y0 = cvRound(rho * sin(theta));

    int alpha = 1000;

    p1.x = cvRound( x0 + alpha *(-sin(theta)));
    p1.y = cvRound( y0 + alpha *(cos(theta)));

    p2.x = cvRound( x0 - alpha *(-sin(theta)));
    p2.y = cvRound( y0 - alpha *(cos(theta)));
    
}

void myHoughRette(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    Mat imgCanny;
    Canny(blur, imgCanny, 100,200);
    imshow("Canny", imgCanny);

    int maxDist =hypot(imgCanny.rows, imgCanny.cols);

    Mat votes = Mat::zeros(maxDist * 2, 180, CV_8UC1);

    double rho, theta;
    double rad = CV_PI/180;

    for (int i = 0; i < imgCanny.rows; i++)
    {
        for (int j = 0; j < imgCanny.cols; j++)
        {
            if(imgCanny.at<uchar>(i,j) == 255){
                for ( theta = 0; theta < 180; theta++)
                {
                    rho = cvRound(i * sin((theta - 90)*rad) + j * cos((theta - 90)*rad) ) + maxDist;
                    votes.at<uchar>(rho,theta)++;
                }
                
            }
        }
    }
    
    vector<pair<Point,Point>> lines;

    for (int i = 0; i < votes.rows; i++)
    {
        for (int j = 0; j < 180; j++)
        {
            if(votes.at<uchar>(i,j) > th){
                rho = i - maxDist;
                theta = (j -90) * rad;
                Point p1, p2;
                convertPolar(rho, theta, p1,p2);
                lines.push_back(make_pair(p1,p2));
            }

        }
        
    }
    
    src.copyTo(dst);

    for (int i = 0; i < lines.size(); i++)
    {
        pair<Point, Point> coordinates = lines.at(i);
        line(dst, coordinates.first, coordinates.second, Scalar(255, 0, 0), 1, LINE_AA);
    }
    
}

int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_COLOR);
    Mat dst;

    if(src.empty())
        return -1;

    imshow("Original",src);
    waitKey(0);

    myHoughRette(src, dst);
    imshow("Hough",dst);
    waitKey(0);

    return 0;
}