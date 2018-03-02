#ifndef _M2P_HPP_
#define _M2P_HPP_

#include <cmath>
#include <vector>

#include <SDL2/SDL.h>

#include "colorMaps.hpp"

#define rmask 0xff000000
#define gmask 0x00ff0000
#define bmask 0x0000ff00
#define amask 0x000000ff

class matrix2pixel
{
	private:
	
		SDL_Window*   window;
		SDL_Renderer* renderer;
		SDL_Surface*  surface;
		SDL_Texture*  texture;
		
		SDL_Event     event;
		
		bool SDL_running = false;
		bool persistence = false;
		
		//MatrixInfo
		
		int matrixWidth = 0;
		int matrixHeight = 0;
		int totalSize = 0;
		
		bool isMinMaxSet = false;
		double matrixMin;
		double matrixMax;
		
		//Pixel Matrix
		
		std::vector<Uint32> pixelMatrix;
		
	public:
	
		matrix2pixel();
		~matrix2pixel();
		
		void setPersistence(){persistence = true;}
		void unsetPersistence(){persistence = true;}
		
		void setMinMax(double min, double max){isMinMaxSet = true;matrixMin=min;matrixMax=max;}
		void unsetMinMax(){isMinMaxSet = false;}
		
		template<typename colorStyle, typename inputMatrixType>
		void drawMatrix(inputMatrixType,int w,int h,double scale);
		
		template<typename colorStyle, typename inputMatrixType>
		void drawMatrix(inputMatrixType**,int w,int h,double scale);
		
		template<typename colorStyle, typename inputMatrixType>
		void drawMatrix(inputMatrixType*,int w,int h,double scale);
		
		void snapshot(char* outputFile){SDL_SaveBMP(surface, outputFile);}
	
};

matrix2pixel::~matrix2pixel()
{
	SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

matrix2pixel::matrix2pixel()
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

template<typename colorStyle, typename inputMatrixType>
void matrix2pixel::drawMatrix(inputMatrixType inputMatrix, int w,int h,double scale)
{
	int i,j;
	
	SDL_running=true;
	
	if((matrixWidth != w) || (matrixHeight != h)){
		matrixWidth=w;
		matrixHeight=h;
		
		surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
		texture = SDL_CreateTexture(renderer, 
								SDL_PIXELFORMAT_RGBA8888,
								SDL_TEXTUREACCESS_STREAMING,
								w, h);
		
		if(totalSize != w*h){
			totalSize = w*h;
			pixelMatrix.resize(totalSize);
		}
		
	}
	
	SDL_ShowWindow(window);
	SDL_SetWindowSize(window,(int)scale*matrixWidth,(int)scale*matrixHeight);
	
	////////////////////////////////////////////////////////////////////
	
	colorStyle color;
	
	if(!isMinMaxSet){
		
		matrixMin=inputMatrix(0,0);
		matrixMax=inputMatrix(0,0);
		
		for(i=0;i<matrixWidth;i++){
			for(j=0;j<matrixHeight;j++){
				if(inputMatrix(i,j)<matrixMin){matrixMin=inputMatrix(i,j);}
				if(inputMatrix(i,j)>matrixMax){matrixMax=inputMatrix(i,j);}
			}
		}
	}
	
	colorMap<colorStyle> cm(color,matrixMin,matrixMax);
	
	SDL_LockTexture(texture, NULL, &surface->pixels, &surface->pitch);	
	{
		for(i=0;i<matrixWidth;i++){
			for(j=0;j<matrixHeight;j++){
				pixelMatrix[i+j*matrixWidth]=cm.applyCM(inputMatrix(i,j));
			}
		}
		
		memcpy(surface->pixels, pixelMatrix.data(), matrixWidth*matrixHeight * sizeof(Uint32));
	}
	SDL_UnlockTexture(texture);
	
	////////////////////////////////////////////////////////////////////
	
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	////////////////////////////////////////////////////////////////////
	
	while (SDL_running && persistence){
		
	    while (SDL_WaitEventTimeout(&event, 10)){
	        switch (event.type){
	            case SDL_QUIT:
	                SDL_running=false;
	                break;
	        }
	    }
	}
}

template<typename T> 
struct pp2data{
	T** data;
	
	double operator()(int i,int j){
		return data[i][j];
	}
};

template<typename colorStyle, typename inputMatrixType>
void matrix2pixel::drawMatrix(inputMatrixType** inputMatrix, int w,int h,double scale){
	drawMatrix<colorStyle,pp2data<inputMatrixType>>(pp2data<inputMatrixType>{inputMatrix},w,h,scale);
}

template<typename T> 
struct p2data{ //Row-major order 
	T* data;
	int nColumns;
	
	double operator()(int i,int j){
		return data[j*nColumns+i];
	}
};

template<typename colorStyle, typename inputMatrixType>
void matrix2pixel::drawMatrix(inputMatrixType* inputMatrix, int w,int h,double scale){
	drawMatrix<colorStyle,p2data<inputMatrixType>>(p2data<inputMatrixType>{inputMatrix,w},w,h,scale);
}

////////////////////////////////////////////////////////////////////////

#endif


