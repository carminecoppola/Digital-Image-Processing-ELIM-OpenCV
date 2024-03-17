#include<iostream>
#include<opencv2/opencv.hpp>
#include<vector>

using namespace std;
using namespace cv;


vector<double> createNormHist(const Mat& src){

    vector<double> normHist(256, 0.0f);
    double nxm = src.cols * src.rows;

    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){
            normHist.at(src.at<uchar>(i,j))++;
        }
    }

    for(auto& prob:normHist)
        prob /= nxm;

    return normHist;

}


vector<int> myOtsu3k(const Mat& src){

    vector<double> normHist = createNormHist(src);

    vector<int> T(4,0);
    vector<double> cumSum(4,0.0f);
    vector<double> cumAvg(4,0.0f);
    double mg = 0.05, iv = 0.0f, interclassVariance = 0.0f;

    cumSum.at(0) = cumAvg.at(0) = normHist.at(0);
    mg = normHist.at(0);

    for(int i = 0; i < 256; i++)
        mg += i * normHist.at(i);

    for(int i = 0; i < 253; i++){

        cumSum.at(0) += normHist.at(i);
        cumAvg.at(0) += i * normHist.at(i);

        for(int j = i + 1; j < 254; j++){

            cumSum.at(1) += normHist.at(j);
            cumAvg.at(1) += j * normHist.at(j);

            for(int k = j + 1; k < 255; k++){

                cumSum.at(2) += normHist.at(k);
                cumAvg.at(2) += k * normHist.at(k);

                for(int l = k + 1; l < 256; l++){

                    cumSum.at(3) += normHist.at(l);
                    cumAvg.at(3) += l * normHist.at(l);

                    for(int z = 0; z < 4; z++){

                        iv += cumSum.at(z) * pow(cumAvg.at(z) / cumSum.at(z) - mg, 2);

                    }
                    if(iv > interclassVariance){

                        interclassVariance = iv;
                        T.at(0) = i;
                        T.at(1) = j;
                        T.at(2) = k;
                    }
                    iv = 0.0f;
                }
                cumSum.at(3) = cumAvg.at(3) = 0.0f;
            }
            cumSum.at(2) = cumAvg.at(2) = 0.0f;
        }
        cumSum.at(1) = cumAvg.at(1) = 0.0f;
    }

    return T;
}


void myTh(const Mat& src, Mat& dst, int th1, int th2, int th3){

    src.copyTo(dst);

    for(int i = 0; i < src.rows; i++){

        for(int j = 0; j < src.cols; j++){

            if(src.at<uchar>(i,j) >= th3)
                dst.at<uchar>(i,j) = 255;
            else if(src.at<uchar>(i,j) >= th2)
                dst.at<uchar>(i,j) = 170;

            else if(src.at<uchar>(i,j) >= th1)
                dst.at<uchar>(i,j) = 85;
            else
                dst.at<uchar>(i,j) = 0;
        }
    }

}


int main(int argc, char** argv){

	Mat src = imread(argv[1], IMREAD_GRAYSCALE);
	//Mat src = imread(argv[1], IMREAD_COLOR);
	Mat dst;

	if(src.empty())
		return -1;

	imshow("Original", src);
	waitKey(0);

    vector<int>T = myOtsu3k(src);
    myTh(src, dst, T.at(0), T.at(1), T.at(2));

    imshow("myOtsu3k", dst);
    waitKey(0);

	

	return 0;
}