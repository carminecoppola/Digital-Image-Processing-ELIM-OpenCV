#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Definizione delle costanti per il rilevamento delle circonferenze
const int Rmin = 30;
const int Rmax = 150;
const int ht = 140;

// Funzione per il rilevamento delle circonferenze mediante trasformata di Hough
void houghC(const Mat& src, Mat& dst){

    // Applicazione del filtro di Gauss per ridurre il rumore
    Mat blur, imgCanny;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    // Applicazione del rilevamento dei bordi tramite l'algoritmo di Canny
    Canny(blur, imgCanny, 100, 200);

    // Dimensioni della matrice dei voti per la trasformata di Hough
    int size[] = {imgCanny.rows, imgCanny.cols, (Rmax-Rmin) + 1};
    Mat votes = Mat::zeros(3, size, CV_8UC1);

    // Ciclo per il calcolo dei voti per ogni possibile centro e raggio
    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x,y) == 255){
                for(int r = Rmin; r < Rmax; r++){
                    for(int t = 0; t < 360; t++){

                        // Calcolo delle coordinate del centro della circonferenza
                        int a = cvRound(x - r*sin(t * CV_PI/180));
                        int b = cvRound(y - r*cos(t * CV_PI/180));

                        // Incremento del voto per il centro e il raggio corrispondenti
                        if(a >= 0 && a < imgCanny.rows && b >= 0 && b < imgCanny.cols)
                            votes.at<uchar>(a,b,r-Rmin)++;
                    }
                }
            }
        }
    }

    // Copia dell'immagine originale nell'immagine di output
    src.copyTo(dst);

    // Evidenziazione delle circonferenze trovate
    for(int r = Rmin; r < Rmax; r++){
        for(int x = 0; x < imgCanny.rows; x++){
            for(int y = 0; y < imgCanny.cols; y++){

                // Se il voto supera la soglia, disegna la circonferenza
                if(votes.at<uchar>(x,y,r-Rmin) > ht){

                    circle(dst, Point(y,x), 3, Scalar(127, 127, 0), 2, 8, 0); // Disegna il centro
                    circle(dst, Point(y,x), r, Scalar(127, 127, 0), 2, 8, 0); // Disegna la circonferenza
                }
            }
        }
    }
}

int main(int argc, char** argv){

    Mat src, dst;
    src = imread(argv[1], IMREAD_COLOR);

    if( src.empty() )
        return -1;

    imshow("Original img", src); 

    houghC(src, dst); 
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}
