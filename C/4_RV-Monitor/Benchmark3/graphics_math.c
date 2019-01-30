/********************************************************************
 * File: graphics_math.c
 * Author: Patrick Carroll <patrick@patrickomatic.com>
 * Date: November 2007
 *
 * This file is part of c-ray-tracer
 *
 * c-ray-tracer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * c-ray-tracer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with c-ray-tracer.  If not, see <http://www.gnu.org/licenses/>.
 *
 * This file defines some useful functions and constants for doing 
 * graphics math
 ********************************************************************/
#include <math.h>
#include <stdio.h>

#include "graphics_math.h"


/* normalize and normcrossprod are adapted from the OpenGL red book
 * http://www.glprogramming.com/red/chapter02.html */
void normalize(double *v) 
{    
	double d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); 

	if (d == 0.0) {
		fprintf(stderr, "Error calculating normal\n");
		return;
	}

	v[0] /= d; 
	v[1] /= d; 
	v[2] /= d; 
}


double vector_length(const double *a)
{
	return sqrt(SQUARE(a[0]) + SQUARE(a[1]) + SQUARE(a[2]));
}


void cross_product(const double *v1, const double *v2, double *out)
{
	out[0] = v1[1] * v2[2] - v1[2] * v2[1]; 
	out[1] = v1[2] * v2[0] - v1[0] * v2[2]; 
	out[2] = v1[0] * v2[1] - v1[1] * v2[0]; 
}


void normal_cross_product(const double *v1, const double *v2, double *out) 
{ 
	cross_product(v1, v2, out);
	normalize(out); 
}


void triangle_normal_vector(double *vn, const double *a, const double *b,
		const double *c)
{
	double ab[3], ac[3];

	subtract_vector(b, a, ab);
	subtract_vector(c, a, ac);
	normal_cross_product(ac, ab, vn);
}


double dot_product(const double *v1, const double *v2)
{
	int i; 
	double dp = 0.0;

	for (i = 0; i < 3; i++)
		dp += v1[i] * v2[i];      
	
	return dp;
}


void add_vector(const double *a, const double *b, double *out)
{
	int i;
	for (i = 0; i < 3; i++) 
		out[i] = a[i] + b[i];	
}


void subtract_vector(const double *a, const double *b, double *out)
{
	int i;
	for (i = 0; i < 3; i++) 
		out[i] = a[i] - b[i];	
}

double vectors_equal(const double *a, const double *b)
{
	return (a[0] == b[0] && a[1] == b[1] && a[2] == b[2]);
}


/* returns true or false */
static double same_side(const double *p1, const double *p2, const double *v1, 
		const double *v2)
{
	double cp1[3], cp2[3];
	double out1[3], out2[3];

	subtract_vector(v2, v1, out1); 
	subtract_vector(p1, v1, out2); 
       	cross_product(out1, out2, cp1);

	subtract_vector(v2, v1, out1); 
	subtract_vector(p2, v1, out2); 
       	cross_product(out1, out2, cp2);

	if (dot_product(cp1, cp2) >= 0) 
		return 1;

	return 0; 
}


/* given v1, v2 and v3 are points making a triangle, and (p[0], p[2]) are
 * x,z coordinates of a point in the triangle, figure out p[1] */
void calculate_y_of_point_in_triangle(const double *v1, const double *v2, 
			const double *v3, double *p)
{
	double v12[3], v13[3];
	double n[3];

	subtract_vector(v2, v1, v12);
	subtract_vector(v3, v1, v13);
	cross_product(v12, v13, n);

	p[1] = (n[0]*v1[0] + n[1]*v1[1] + n[2]*v1[2] - n[0]*p[0] - n[2]*p[2]) 
		/ n[1];
}


/* returns true or false */
double point_in_triangle(const double *v1, const double *v2, const double *v3, double *p)
{
	if (same_side(p, v1, v2, v3) 
			&& same_side(p, v2, v1, v3) 
			&& same_side(p, v3, v1, v2))
		return 1;
	else 
		return 0; 
}


double radians_to_degrees(double radians)
{
	return (double)(radians * (180.0 / PI));
}


double degrees_to_radians(double degrees)
{
	return ((double)degrees * (PI / 180.0));
}


double angle_between_vectors(const double *a, const double *b)
{
	double r = acos(dot_product(a, b) 
			/ (vector_length(a) * vector_length(b)));
	double d;

	/* this checks for NaN */
	if (r != r)
		return 0;

	d = radians_to_degrees(r);
	if (d > 180)
		return 360 - d;
	return d;
}


void rotate_vector(double angle, const double *a, double *out)
{
	double rads = degrees_to_radians(angle);

	out[0] = cos(rads)*a[0] - sin(angle)*out[1];
	out[1] = sin(rads)*a[0] - cos(angle)*out[1];
	out[2] = a[2];
}


void multiply_vectors(const double *a, const double *b, double *out)
{
	int i;
	for (i = 0; i < 3; i++)
		out[i] = a[i] * b[i];
}

void multiply_vector(const double *a, double b, double *out)
{
	int i;
	for (i = 0; i < 3; i++)
		out[i] = a[i] * b;
}


void divide_vector(const double *a, double b, double *out)
{
	multiply_vector(a, (1.0 / b), out);
}


double distance_between_points(double x1, double y1, double x2, double y2)
{
	int a = x2 - x1, b = y2 - y1;
	return sqrt(SQUARE(a) + SQUARE(b));
}
