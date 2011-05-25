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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "image.h"

Image::Image(void)
{
	width = 0;
	height = 0;
	data = NULL;
}

Image::Image(long width, long height)
{
	this->width = width;
	this->height = height;
	data = (unsigned char *)malloc(3*width*height);
	memset(data,0,3*width*height);
}

Image::Image(const Image& src) {
	width = src.width;
	height = src.height;
	data = (unsigned char *)malloc(3*width*height);
	memcpy(data,src.data,3*width*height);
}

Image& Image::operator=(const Image &src) {
	width = src.width;
	height = src.height;
	if(data) free(data);
	data = (unsigned char *)malloc(3*width*height);
	memcpy(data,src.data,3*width*height);
	return *this;
}

Image::~Image(void)
{
	if(data) free(data);
}

void Image::Init(long width, long height)
{
	if(data) free(data);
	this->width = width;
	this->height = height;
	data = (unsigned char *)malloc(3*width*height);
	memset(data,0,3*width*height);
}

unsigned char Image::GetMeanGray() {
	long sum = 0;
	for(long i = 0; i <height; i++) {
		for(long j = 0; j<width;j++) {
			sum += GetPixelGray(j,i);
		}
	}
	return (unsigned char)(sum/(width*height));
}

void Image::Binarize(unsigned char threshold) {
	unsigned char c;
	for(long i = 0; i <height; i++) {
		for(long j = 0; j<width;j++) {
			c = GetPixelGray(j,i);
			if(c > threshold) {
				SetPixelGray(j,i,255);
			} else {
				SetPixelGray(j,i,0);
			}
		}
	}
}

void Image::FlipHorizontal() {
	unsigned char *newdata;
	newdata = (unsigned char *)malloc(height*width*3);
	for(long i = 0; i <height; i++) {
		for(long j = 0; j<width;j++) {
			long index1 = 3*((i*width)+j);
			long index2 = 3*((i*width)+width-j-1);
			newdata[index1] = data[index2];
			index1++; index2++;
			newdata[index1] = data[index2];
			index1++; index2++;
			newdata[index1] = data[index2];
			index1++; index2++;
		}
	}
	free(data);
	data = newdata;
}

void Image::FlipVertical() {
	unsigned char *newdata;
	newdata = (unsigned char *)malloc(height*width*3);
	for(long i = 0; i <height; i++) {
		for(long j = 0; j<width;j++) {
			long index1 = 3*((i*width)+j);
			long index2 = 3*(((height-i-1)*width)+j);
			newdata[index1] = data[index2];
			index1++; index2++;
			newdata[index1] = data[index2];
			index1++; index2++;
			newdata[index1] = data[index2];
			index1++; index2++;
		}
	}
	free(data);
	data = newdata;
}

Image::Pixel Image::GetPixelBilinear(float x, float y) {
	Image::Pixel c={0,0,0},c1,c2,c3,c4;
	if(x<0) return c;
	if(y<0) return c;
	if(x>(width-1)) return c;
	if(y>(height-1)) return c;
	long x1,y1;
	float dx,dy;
	x1=(long)floor(x);
	y1=(long)floor(y);
	dx=x-x1;
	dy=y-y1;
	c1=GetPixelColor(x1,y1);
	c2=GetPixelColor(x1+1,y1);
	c3=GetPixelColor(x1,y1+1);
	c4=GetPixelColor(x1+1,y1+1);
	c.R=(unsigned char)round(interpolate(c1.R,c2.R,c3.R,c4.R,dx,dy));
	c.G=(unsigned char)round(interpolate(c1.G,c2.G,c3.G,c4.G,dx,dy));
	c.B=(unsigned char)round(interpolate(c1.B,c2.B,c3.B,c4.B,dx,dy));
	return(c);
}

float Image::interpolate(float x1,float x2,float x3, float x4, float dx, float dy) {
	float x5,x6;
	x5=x2*dx+x1*(1-dx);
	x6=x4*dx+x3*(1-dx);
	return(x6*dy+x5*(1-dy));
}

long Image::round(float x) {
	if((x-floor(x))<0.5) return (long)floor(x);
	else return (long)ceil(x);
}

void Image::GetHistogramGray(long *histogram) {
	for(long i =0;i<256;i++) histogram[i] = 0;
	for(long i = 0; i <height; i++) {
		for(long j = 0; j<width;j++) {
			histogram[GetPixelGray(j,i)]++;
		}
	}
}

void Image::Invert() {
	long i,n = 3*width*height;
	for(i=0;i<n;i++) {
		data[i]=255-data[i];
	}
}

Image Image::Crop(int posx, int posy, int width, int height) {
	Image resultimg;
	Image::Pixel rgb;
	resultimg.Init(width,height);
	long i,j;
	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			rgb = this->GetPixelColor(j+posx,i+posy);
			resultimg.SetPixelColor(j,i,rgb);
		}
	}
	return resultimg;
}

