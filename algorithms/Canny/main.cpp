#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat src, dst;

int ht = 100, lt = 30;


void canny(const Mat& src, Mat&dst){

    //1) Smooting con la Gaussiana (sfocare l'immagine)

    Mat gaussBlur;
    GaussianBlur(src,gaussBlur, Size(3,3), 0, 0);


    //2) Calcolare la magnitudo e l'rientazione del gradiente
    //2.1) Gradiente

    Mat sobelX, sobelY;

    Sobel(gaussBlur, sobelX, src.type(), 1, 0);
    Sobel(gaussBlur, sobelY, src.type(), 0, 1);

    //2.2) Magnitudo

    Mat magnitudo;

    magnitudo = abs(sobelX) + abs(sobelY);
    sobelX.convertTo(sobelX, CV_32FC1);
    sobelY.convertTo(sobelY, CV_32FC1);

    //Angolo di fase

    Mat magNorm, orientation;
    normalize(magnitudo, magNorm, 0, 255, NORM_MINMAX, CV_8UC1);
    phase(sobelX, sobelY, orientation, true);                       //TRUE mi ritorna l'angolo in gradi


    //3) Applicare la NON MAXIMA SUPPRESSION
    Mat maxSupp;  //= Mat::zeros(magNorm.rows,magNorm.cols, magNorm.type());
    magNorm.copyTo(maxSupp);

    copyMakeBorder(maxSupp, maxSupp, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(0));

     /**
     * Per ogni pixel (i,j), che è il pixel centrale che sto considerando nell’intorno, verifico se è 
     * maggiore dei suoi vicini in quella direzione (in tal caso, è un edge forte). 
     * 
     * Orizzontale: i vicini sono quelli a sinistra e a destra del pixel centrale, dunque rispetto (i,j-1) e (i, j+1);
    */

    for (int i = 1; i < maxSupp.rows-1; i++){
        for (int j = 1; j < maxSupp.cols-1; j++){
            float ang = orientation.at<float>(i - 1, j - 1) > 180 ? orientation.at<float>(i - 1, j - 1) - 180: orientation.at<float>(i -1, j - 1); 
        
            //Orizzontale
            if( 0 <= ang && ang <= 22.5 || 157.5 < ang && ang <= 180){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i, j - 1) || maxSupp.at<uchar>(i, j) < maxSupp.at<uchar>(i, j + 1) )
                    maxSupp.at<uchar>(i,j) = 0; 
            }

            //Verticale
            else if(67.5 < ang && ang <= 112.5){
                if( maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j) || maxSupp.at<uchar>(i, j) < maxSupp.at<uchar>(i + 1, j))
                    maxSupp.at<uchar>(i,j) = 0;
            }

            //+45°
            else if( 22.5 < ang && ang <= 67.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j - 1) || maxSupp.at<uchar>(i, j) < maxSupp.at<uchar>(i + 1, j + 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }

            //-45°
            else if( 112.5 < ang && ang <= 157.5){
                if(maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i + 1, j - 1) || maxSupp.at<uchar>(i,j) < maxSupp.at<uchar>(i - 1, j + 1))
                    maxSupp.at<uchar>(i,j) = 0;
            }
        }
    }
    

    //4) Applicare il thresholding con isteresi
    for (int i = 1; i < maxSupp.rows - 1; i++){
        for (int j = 0; j < maxSupp.cols - 1; j++){
            if(maxSupp.at<uchar>(i,j) > ht){
                maxSupp.at<uchar>(i,j) = 255;
            }
            else if(maxSupp.at<uchar>(i, j) < lt){
                maxSupp.at<uchar>(i,j) = 0;
            }
            else if(maxSupp.at<uchar>(i,j) <= ht && maxSupp.at<uchar>(i,j) >= lt){
                bool strong_n = false;
                for (int l = -1; l <= 1 && !strong_n; l++){
                    for (int k = -1; k <= 1 && !strong_n; k++){
                        if(maxSupp.at<uchar>(i + l, j + k) < ht){
                            strong_n = true;
                        }
                    }
                    
                }
                if(strong_n){
                    maxSupp.at<uchar>(i,j) = 255;
                }
                else{
                    maxSupp.at<uchar>(i,j) = 0;
                }
                
            }
        }
        
    }

    maxSupp.copyTo(dst);

}

void CannyThreshold(int , void*){
    canny(src, dst);

    imshow("Canny", dst);
}


int main (int argc, char** argv){

    src = imread(argv[1], IMREAD_GRAYSCALE);

    if(src.empty())
        return -1;
    
    imshow("Original", src);
    waitKey(0);

    namedWindow("Canny");
    createTrackbar("Trackbar th", "Canny", &ht, 255, CannyThreshold);
    createTrackbar("Trackbar lh", "Canny", &lt, 255, CannyThreshold);
    CannyThreshold(0, 0);

    waitKey(0);

    Mat cannyCV;
    Canny(src, cannyCV, 30, 100);
    imshow("cv Canny", cannyCV);
    waitKey(0);
    

    return 0;
}