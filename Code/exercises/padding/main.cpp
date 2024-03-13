/* Traccia:
    - Realizzare una funzione che effettui il padding di un'immagine 
    - Sostituire al valore di ogni pixel il valore medio dei livelli di grigio
      in un intorno 3x3
*/
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

/**
    * Il Padding ci serve perché senza di esso faremmo un accesso illegare alla memoria quando andiamo a calcolare degli intorni: 
    * si pensi al primo pixel (0,0), se vogliamo calcolare un intorno 3x3 ci accorgeremo che sopra e a sinistra non abbiamo pixel, 
    * e ciò comporterà un errore; tuttavia se aggiungiamo un padding (pixel fantoccio) a quei lati, potremo calcolare l’intorno.
    * Se l’intorno è 3x3 ne basta 1, se 5x5 allora 2, se 7x7 allora 3, ecc.
*/


/**
 * In subMat3x3, inizializziamo a 0 (nero) la somma sum, di tipo Vec3i. 
 * Il motivo per cui usiamo un vector di interi (e non byte) e perché i byte hanno valore massimo 255, dunque andremmo subito in overflow.
 * Usiamo un doppio ciclo for dove sommiamo tutti gli elementi di una sottomatrice 3x3 che parte da (istart,jstart).
 * Infine ritorniamo sum, effettuando un cast a Vec3b (perché è il tipo che vogliamo ritornare).
 * 
 * 
 * Il motivo per cui istart e jstart partono da (-1) è perché vogliamo considerare il “centro del quadrato 3x3”, 
 * e dunque se (i,j) rappresentano il centro, allora (i-1, j-1) rappresentano l’angolo in alto a sx.
*/

Vec3b subMat3x3Color(Mat img, int istart, int jstart){
    Vec3i sum(0, 0, 0);

    for(int i = istart-1; i < istart+2; i++)
        for(int j = jstart-1; j < jstart+2; j++)
            sum += img.at<Vec3b>(i,j);

    return Vec3b(sum/9);

}


unsigned char subMat3x3BW(Mat img, int istart, int jstart){
    int sum = 0;

    for(int i = istart-1; i < istart+2; i++)
        for(int j = jstart-1; j < jstart+2; j++)
            sum += img.at<unsigned char>(i,j);

    return (unsigned char)(sum/9);

}

 /**
     * 1. creazione dell’immagine con padding. Aggiungeremo 1 vector di pixel neri ad ogni lato;
     * 2. definiamo la matrice result. Ovviamente avrà lo stesso numero di righe e colonne, e lo stesso tipo dell’immagine
     * 3. Effettuiamo il calcolo. Usiamo un doppio ciclo for perché vogliamo iterare su ogni elemento della matrice.
     * 
     * Se che vogliamo fare questa operazione su un’immagine a colori, ogni pixel sarà composto da 3 valori (BGR), -> Scalar value(0,0,0) 
     * e i pixel li  dovremmo rappresentare più come Vec3b (3 canali, byte).
     * Il valore del pixel nell’intorno 3x3 sarà calcolato attraverso la funzione submean3x3.
    */

int main(int argc, char** argv ){

    // string img_path = samples::findFile("img/starry_night.jpg"); //Statico
    // Mat img = imread(img_path, IMREAD_GRAYSCALE);

    Mat img = imread(argv[1], IMREAD_GRAYSCALE); //Dinamico

    if( img.empty() ){
        return -1;
    }
    
    namedWindow("Example 1", WINDOW_AUTOSIZE);
    imshow("Example 1",img);
    waitKey(0);
    cout<<img.size()<<endl;
    

    Mat imgPadding;
    // Scalar value(0, 0 ,0); //Color (RGB) version
    Scalar value(0);
    copyMakeBorder(img, imgPadding, 1, 1, 1, 1, BORDER_CONSTANT); // Con intorno 3x3 aggiungiamo 1 a tutti quanti
    namedWindow("Example Padding",WINDOW_AUTOSIZE);
    imshow("Example Padding",imgPadding);
    waitKey(0);
    cout<<imgPadding.size()<<endl;
    

    Mat result(img.rows, img.cols, img.type());

    for (int i = 1; i < imgPadding.rows - 1; i++){
        for (int j = 1; j < imgPadding.cols - 1; j++){
            result.at<unsigned char>(i-1,j-1) = subMat3x3BW(imgPadding, i, j);
            //result.at<Vec3b>(i-1, j-1) = subMat3x3Color(imgPadded, i, j);       //se lo volessimo a colori
        }
    }
    
    namedWindow("Example Result",WINDOW_AUTOSIZE);
    imshow("Example Result", result);
    waitKey(0);
    cout<<result.size()<<endl;
    


    return 0;
}