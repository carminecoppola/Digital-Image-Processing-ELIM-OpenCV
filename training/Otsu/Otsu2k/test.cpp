#include <opencv2/opencv.hpp>
#include <stdlib.h>
#include <vector>


using namespace cv;
using namespace std;


vector<double> createNormalizedHisto(const Mat src){

    vector<double> normalizedHistogramm(256, 0.0f);

    double nxm = src.rows * src.cols;

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            normalizedHistogramm.at( src.at<uchar>(i,j))++;
        }
    }

    for (auto& prob : normalizedHistogramm)
    {
        prob /= nxm;
    }

    return normalizedHistogramm;
}


vector<int> otsu2k(const Mat src){

    vector<int> T(3, 0.0f);
    vector<double> normalizedHistogramm = createNormalizedHisto(src);
    vector<double> cumSum(3, 0.0f);
    vector<double> cumAvg(3, 0.0f);

    double mg = 0.0f, iv = 0.0f, interclassVariance = 0.0f;

    cumSum.at(0) = normalizedHistogramm.at(0);
    cumAvg.at(0) = normalizedHistogramm.at(0);

    mg = normalizedHistogramm.at(0);


    for (int i = 1; i < 256; i++)
        mg += normalizedHistogramm.at(i);

    for (int i = 1; i < 256 - 2; i++)
    {
        cumSum.at(0) += normalizedHistogramm.at(i);
        cumAvg.at(0) += i * normalizedHistogramm.at(i);

        for (int j = i + 1; j < 256; j++)
        {
            cumSum.at(1) += normalizedHistogramm.at(j);
            cumAvg.at(1) += j * normalizedHistogramm.at(j);

            for (int k = j + 1; k < 256; k++)
            {
                cumSum.at(2) += normalizedHistogramm.at(k);
                cumAvg.at(2) += k * normalizedHistogramm.at(k);

                for (int z = 0; z < 3; z++)
                    iv += cumSum.at(z) * pow(cumAvg.at(z) / cumSum.at(z) - mg, 2);
                
                if(iv > interclassVariance)
                {
                    interclassVariance = iv;
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


void myTh(const Mat src, Mat& dst, int th1, int th2){

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


int main(int argc, char** argv){


    Mat src = imread(argv[1], IMREAD_GRAYSCALE);
    Mat dst;

    if(src.empty())
        return -1;

    imshow("Original",src);
    waitKey(0);


    vector<int> T = otsu2k(src);
    myTh(src, dst, T.at(0), T.at(1));

    imshow("Otsu2k",dst);
    waitKey(0);

    


    return 0;
}