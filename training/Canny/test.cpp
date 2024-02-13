#include<opencv4/opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

int ht = 100;
int lt = 70;

void mycanny(const Mat& src, Mat& dst){

    //Blur
    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    //Derivate
    Mat dx, dy, magn;
    Sobel(blur, dx, src.type(), 1, 0);
    Sobel(blur, dy, src.type(), 0, 1);

    //Magnitudo

    magn = abs(dx) + abs(dy);
    normalize( magn, magn, 0 , 255 ,NORM_MINMAX);

    //Orientation
    Mat orientation;
    dx.convertTo(dx, CV_32FC1);
    dy.convertTo(dy, CV_32FC1);

    phase(dx, dy, orientation, true);

    //No max supp
    Mat maxSupp;
    magn.copyTo(maxSupp);
    copyMakeBorder(magn, magn, 1, 1, 1, 1, BORDER_CONSTANT);

    for (int i = 1; i < maxSupp.rows - 1; i++)
    {
        for (int j = 1; j < maxSupp.cols - 1; j++)
        {
            float ang = orientation.at<float>(i - 1, j - 1) > 180?  orientation.at<float>(i - 1, j - 1) - 180 : orientation.at<float>(i - 1, j - 1);

            //Orizzonatale
            if( 0 <= ang &&  ang <= 22.5|| 157.5 <= ang && ang <= 180){
                if( maxSupp.at<uchar>(i , j) < maxSupp.at<uchar>(i ,j - 1) ||  maxSupp.at<uchar>(i ,j ) < maxSupp.at<uchar>(i ,j + 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }// -45°
            else if( 22.5 <= ang &&  ang <= 67.5){
                if( maxSupp.at<uchar>(i , j) < maxSupp.at<uchar>(i + 1, j + 1) ||  maxSupp.at<uchar>(i ,j ) < maxSupp.at<uchar>(i - 1, j - 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }// Verticale
            else if( 67.5 <= ang &&  ang <= 112.5){
                if( maxSupp.at<uchar>(i , j) < maxSupp.at<uchar>(i + 1, j ) ||  maxSupp.at<uchar>(i ,j ) < maxSupp.at<uchar>(i - 1, j ))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            // + 45°
            else if( 112.5 <= ang &&  ang <= 157.5){
                if( maxSupp.at<uchar>(i , j) < maxSupp.at<uchar>(i - 1, j + 1) ||  maxSupp.at<uchar>(i ,j ) < maxSupp.at<uchar>(i + 1, j - 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }
            
        }
    }
    

    for (int i = 1; i < maxSupp.rows; i++)
    {
        for (int j = 1; j < maxSupp.cols; j++)
        {
            if( maxSupp.at<uchar>(i,j) > ht)
                maxSupp.at<uchar>(i,j) = 255;
            else if( maxSupp.at<uchar>(i,j) < lt)
                maxSupp.at<uchar>(i,j) = 0;
            else if( maxSupp.at<uchar>(i,j) > lt && maxSupp.at<uchar>(i,j) < ht ){
                bool strong_e = false;

                for (int k = -1; k <= 1 && !strong_e; k++)
                {
                    for (int l = -1; l <= 1 && !strong_e; l++)
                    {
                        if(maxSupp.at<uchar>(i + k, j + l) > ht)
                            maxSupp.at<uchar>(i , j ) = 255;
                    }
                }
                
                if(strong_e)
                    strong_e = true;
                else
                    strong_e = false;
            }
        }
    }
    
    maxSupp.copyTo(dst);

}


int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst;

    if(src.empty())
        return -1;
    
    imshow("Original", src);
    waitKey(0);

    mycanny(src, dst);
    imshow("Canny", dst);
    waitKey(0);



    return 0;
}