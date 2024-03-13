#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

// Definizione della funzione di Harris per il rilevamento degli angoli
void harris(const Mat& src, Mat& dst, int ht = 180){

    // Calcolo delle derivate rispetto a x e y
    Mat dX, dY;
    Sobel(src, dX, CV_32FC1, 1, 0); // Derivata rispetto a x
    Sobel(src, dY, CV_32FC1, 0, 1); // Derivata rispetto a y

    // Calcolo delle derivate al quadrato e del prodotto delle derivate
    Mat dX2, dY2, dXY;
    pow(dX, 2, dX2); // Derivata al quadrato rispetto a x
    pow(dY, 2, dY2); // Derivata al quadrato rispetto a y
    multiply(dX, dY, dXY); // Prodotto delle derivate

    // Applicazione del filtro Gaussiano
    Mat C00, C10, C01, C11;
    GaussianBlur(dX2, C00, Size(3,3), 0, 0); // Filtraggio di dX^2
    GaussianBlur(dY2, C11, Size(3,3), 0, 0); // Filtraggio di dY^2
    GaussianBlur(dXY, C10, Size(3,3), 0, 0); // Filtraggio di dX * dY
    C10.copyTo(C01); // Copia di C10 in C01

    // Calcolo delle componenti della matrice di Harris
    Mat C1, C2;
    multiply(C00, C11, C1); // Componente C1
    multiply(C10, C01, C2); // Componente C2

    // Calcolo della risposta di Harris
    double k = 0.05;
    Mat R, det, traccia;
    det = C1 - C2; // Determinante
    traccia = C00 + C11; // Traccia
    pow(traccia, 2, traccia); // Quadrato della traccia
    R = det - k * traccia; // Risposta di Harris

    // Normalizzazione della risposta di Harris
    normalize(R, dst, 0, 255, NORM_MINMAX);

    // Conversione dell'immagine normalizzata in scala di grigi a un'immagine in scala di grigi a 8 bit senza segno
    Mat dst_scale;
    convertScaleAbs(dst, dst_scale);

    // Sogliatura della risposta di Harris e evidenziazione degli angoli trovati
    for(int i = 1; i < dst.rows-1; i++){
        for(int j = 1; j < dst.cols-1; j++){
            if(dst.at<float>(i,j) > ht){
                circle(dst_scale, Point(j,i), 5, Scalar(0, 0, 255), 2, 8, 0); // Evidenzia l'angolo trovato con un cerchio rosso
            }
        }
    } 

    // Visualizzazione dell'immagine con gli angoli evidenziati
    imshow("Harris", dst_scale);
}

int main(int argc, char **argv){

    Mat dst;
    Mat src = imread(argv[1], IMREAD_GRAYSCALE); // Caricamento dell'immagine in scala di grigi
    if( src.empty() )
        return -1;

    imshow("My Img", src); // Visualizzazione dell'immagine originale

    harris(src, dst); // Applicazione della funzione di Harris per il rilevamento degli angoli
    waitKey(0);

    return 0;
}
