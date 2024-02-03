#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>
#include"filteredValue.cpp"

/**
 * Applica il filtro di correlazione all'immagine.
 * Aggiunge il padding all'immagine usando copyMakeBorder().
 * Utilizza una doppia iterazione sugli elementi dell'immagine risultante 
 * e chiama la funzione filteredValue() per ottenere il valore filtrato.
 */


using namespace std;
using namespace cv;

/**
 * Per poter appliccare il filtro, dobbiamo aggiungere il Padding. 
 * l’ampiezza del bordo è calcolata come il numero di colonne (o righe) diviso 2, che sarà approssimato per difetto. 
 * Infatti se ho una mask 3x3 dovrò aggiungere 1 riga per ogni lato (3/2=1) dell’immagine, 
 * per una mask 5x5 dovrò aggiungere 2 righe per ogni lato (5/2=2), ecc.
 * 
 * Il doppio ciclo for si occuperà solo di scorrere sugli elementi della matrice che vogliamo restituire.
 * Ad ogni posizione (i,j) richiamerà la funzione filteredValue() il quale restituirà il valore calcolato, 
 * e a cui passeremo la matrice col padding, la maschera, e i due indici 
*/

Mat Correlazione(Mat mat, Mat mask){

    Mat matOutput(mat.rows,mat.cols,mat.type());

    Mat padd;
    int bw = mask.rows/2;   //Numero di righe diviso 2 

    copyMakeBorder(mat,padd,bw,bw,bw,bw,BORDER_DEFAULT); //Aggiunge il padding all'immagine

    for (int i = 0; i < matOutput.rows; i++)
        for (int j = 0; j < matOutput.cols; j++)
            matOutput.at<uchar>(i,j) = filteredValue(padd,mask,i,j);
    
    return matOutput;
}