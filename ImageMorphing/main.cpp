#include<iostream>
#include<algorithm>
#include<cmath>
#include<vector>
#include "CImg.h"
#include "delaunay.h"
#include<stdlib.h>
#include <Eigen/Dense>
using Eigen::MatrixXd;
using namespace std;
using namespace cimg_library;
const int maxn = 70;
double srcx[maxn], srcy[maxn], tarx[maxn], tary[maxn];
const int fsz = 42+4;
//const int fsz = 42;
CImg<float> src, tar;

void loadfeature() {
	int width = src._width; int  height = src._height;
	freopen("feature.txt", "r", stdin);
	srcx[0] = 0; srcy[0] = 0;
	srcx[1] = width - 1; srcy[1] = 0;
	srcx[2] = width - 1; srcy[2] = height-1;
	srcx[3] = 0; srcy[3] = height-1;

	for (int i = 4; i < fsz; i++) cin >> srcx[i] >> srcy[i];


	tarx[0] = 0; tary[0] = 0;
	tarx[1] = width - 1; tary[1] = 0;
	tarx[2] = width - 1; tary[2] = height - 1;
	tarx[3] = 0; tary[3] = height - 1;
	for (int i = 4; i < fsz; i++) cin >> tarx[i] >> tary[i];
	fclose(stdin);


}

Delaunay findTriangle(int width, int height, double *X, double *Y) {

	Point p[4];
	p[0] = { 0.0 ,0.0, 0.0 };
	p[1] = { width - 1.0, 0.0, 0.0 };
	p[2] = { width - 1.0,height - 1.0, 0.0 };
	p[3] = { 0.0 ,height - 1.0, 0.0 };
	Delaunay MyMesh(p[0], p[1], p[2], p[3]);
	for (int i = 4; i < fsz; i++) {
		MyMesh.AddPoint(X[i], Y[i], 0);
	}

	return MyMesh;

}

void animate(double alpha, string name) {
	int width = src._width;
	int height = src._height;
	CImg<double> med(src._width, src._height, 1, 3);
	double X[fsz], Y[fsz];
	for (int i = 0; i < fsz; i++) {
		X[i] = (1 - alpha) * srcx[i] + alpha * tarx[i];
		Y[i] = (1 - alpha) * srcy[i]+ alpha * tary[i];
	}
	med.fill(0);

	Delaunay del = findTriangle(width, height, X, Y);
	int trisnum = del.m_Tris.size();

	double tranS[100][3][3], tranT[100][3][3];
	for (int i = 0; i < trisnum; i++) {
		int* v = del.m_Tris[i].v;
		MatrixXd A(3, 3);
		A << X[v[0]], X[v[1]], X[v[2]],
			Y[v[0]], Y[v[1]], Y[v[2]],
			1.0, 1.0, 1.0;
		MatrixXd S(3, 3);
		S << srcx[v[0]], srcx[v[1]], srcx[v[2]],
			srcy[v[0]], srcy[v[1]], srcy[v[2]],
			1.0, 1.0, 1.0;
		MatrixXd T(3, 3);
		T << tarx[v[0]], tarx[v[1]], tarx[v[2]],
			tary[v[0]], tary[v[1]], tary[v[2]],
			1.0, 1.0, 1.0;
		MatrixXd invA(3, 3);
		invA = A.inverse();
		MatrixXd trans(3, 3);
		MatrixXd trant(3, 3);
		trans = S*invA;
		trant = T*invA;
		for (int p = 0; p < 3; p++) {
			for (int q = 0; q < 3; q++) {
				tranS[i][p][q] = trans(p, q);
				tranT[i][p][q] = trant(p, q);
			}
		}
	}
	cimg_forXY(med, x, y) {
		Point p(x, y, 0);
		for (int i = 0; i < del.m_Tris.size(); i++) {
			int* v = del.m_Tris[i].v;
			bool in = PointinTriangle1(Point(X[v[0]], Y[v[0]], 0), Point(X[v[1]], Y[v[1]], 0), Point(X[v[2]], Y[v[2]], 0), p);
			if (!in) continue;
	
			double u1 = tranS[i][0][0] * x + tranS[i][0][1] * y + tranS[i][0][2];
			double v1 = tranS[i][1][0] * x + tranS[i][1][1] * y + tranS[i][1][2];
			double u2 = tranT[i][0][0] * x + tranT[i][0][1] * y + tranT[i][0][2];
			double v2 = tranT[i][1][0] * x + tranT[i][1][1] * y + tranT[i][1][2];

			double col1[3], col2[3];
			for (int k = 0; k < 3; k++) {
				double a = u1 - floor(u1);
				double b = v1 - floor(v1);
				int i = (int)u1;
				int j = (int)v1;
				if (i >= width - 1) i = width - 2;
				if (j >= height - 1) j = height - 2;
				col1[k] = (1 - a)*(1 - b)*src(i, j, k) + a*(1 - b)*src(i + 1, j, k) +
					(1 - a)*b*src(i, j + 1, k) + a*b*src(i + 1, j + 1, k);
				
			}
			for (int k = 0; k < 3; k++) {
				double a = u2 - floor(u2);
				double b = v2 - floor(v2);
				int i = (int)u2;
				int j = (int)v2;
				if (i >= width - 1) i = width - 2;
				if (j >= height - 1) j = height - 2;
				col2[k] = (1 - a)*(1 - b)*tar(i, j, k) + a*(1 - b)*tar(i + 1, j, k) +
					(1 - a)*b*tar(i, j + 1, k) + a*b*tar(i + 1, j + 1, k);

			}
			for (int k = 0; k < 3; k++) {
				med(x, y, k) = (1 - alpha) * col1[k] + alpha * col2[k];	
			}
			break;
		}

	}
	cout << name << " finished\n";
//	med.display(name);
	med.save(name.c_str());
}



int main() {

	src.load_bmp("1.bmp");
	tar.load_bmp("2.bmp");
    loadfeature();

	for (int i = 0; i <= 10; i++) {
		char s[5];
		string head = "result/";
		string name = itoa(i, s, 10);
		name = head + name + ".bmp";
		animate(1.0*i / 10.0, name);
	}


}