// Leapfrog procedure for orbits
// Neglects tidal forces
// Peter Boyle, 25Nov17

#include "stdafx.h"
#include <iostream>
#include <cmath>
#include <fstream>
using namespace std;

class Point		//Point class to handle all operations
{
private:
	double x_val, y_val, z_val;

public:
	Point(double x = 0.0, double y = 0.0, double z = 0.0)
	{
		x_val = x;
		y_val = y;
		z_val = z;
	}

	double Getx()					//Extract x, y, z
	{ 
		return x_val; 
	}								
	double Gety() 
	{ 
		return y_val; 
	}
	double Getz() 
	{ 
		return z_val; 
	}

	double dist(Point b)									//distance functions
	{
		double xd = x_val - b.x_val;
		double yd = y_val - b.y_val;
		double zd = z_val - b.z_val;
		return sqrt(xd*xd + yd*yd + zd*zd);
	}
	double xdist(Point b)
	{
		return x_val - b.x_val;
	}
	double ydist(Point b)
	{
		return y_val - b.y_val;
	}
	double zdist(Point b)
	{
		return z_val - b.z_val;
	}

	Point add(Point b)										//vector addition
	{
		return Point(x_val + b.x_val, y_val + b.y_val, z_val + b.z_val);
	}
	Point sub(Point b)										//vector subtraction
	{
		return Point(x_val - b.x_val, y_val - b.y_val, z_val - b.z_val);
	}
	
	double dot(Point b)										//dot product
	{
		return double (x_val * b.x_val + y_val * b.y_val + z_val * b.z_val);
	}
	Point cross(Point b)									//cross product
	{
		return Point(y_val*b.z_val - z_val*b.y_val, z_val*b.x_val - x_val*b.z_val, x_val*b.y_val - y_val *b.x_val);
	}

	Point mult(double b)
	{
		return Point(x_val*b, y_val*b, z_val*b);
	}
	Point div(double b)
	{
		return Point(x_val / b, y_val / b, z_val / b);
	}

	Point add_scaler(double b)
	{
		return Point(x_val + b, y_val + b, z_val + b);
	}
	Point sub_scaler(double b)
	{
		return Point(x_val - b, y_val - b, z_val - b);
	}

	Point GetAccel(Point a, Point b, double mass_b)					//acceleration on point a by b
	{
		static const double G = 6.67408e-11;						//Gravitational constant
		Point distance = Point(a.xdist(b), a.ydist(b), a.zdist(b));	//distance points
		double accel_g = -G*mass_b/(a.dist(b)*a.dist(b)*a.dist(b));	//acceleration due to gravity

		Point accel = Point(distance.mult(accel_g));
		return accel;
	}
	Point GetVelocity(Point v_n, Point a_n, Point a_n_1, int del_t)	//velocity, accelerations, time step
	{
		Point a = Point(a_n.add(a_n_1));
		Point a_av = Point(a.mult(0.5));
		Point v = Point(v_n.add(a_av.mult(del_t)));
		return v;
	}
	Point GetPosition(Point r_n, Point v_n, Point a_n, int del_t)	//Position, velocity, acceleration
	{
		Point a = Point(a_n.mult((0.5)*(del_t*del_t)));
		Point v = Point(v_n.mult(del_t));
		Point r = Point(r_n.add(a.add(v)));
		return r;
	}
};

//class Leapfrog														//Leapfrog integrator
//{	Point leapfrog_integrate(Point v_subject, Point r_subject, Point v_gravitator, Point r_gravitator,
//		double mass_s, double mass_g, int max_t, int del_t)
//	{
//		Point r_s = r_subject;		Point r_g = r_gravitator;
//		Point v_s = v_subject;		Point v_g = v_gravitator;
//	
//		for (int t = 0; t <= max_t; t + del_t)
//		{
//			Point a_s = GetAccel(r_s, r_g, mass_g);
//			Point a_g = GetAccel(r_g, r_s, mass_s);
//			Point r_s_1 = GetPosition(r_s, v_s, a_s, del_t);
//			Point r_g_1 = GetPosition(r_g, v_g, a_g, del_t);
//			Point a_s_1 = GetAccel(r_s_1, r_g_1, mass_g);
//			Point a_g_1 = GetAccel(r_g_1, r_s_1, mass_s);
//			Point v_s_1 = GetVelocity(v_s, a_s, a_s_1, del_t);
//			Point v_g_1 = GetVelocity(v_g, a_g, a_g_1, del_t);
//
//			v_s = v_s_1;
//			v_g = v_g_1;
//			r_s = r_s_1;
//			r_g = r_g_1;
//		}
//	}
//};

