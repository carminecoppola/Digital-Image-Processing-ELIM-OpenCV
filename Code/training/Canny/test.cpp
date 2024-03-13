#include<iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int ht = 100, lt = 30;


void myCanny(const Mat& src, Mat& dst){

    Mat blur;
    GaussianBlur(src,blur, Size(3,3),0,0);

    Mat dx, dy;
    Sobel(blur, dx, src.type(), 1, 0);
    Sobel(blur, dy, src.type(), 0, 1);

    Mat magn;
    magn = abs(dx) + abs(dy);
    //magnitude(dx,dy,magn);
    normalize(magn, magn, 0, 255, NORM_MINMAX);

    Mat orientation;
    dx.convertTo(dx,CV_32FC1);
    dy.convertTo(dy,CV_32FC1);
    phase(dx, dy, orientation, true);

    Mat maxSupp;
    magn.copyTo(maxSupp);
    copyMakeBorder(maxSupp, maxSupp, 1, 1, 1, 1, BORDER_CONSTANT);

    for (int i = 0; i < maxSupp.rows; i++)
    {
        for (int j = 0; j < maxSupp.cols; j++)
        {
            float ang = orientation.at<uchar>(i - 1, j - 1) > 180? orientation.at<uchar>(i - 1, j - 1) - 180 : orientation.at<uchar>(i - 1, j - 1);

            if (0 <= ang && ang <= 22.5 || 157.5 <= ang && ang <= 180)
            {
                if (maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i ,j - 1) || maxSupp.at<uchar>(i, j) < maxSupp.at<uchar>(i ,j + 1))
                {
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            else if (22.5 < ang && ang <= 67.5)
            {
                if (maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1 ,j - 1) || maxSupp.at<uchar>( i, j) < maxSupp.at<uchar>(i + 1 ,j + 1))
                {
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            else if (67.5 < ang && ang <= 112.5)
            {
                if (maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1 ,j) || maxSupp.at<uchar>(i , j) < maxSupp.at<uchar>(i + 1 ,j))
                {
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
            else if (112.5 < ang && ang <= 157.5)
            {
                if (maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1 ,j - 1) || maxSupp.at<uchar>( i, j) < maxSupp.at<uchar>(i - 1 ,j + 1))
                {
                    maxSupp.at<uchar>(i,j) = 0;
                }
            }
        }
    }
    

    


    for(int i = 0; i < maxSupp.rows; i++){

        for(int j = 0; j < maxSupp.cols; j++){

            if (maxSupp.at<uchar>(i,j) > ht)
            {
                maxSupp.at<uchar>(i,j) = 255;
            }
            else if (maxSupp.at<uchar>(i,j) < lt)
            {
                maxSupp.at<uchar>(i,j) = 0;
            }
            else 
            {
                bool strong_e = false;

                for (int k = -1; k <= 1 && !strong_e; k++)
                {
                    for (int l = -1; l <= 1 && !strong_e; l++)
                    {
                        if (maxSupp.at<uchar>(i + k , j + l) > ht)
                        {
                            strong_e = true;
                        }
                    }
                }

                if (strong_e)
                    maxSupp.at<uchar>(i,j) = 255;
                else
                    maxSupp.at<uchar>(i,j) = 0;
            }
        
        }
    }

    maxSupp.copyTo(dst);

}


int main(int argc, char** argv){

    Mat src = imread(argv[1],IMREAD_GRAYSCALE);
    Mat dst;

    if (src.empty())
        return -1;

    imshow("Original",src);
    waitKey(0);


    myCanny(src,dst);
    imshow("Canny",dst);
    waitKey(0);


    return 0;
}