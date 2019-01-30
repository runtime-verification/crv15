/********************************************************************
 * File: nff_entities.h
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
 * This file contains definitions for data structures to be used
 * with our code for reading in NFF data.
 ********************************************************************/
#ifndef __NFF_ENTITIES_H
#define __NFF_ENTITIES_H

typedef double point3[3];

typedef double point4[3];

typedef double color[3];


typedef struct {
	color light_color;
	point3 position;
	double intensity;
} light;


typedef struct {
	color fill_color;
	double Kd;
	double Ks;
	double T;
	double index_of_refraction;
	double phong_power;
} object_fill;


typedef struct {
	point3 center;
	double radius;
	object_fill sphere_fill;
} sphere;


typedef struct {
	unsigned int is_patch;
	point3 vertices[3];
	point3 vertice_normals[3];
	point3 normal;
	object_fill triangle_fill;
} triangle;


typedef struct {
	point3 from;
	point3 at;
	point3 up;
	double angle;
	double hither;
	double aspect_ratio;
	double resolution[2];
	double t;
	double b;
	double r;
	double l;
} viewpoint;


#define COPY_OBJECT_FILL(a,b)	{ (a).fill_color[0] = (b).fill_color[0]; \
				(a).fill_color[1] = (b).fill_color[1]; \
				(a).fill_color[2] = (b).fill_color[2]; \
				(a).Kd = (b).Kd; \
				(a).Ks = (b).Ks; \
				(a).phong_power = (b).phong_power; \
				(a).T = (b).T; \
				(a).index_of_refraction = (b).index_of_refraction; }

#define COPY_POINT3(a,b)	{ (a)[0] = (b)[0]; \
				(a)[1] = (b)[1]; \
				(a)[2] = (b)[2]; }

#define COPY_POINT4(a,b)	{ (a)[0] = (b)[0]; \
				(a)[1] = (b)[1]; \
				(a)[2] = (b)[2]; \
				(a)[3] = (b)[3]; }

#define COPY_COLOR(a,b)		{ (a)[0] = (b)[0]; \
				(a)[1] = (b)[1]; \
				(a)[2] = (b)[2]; }

#define SET_COLOR(r,R,G,B)	{ (r)[0] = (R); \
				(r)[1] = (G); \
				(r)[2] = (B); }

#define SET_COLOR(r,R,G,B)	{ (r)[0] = (R); \
				(r)[1] = (G); \
				(r)[2] = (B); }

#define SET_POINT3(r,X,Y,Z)     { (r)[0] = (X); \
				(r)[1] = (Y); \
				(r)[2] = (Z); }

#define SET_POINT4(r,X,Y,Z,R)     { (r)[0] = (X); \
				(r)[1] = (Y); \
				(r)[2] = (Z); \
				(r)[3] = (R); }



#endif

