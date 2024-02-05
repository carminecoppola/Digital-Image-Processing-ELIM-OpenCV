#include <iostream>
#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

void edgeDetection(const Mat& src, Mat& dst, int th){
    Mat lapImg, edgeImg;

    Laplacian(src, lapImg, src.type());
    convertScaleAbs(lapImg, edgeImg);
    threshold(edgeImg, dst, th, 255, THRESH_BINARY);
}

int pixelEdge(const Mat& edgeImg, vector<Point> &pixelEdges){

    int numPixel = 0;

    for(int x = 0; x < edgeImg.rows; x++){
        for(int y = 0; y < edgeImg.cols; y++){
            if(edgeImg.at<uchar>(x,y) == 255){
                numPixel++;
                Point edge;
                edge.x = x;
                edge.y = y;
                pixelEdges.push_back(edge);
            }
        }
    }

    return numPixel;
}   

vector<float> normHist(const Mat& src, const vector<Point>& pixelEdges, int numPixel){

    vector<float> hist(256, 0.0f);

    for(auto &i : pixelEdges){
        hist.at(src.at<uchar>(i.x, i.y))++;
    }

    for(int i = 0; i < 256; i++){
        hist.at(i) = numPixel > 0 ? hist.at(i)/numPixel : 0;
    }

    return hist;
}

vector<float> probability(vector<float> hist){

    vector<float> prob(256, 0.0f);
    prob.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        prob.at(i) = prob.at(i-1) + hist.at(i);
    }

    return prob;
}

vector<float> cumulativeAvg(vector<float> hist){

    vector<float> cumAvg(256, 0.0f);
    cumAvg.at(0) = hist.at(0);

    for(int i = 1; i < 256; i++){
        cumAvg.at(i) = cumAvg.at(i-1) + i * hist.at(i);
    }

    return cumAvg;

}


float globalAvg(vector<float> hist){

    float globAvg = 0.0f;

    for(int i = 0; i < 256; i++){
        globAvg += i * hist.at(i);
    }

    return globAvg;
}

vector<float> interVariance(vector<float> prob, vector<float> cumAvg, float globAvg){

    vector<float> sigma(256, 0.0f);

    for(int i = 0; i < 256; i++){
        float num = pow(prob.at(i)*globAvg - cumAvg.at(i), 2);
        float denom = prob.at(i) * (1 - prob.at(i));

        sigma.at(i) = denom == 0 ? 0 : num/denom;
    }

    return sigma;
}

int kStar(vector<float> sigma){

    float maxVariance = sigma.at(0);
    int k = 0;

    for(int i = 1; i < 256; i++){

        if(sigma.at(i) > maxVariance){
            maxVariance = sigma.at(i);
            k = i;
        }
    }

    return k;
}

int otsu(const Mat& src, int th){

    Mat blur, dst;
    vector<Point> pixel;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    edgeDetection(src, dst, th);
    int numPixel = pixelEdge(dst, pixel);

    vector<float> hist = normHist(blur, pixel, numPixel);
    vector<float> prob = probability(hist);
    vector<float> cumAvg = cumulativeAvg(hist);
    float globAvg = globalAvg(hist);
    vector<float> sigma = interVariance(prob, cumAvg, globAvg);

    int k = kStar(sigma);

    return k;

}


int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_GRAYSCALE);

    if( src.empty() ){
        return -1;
    }

    imshow("Original", src);

    threshold(src, dst, otsu(src , 5), 255, THRESH_BINARY);
    imshow("Otsu", dst);
    waitKey(0);

    return 0;

}