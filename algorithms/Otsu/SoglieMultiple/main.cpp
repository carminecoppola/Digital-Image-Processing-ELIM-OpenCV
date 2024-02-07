#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// Funzione per normalizzare un istogramma
vector<float> normHistogram(const Mat& src){

    // Creazione di un vettore per l'istogramma inizializzato con 256 elementi float a 0
    vector<float> hist(256, 0.0f);

    // Calcolo dell'istogramma
    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            hist.at(src.at<uchar>(x,y))++;
        }
    }

    // Normalizzazione dell'istogramma dividendo per il numero totale di pixel
    for(int i = 0; i < 256; i++){
        hist.at(i) /= src.rows*src.cols;
    }

    return hist;
}

// Calcolo della media globale
float globalAvg(vector<float> hist){

    float globAvg = 0.0f;

    for(int i = 0; i < 256; i++){
        globAvg += i * hist.at(i);
    }

    return globAvg;
}

// Funzione per trovare le due soglie ottimali utilizzando il metodo di Otsu con due soglie (k-star)
vector<int> kStar(vector<float> hist, float globalAvg){

    // Creazione di vettori per le probabilità, le medie cumulative e le soglie
    vector<float> prob(3, 0.0f);
    vector<float> cumAvg(3, 0.0f);
    vector<int> kstar(2, 0);
    // Variabile per memorizzare la massima varianza interclasse
    float maxVariance = 0.0f;

    // Loop per calcolare le soglie ottimali
    for(int i = 0; i < 254; i++){
        prob.at(0) += hist.at(i);
        cumAvg.at(0) += i * hist.at(i);

        for(int j = i+1; j < 255; j++){
            prob.at(1) += hist.at(j);
            cumAvg.at(1) += j * hist.at(j);

            for(int k = j+1; k < 256; k++){
                prob.at(2) += hist.at(k);
                cumAvg.at(2) += k * hist.at(k);

                // Calcolo della varianza interclasse
                float sigma = 0.0f;
                for(int z = 0; z < 3; z++){
                    sigma += prob.at(z) * pow((cumAvg.at(z)/prob.at(z) - globalAvg), 2); 
                }

                // Aggiornamento della massima varianza e delle soglie
                if(sigma > maxVariance){
                    maxVariance = sigma;
                    kstar.at(0) = i;
                    kstar.at(1) = j;
                }
            }
            prob.at(2) = cumAvg.at(2) = 0.0f;
        }
        prob.at(1) = cumAvg.at(1) = 0.0f;
    }
    return kstar;
}

// Funzione per applicare la soglia alle immagini
void myThreshold(const Mat& src, Mat& dst, vector<int> kstar){
    // Inizializza l'immagine di destinazione con tutti i pixel a 0
    dst = Mat::zeros(src.size(), src.type());

    // Applica la soglia alle immagini
    for(int x = 0; x < src.rows; x++){
        for(int y = 0; y < src.cols; y++){
            if( src.at<uchar>(x,y) >= kstar.at(1))
                dst.at<uchar>(x,y) = 255; // Se il pixel supera la soglia superiore, lo impostiamo a 255 (bianco)
            else if(src.at<uchar>(x,y) >= kstar.at(0))
                dst.at<uchar>(x,y) = 127; // Se il pixel supera la soglia inferiore, lo impostiamo a 127 (grigio)
        }
    }
}


void otsu2K(const Mat& src){

    // Applica un filtro di sfocatura gaussiana all'immagine di input
    Mat blur;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    vector<float> hist = normHistogram(blur);
    float globAvg = globalAvg(hist);
    vector<int> k = kStar(hist, globAvg);

    // Applica la soglia trovata all'immagine sfocata
    Mat dst;
    myThreshold(blur, dst, k);

    imshow("otsu2k", dst);
}


int main(int argc, char** argv){

    Mat src;
    src = imread(argv[1], IMREAD_GRAYSCALE);
    
    if( src.empty() )
        return -1;

    imshow("Original img", src);

    otsu2K(src);
    waitKey(0);

    return 0;
}
