#ifndef _COLOR_MAPS_HPP_
#define _COLOR_MAPS_HPP_

template<typename colorStyle>
struct colorMap{
	
	double min;
	double max;
	
	colorStyle cStyle;
	
	colorMap(colorStyle cStyle,int min,int max):cStyle(cStyle),min(min),max(max){}
	
	Uint32 applyCM(double value){
		
		double x = (value-min)/(max-min);
		if(x<=0){return cStyle.min();}
		if(x>=1){return cStyle.max();}
		
		return cStyle.rgb(x);
		
	}
};

struct grayscale{
	
	Uint32 min(){return 0x00000000;}
	
	Uint32 max(){return 0xffffffff;}
	
	Uint32 rgb(double x){
		
		Uint32 r = Uint32(round(x*255))<<24;
		Uint32 g = Uint32(round(x*255))<<16;
		Uint32 b = Uint32(round(x*255))<<8;
		Uint32 a = 0x000000ff;
		 
		return Uint32(r|g|b|a);
	}
};



struct lava{
	
	Uint32 min(){return 0x00000000;}
	
	Uint32 max(){return 0xffffffff;}
	
	Uint32 rgb(double x){
		
		Uint32 r = Uint32(255)<<24;
		Uint32 g = Uint32(round((1-x)*255))<<16;
		Uint32 b = Uint32(0)<<8;
		Uint32 a = 0x000000ff;
		 
		return Uint32(r|g|b|a);
	}
};

struct rainbow{
	
	Uint32 min(){return 0x0000ffff;}
	
	Uint32 max(){return 0xff0000ff;}
	
	Uint32 rgb(double x){
		
		double y = (1-x)/0.25;
		
		Uint32 X = floor(y);
		Uint32 Y = floor(255*(y-X));
		
		Uint32 r;
		Uint32 g;
		Uint32 b;
		Uint32 a;
		
		switch(X)
		{
			case 0: r=255;   g=Y;     b=0;    break;
			case 1: r=255-Y; g=255;   b=0;    break;
			case 2: r=0;     g=255;   b=Y;    break;
			case 3: r=0;     g=255-Y; b=255;  break;
			case 4: r=0;     g=0;     b=255;  break;
		}
		
		r = r<<24;
		g = g<<16;
		b = b<<8;
		a = 0x000000ff;
		
		return Uint32(r|g|b|a);
	}
};

struct longRainbow{
	
	Uint32 min(){return 0xff00ffff;}
	
	Uint32 max(){return 0xff0000ff;}
	
	Uint32 rgb(double x){
		
		double y = (1-x)/0.2;
		
		Uint32 X = floor(y);
		Uint32 Y = floor(255*(y-X));
		
		Uint32 r;
		Uint32 g;
		Uint32 b;
		Uint32 a;
		
		switch(X)
		{
			case 0: r=255;   g=Y;     b=0;    break;
			case 1: r=255-Y; g=255;   b=0;    break;
			case 2: r=0;     g=255;   b=Y;    break;
			case 3: r=0;     g=255-Y; b=255;  break;
			case 4: r=Y;     g=0;     b=255;  break;
			case 5: r=255;   g=0;     b=255;  break;
		}
		
		r = r<<24;
		g = g<<16;
		b = b<<8;
		a = 0x000000ff;
		
		return Uint32(r|g|b|a);
	}
};

#endif
