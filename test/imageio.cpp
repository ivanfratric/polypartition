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

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "image.h"
#include "imageio.h"

const char *ImageIO::GetFileExtension(const char *filename) {
	//first remove the folder if present
	const char *filename2,*tmp,*extension;
	filename2 = strrchr(filename,'/');
	tmp = strrchr(filename,'\\');
	if(tmp > filename2) filename2 = tmp;
	if(!filename2) filename2 = filename;
	//now find the dot
	extension = strrchr(filename2,'.');
	if(extension) extension++;
	else extension = filename2 + strlen(filename2);
	return extension;
}

int ImageIO::GetImageType(const char *filename) {
	const char *extension = GetFileExtension(filename);
	if((strcmp(extension,"bmp")==0)||(strcmp(extension,"BMP")==0)) {
		return IMGTYPE_BMP;
	} else if((strcmp(extension,"ppm")==0)||(strcmp(extension,"PPM")==0)) {
		return IMGTYPE_PPM;
	} else if((strcmp(extension,"pgm")==0)||(strcmp(extension,"PGM")==0)) {
		return IMGTYPE_PGM;
	} else if((strcmp(extension,"raw")==0)||(strcmp(extension,"RAW")==0)) {
		return IMGTYPE_RAW;
	} else return IMGTYPE_UNSUPPORTED;
}

void ImageIO::LoadImage(const char *filename, Image *image) {
	int imgType = GetImageType(filename);
	if(imgType == IMGTYPE_UNSUPPORTED) {
		printf("Error loading image %s, unsupported image type!\n", filename);
		return;
	}
	LoadImage(filename, image, imgType);
}

void ImageIO::LoadImage(const char *filename, Image *image, int imageType) {
	switch(imageType) {
		case IMGTYPE_BMP:
			LoadImageBMP(filename, image);
			break;
		case IMGTYPE_PPM:
			LoadImagePPM(filename, image);
			break;
		case IMGTYPE_PGM:
			LoadImagePGM(filename, image);
			break;
		case IMGTYPE_RAW:
			LoadImageRAW(filename, image);
			break;
	}
}

void ImageIO::LoadImageBMP(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"rb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	char header[2];
	fread(header,1,2,fp);
	if(!((header[0]=='B')&&(header[1]=='M'))) {
		fclose(fp);
		printf("Error loading image %s, wrong file format!\n",filename);
		return;
	}

	long bmOffset;
	fseek(fp,10,SEEK_SET);
	fread(&bmOffset,4,1,fp);

	fseek(fp,18,SEEK_SET);
	fread(&(image->width),4,1,fp);
	fread(&(image->height),4,1,fp);

	short bpp;
	fseek(fp,28,SEEK_SET);
	fread(&bpp,2,1,fp);
	if(!((bpp==8)||(bpp==24))) {
		fclose(fp);
		printf("Error loading image %s, can only load BMP files with 8 or 24 bpp!\n",filename);
		return;
	}

	long compression;
	fread(&compression,4,1,fp);
	if(compression) {
		fclose(fp);
		printf("Error loading image %s, can only load uncompressed BMP files!\n",filename);
		return;
	}

	if(image->data) free(image->data);
	image->data = (unsigned char *)malloc(image->height * image->width * 3);

	if(bpp == 8) {

		//bytes per row
		long bpr,tmp;
		tmp = image->width % 4;
		if(tmp == 0) {
			bpr = image->width;
		} else {
			bpr = image->width + 4 - tmp;
			tmp = 4 - tmp;
		}


		long bmSize = bpr * image->height;
		unsigned char *buffer = (unsigned char *)malloc(bmSize);
		fseek(fp,bmOffset,SEEK_SET);
		fread(buffer,1,bmSize,fp);

		long pos = 0;
		for(long i=0;i<image->height;i++) {
			for(long j=0;j<image->width;j++) {
				
				image->SetPixelGray(j,image->height - i - 1,buffer[pos]);
				pos++;
			}
			pos += tmp;
		}

		free(buffer);

	} else if (bpp == 24) {

		//bytes per row
		long bpr,tmp;
		tmp = (image->width*3) % 4;
		if(tmp == 0) {
			bpr = image->width * 3;
		} else {
			bpr = image->width * 3 + 4 - tmp;
			tmp = 4 - tmp;
		}

		long bmSize = bpr * image->height;
		unsigned char *buffer = (unsigned char *)malloc(bmSize);
		fseek(fp,bmOffset,SEEK_SET);
		fread(buffer,1,bmSize,fp);

		long pos = 0;
		Image::Pixel rgb;
		for(long i=0;i<image->height;i++) {
			for(long j=0;j<image->width;j++) {
				rgb.B = buffer[pos++];
				rgb.G = buffer[pos++];
				rgb.R = buffer[pos++];
				image->SetPixelColor(j,image->height - i - 1,rgb);
			}
			pos += tmp;
		}

		free(buffer);	
	}

	fclose(fp);
}

