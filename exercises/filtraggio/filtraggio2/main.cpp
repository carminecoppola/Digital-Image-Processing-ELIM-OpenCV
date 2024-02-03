/**
* Implementare il Laplaciano con kernel isotropico a 45° e 90° utilizzando la funzione di correlazione/convoluzione (o filter2D())
*    - Per normalizzare i livelli di grigio è possibile usare la funzione normalize()
*    - normalize(src, dst, 0, 255, NORM_MINMAX, CV_8U);
*
* Implementare il filtro di Sobel (gx e gy) utilizzando la funzione di correlazione/convoluzione (o filter2D())
*    - Calcolare la risposta di entrambi i filtri
*    - Calcolare la magnitudo del gradiente (entrambe le formulazioni)
*    - Utilizzare la risposta ottenuta per effettuare lo sharpening di un'immagine
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include "correlazione.cpp"

using namespace cv;


int main(int argc, char** argv){

    Mat img = imread(argv[1],IMREAD_GRAYSCALE);
    if(img.empty())
        return -1;
    
    Mat laplaciano45 = (Mat_<float>(3,3) << 
                            1.0, 1.0, 1.0, 
                            1.0, -8.0, 1.0,
                            1.0,  1.0, 1.0
                        );

     Mat laplaciano90 = (Mat_<float>(3,3) << 
                            0.0, 1.0, 0.0, 
                            1.0, -4.0, 1.0,
                            0.0,  1.0, 0.0
                        );


    Mat filter45, filter90;
    filter2D(img, filter45, img.type(), laplaciano45);
    filter2D(img, filter90, img.type(), laplaciano90);
    
    Mat imgLap45, imgLap90;
    Laplacian(img, imgLap45, img.type(), 3);         // 3 = laplaciano a 45°
    Laplacian(img, imgLap90, img.type(), 1);        // 1 = laplaciano a 90°
    

    //Show img
    imshow("Original", img);
    waitKey(0);
    imshow("Filter 2D 45°", filter45);
    waitKey(0);
    imshow("Filter 2D 90°", filter90);
    waitKey(0);

    imshow("Laplaciano 45°", imgLap45);
    waitKey(0);
    imshow("Laplaciano 90°", imgLap90);
    waitKey(0);

    //Filtro di Sobel con Correlazione
    
    Mat corrImg45 = Correlazione(img, laplaciano45);
    imshow("Correlazione 45°", corrImg45);
    waitKey(0);

    Mat normalize1;
    normalize(corrImg45, normalize1, 0, 255, NORM_MINMAX, CV_8U);
    imshow("Corr_Norm 45°", normalize1);
    waitKey(0);


    Mat corrImg90 = Correlazione(img, laplaciano90);
    imshow("Correlazione 90°", corrImg90);
    waitKey(0);

    Mat normalize2;
    normalize(corrImg90, normalize2, 0, 255, NORM_MINMAX, CV_8U);
    imshow("Corr_Norm 90°", normalize2);
    waitKey(0);



    return 0;
}