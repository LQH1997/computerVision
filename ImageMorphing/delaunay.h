#ifndef DELAUNAY_H_INCLUDED  
#define DELAUNAY_H_INCLUDED  
#include <cstdlib>  
#include <iostream>  
#include <cstring>  
#include <string>  
#include <fstream>  
#include <math.h>  
#include <vector>

using namespace std;
// 3D vector
class Point {
public:
	Point() {}

	Point(double fx, double fy, double fz)
		:x(fx), y(fy), z(fz) {
	}

	// Subtract
	Point operator - (const Point& v) const {
		return Point(x - v.x, y - v.y, z - v.z);
	}

	// Dot product
	double Dot(const Point& v) const {
		return x * v.x + y * v.y + z * v.z;
	}
	void show() {
		cout << " x  " << x << "   y  " << y << endl;
	}
	// Cross product
	Point Cross(const Point& v) const {
		return Point(
			y * v.z - z * v.y,
			z * v.x - x * v.z,
			x * v.y - y * v.x);
	}

public:
	double x, y, z;
};

// Determine whether two vectors v1 and v2 point to the same direction
// v1 = Cross(AB, AC)
// v2 = Cross(AB, AP)
bool SameSide(const Point &A, const Point &B, const Point &C, const Point &P) {
	Point AB = B - A;
	Point AC = C - A;
	Point AP = P - A;

	Point v1 = AB.Cross(AC);
	Point v2 = AB.Cross(AP);

	// v1 and v2 should point to the same direction
	return v1.Dot(v2) >= 0;
}
const double eps = 1e-8;
int dcmp(double x) {
	if (fabs(x) < eps) return 0;
	else return x < 0 ? -1 : 1;

}
double dot(Point a, Point b) {
	return a.x*b.x + a.y*b.y;
}
double cross(Point a, Point b) {
	return a.x*b.y - a.y*b.x;
}
bool ison(Point p, Point a1, Point a2) {
	return dcmp(cross(a1 - p, a2 - p)) == 0 && dcmp(dot(a1 - p, a2 - p)) < 0;
}
bool ispointinoronpoly(const Point &p, vector<Point> & poly) {
	int wn = 0;
	for (int i = 0; i < 3; i++) {
		if (ison(p, poly[i], poly[(i + 1) % 3])) return true;
		int k = dcmp(cross(poly[(i + 1) % 3] - poly[i], p - poly[i]));
		int d1 = dcmp(poly[i].y - p.y);
		int d2 = dcmp(poly[(i+1)%3].y - p.y);
		if (k > 0 && d1 <= 0 && d2 > 0) wn++;
		if (k < 0 && d2 <= 0 && d1 > 0) wn--;
	}
	if (wn != 0) return true;
	return false;
}

//bool PointinTriangle1( Point A,  Point B,  Point C,  Point P) {
//	vector<Point> poly;
//	poly.push_back(A); poly.push_back(B); poly.push_back(C);
//	return ispointinoronpoly(P, poly);
//}
bool PointinTriangle1(const Point &A, const Point &B, const Point &C, const Point &P) {
	return SameSide(A, B, C, P) &&
		SameSide(B, C, A, P) &&
		SameSide(C, A, B, P);
}

/*Bowyer-Watson算法
的基本步骤是：
1、构造一个超级三角形，包含所有散点，放入三角形链表。
2、将点集中的散点依次插入，在三角形链表中找出外接圆包含插入点的三角形（称为该点的影响三角形），删除影响三角形的公共边，将插入点同影响三角形的全部顶点连接起来，完成一个点在Delaunay三角形链表中的插入。
3、根据优化准则对局部新形成的三角形优化。将形成的三角形放入Delaunay三角形链表。
4、循环执行上述第2步，直到所有散点插入完毕。*/

typedef std::vector<Point> PointArray;//定义点类的vector容器  

typedef struct
{
	int left;
	int right;
	int count;//边的计数，如果计数为0，则删除此边  
}Edge;//定义边类  
typedef std::vector<Edge> EdgeArray;//定义边类的vector容器  

typedef struct
{
	int v[3];//三角形的三个顶点  
	Edge s[3];//三角形的三条边  
	double xc;//三角形外接圆圆心的x坐标  
	double yc;//三角形外接圆圆心的y坐标  
	double r;//三角形外接圆的半径
	double** m1; //到原图的映射矩阵
	double** m2; //到目标图的映射矩阵
}Triangle;//定义三角形类  
typedef std::vector<Triangle> TriangleArray;//定义三角形类的vector容器  

