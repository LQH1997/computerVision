#ifndef _HOUGHTRANSFORM_H
#define _HOUGHTRANSFORM_H

#include "canny.h"
#include <vector>
#include <map>
#include <set>

class HoughTransform
{
public:
	struct hough {
		hough(int i, int r) : i(i), r(r) {}
		hough() {}
		friend bool operator<(const hough& a, const hough& b) {
			return a.r < b.r || (a.r == b.r && a.i < b.i);
		}
		int i;
		int r;
	};

	struct point{
		int x, y;
		point(int a, int b) {
			x = a;
			y = b;
		}
	};

	struct circle {
		circle(int a, int b, int r):a(a), b(b), r(r) {}
		circle() {}
		friend bool operator < (const circle& c1, const circle& c2){
			return c1.r < c2.r || (c1.r == c2.r && c1.a < c2.a) ||
			(c1.r == c2.r && c1.a == c2.a && c1.b < c2.b);
		}
		int a;
		int b;
		int r;
	};


	HoughTransform(char* filename, int method);
	CImg<unsigned char> findLine();
	CImg<unsigned char> findCircle(CImg<unsigned char> smallOrigin);

	vector<vector<double> >createFilter(int row, int column, double sigmaIn);
	CImg<unsigned char>useFilter(CImg<unsigned char> img_in, vector<vector<double> > filterIn);

private:
	CImg<unsigned char> origin;
	CImg<unsigned char> edge;
	CImg<unsigned char> lineOutput;
	CImg<unsigned char> circleOutput;
	CImg<unsigned char> houghGraph;

	std::vector<point> intersetcions;
	map<hough, int> houghSpace;
	map<int, hough> sortHough;

	map<circle, int> circleHoughSpace;
	map<int, circle> sortCircleHoughSpace;

	set<hough> line;
	set<circle> originCircle;

	char* filename;
	int method;
};


#endif