void ImageIO::SaveImage(const char *filename, Image *image) {
	int imgType = GetImageType(filename);
	if(imgType == IMGTYPE_UNSUPPORTED) {
		printf("Error saving image to %s, unknown image format!\n", filename);
		return;
	}
	SaveImage(filename, image, imgType);
}

void ImageIO::SaveImage(const char *filename, Image *image, int imageType) {
	switch(imageType) {
		case IMGTYPE_BMP:
			SaveImageBMP(filename, image);
			break;
		case IMGTYPE_PPM:
			SaveImagePPM(filename, image);
			break;
		case IMGTYPE_PGM:
			SaveImagePPM(filename, image);
			break;
		case IMGTYPE_RAW:
			SaveImageRAW(filename, image);
			break;
	}
}

void ImageIO::SaveImageBMP(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"wb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	char header[2];
	long tmpl;
	short tmps;

	//header
	header[0] = 'B';
	header[1] = 'M';
	fwrite(header,2,1,fp);

	//filesize;
	long rowsize;
	long tmp = (image->width*3)%4;
	if(tmp==0) rowsize = image->width*3;
	else rowsize = image->width*3 + 4 - tmp;
	unsigned char *row = (unsigned char *)malloc(rowsize);
	tmpl = 54 + rowsize*image->height;
	fwrite(&tmpl,4,1,fp);

	tmps = 0;
	fwrite(&tmps,2,1,fp);
	fwrite(&tmps,2,1,fp);

	//offset to the beginning of bm data
	tmpl = 54;
	fwrite(&tmpl,4,1,fp);

	//info header size
	tmpl = 40;
	fwrite(&tmpl,4,1,fp);

	//size
	tmpl = image->width;
	fwrite(&tmpl,4,1,fp);
	tmpl = image->height;
	fwrite(&tmpl,4,1,fp);

	tmps = 1;
	fwrite(&tmps,2,1,fp);
	tmps = 24; //bpp
	fwrite(&tmps,2,1,fp);
	tmpl = 0;
	fwrite(&tmpl,4,1,fp);
	fwrite(&tmpl,4,1,fp);
	fwrite(&tmpl,4,1,fp);
	fwrite(&tmpl,4,1,fp);
	fwrite(&tmpl,4,1,fp);
	fwrite(&tmpl,4,1,fp);

	//actual bitmap data
	for(long i = 0; i<image->height;i++) {
		memset(row,0,rowsize);
		memcpy(row,image->data + (3*image->width)*(image->height-i-1),3*image->width);
		fwrite(row,rowsize,1,fp);
	}

	free(row);

	fclose(fp);
}

void ImageIO::LoadImagePPM(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"rb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	long filesize;
	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	unsigned char *buffer = (unsigned char *)malloc(filesize);
	fread(buffer,1,filesize,fp);

	char id[1024];
	long sizex, sizey, levels;
	sscanf((char *)buffer,"%s\n%ld %ld\n%ld\n",id,&sizex,&sizey,&levels);
	
	if((strncmp(id,"P6",2)!=0)||(levels!=255)) {
		free(buffer);
		fclose(fp);
		printf("Error loading image %s, wrong file format!\n",filename);
		return;
	}

	image->width = sizex;
	image->height = sizey;

	if(image->data) free(image->data);
	image->data = (unsigned char *)malloc(image->height * image->width * 3);

	long pos = filesize - sizex*sizey*3;
	for(long i = 0; i < sizey;i++) {
		for(long j = 0; j < sizex;j++) {
			Image::Pixel rgb;
			rgb.R = buffer[pos++];
			rgb.G = buffer[pos++];
			rgb.B = buffer[pos++];
			image->SetPixelColor(j,i,rgb);
		}
	}

	free(buffer);

	fclose(fp);
}

