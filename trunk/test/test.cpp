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
#include <list>
#include <limits>

using namespace std;

#include "polypartition.h"

#include "image.h"
#include "imageio.h"

void ReadPoly(FILE *fp, TPPLPoly *poly) {
	int i,numpoints,hole;
	float x,y;

	fscanf(fp,"%d\n",&numpoints);
	poly->Init(numpoints);

	fscanf(fp,"%d\n",&hole);
	if(hole) poly->SetHole(true);

	for(i=0;i<numpoints;i++) {
		fscanf(fp,"%g %g\n",&x, &y);
		(*poly)[i].x = x;
		(*poly)[i].y = y;
	}
}

void ReadPoly(const char *filename, TPPLPoly *poly) {
	FILE *fp = fopen(filename,"r");
	if(!fp) {
		printf("Error reading file %s\n", filename);
		return;
	}
	ReadPoly(fp,poly);
	fclose(fp);	
}

void ReadPolyList(FILE *fp, list<TPPLPoly> *polys) {
	int i,numpolys;
	TPPLPoly poly;

	polys->clear();
	fscanf(fp,"%d\n",&numpolys);
	for(i=0;i<numpolys;i++) {
		ReadPoly(fp,&poly);
		polys->push_back(poly);
	}
}

void ReadPolyList(const char *filename, list<TPPLPoly> *polys) {
	FILE *fp = fopen(filename,"r");
	if(!fp) {
		printf("Error reading file %s\n", filename);
		return;
	}
	ReadPolyList(fp,polys);
	fclose(fp);
}


void WritePoly(FILE *fp, TPPLPoly *poly) {
	int i,numpoints;
	numpoints = poly->GetNumPoints();

	fprintf(fp,"%d\n",numpoints);
	
	if(poly->IsHole()) {
		fprintf(fp,"1\n");
	} else {
		fprintf(fp,"0\n");
	}

	for(i=0;i<numpoints;i++) {
		fprintf(fp,"%g %g\n",(*poly)[i].x, (*poly)[i].y);
	}
}

void WritePoly(const char *filename, TPPLPoly *poly) {
	FILE *fp = fopen(filename,"w");
	if(!fp) {
		printf("Error writing file %s\n", filename);
		return;
	}
	WritePoly(fp,poly);
	fclose(fp);	
}

void WritePolyList(FILE *fp, list<TPPLPoly> *polys) {
	list<TPPLPoly>::iterator iter;

	fprintf(fp,"%ld\n",polys->size());

	for(iter = polys->begin(); iter != polys->end(); iter++) {
		WritePoly(fp,&(*iter));
	}
}

void WritePolyList(const char *filename, list<TPPLPoly> *polys) {
	FILE *fp = fopen(filename,"w");
	if(!fp) {
		printf("Error writing file %s\n", filename);
		return;
	}
	WritePolyList(fp,polys);
	fclose(fp);	
}

void DrawPoly(Image *img, TPPLPoly *poly, tppl_float xmin, tppl_float xmax, tppl_float ymin, tppl_float ymax) {
	TPPLPoint p1,p2,p1img,p2img,polymin,imgmin;
	long i;
	Image::Pixel color={0,0,0};

	polymin.x = xmin;
	polymin.y = ymin;
	imgmin.x = 5;
	imgmin.y = 5;

	tppl_float polySizeX = xmax - xmin;
	tppl_float polySizeY = ymax - ymin;
	tppl_float imgSizeX = (tppl_float)img->GetWidth()-10;
	tppl_float imgSizeY = (tppl_float)img->GetHeight()-10;
	
	tppl_float scalex = 0;	
	tppl_float scaley = 0;
	tppl_float scale;
	if(polySizeX>0) scalex = imgSizeX/polySizeX;
	if(polySizeY>0) scaley = imgSizeY/polySizeY;

	if(scalex>0 && scalex<scaley) scale = scalex;
	else if(scaley>0) scale = scaley;
	else scale = 1;

	for(i=0;i<poly->GetNumPoints();i++) {
		p1 = poly->GetPoint(i);
		p2 = poly->GetPoint((i+1)%poly->GetNumPoints());
		p1img = (p1 - polymin)*scale + imgmin;
		p2img = (p2 - polymin)*scale + imgmin;
		img->DrawLine((int)p1img.x,(int)p1img.y,(int)p2img.x,(int)p2img.y,color);
	}
}

