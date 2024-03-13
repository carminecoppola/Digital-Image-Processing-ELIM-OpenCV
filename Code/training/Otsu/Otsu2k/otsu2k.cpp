#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;


vector<double> createNormHist(const Mat& src, Mat& dst){

    vector<double> normHist(256,0.0f);

    double nxm= src.rows * src.cols;

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            normHist.at(src.at<uchar>(i,j))++;
        }
    }

    for(auto& prob:normHist)
        prob /= nxm;

    return normHist;
}

vector<int> myOtsu2k(const Mat& src, Mat& dst){

    vector<double> normHist = createNormHist(src,dst);

    vector<int> T(3,0);
    vector<double> cumSum(3,0.0f);
    vector<double> cumAvg(3,0.0f);

    double mg = 0.05, iv = 0.0f, intClassVariance = 0.0f;

    cumSum.at(0) = normHist.at(0);
    cumAvg.at(0) = normHist.at(0);
    mg = normHist.at(0);


    for (int i = 0; i < 256; i++)
    {
        mg += i * normHist.at(i);
    }

    for (int i = 0; i < 254; i++)
    {
        cumSum.at(0) += normHist.at(i);
        cumAvg.at(0) += i * normHist.at(i);

        for (int j = i + 1; j < 255; j++)
        {
            cumSum.at(1) += normHist.at(j);
            cumAvg.at(1) += j * normHist.at(j);

            for (int k = j + 1; k < 256; k++)
            {
                cumSum.at(2) += normHist.at(k);
                cumAvg.at(2) += k * normHist.at(k);


                for (int z = 0; z < 3; z++)
                {
                    iv += cumSum.at(z) * pow(cumAvg.at(z) / cumSum.at(z) - mg, 2);
                }
                if (iv > intClassVariance)
                {
                    intClassVariance = iv;
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


void myTh(const Mat& src, Mat& dst , int th1, int th2){

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


int main(int argc, char**argv) {

    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst;

    if (src.empty())
        return -1;

    imshow("Original", src);
    waitKey(0);


    vector<int> T = myOtsu2k(src, dst);
    myTh(src,dst, T.at(0), T.at(1));

    imshow("Otsu2k", dst);
    waitKey(0);


    waitKey(0);

    return 0;
}