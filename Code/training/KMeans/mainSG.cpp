//versione in scala di grigi
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;

void createClusterInfo(const Mat& src, int clustersNum, vector<uchar> &clustersCenters, vector<vector<Point>> &ptInClusters){

    RNG random(getTickCount());

    for(int k = 0; k < clustersNum; k++){

        Point pixel;
        pixel.x = random.uniform(0, src.cols);
        pixel.y = random.uniform(0, src.rows);

        uchar center = src.at<uchar>(pixel);
        clustersCenters.push_back(center);

        vector<Point> ptInClusterK;
        ptInClusters.push_back(ptInClusterK);
    }
}

double colorDistance(uchar pixel, uchar clusterPixel){
    return abs(pixel - clusterPixel); 
}

void findKCluster(const Mat& src, int clustersNum, vector<uchar> &clustersCenters, vector<vector<Point>> &ptInClusters){

    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){

            double minDistance = INFINITY;
            int indexClosestCluster = 0;
            uchar pixel = src.at<uchar>(x,y);

            for(int k = 0; k < clustersNum; k++){

                double distance = colorDistance(pixel, clustersCenters[k]);
                if(distance < minDistance){
                    minDistance = distance;
                    indexClosestCluster = k;
                }
            }

            ptInClusters[indexClosestCluster].push_back(Point(y,x));
        }
    }
}

double fixCenters(const Mat& src, int clustersNum, vector<uchar> &clustersCenters, vector<vector<Point>> &ptInClusters, double& oldValue, double newValue){

    double diffChange = 0;

    for(int k = 0; k < clustersNum; k++){

        vector<Point> ptInClusterK = ptInClusters[k];
        double newGray = 0;

        for(int i = 0; i < ptInClusterK.size(); i++){
            uchar pixel = src.at<uchar>(ptInClusterK[i]);
            newGray += pixel;
        }

        newGray /= ptInClusterK.size();

        uchar center = newGray;
        newValue += colorDistance(center, clustersCenters[k]);
        clustersCenters[k] = center;
    }

    newValue /= clustersNum;
    diffChange = abs(oldValue - newValue);
    oldValue = newValue;
    return diffChange;
}

void applyFinalChange(Mat& dst, int clustersNum, vector<uchar> &clustersCenters, vector<vector<Point>> &ptInClusters){

    for(int k = 0; k < clustersNum; k++){

        vector<Point> ptInClusterK = ptInClusters[k];
        
        for(int i = 0; i < ptInClusterK.size(); i++){

            uchar pixel = clustersCenters[k];

            dst.at<uchar>(ptInClusterK[i]) = pixel;
        }
    }
}

int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_GRAYSCALE);

    if(src.empty())
        return -1;

    
    resize(src, src, Size(0, 0), 0.5, 0.5);

    imshow("Original img", src);

    int clustersNum = stoi(argv[2]);
    double oldValue = INFINITY;
    double newValue = 0;
    double diffChange = oldValue - newValue;

    vector<uchar> clustersCenters;
    vector<vector<Point>> ptInClusters;
    double th = 0.1;

    createClusterInfo(src, clustersNum, clustersCenters, ptInClusters);

    while(diffChange > th){

        newValue = 0;

        for(int k = 0; k < clustersNum; k++){
            ptInClusters[k].clear();
        }


        findKCluster(src, clustersNum, clustersCenters, ptInClusters);

        diffChange = fixCenters(src, clustersNum, clustersCenters, ptInClusters, oldValue, newValue);
    }

    Mat dst = src.clone();

    applyFinalChange(dst, clustersNum, clustersCenters, ptInClusters);
    imshow("dst", dst);
    waitKey(0);


    return 0;
}