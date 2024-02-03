#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>
#include"correlazione.cpp"
#include"convoluzione.cpp"

using namespace std;
using namespace cv;

/**
*   |1 2 1| 
*   |2 4 2| -> la somma di questo filtro è 16, quindi dobbiamo dividere ogni valore per 16
*   |1 2 1|
*   
*   1/16 -> 1/16. 
*   2/16 -> 1/8. 
*   4/16 -> 1/4. 
*/

int main(int argc, char** argv){

    // Caricamento dell'immagine in scala di grigi
    Mat img = imread(argv[1],IMREAD_GRAYSCALE);
    if(img.empty())
        return -1;

    // Filtro Media
    Mat filtroMedia = (Mat_<float>(3,3) << 1/16.0, 1/8.0, 1/16.0,
                                          1/8.0,  1/4.0, 1/8.0,
                                          1/16.0, 1/8.0, 1/16.0);

    Mat filter2Dimg;

    // Applicazione del filtro di media usando la funzione filter2D di OpenCV
    filter2D(img, filter2Dimg, img.type(), filtroMedia);

    // Applicazione delle funzioni di correlazione e convoluzione implementate separatamente
    Mat corrImg = Correlazione(img, filtroMedia);
    Mat convImg = Convoluzione(img, filtroMedia);

    // Visualizzazione delle immagini
    imshow("Original",img);
    waitKey(0);
    imshow("Filter2D",filter2Dimg);
    waitKey(0);
    imshow("Correlazione",corrImg);
    waitKey(0);
    imshow("Convoluzione",convImg);
    waitKey(0);

    return 0;
} 