void DrawPoly(const char *filename, TPPLPoly *poly) {
	Image img(500,500);
	Image::Pixel white={255,255,255};
	img.Clear(white);
	ImageIO io;

	tppl_float xmin = std::numeric_limits<tppl_float>::max();
	tppl_float xmax = std::numeric_limits<tppl_float>::min();
	tppl_float ymin = std::numeric_limits<tppl_float>::max();
	tppl_float ymax = std::numeric_limits<tppl_float>::min();
	for(int i=0;i<poly->GetNumPoints();i++) {
		if(poly->GetPoint(i).x < xmin) xmin = poly->GetPoint(i).x;
		if(poly->GetPoint(i).x > xmax) xmax = poly->GetPoint(i).x;
		if(poly->GetPoint(i).y < ymin) ymin = poly->GetPoint(i).y;
		if(poly->GetPoint(i).y > ymax) ymax = poly->GetPoint(i).y;
	}

	DrawPoly(&img, poly, xmin, xmax, ymin, ymax);

	io.SaveImage(filename,&img);
}

void DrawPolyList(const char *filename, list<TPPLPoly> *polys) {
	Image img(500,500);
	Image::Pixel white={255,255,255};
	img.Clear(white);

	ImageIO io;
	list<TPPLPoly>::iterator iter;

	tppl_float xmin = std::numeric_limits<tppl_float>::max();
	tppl_float xmax = std::numeric_limits<tppl_float>::min();
	tppl_float ymin = std::numeric_limits<tppl_float>::max();
	tppl_float ymax = std::numeric_limits<tppl_float>::min();
	for(iter=polys->begin(); iter!=polys->end(); iter++) {
		for(int i=0;i<iter->GetNumPoints();i++) {
			if(iter->GetPoint(i).x < xmin) xmin = iter->GetPoint(i).x;
			if(iter->GetPoint(i).x > xmax) xmax = iter->GetPoint(i).x;
			if(iter->GetPoint(i).y < ymin) ymin = iter->GetPoint(i).y;
			if(iter->GetPoint(i).y > ymax) ymax = iter->GetPoint(i).y;
		}
		//if(iter->GetOrientation() == TPPL_CCW) printf("CCW\n");
		//else if (iter->GetOrientation() == TPPL_CW) printf("CW\n");
		//else printf("gfdgdg\n");
	}
	//printf("\n");

	for(iter=polys->begin(); iter!=polys->end(); iter++) {
		DrawPoly(&img, &(*iter), xmin, xmax, ymin, ymax);
	}

	io.SaveImage(filename,&img);
}

bool ComparePoly(TPPLPoly *p1, TPPLPoly *p2) {
	long i,n = p1->GetNumPoints();
	if(n!=p2->GetNumPoints()) return false;
	for(i=0;i<n;i++) {
		if((*p1)[i]!=(*p2)[i]) return false;
	}
	return true;
}

bool ComparePoly(list<TPPLPoly> *polys1, list<TPPLPoly> *polys2) {
	list<TPPLPoly>::iterator iter1, iter2;
	long i,n = (long)polys1->size();
	if(n!=(signed)polys2->size()) return false;
	iter1 = polys1->begin();
	iter2 = polys2->begin();
	for(i=0;i<n;i++) {
		if(!ComparePoly(&(*iter1),&(*iter2))) return false;
		iter1++;
		iter2++;
	}
	return true;
}


