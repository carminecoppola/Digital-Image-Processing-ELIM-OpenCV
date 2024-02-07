#include<opencv4/opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;


//normalizzazione hist

vector<float> normHist(const Mat& src){

    cout<<"\nSono in norm"<<endl;

    vector<float> hist(256,0.0f);

    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            hist.at(src.at<uchar>(i,j))++;
        }
        
    }
    
    for (int i = 0; i < 256; i++)
    {
        hist.at(i) /= src.cols * src.rows;
    }
    return hist;

}

//Probabilità

vector<float> probability(vector<float> hist){

    cout<<"\nSono in prob"<<endl;

    vector<float> prob(256, 0.0f);

    prob.at(0) = hist.at(0);

    for (int i = 1; i < 256; i++){
        prob.at(i) = prob.at(i -1) + hist.at(i);
    }
    
    return prob;
}

//media cumulativa
vector<float> cumulativeAvg( vector<float> hist){

    cout<<"\nSono in cumm"<<endl;

    vector<float> cumAvg(256, 0.0f);

    cumAvg.at(0) = hist.at(0);

    for (int i = 1; i < 256; i++){
        cumAvg.at(i) = cumAvg.at(i -1) + i * hist.at(i);
    }
    
    return cumAvg;
}

//Intensità globale media
float globalAvg(vector<float> hist){
    
    cout<<"\nSono in global"<<endl;

    float globAvg = 0.0f;

    for (int i = 0; i < 256; i++)
    {
        globAvg += i * hist.at(i);
    }
    
    return globAvg; 
}


//Varianza interclasse
vector<float> interVariance(vector<float> prob, vector<float> cumAvg, float globAvg){

    cout<<"\nSono in interV"<<endl;

    vector<float> sigma(256, 0.0f);

    for (int i = 0; i < 256; i++)
    {
        float num = pow(prob.at(i) * globAvg - cumAvg.at(i), 2);
        float denom = prob.at(i) * (1 - prob.at(i));

        sigma.at(i) = denom == 0 ? 0: num/denom;
    }
    
    return sigma;
}

//Soglia k*
int kStar(vector<float> sigma){

    cout<<"\nSono in kstar"<<endl;

    float maxVariance = sigma.at(0);
    int k = 0;

    //Sto facendo la ricerca del massimo valore k

    for (int i = 1; i < 256; i++)
    {
        if(sigma.at(i) > maxVariance){
            maxVariance = sigma.at(i);
            k = i;
        }
    }
    return k;
}

int otsu(const Mat& src){

    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    //Richiamo tutte le funzioni
    vector<float> hist = normHist(src);
    vector<float> prob = probability(hist);
    vector<float> cumAvg = cumulativeAvg(hist);
    float globAvg = globalAvg(hist);

    cout<<"\nSono in otsu"<<endl;

    vector<float> sigma = interVariance(prob,cumAvg, globAvg);

    int k = kStar(sigma);

    return k;
}


int main(int argc, char** argv){

    Mat src, dst;

    src = imread(argv[1], IMREAD_GRAYSCALE);

    if(src.empty())
        return -1;

    imshow("Orignial", src);
    waitKey(0);


    threshold(src, dst, otsu(src), 255, THRESH_BINARY);
    imshow("Otsu Single", dst);
    waitKey(0);


    return 0;
}