typedef std::vector<int> intArray;//定义int类的vector容器  

class Delaunay//定义Delaunay类  
{
public:
	Delaunay::Delaunay(Point p1, Point p2, Point p3, Point p4)
	{
		m_Pts.resize(4);
		m_Pts[0] = p1;
		m_Pts[1] = p2;
		m_Pts[2] = p3;
		m_Pts[3] = p4;
		m_Edges.resize(4);
		Edge l1 = { 0,1,-1 };
		Edge l2 = { 1,2,-1 };
		Edge l3 = { 0,3,-1 };
		Edge l4 = { 2,3,-1 };
		m_Edges[0] = l1;
		m_Edges[1] = l2;
		m_Edges[2] = l3;
		m_Edges[3] = l4;
		MakeTriangle(0, 1, 2);
		MakeTriangle(0, 2, 3);
	}
	Delaunay::Delaunay() {

	}

	Delaunay::~Delaunay()
	{
		m_Pts.resize(0);
		m_Edges.resize(0);
		m_Tris.resize(0);
	}

	bool AddPoint(double xx, double yy, double zz)
	{
		EdgeArray BoundEdges;
		Point newPoint(xx, yy, zz);
		m_Pts.push_back(newPoint);
		intArray badTriangle;
		int TriSize = (int)m_Tris.size();
		for (int i = 0; i < TriSize; i++)
		{
			if (inCircle(xx, yy, m_Tris[i]) == true) badTriangle.push_back(i);
		}
		for (int i = 0; i < (int)badTriangle.size(); i++)
		{
			DelTriangle(badTriangle[i], BoundEdges);
			for (int j = i + 1; j < (int)badTriangle.size(); j++) badTriangle[j] -= 1;
		}
		int PtSize = (int)m_Pts.size();
		for (int i = 0; i < (int)BoundEdges.size(); i++)
		{
			if (PtSize - 1 < BoundEdges[i].left) MakeTriangle(PtSize - 1, BoundEdges[i].left, BoundEdges[i].right);
			else if (PtSize - 1 > BoundEdges[i].left && PtSize - 1 < BoundEdges[i].right) MakeTriangle(BoundEdges[i].left, PtSize - 1, BoundEdges[i].right);
			else MakeTriangle(BoundEdges[i].left, BoundEdges[i].right, PtSize - 1);
		}
		return true;
	}

	void Delete_Frame()
	{
		EdgeArray BoundEdges;
		for (int i = 0; i<4; i++) m_Pts.erase(m_Pts.begin());
		for (int i = 0; i<(int)m_Tris.size(); i++)
		{
			if (m_Tris[i].v[0] == 0 || m_Tris[i].v[0] == 1 || m_Tris[i].v[0] == 2 || m_Tris[i].v[0] == 3)
			{
				DelTriangle(i, BoundEdges);
				BoundEdges.resize(0);
				i--;
			}
			else
			{
				for (int j = 0; j<3; j++)
				{
					m_Tris[i].v[j] -= 4;
					m_Tris[i].s[j].left -= 4;
					m_Tris[i].s[j].right -= 4;
				}
			}
		}
		for (int i = 0; i<4; i++) m_Edges.erase(m_Edges.begin());
		for (int i = 0; i<(int)m_Edges.size(); i++)
		{
			m_Edges[i].left -= 4;
			m_Edges[i].right -= 4;
		}
	}

	void Boundary_Recover(int fromPoint, int toPoint)
	{
		EdgeArray BoundEdges;
		for (int i = 0; i<(int)m_Tris.size(); i++)
		{
			if (m_Tris[i].v[0] >= (fromPoint - 1) && m_Tris[i].v[2] <= (toPoint - 1))
			{
				DelTriangle(i, BoundEdges);
				BoundEdges.resize(0);
				i--;
			}
		}
	}

