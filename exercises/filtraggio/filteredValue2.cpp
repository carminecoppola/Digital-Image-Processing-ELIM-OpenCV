#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <iostream>

/**
 * filteredValue2() calcola il valore filtrato per un punto specifico nell'immagine con una 
 * maschera data. Utilizza il padding padd per considerare il contorno dell'immagine.
 * La maschera e l'immagine devono essere di tipo float e uchar rispettivamente.
 */


using namespace std;
using namespace cv;


/**
 * value=0 conterrà il valore finale che verrà restituito. 
 * Sebbene dovremo restituire un valore uchar, le operazioni sono di tipo float (abbiamo numeri con la virgola)
 * 
 * il doppio ciclo for si occuperà di effetturare la somma dei prodotti (elemento per elemento)
 * della maschera con un intorno (pad) dim*dim dell’immagine con il padding. 
 * 
 * Notiamo 2 cose
 *  1. gli elementi di mask sono di tipo float;
 *  2. gli elementi di "pad" hanno un OFFSET, con il quale stabiliamo l’intorno.
 * 
 * Ad es, se stessimo calcolato il pixel(0,2), allora: offx = 0, offy = 2.
 * Ovviamente l’intorno di cui teniamo conto è quello del padding.
 * 
 * N.b stiamo facendo la doppia sommatoria di w(s, t)*f(x+s, y+t)
 * dove w è il filtro/maschera e f e l'immagine
*/
uchar filteredValue2(Mat pad, Mat mask, int offx, int offy){
    float value = 0;
    for( int i = 0; i < mask.rows; i++ )
        for( int j = 0; j < mask.cols; j++ )
                value += mask.at<float>(i,j) * pad.at<uchar>( i+offx, j+offy );


    return uchar(value);
}