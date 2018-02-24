#include "../m2p/m2p.hpp"

#include "Eigen/Core"

#define xSize 80
#define ySize 80

int main(int argc, char** argv)
{
	int i,j;
	
	matrix2pixel test;
	srand((unsigned int) time(NULL));
	
	double **matrixTest;
	matrixTest=(double**)malloc(xSize*sizeof(double*));
	for(i=0;i<xSize;i++){
		matrixTest[i]=(double*)malloc(ySize*sizeof(double));
	}
	
	for(i=0;i<xSize;i++){
		for(j=0;j<ySize;j++){
			matrixTest[i][j]=rand();
		}
	}
	
	double *vectorTest;
	vectorTest=(double*)malloc(xSize*ySize*sizeof(double*));
	
	for(j=0;j<xSize*ySize;j++){
			vectorTest[j]=rand();
	}
	
	Eigen::MatrixXf mat = Eigen::MatrixXf::Random(xSize,ySize);
	
	test.setPersistence();
	
	test.setMinMax(-1,1);
	test.drawMatrix<rainbow,Eigen::MatrixXf>(mat,xSize,ySize,10);
	
	test.unsetMinMax();
	test.drawMatrix<lava,double>(matrixTest,xSize,ySize,10);
	test.drawMatrix<grayscale,double>(vectorTest,xSize,ySize,10);
	
    return 0;
}