void GenerateTestData() {
	TPPLPartition pp;
	
	list<TPPLPoly> testpolys,result,expectedResult;

	ReadPolyList("test_input.txt",&testpolys);

	DrawPolyList("test_input.bmp",&testpolys);

	pp.Triangulate_EC(&testpolys,&result);
	//pp.Triangulate_EC(&(*testpolys.begin()),&result);
	DrawPolyList("test_triangulate_EC.bmp",&result);
	WritePolyList("test_triangulate_EC.txt",&result);

	result.clear(); expectedResult.clear();

	pp.Triangulate_OPT(&(*testpolys.begin()),&result);
	DrawPolyList("test_triangulate_OPT.bmp",&result);
	WritePolyList("test_triangulate_OPT.txt",&result);

	result.clear(); expectedResult.clear();

	pp.Triangulate_MONO(&testpolys,&result);
	//pp.Triangulate_MONO(&(*testpolys.begin()),&result);
	DrawPolyList("test_triangulate_MONO.bmp",&result);
	WritePolyList("test_triangulate_MONO.txt",&result);

	result.clear(); expectedResult.clear();

	pp.ConvexPartition_HM(&testpolys,&result);
	//pp.ConvexPartition_HM(&(*testpolys.begin()),&result);
	DrawPolyList("test_convexpartition_HM.bmp",&result);
	WritePolyList("test_convexpartition_HM.txt",&result);

	result.clear(); expectedResult.clear();

	pp.ConvexPartition_OPT(&(*testpolys.begin()),&result);
	DrawPolyList("test_convexpartition_OPT.bmp",&result);
	WritePolyList("test_convexpartition_OPT.txt",&result);
}

int main() {
	TPPLPartition pp;
	list<TPPLPoly> testpolys,result;

	ReadPolyList("failing_mono_clean - copy.txt",&testpolys);
	DrawPolyList("test.bmp", &testpolys);
	if(!pp.Triangulate_MONO(&testpolys,&result)) printf("Error\n");
	DrawPolyList("test2.bmp", &result);

}

/*int main()
{
	TPPLPartition pp;
	
	list<TPPLPoly> testpolys,result,expectedResult;

	ReadPolyList("test_input.txt",&testpolys);

	DrawPolyList("test_input.bmp",&testpolys);

	printf("Testing Triangulate_EC : ");
	pp.Triangulate_EC(&testpolys,&result);
	ReadPolyList("test_triangulate_EC.txt",&expectedResult);
	if(ComparePoly(&result,&expectedResult)) {
		printf("success\n");
	} else {
		printf("failed\n");
	}

	result.clear(); expectedResult.clear();

	printf("Testing Triangulate_OPT : ");
	pp.Triangulate_OPT(&(*testpolys.begin()),&result);
	ReadPolyList("test_triangulate_OPT.txt",&expectedResult);
	if(ComparePoly(&result,&expectedResult)) {
		printf("success\n");
	} else {
		printf("failed\n");
	}

	result.clear(); expectedResult.clear();

	printf("Testing Triangulate_MONO : ");
	pp.Triangulate_MONO(&testpolys,&result);
	ReadPolyList("test_triangulate_MONO.txt",&expectedResult);
	if(ComparePoly(&result,&expectedResult)) {
		printf("success\n");
	} else {
		printf("failed\n");
	}

	result.clear(); expectedResult.clear();

	printf("Testing ConvexPartition_HM : ");
	pp.ConvexPartition_HM(&testpolys,&result);
	ReadPolyList("test_convexpartition_HM.txt",&expectedResult);
	if(ComparePoly(&result,&expectedResult)) {
		printf("success\n");
	} else {
		printf("failed\n");
	}

	result.clear(); expectedResult.clear();

	printf("Testing ConvexPartition_OPT : ");
	pp.ConvexPartition_OPT(&(*testpolys.begin()),&result);
	ReadPolyList("test_convexpartition_OPT.txt",&expectedResult);
	if(ComparePoly(&result,&expectedResult)) {
		printf("success\n");
	} else {
		printf("failed\n");
	}

	return 0;
}*/

