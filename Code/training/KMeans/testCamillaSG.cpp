#include<iostream>
#include<opencv2/opencv.hpp>
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;


//ClusterCasuali
void createCenterCluster(Mat& src, int numCluster, vector<uchar>& clustersCenters, vector<vector<Point>>& ptInCluster ){

    RNG random(getTickCount());

    for(int k = 0; k < numCluster; k++){

        Point pixel;

        pixel.x = random.uniform(0, src.cols);
        pixel.y = random.uniform(0, src.rows);

        uchar center = src.at<uchar>(pixel);

        clustersCenters.push_back(center);

        vector<Point> ptInClusterK;
        ptInCluster.push_back(ptInClusterK);
    }
}

double colorDistance(uchar pixel, uchar clusterPixel){
    return abs(pixel - clusterPixel);
}


void findKCluster(Mat& src, int numCluster, vector<uchar>& clustersCenters, vector<vector<Point>>& ptInCluster){

    for(int i = 0; i < src.rows; i++){
        for(int j = 0; j < src.cols; j++){

            double minDist = INFINITY;
            int indexCluster = 0;
            uchar pixel = src.at<uchar>(i,j);

            for(int k = 0; k < numCluster; k++){
            
               double newDistance = colorDistance(pixel, clustersCenters[k]);

               if(newDistance < minDist){
                    minDist = newDistance;
                    indexCluster = k;
               }
            }
            ptInCluster[indexCluster].push_back(Point(j,i));
        }
    }
}

//Aggiustamento dei centri
double fixCenter(Mat& src, int numCluster, vector<uchar>& clustersCenters, vector<vector<Point>>& ptInCluster, double& oldValue, double newValue){

    double diffChange = 0;

    for(int k = 0; k < numCluster; k++){

        vector<Point> ptInClusterK = ptInCluster[k];
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

    newValue /= numCluster;
    diffChange = abs(oldValue - newValue);
    oldValue = newValue;

    return diffChange;
}


//Applico cambiamenti finali
void applyChange(Mat& dst, int numCluster, vector<uchar>& clustersCenters, vector<vector<Point>>& ptInCluster){

    for(int k = 0; k < numCluster; k++){
    
        vector<Point> ptInClusterK = ptInCluster[k];

        for(int i = 0; i < ptInClusterK.size(); i++){

            uchar pixel = clustersCenters[k];

            dst.at<uchar>(ptInClusterK[i]) = pixel;
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

    int numCluster = stoi(argv[2]);
    double oldValue = INFINITY;
    double newValue = 0;
    double diffChange = oldValue - newValue;

    vector<uchar> clustersCenters;
    vector<vector<Point>> ptInCluster;
    double th = 0.1;

    createCenterCluster(src, numCluster ,clustersCenters, ptInCluster);

    while(diffChange > th){

        newValue = 0;

        for(int k = 0; k < numCluster; k++){
            ptInCluster[k].clear();
        }

        findKCluster(src, numCluster, clustersCenters, ptInCluster);
        diffChange = fixCenter(src, numCluster, clustersCenters, ptInCluster, oldValue, newValue);

    }

    dst = src.clone();

    applyChange(dst, numCluster, clustersCenters, ptInCluster);

    imshow("K-means", dst);
    waitKey(0);


	return 0;
}