void ImageIO::LoadImagePGM(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"rb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	long filesize;
	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);

	unsigned char *buffer = (unsigned char *)malloc(filesize);
	fread(buffer,1,filesize,fp);

	char id[1024];
	long sizex, sizey, levels;
	sscanf((char *)buffer,"%s\n%ld %ld\n%ld\n",id,&sizex,&sizey,&levels);
	
	if((strncmp(id,"P5",2)!=0)||(levels!=255)) {
		free(buffer);
		fclose(fp);
		printf("Error loading image %s, wrong file format!\n",filename);
		return;
	}

	image->width = sizex;
	image->height = sizey;

	if(image->data) free(image->data);
	image->data = (unsigned char *)malloc(image->height * image->width * 3);

	long pos = filesize - sizex*sizey;
	for(long i = 0; i < sizey;i++) {
		for(long j = 0; j < sizex;j++) {
			image->SetPixelGray(j,i,buffer[pos]);
			pos++;
		}
	}

	free(buffer);

	fclose(fp);
}

void ImageIO::SaveImagePPM(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"wb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	fprintf(fp,"P6\n%ld %ld\n255\n",image->width,image->height);
	
	long sizex = image->width;
	long sizey = image->height;
	unsigned char *buffer = (unsigned char *)malloc(image->width * image->height * 3);
	long pos = 0;
	for(long i = 0; i < sizey;i++) {
		for(long j = 0; j < sizex;j++) {
			Image::Pixel rgb = image->GetPixelColor(j,i);
			buffer[pos++] = rgb.R;
			buffer[pos++] = rgb.G;
			buffer[pos++] = rgb.B;
		}
	}

	fwrite(buffer,1,image->width * image->height * 3,fp);

	free(buffer);
	fclose(fp);
}

void ImageIO::SaveImagePGM(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"wb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	fprintf(fp,"P5\n%ld %ld\n255\n",image->width,image->height);
	
	long sizex = image->width;
	long sizey = image->height;
	unsigned char *buffer = (unsigned char *)malloc(image->width * image->height);
	long pos = 0;
	for(long i = 0; i < sizey;i++) {
		for(long j = 0; j < sizex;j++) {
			buffer[pos++] = image->GetPixelGray(j,i);
		}
	}

	fwrite(buffer,1,image->width * image->height,fp);

	free(buffer);
	fclose(fp);
}

void ImageIO::LoadImageRAW(const char *filename, Image *image, long width, long height) {
	FILE *fp = fopen(filename,"rb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	if((width==0)||(height==0)) {
		long filesize;
		fseek(fp,0,SEEK_END);
		filesize = ftell(fp);	
		fseek(fp,0,SEEK_SET);
		width = (long)sqrt((double)filesize);
		height = width;
		if((height*width)!=filesize) {
			fclose(fp);
			printf("Error loading image %s, wrong file format!\n",filename);
			return;	
		}
	}

	image->width = width;
	image->height = height;
	if(image->data) free(image->data);
	image->data = (unsigned char *)malloc(image->height * image->width * 3);

	unsigned char *buffer = (unsigned char *)malloc(image->width * image->height);
	fread(buffer,1,image->width * image->height,fp);

	long pos = 0;
	for(long i = 0; i < height;i++) {
		for(long j = 0; j < width;j++) {
			unsigned char c = buffer[pos++];
			image->SetPixelGray(j,i,c);
		}
	}

	free(buffer);
	fclose(fp);
}

void ImageIO::SaveImageRAW(const char *filename, Image *image) {
	FILE *fp = fopen(filename,"wb");
	if(!fp) {
		printf("Error opening %s!\n", filename);
		return;
	}

	long sizex = image->width;
	long sizey = image->height;
	unsigned char *buffer = (unsigned char *)malloc(image->width * image->height);
	
	long pos = 0;
	for(long i = 0; i < sizey;i++) {
		for(long j = 0; j < sizex;j++) {
			unsigned char c = image->GetPixelGray(j,i);
			buffer[pos++] = c;
		}
	}

	fwrite(buffer,1,image->width * image->height,fp);

	free(buffer);
	fclose(fp);
}