int main()
{
	long double mass_S = 1.989e30;	//mass of the sun
	long double mass_P = 5.972e24;	//1.898e27 J, 5.972E24 E
	long double mass_g = mass_S;
	long double mass_s = mass_P;
	long double mass_a = 0;
	Point r_sun = Point(0, 0, 0);	//location of sun, sun frame
	Point r_g = r_sun;
	Point v_sun = Point(0, 0, 0);	//velocity of sun, sun frame
	Point v_g = v_sun;
	Point r_planet = Point(1.496e11, 0, 0);
	Point r_s = r_planet;
	Point v_planet = Point(0, 29780, 0);
	Point v_s = v_planet;								//L2 1.5 million km behind earth, L4 (0.5*1AU, sqrt(3)/2 * 1AU, 0)
	Point r_ast = Point(0.5*(1.496e11), (sqrt(3)/2)*(1.496e11),0);//generate asteroid in appropriate range (r = 2.2 - 3.2AU, 3.3e11 - 4.5e11)
	Point v_ast = Point((sqrt(3) / 2)*29780, 0.5*29780, 0);				//v = 16.7-25 km/s
	long double max_t = 3.6e10;							//1.8e17 is ~5.7 billion years, 3.6e14 is ~11.4 million years
	int del_t = 3600;									//time in seconds, max ~1/100 orbital period of object (P = 3.26 - 5.7 years)
	double k = 0;										//1080000 ~ 0.03 years
	ofstream myfile;

	myfile.open("earth_L4_10.2.csv");
	myfile << "x_a, y_a, z_a";
	//myfile << "\n" << test3.Getx() << "," << test3.Gety() << "," << test3.Getz();
	//myfile << "\n" << test3.Getx() << "," << test3.Gety() << "," << test3.Getz();
	//myfile << "\n" << testdist.Getx() << "," << testdist.Gety() << "," << testdist.Getz() << "," << test2.div(2).Getx();
	myfile.close();
	myfile.open("earth_L4_10.2.csv", ios::app);

	for (int i = 0; i <= int(max_t/del_t); i++)
	{
		Point a_s = Point(a_s.GetAccel(r_s, r_g, mass_g));
		Point a_g = Point(0, 0, 0);
		Point a_ast = Point(a_ast.GetAccel(r_ast, r_g, mass_g)).add(Point(a_ast.GetAccel(r_ast, r_s, mass_s)));	
		Point r_s_1 = Point(r_s_1.GetPosition(r_s, v_s, a_s, del_t));
		Point r_g_1 = Point(0, 0, 0);
		Point r_ast_1 = Point(r_ast_1.GetPosition(r_ast, v_ast, a_ast, del_t));
		Point a_s_1 = Point(a_s_1.GetAccel(r_s_1, r_g_1, mass_g));
		Point a_g_1 = Point(0, 0, 0);
		Point a_ast_1 = Point(a_ast_1.GetAccel(r_ast_1, r_g_1, mass_g)).add(Point(a_ast.GetAccel(r_ast_1, r_s_1, mass_s)));
		Point v_s_1 = Point(v_s_1.GetVelocity(v_s, a_s, a_s_1, del_t));
		Point v_g_1 = Point(0, 0, 0);
		Point v_ast_1 = Point(v_ast_1.GetVelocity(v_ast, a_ast, a_ast_1, del_t));

		v_s = v_s_1;				//these don't include v_g because we are in gravitator's frame of reference
		v_ast = v_ast_1;
		r_s = r_s_1;
		r_ast = r_ast_1;
		
		if (i % 10 == 0)								//write every tenth calculation point to file
		{
			myfile << "\n" << r_ast.Getx() << "," << r_ast.Gety() << "," << r_ast.Getz();
		}
		
		if ((i / (max_t / del_t)) == k / 100)			//console progress indicator
		{
			cout << "\n" << int(k) << "% complete";
			k++;
		}
	}
	myfile.close();

    return 0;
}