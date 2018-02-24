#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>

#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff


struct grayscale{
	
	Uint32 min(){
		return 0x00000000;
	}
	
	Uint32 max(){
		return 0xffffffff;
	}
	
	Uint32 rgb(double x){
		
		Uint32 r = Uint32(round(x*255))<<24;
		Uint32 g = Uint32(round(x*255))<<16;
		Uint32 b = Uint32(round(x*255))<<8;
		Uint32 a = amask;
		
		printf("%lf %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x 0x%08x 0x%08x \n", x,round(x*255),Uint32(round(x*255)),r,
																							   round(x*255),Uint32(round(x*255)),g,
																							   round(x*255),Uint32(round(x*255)),b,
																							   a,r|g|b|a);
		 
		return Uint32(r|g|b|a);
	}
};



struct lava{
	
	Uint32 min(){
		return 0x00000000;
	}
	
	Uint32 max(){
		return 0xffffffff;
	}
	
	Uint32 rgb(double x){
		
		Uint32 r = Uint32(255)<<24;
		Uint32 g = Uint32(round((1-x)*255))<<16;
		Uint32 b = Uint32(0)<<8;
		Uint32 a = amask;
		
		printf("%lf %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x 0x%08x 0x%08x \n", x,round(x*255),Uint32(round(x*255)),r,
																							   round(x*255),Uint32(round(x*255)),g,
																							   round(x*255),Uint32(round(x*255)),b,
																							   a,r|g|b|a);
		 
		return Uint32(r|g|b|a);
	}
};

typedef struct rainbow{
	
	Uint32 min(){
		return 0x0000ffff;
	}
	
	Uint32 max(){
		return 0xff0000ff;
	}
	
	Uint32 rgb(double x){
		
		double y = (1-x)/0.25;
		
		Uint32 X = floor(y);
		Uint32 Y = floor(255*(y-X));
		
		Uint32 r;
		Uint32 g;
		Uint32 b;
		
		switch(X)
		{
			case 0: r=255;g=Y;b=0;break;
			case 1: r=255-Y;g=255;b=0;break;
			case 2: r=0;g=255;b=Y;break;
			case 3: r=0;g=255-Y;b=255;break;
			case 4: r=0;g=0;b=255;break;
		}
		
		r = r<<24;
		g = g<<16;
		b = b<<8;
		Uint32 a = amask;
		
		/*
		printf("%lf %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x %lf 0x%08x 0x%08x 0x%08x 0x%08x \n", x,round(x*255),Uint32(round(x*255)),r,
																							   round(x*255),Uint32(round(x*255)),g,
																							   round(x*255),Uint32(round(x*255)),b,
																							   a,r|g|b|a);
		 */
		return Uint32(r|g|b|a);
	}
}rainbow;

template<typename T>
struct colorMap{
	
	double min = 0;
	double max = RAND_MAX;
	
	T cm;
	
	colorMap(T cm):cm(cm){}
	
	Uint32 applyCM(double x){
		if(x<min){return cm.min();}
		if(x>max){return cm.max();}
		
		return cm.rgb((x-min)/(max-min));
		
	}
	
	
};

class SDL_Daemon
{
	private:
	
		SDL_Window*   window;
		SDL_Renderer* renderer;
		SDL_Surface*  surface;
		SDL_Texture*  texture;
		
		SDL_Event     event;
		
		bool SDL_running=false;
		
		//Matrix
		
		double** dataMatrix;
		
		int matrixWidth;
		int matrixHeight;
		
		double maxValue;
		double minValue;
		
		//Pixel Matrix
		
		Uint32* pixelMatrix;
		
		
	public:
	
		SDL_Daemon();
		~SDL_Daemon();
		
		void addMatrix(double **dataMatrixInput, int w, int h);
		
		void drawMatrix(double scale);
	
};

void SDL_Daemon::addMatrix(double **dataMatrixInput, int w, int h)
{
	int i,j;
	
	matrixWidth=w;
	matrixHeight=h;
	
	dataMatrix=(double**)malloc(w*sizeof(double*));
	for(i=0;i<w;i++){
		dataMatrix[i]=(double*)malloc(h*sizeof(double));
	}
	
	pixelMatrix=(Uint32*)malloc(w*h*sizeof(Uint32));
	
	maxValue=dataMatrixInput[0][0];
	minValue=dataMatrixInput[0][0];
	
	for(i=0;i<w;i++){
		for(j=0;j<h;j++){
			dataMatrix[i][j]=dataMatrixInput[i][j];
			
			if(dataMatrixInput[i][j]>maxValue){maxValue=dataMatrixInput[i][j];}
			if(dataMatrixInput[i][j]<minValue){minValue=dataMatrixInput[i][j];}
		}
	}
	
	surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
	texture = SDL_CreateTexture(renderer, 
								SDL_PIXELFORMAT_RGBA8888,
								SDL_TEXTUREACCESS_STREAMING,
								w, h);
	
	
}

void SDL_Daemon::drawMatrix(double scale)
{
	int i,j;
	
	SDL_running=true;
	
	SDL_ShowWindow(window);
	SDL_SetWindowSize(window,(int)scale*matrixWidth,(int)scale*matrixHeight);
	
	SDL_LockTexture(texture, NULL, &surface->pixels, &surface->pitch);
	
	rainbow color;
	colorMap<rainbow> cm(color);
	
	for(i=0;i<matrixWidth;i++)
	{
		for(j=0;j<matrixHeight;j++)
		{
			pixelMatrix[i+j*matrixWidth]=cm.applyCM(dataMatrix[i][j]);
		}
	}
	
	memcpy(surface->pixels, pixelMatrix, matrixWidth*matrixHeight * sizeof(Uint32));
	SDL_UnlockTexture(texture);
	
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	while (SDL_running){
		
	    while (SDL_WaitEventTimeout(&event, 10)){
	        switch (event.type){
	            case SDL_QUIT:
	                SDL_running=false;
	                break;
	        }
	    }
	}
}



SDL_Daemon::~SDL_Daemon()
{
	SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

SDL_Daemon::SDL_Daemon()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	
	window = SDL_CreateWindow("",
							  0,
							  0,
	                          0,0,
	                          SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	                          
	//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	
	SDL_running=true;
	
}



int main(int argc, char** argv)
{
	int i,j;
	
	SDL_Daemon test;
	srand((unsigned int) time(NULL));
	
	double **matrixTest;
	
	matrixTest=(double**)malloc(64*sizeof(double*));
	for(i=0;i<64;i++){
		matrixTest[i]=(double*)malloc(64*sizeof(double));
	}
	
	
	
	
	for(i=0;i<64;i++)
	{
		for(j=0;j<64;j++)
		{
			matrixTest[i][j]=rand();
		}
	}
	
	test.addMatrix(matrixTest,64,64);
	
	test.drawMatrix(10);
	
	
    return 0;
}
