//Copyright (C) 2011 by Ivan Fratric
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.


#pragma once

//a simple image class with some (very basic) image processing operations
class Image
{
	friend class ImageIO;

protected:
	unsigned char *data;
	long width;
	long height;

	long round(float x);
	float interpolate(float x1,float x2,float x3, float x4, float dx, float dy);

public:

	struct Pixel {
		unsigned char B;
		unsigned char G;
		unsigned char R;
	};

	//constructors/destructor
	Image(void);
	Image(long width, long height);
	Image(const Image& src);
	Image& operator=(const Image &src);
	~Image(void);

	//initializes the image of specified width and height
	//all pixels are set to black
	void Init(long width, long height);

	//property getters
	long GetWidth();
	long GetHeight();
	unsigned char *GetData();

	//pixel getters and setters
	unsigned char GetPixelGray(long x,long y);
	unsigned char GetPixelRed(long x,long y);
	unsigned char GetPixelGreen(long x,long y);
	unsigned char GetPixelBlue(long x,long y);
	Image::Pixel GetPixelColor(long x,long y);
	void SetPixelGray(long x,long y, unsigned char c);
	void SetPixelColor(long x,long y, Image::Pixel rgb);
	void SetPixelRed(long x,long y, unsigned char c);
	void SetPixelGreen(long x,long y, unsigned char c);
	void SetPixelBlue(long x,long y, unsigned char c);

	//returns the color of a pixel at real coordinates (x,y)
	//using bilinear interpolation
	Image::Pixel GetPixelBilinear(float x, float y);

	/////////////////////////////////////////
	//some basic image processing functions//
	/////////////////////////////////////////

	//returns the mean value of pixel intensity
	unsigned char GetMeanGray();
	
	//computes the histogram of image intensity and returns it via histogram parameter
	//parameters:
	//		histogram : an array of 256 components, used to return the histogram
	void GetHistogramGray(long *histogram);
	
	//binarizes the image
	//all pixels with the intensity lower than threshold become black
	//all others become white
	void Binarize(unsigned char threshold);
	
	//flips the image in horizontal direction
	void FlipHorizontal();

	//flips the image in vertical direction
	void FlipVertical();

	//inverts image colors
	void Invert();

	//crops the image
	//returns the subimage with upper left corner at (posx,posy)
	//the returned image is of size (width, height)
	Image Crop(int posx, int posy, int width, int height);

	//resizes the image for a intager facor
	//for example, if factor is 2, returns the image with size (width/2, height/2)
	//each pixel in a new image is obtained as an average of corresponding pixels in the original image
	Image Resize(int factor);

	//computes the convolution of image and a filter
	//parameters
	//		filter : an filterwidth x filterheight array containing the filter coefficients
	//		filterwidth : width of the filter
	//		filterheight : height of the filter
	//		zero : a value that will be added to each pixel component after filtering, 0 by default
	Image Filter(float *filter, long filterwidth, long filterheight);

	//filters the image using Gaussian blur
	//parameters
	//		sigma : the standard deviation of the Gaussian filter
	//		masksize : the size of the corresponding filter
	//				   if set to 0, the masksize will be calculated as sigma*2*2+1
	Image GaussBlur(float sigma, long masksize = 0);

	//paints the whole image with color 'color'
	void Clear(Pixel color);

	//draws a line from point (x1,y1) to point (x2,y2)
	//the line is in color 'color'
	void DrawLine(int x1, int y1, int x2, int y2, Pixel color);
};

inline unsigned char Image::GetPixelGray(long x,long y) {
	unsigned char c;
	long index = 3*((y*width)+x);
	c = (unsigned char)(((long)(data[index])+(long)(data[index+1])+(long)(data[index+2]))/3);
	return c;
}

inline unsigned char Image::GetPixelRed(long x,long y) {
	long index = 3*((y*width)+x);
	return data[index];
}

inline unsigned char Image::GetPixelGreen(long x,long y) {
	long index = 3*((y*width)+x)+1;
	return data[index];
}

inline unsigned char Image::GetPixelBlue(long x,long y) {
	long index = 3*((y*width)+x)+2;
	return data[index];
}

inline void Image::SetPixelRed(long x,long y, unsigned char c) {
	long index = 3*((y*width)+x);
	data[index]=c;
}

inline void Image::SetPixelGreen(long x,long y, unsigned char c) {
	long index = 3*((y*width)+x)+1;
	data[index]=c;
}

inline void Image::SetPixelBlue(long x,long y, unsigned char c) {
	long index = 3*((y*width)+x)+2;
	data[index]=c;
}

inline Image::Pixel Image::GetPixelColor(long x,long y) {
	Image::Pixel rgb;
	long index = 3*((y*width)+x);
	rgb.B = data[index];
	rgb.G = data[index+1];
	rgb.R = data[index+2];
	return rgb;
}

inline void Image::SetPixelGray(long x,long y, unsigned char c) {
	long index = 3*((y*width)+x);
	data[index] = c;
	data[index+1] = c;
	data[index+2] = c;
}

inline void Image::SetPixelColor(long x,long y, Image::Pixel rgb) {
	if(x<0) return;
	if(y<0) return;
	if(x>=width) return;
	if(y>=height) return;
	long index = 3*((y*width)+x);
	data[index] = rgb.B;
	data[index+1] = rgb.G;
	data[index+2] = rgb.R;
}

inline long Image::GetWidth() {
	return width;
}

inline long Image::GetHeight() {
	return height;
}

inline unsigned char *Image::GetData() {
	return data;
}

