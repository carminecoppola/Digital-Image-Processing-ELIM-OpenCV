#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

int th = 150;

// Funzione per convertire coordinate polari in coordinate cartesiane
void convertPolar(double rho, double theta, Point& p1, Point& p2){

    // Calcolo delle coordinate cartesiane per il punto p1
    int x0 = cvRound(rho * cos(theta));
    int y0 = cvRound(rho * sin(theta));
    int alpha = 1000;
    p1.x = cvRound(x0 + alpha*(-sin(theta)));
    p1.y = cvRound(y0 + alpha*(cos(theta)));

    // Calcolo delle coordinate cartesiane per il punto p2
    p2.x = cvRound(x0 - alpha*(-sin(theta)));
    p2.y = cvRound(y0 - alpha*(cos(theta)));
}

// Funzione per il rilevamento delle linee tramite trasformata di Hough
void houghRette(const Mat& src, Mat& dst){

    // Applicazione del filtro di Gauss per ridurre il rumore
    Mat blur, imgCanny;
    GaussianBlur(src, blur, Size(3,3), 0, 0);

    // Applicazione del rilevamento dei bordi tramite l'algoritmo di Canny
    Canny(blur, imgCanny, 100, 200);
    imshow("Canny", imgCanny);

    // Calcolo della lunghezza massima della retta
    int maxDist = hypot(imgCanny.rows, imgCanny.cols);

    // Creazione della matrice dei voti per la trasformata di Hough
    Mat votes = Mat::zeros(maxDist*2, 180, CV_8UC1);

    // Variabili per le coordinate polari
    double rho, theta;
    double rad = CV_PI/180;

    // Ciclo per il calcolo dei voti per ogni possibile retta
    for(int x = 0; x < imgCanny.rows; x++){
        for(int y = 0; y < imgCanny.cols; y++){
            if(imgCanny.at<uchar>(x,y) == 255){
                for(theta = 0; theta < 180; theta++){
                    rho = cvRound(x * sin((theta-90) * rad) + y * cos((theta-90) * rad)) + maxDist;
                    votes.at<uchar>(rho, theta)++;
                }
            }
        }
    }

    // Vettore per memorizzare le linee trovate
    vector<pair<Point, Point>> lines;

    // Ciclo per estrarre le linee con un numero di voti superiore alla soglia
    for(int r = 0; r < votes.rows; r++){
        for(int t = 0; t < 180; t++){
            if(votes.at<uchar>(r,t) > th){
                rho = r - maxDist;
                theta = (t-90)*rad;
                Point p1, p2;
                convertPolar(rho, theta, p1, p2);
                lines.push_back(make_pair(p1, p2));
            }
        }
    }

    // Copia dell'immagine originale nell'immagine di output
    src.copyTo(dst);

    // Disegno delle linee trovate sull'immagine di output
    for(int i = 0; i < lines.size(); i++){
        pair<Point, Point> coordinates = lines.at(i);
        line(dst, coordinates.first, coordinates.second, Scalar(255, 0, 0), 1, LINE_AA);
    }
}

int main(int argc, char** argv){

    Mat src = imread(argv[1], IMREAD_COLOR);
    Mat dst; 

    if( src.empty() )
        return -1;

    imshow("Original Img", src);

    houghRette(src, dst);
    imshow("Hough", dst);
    waitKey(0);

    return 0;
}
