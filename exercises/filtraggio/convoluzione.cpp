#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>
#include"filteredValue2.cpp"


using namespace std;
using namespace cv;

/**
 * Applica il filtro di convoluzione all'immagine.
 * Ruota la maschera di 180 gradi e aggiunge il padding all'immagine.
 * Utilizza una doppia iterazione sugli elementi dell'immagine risultante 
 * e chiama la funzione filteredValue2 per ottenere il valore filtrato.
 */

Mat Convoluzione(Mat src, Mat mask){

    Mat tmp;

    rotate(mask, tmp, ROTATE_180);

    Mat matOutput(src.rows, src.cols,src.type());

    Mat padd;

    int bw = mask.rows/2;

    copyMakeBorder(src, padd, bw, bw, bw, bw, BORDER_DEFAULT);

    for (int i = 0; i < matOutput.rows; i++)
        for (int j = 0; j < matOutput.cols; j++)
            matOutput.at<uchar>(i,j) = filteredValue2(padd, tmp, i, j);

    return matOutput;
}