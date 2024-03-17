#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
#include<vector>

using namespace std;
using namespace cv;

int tSize;
double th;

class TNode{

	private:
		Rect region;
		TNode *UL, *UR, *LL, *LR;
		vector<TNode *> merged;
		vector<bool> mergedB = vector<bool>(4,false);
		double stdDev, mean;
	public:
		TNode(Rect r){
			region = r;
			UL = UR = LL = LR = nullptr;
		}

		void addRegion(TNode *R){merged.push_back(R);};
		Rect& getRegion(){return region;};

		void setUL(TNode *UL){this->UL = UL;};
		void setUR(TNode *UR){this->UR = UR;};
		void setLL(TNode *LL){this->LL = LL;};
		void setLR(TNode *LR){this->LR = LR;};


		TNode* getUL(){return UL;};
		TNode* getUR(){return UR;};
		TNode* getLL(){return LL;};
		TNode* getLR(){return LR;};

		void setStdDev(double stdDev){this->stdDev = stdDev;};
		double getStdDev(){return stdDev;};

		void setMean(double mean){this->mean = mean;};
		double getMean(){return mean;};

		vector<TNode *> getMerged(){return merged;};

		void setMergedB(int i){mergedB[i] = true;};
		bool getMergedB(int i){return mergedB[i];};

};


TNode* split(Mat& img, Rect R){

	TNode *root = new TNode(R);
	Scalar mean, stdDev;
	meanStdDev(img(R), mean, stdDev);

	root->setMean(mean[0]);
	root->setStdDev(stdDev[0]);
	
	if(R.width > tSize && root->getStdDev() > th) //Se non sono simili
	{
		Rect ul(R.x , R.y, R.height/2 ,R.width/2);
		root->setUL(split(img,ul));

		Rect ur(R.x , R.y + R.width/2 , R.height/2 ,R.width/2);
		root->setUR(split(img,ur));

		Rect lr(R.x + R.height/2, R.y + R.width/2, R.height/2 ,R.width/2);
		root->setLR(split(img,lr));

		Rect ll(R.x + R.height/2, R.y, R.height/2 ,R.width/2);
		root->setLL(split(img,ll));
	}

	rectangle(img, R, Scalar(0));

	return root;
}


void merge(TNode *root){

	if(root->getRegion().width > tSize  && root->getStdDev() > th){

		//Prima riga UL- UR
		if(root->getUL()->getStdDev() <= th && root->getUR()->getStdDev() <= th ){

			root->addRegion(root->getUL());
			root->addRegion(root->getUR());

			root->setMergedB(0);
			root->setMergedB(1);

			if(root->getLR()->getStdDev() <= th && root->getLL()->getStdDev() <= th ){

				root->addRegion(root->getLR());
				root->addRegion(root->getLL());

				root->setMergedB(2);
				root->setMergedB(3);
			}
			else{

				merge(root->getLR());
				merge(root->getLL());				
			}
		}
		//Seconda Colonna UR-LR
		else if(root->getUR()->getStdDev() <= th && root->getLR()->getStdDev() <= th ){

			root->addRegion(root->getUR());
			root->addRegion(root->getLR());

			root->setMergedB(1);
			root->setMergedB(2);

			if(root->getLL()->getStdDev() <= th && root->getUL()->getStdDev() <= th ){

				root->addRegion(root->getLL());
				root->addRegion(root->getUL());

				root->setMergedB(3);
				root->setMergedB(0);
			}
			else{

				merge(root->getLL());
				merge(root->getUL());				
			}
		}
		//Seconda riga LR-LL
		else if(root->getLR()->getStdDev() <= th && root->getLL()->getStdDev() < th ){

			root->addRegion(root->getLR());
			root->addRegion(root->getLL());

			root->setMergedB(2);
			root->setMergedB(3);

			if(root->getUL()->getStdDev() <= th && root->getUR()->getStdDev() <= th ){

				root->addRegion(root->getUL());
				root->addRegion(root->getUR());

				root->setMergedB(0);
				root->setMergedB(1);
			}
			else{

				merge(root->getUL());
				merge(root->getUR());				
			}
		}
		//Prima colonna UL-LL
		else if(root->getLL()->getStdDev() <= th && root->getUL()->getStdDev() <= th ){
			root->addRegion(root->getLL());
			root->addRegion(root->getUL());

			root->setMergedB(3);
			root->setMergedB(0);

			if(root->getUR()->getStdDev() <= th && root->getLR()->getStdDev() <= th ){
				root->addRegion(root->getUR());
				root->addRegion(root->getLR());
				root->setMergedB(1);
				root->setMergedB(2);
			}
			else{
				merge(root->getUR());
				merge(root->getLR());				
			}
		}
		else{
			merge(root->getUL());
			merge(root->getUR());	
			merge(root->getLR());
			merge(root->getLL());	
		}
	}
	else{
		root->addRegion(root);
		root->setMergedB(0);
		root->setMergedB(1);
		root->setMergedB(2);
		root->setMergedB(3);
	}

}

void segment(Mat& img ,TNode* root){

	vector<TNode*> tmp = root->getMerged();

	if(!tmp.size()){
		segment(img, root->getUL());
		segment(img, root->getUR());
		segment(img, root->getLR());
		segment(img, root->getLL());
	}
	else{
		double val = 0;
		for(auto x : tmp){
			val += (int)x->getMean();
		}

		val /= tmp.size(); 
		
		for(auto x: tmp){
			img(x->getRegion()) = (int)val;
		}


		// size() > 1
		if(tmp.size() > 1){

			if(!root->getMergedB(0))
				segment(img, root->getUL());
			if(!root->getMergedB(1))
				segment(img, root->getUR());
			if(!root->getMergedB(2))
				segment(img, root->getLR());
			if(!root->getMergedB(3))
				segment(img, root->getLL());
		}
	}
	

}



int main(int argc, char** argv){

	Mat src = imread( argv[1], IMREAD_GRAYSCALE );
    if( src.empty())
        exit(-1);
    
    th = stod(argv[2]);
    tSize = stoi(argv[3]);
    
    GaussianBlur(src, src, Size(5,5), 0, 0);

    int exponent = log(min(src.cols, src.rows)) / log (2);
    int s = pow(2.0, (double)exponent);
    Rect square = Rect(0,0, s,s);
    src = src(square).clone();
    Mat srcSeg=src.clone();

    TNode *root = split(src, Rect(0,0, src.rows, src.cols));
    merge(root);
    segment(srcSeg,root);

    imshow("Quad Tree", src);
	waitKey(0);
	
    imshow("Segmented", srcSeg);
    waitKey(0);


	return 0;
}