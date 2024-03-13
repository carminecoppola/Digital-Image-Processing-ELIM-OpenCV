#include <opencv2/opencv.hpp>
#include<stdlib.h>
#include <vector>


using namespace std;
using namespace cv;

vector<double> createNormalizedHistogramm(const Mat src){

    vector<double> normalizedHist(256, 0.0f);
    double nxm = src.rows * src.cols;

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            normalizedHist.at(src.at<uchar>(i,j))++;
        }
    }

    for (auto& prob: normalizedHist){
        prob /= nxm;
    }

    return normalizedHist;
}

vector<int> myOtsu2k(const Mat& src, Mat& dst){

    vector<int> T(3,0);

    vector<double> normalizedHist = createNormalizedHistogramm(src);

    vector<double> cumSum(3,0.0f), cumAvg(3,0.0f);


    double mg = 0.05, interclassVariance = 0.0f, iv = 0.0f;

    cumSum.at(0) = normalizedHist.at(0);
    cumAvg.at(0) = normalizedHist.at(0);
    mg = normalizedHist.at(0);


    for (int i = 0; i < 256; i++){
        mg += i * normalizedHist.at(i);
    }

    for (int i = 0; i < 254; i++){
        
        cumSum.at(0) += normalizedHist.at(i);
        cumAvg.at(0) += i * normalizedHist.at(i);

        for (int j = i + 1; j < 255; j++){
            cumSum.at(1) += normalizedHist.at(j);
            cumAvg.at(1) += j * normalizedHist.at(j);

            for (int k = j + 1; k < 256; k++){
                cumSum.at(2) += normalizedHist.at(k);
                cumAvg.at(2) += k * normalizedHist.at(k);


                for (int z = 0; z < 3; z++)
                {
                    iv += cumSum.at(z) * pow( cumAvg.at(z) / cumSum.at(z) - mg, 2);
                }
                if (iv > interclassVariance)
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

void myTh(const Mat& src, Mat& dst, int th1, int th2){


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
            {
                dst.at<uchar>(i,j) = 0;
            }
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


    vector<int> T = myOtsu2k(src,dst);
    myTh(src,dst, T.at(0), T.at(1));

    imshow("Otsu2k",dst);
    waitKey(0);

    


    return 0;
}