Image Image::Resize(int factor) {
	long width,height;
	int factor2;
	long sumR, sumG, sumB;

	width = this->width / factor;
	height = this->height / factor;
	factor2 = factor*factor;

	Image resultimg(width,height);
	Image::Pixel rgb;
	long i,j,i2,j2,minx,miny,maxx,maxy;

	for(i=0;i<height;i++) {
		for(j=0;j<width;j++) {
			minx = j*factor;
			miny = i*factor;
			maxx = minx+factor;
			maxy = miny+factor;
			
			sumR = 0; sumG = 0; sumB = 0;

			for(i2=miny;i2<maxy;i2++) {
				for(j2=minx;j2<maxx;j2++) {
					rgb = GetPixelColor(j2,i2);
					sumR += rgb.R;
					sumG += rgb.G;
					sumB += rgb.B;
				}
			}
			rgb.R = (unsigned char)round(sumR/((float)factor2));
			rgb.G = (unsigned char)round(sumG/((float)factor2));
			rgb.B = (unsigned char)round(sumB/((float)factor2));
			resultimg.SetPixelColor(j,i,rgb);
		}
	}
	return resultimg;
}

Image Image::Filter(float *filter, long filterwidth, long filterheight) {
	Image ret(width,height);
	long i1,j1,i2,j2;
	long fox,foy,filtersize;
	long x,y;
	float sumr,sumg,sumb;
	long offset1,offset2,offset3;
	unsigned char *data1,*data2;
	fox = filterwidth/2;
	foy = filterheight/2;
	filtersize = filterwidth*filterheight;
	data1 = ret.GetData();
	data2 = data;
	for(i1=0;i1<height;i1++) {
		for(j1=0;j1<width;j1++) {
			offset1 = (i1*width+j1)*3;
			sumr = 0;
			sumg = 0;
			sumb = 0;
			for(i2=-foy;i2<=foy;i2++) {
				for(j2=-fox;j2<=fox;j2++) {
					x = j1+j2;
					y = i1+i2;
					if(x<0) x = 0;
					if(y<0) y = 0;
					if(x>=width) x = width-1;
					if(y>=height) y = height-1;
					offset2 = (y*width+x)*3;
					offset3 = (i2+foy)*filterwidth+j2+fox;
					sumr += data2[offset2]*filter[offset3];
					sumg += data2[offset2+1]*filter[offset3];
					sumb += data2[offset2+2]*filter[offset3];
				}
			}
			data1[offset1] = (unsigned char)round(sumr);
			data1[offset1+1] = (unsigned char)round(sumg);
			data1[offset1+2] = (unsigned char)round(sumb);
		}
	}
	return ret;
}

Image Image::GaussBlur(float sigma, long masksize) {
	Image ret;
	float *filter;
	long fsize;
	long i,j;
	long fo;
	float pi = 3.1415926538f;

	if(!masksize) masksize = round(sigma)*2*2+1;

	fsize = masksize*masksize;
	fo = masksize/2;
	filter = (float *)malloc(fsize*sizeof(float));

	for(i=-fo;i<=fo;i++) {
		for(j=-fo;j<=fo;j++) {
			filter[(i+fo)*masksize+j+fo] = (1/(2*pi*sigma))*exp(-(i*i+j*j)/(2*sigma*sigma));
		}
	}

	float sum = 0;
	for(i=0;i<fsize;i++) sum += filter[i];
	for(i=0;i<fsize;i++) filter[i]=filter[i]/sum;

	ret = this->Filter(filter,masksize,masksize);

	delete filter;
	return ret;
}

void Image::Clear(Pixel color) {
	long x,y;
	for(y=0;y<height;y++) {
		for(x=0;x<width;x++) {
			SetPixelColor(x,y,color);
		}
	}
}

void Image::DrawLine(int x1, int y1, int x2, int y2, Pixel color) {
	long i;
	long y;
	long dx,dy;
	dx=labs(x2-x1);
	dy=labs(y2-y1);
	if(dx>dy) {
		if(x2>x1) {
			for(i=x1;i<=x2;i++) {
				y=(long)(((y2-(float)(y1))/(x2-x1))*(i-x1)+y1);
				SetPixelColor(i,y,color);
			}
		} else {
			for(i=x2;i<=x1;i++) {
				y=(long)(((y1-(float)(y2))/(x1-x2))*(i-x2)+y2);
				SetPixelColor(i,y,color);
			}
		}
	} else {
		if(y2>y1) {
			for(i=y1;i<=y2;i++) {
				y=(long)(((x2-(float)(x1))/(y2-y1))*(i-y1)+x1);
				SetPixelColor(y,i,color);
			}
		} else {
			for(i=y2;i<=y1;i++) {
				y=(long)(((x1-(float)(x2))/(y1-y2))*(i-y2)+x2);
				SetPixelColor(y,i,color);
			}
		}
	}
}