	void output()
	{
		ofstream outfile("output.log");
		if (!outfile)
		{
			cout << "Unable to output nodes!";
			exit(1);
		}
		outfile << "/PREP7" << endl;
		for (int i = 0; i<(int)m_Pts.size(); i++)
		{
			outfile << "K," << i + 1 << "," << m_Pts[i].x << "," << m_Pts[i].y << "," << m_Pts[i].z << endl;
		}
		for (int i = 0; i<(int)m_Edges.size(); i++)
		{
			outfile << "L," << m_Edges[i].left + 1 << "," << m_Edges[i].right + 1 << endl;
		}
		outfile.close();
	}

	PointArray m_Pts;//m_Pts用于存储所有点  
	EdgeArray m_Edges;//m_Edges用于存储所有边  
	TriangleArray m_Tris;//m_Tris用于存储所有三角形

private:
	void Delaunay::Cal_Centre(double &x_centre, double &y_centre, double &radius, int n1, int n2, int n3)
	{
		double x1, x2, x3, y1, y2, y3;
		x1 = m_Pts[n1].x;
		y1 = m_Pts[n1].y;
		x2 = m_Pts[n2].x;
		y2 = m_Pts[n2].y;
		x3 = m_Pts[n3].x;
		y3 = m_Pts[n3].y;
		x_centre = ((y2 - y1)*(y3*y3 - y1*y1 + x3*x3 - x1*x1) - (y3 - y1)*(y2*y2 - y1*y1 + x2*x2 - x1*x1)) / (2 * (x3 - x1)*(y2 - y1) - 2 * ((x2 - x1)*(y3 - y1)));
		y_centre = ((x2 - x1)*(x3*x3 - x1*x1 + y3*y3 - y1*y1) - (x3 - x1)*(x2*x2 - x1*x1 + y2*y2 - y1*y1)) / (2 * (y3 - y1)*(x2 - x1) - 2 * ((y2 - y1)*(x3 - x1)));
		radius = sqrt((x1 - x_centre)*(x1 - x_centre) + (y1 - y_centre)*(y1 - y_centre));
	}

	void Delaunay::MakeTriangle(int n1, int n2, int n3)
	{
		double x_centre, y_centre, radius;
		Cal_Centre(x_centre, y_centre, radius, n1, n2, n3);
		Triangle newTriangle = { { n1,n2,n3 },{ { n1,n2,1 },{ n2,n3,1 },{ n1,n3,1 } },x_centre,y_centre,radius, NULL, NULL };
		m_Tris.push_back(newTriangle);
		int EdgeSzie = (int)m_Edges.size();
		int flag;
		for (int i = 0; i < 3; i++)
		{
			flag = 1;
			for (int j = 0; j < EdgeSzie; j++)
			{
				if (newTriangle.s[i].left == m_Edges[j].left&&newTriangle.s[i].right == m_Edges[j].right&&m_Edges[j].count != -1) { flag = 0; m_Edges[j].count += 1; break; }
				else if (newTriangle.s[i].left == m_Edges[j].left&&newTriangle.s[i].right == m_Edges[j].right&&m_Edges[j].count == -1) { flag = 0; break; }
			}
			if (flag == 1) m_Edges.push_back(newTriangle.s[i]);
		}
	}

	bool inCircle(double xx, double yy, Triangle currentTris)
	{
		double dis = sqrt((currentTris.xc - xx)*(currentTris.xc - xx) + (currentTris.yc - yy)*(currentTris.yc - yy));
		if (dis > currentTris.r) return false;
		else return true;
	}

	void DelTriangle(int n, EdgeArray &BoundEdges)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < (int)m_Edges.size(); j++)
			{
				if (m_Edges[j].left == m_Tris[n].s[i].left&&m_Edges[j].right == m_Tris[n].s[i].right)
				{
					if (m_Edges[j].count == 2)
					{
						m_Edges[j].count = 1;
						BoundEdges.push_back(m_Edges[j]);
					}
					else if (m_Edges[j].count == -1) BoundEdges.push_back(m_Edges[j]);
					else if (m_Edges[j].count == 1)
					{
						for (int k = 0; k < (int)BoundEdges.size(); k++)
						{
							if (BoundEdges[k].left == m_Edges[j].left&&BoundEdges[k].right == m_Edges[j].right)
							{
								BoundEdges.erase(BoundEdges.begin() + k);
								break;
							}
						}
						m_Edges.erase(m_Edges.begin() + j);
						j--;
					}
					break;
				}
			}
		}
		m_Tris.erase(m_Tris.begin() + n);
	}




};
#endif // DELAUNAY_H_INCLUDED