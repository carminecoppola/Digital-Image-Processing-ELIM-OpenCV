//OpenCV includes
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/imgproc/imgproc.hpp>
#include <opencv4/opencv2/highgui/highgui.hpp>

#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    //Effettuo la lettura dell'immagine
    string image_path = samples::findFile("img/starry_night.jpg");
    Mat img = imread(image_path, IMREAD_COLOR);// imread()
    

    //! [empty]
    if(img.empty()){
        cout << "Could not read the image: " << image_path << endl;
        return 1;
    }

    
    imshow("Display window", img); //Mostra l'immagine nella finestra
    int k = waitKey(0); // Attende che premiamo un tasto sull'immagine
    

    
    if(k == 's'){
        imwrite("starry_night.png", img); //Salva l'immagine
    }
    

    return 0;
}
