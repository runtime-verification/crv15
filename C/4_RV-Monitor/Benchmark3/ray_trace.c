/********************************************************************
 * File: ray_trace.c
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
 * This file contains functions for implementing a ray tracer.  
 * ray_trace() is the main algorithm which should be called for
 * doing ray tracing.
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graphics_math.h"
#include "nff_entities.h"
#include "ray_trace.h"
#include "__RVC_Instrument.h"

#define INFINITY 		100000000000.0
#define EPSILON			0.001
#define MAX_REFLECTION_BOUNCES	5


static unsigned int ray_sphere_intersection(const point3 ray_e,
		const point3 ray_d, const sphere *sph, point3 surface_normal,
		double *t0, double *t1)
{
	double discriminant;
	double t, t_poss1, t_poss2;
	double d_dot_d;
	point3 p, e_minus_c, tmp;

	subtract_vector(ray_e, sph->center, e_minus_c);
	__RVC_access(sph);
	d_dot_d = dot_product(ray_d, ray_d);

	discriminant = SQUARE(dot_product(ray_d, e_minus_c)) - d_dot_d 
		* (dot_product(e_minus_c, e_minus_c) - SQUARE(sph->radius));
	__RVC_access(sph);

	if (discriminant < 0)
		return 0;

	multiply_vector(ray_d, -1.0, tmp);
	t_poss1 = -(dot_product(tmp, e_minus_c) + sqrt(discriminant)) / d_dot_d;
	t_poss2 = -(dot_product(tmp, e_minus_c) - sqrt(discriminant)) / d_dot_d;

	if (t_poss1 > 0 && t_poss1 < t_poss2) 
		t = t_poss1;
	else 
		t = t_poss2;

	if (t < *t0 || t > *t1)
		return 0;

	*t1 = t;

	/* calculate the surface normal */
	/* p(t) = e + td */
	multiply_vector(ray_d, t, p);
	add_vector(p, ray_e, p);
	
	subtract_vector(p, sph->center, surface_normal);
	__RVC_access(sph);
	multiply_vector(surface_normal, -1.0, surface_normal);
	normalize(surface_normal);

	return 1;
}


static unsigned int ray_triangle_intersection(const point3 ray_e, 
		const point3 ray_d, const triangle *tri, point3 surface_normal,
		double *t0, double *t1) 
{
	double a, b, c, d, e, f, g, h, i, j, k, l;
	double ei_minus_hf, gf_minus_di, dh_minus_eg;
	double ak_minus_jb, jc_minus_al, bl_minus_kc;
	double alpha, beta, gamma, t, M;
	point3 n0, n1, n2;

	a = tri->vertices[0][0] - tri->vertices[1][0];
	__RVC_access(tri);
	b = tri->vertices[0][1] - tri->vertices[1][1];
	__RVC_access(tri);
	c = tri->vertices[0][2] - tri->vertices[1][2];
	__RVC_access(tri);
	d = tri->vertices[0][0] - tri->vertices[2][0];
	__RVC_access(tri);
	e = tri->vertices[0][1] - tri->vertices[2][1];
	__RVC_access(tri);
	f = tri->vertices[0][2] - tri->vertices[2][2];
	__RVC_access(tri);
	g = ray_d[0];
	h = ray_d[1];
	i = ray_d[2];
	j = tri->vertices[0][0] - ray_e[0];
	__RVC_access(tri);
	k = tri->vertices[0][1] - ray_e[1];
	__RVC_access(tri);
	l = tri->vertices[0][2] - ray_e[2];
	__RVC_access(tri);

	ei_minus_hf = e*i - h*f;
	gf_minus_di = g*f - d*i;
	dh_minus_eg = d*h - e*g;
	M = a * ei_minus_hf + b * gf_minus_di + c * dh_minus_eg;

	ak_minus_jb = a*k - j*b;
	jc_minus_al = j*c - a*l;
	bl_minus_kc = b*l - k*c;

	t = (f * ak_minus_jb + e * jc_minus_al + d * bl_minus_kc) / M;
	if (t < *t0 || t > *t1)
		return 0;

	gamma = (i * ak_minus_jb + h * jc_minus_al + g * bl_minus_kc) / M;
	if (gamma < 0.0 || gamma > 1.0)
		return 0;

	beta = (j * ei_minus_hf + k * gf_minus_di + l * dh_minus_eg) / M;
	if (beta < 0.0 || beta > (1.0 - gamma))
		return 0;

	if (tri->is_patch) {
		alpha = 1.0 - beta - gamma;
		/* n = alpha*n0 + beta*n1 + gamma*n2 */
		multiply_vector(tri->vertice_normals[0], alpha, n0);
		__RVC_access(tri);
		multiply_vector(tri->vertice_normals[1], beta, n1);
		__RVC_access(tri);
		multiply_vector(tri->vertice_normals[2], gamma, n2);
		__RVC_access(tri);
		add_vector(n0, n1, surface_normal);
		add_vector(surface_normal, n2, surface_normal);
		multiply_vector(surface_normal, -1.0, surface_normal);
	} else {
		COPY_POINT3(surface_normal, tri->normal);
	}
	__RVC_access(tri);
	/* save this as the closest distance */
	*t1 = t;
	return 1;
}


static void compute_viewing_ray(point3 d, const point3 u, const point3 v, 
		const point3 w, unsigned int i, unsigned int j, 
		const viewpoint *view)
{
	double u_s, v_s, w_s;
	point3 u_tmp, v_tmp, w_tmp, s;

	subtract_vector(view->from, view->at, w_tmp);
	w_s = vector_length(w_tmp); 
	u_s = view->l + ((view->r - view->l) 
			* (((float)i + 0.5) / view->resolution[0]));
	v_s = view->b + ((view->t - view->b) 
			* (((float)j + 0.5) / view->resolution[1]));

	/* s = e + u_s * u + v_s * v + w_s * w */
	multiply_vector(u, u_s, u_tmp);
	multiply_vector(v, v_s, v_tmp);
	multiply_vector(w, w_s, w_tmp);
	add_vector(view->from, u_tmp, s);
	add_vector(s, v_tmp, s);
	add_vector(s, w_tmp, s);

	/* p(t) = e + td = e + t(s - e) */
	subtract_vector(s, view->from, d);
}


static void compute_specular_and_diffuse(double *diffuse, double *specular, 
		const point3 d, const point3 l, const point3 n, 
		double phong_pow) 
{
	point3 h, d_copy, l_copy;

	/* h = normalize(normalize(l) + normalize(-d)) */
	COPY_POINT3(d_copy, d);
	multiply_vector(d_copy, -1, d_copy);
	normalize(d_copy);
	COPY_POINT3(l_copy, l);
	normalize(l_copy);
	add_vector(l_copy, d_copy, h);
	normalize(h);

	/* diffuse = max(0, dot_product(n, l)) */
	*diffuse = MAX(0, dot_product(n, l_copy));

	/* specular = (dot_product(h, n))^p */
	*specular = pow(MAX(0, dot_product(h, n)), phong_pow);
}


static void protect_color_overflow(color c)
{
	int i;
	for (i = 0; i < 3; i++)
		if (c[i] > 1.0)
			c[i] = 1.0;
}


static double ray_hit_object(const point3 e, const point3 d, double t0, 
		double t1, point3 normal, const triangle_node triangles, 
		triangle_node *hit_triangle, const triangle_node last_triangle,
		const sphere_node spheres, sphere_node *hit_sphere, 
		const sphere_node last_sphere)
{
	triangle_node tri;
	sphere_node sphere;

	/* set these to not hit */
	*hit_triangle = NULL;
	*hit_sphere = NULL;

	for (tri = triangles; tri != NULL; tri = tri->next) {
		if (tri == last_triangle)
			continue;

		if (ray_triangle_intersection(e, d, &(tri->element), normal,
					&t0, &t1)) 
			/* hit is closest so far */
			*hit_triangle = tri;
	}

	/* check the spheres */
	for (sphere = spheres; sphere != NULL; sphere = sphere->next) {
		if (sphere == last_sphere)
			continue;

		if (ray_sphere_intersection(e, d, &(sphere->element), normal,
					&t0, &t1))
			*hit_sphere = sphere;
	}

	/* the sphere would have to have been closer */
	if (*hit_sphere)
		*hit_triangle = NULL;

	return t1;
}


static void calculate_local_color(color local_color, const color light_color,
		double diffuse, double specular, const object_fill *fill)
{
	color diffuse_part, specular_part;

	/* localColor = lightColor * (Kd * surfaceColor * diffuse 
	 				+ Ks * specular) */
	COPY_COLOR(diffuse_part, fill->fill_color);
	multiply_vector(diffuse_part, fill->Kd, diffuse_part);
	multiply_vector(diffuse_part, diffuse, diffuse_part);
	multiply_vectors(diffuse_part, light_color, diffuse_part);

	COPY_COLOR(specular_part, light_color);
	multiply_vector(specular_part, fill->Ks, specular_part);
	multiply_vector(specular_part, specular, specular_part);


	add_vector(diffuse_part, specular_part, diffuse_part);
	add_vector(local_color, diffuse_part, local_color);
}


static void calculate_reflection_ray(point3 r, const point3 d, const point3 n)
{
	/* r = d + 2(d . n)n */
	multiply_vector(n, 2.0 * -dot_product(d, n), r);
	add_vector(r, d, r);
}


static void calculate_refraction_ray(point3 t, const point3 d, const point3 n,
		double ior_from, double ior_to)
{
	/* t = (n_f(d - n(d.n)))/n_t - n*sqrt(1 - (n_f^2(1 - (d.n)^2))/n_t^2) */
	point3 tmp;
	double x;

	/* first part */
	multiply_vector(n, dot_product(d, n), tmp);
	subtract_vector(d, tmp, t);
	multiply_vector(t, ior_from, t);
	multiply_vector(t, 1.0/ior_to, t);

	/* second part */
	x = sqrt(1 - ((SQUARE(ior_from) * (1 - SQUARE(dot_product(d, n)))) 
		/ SQUARE(ior_to)));
	multiply_vector(n, x, tmp);
	subtract_vector(t, tmp, t);
}


__inline__ static unsigned int ray_color(const point3 e, int t,
		const point3 d, const triangle_node triangles, 
		const sphere_node spheres, const light_node lights, 
		color object_color, int bounces_left, 
		const triangle_node last_triangle, 
		const sphere_node last_sphere) 
{
	triangle_node hit_tri = NULL, light_hit_tri = NULL;
	sphere_node hit_sphere = NULL, light_hit_sphere = NULL;
	light_node light;
	point3 p, l, r, surface_normal, ignore_me;
	color reflection_part, refraction_part;
	double t1 = INFINITY, diffuse, specular, ior_f;
	object_fill fill;

	/* might be a reflection ray, so check how many times we've bounced */
	if (bounces_left < 0) {
		SET_COLOR(object_color, 0.0, 0.0, 0.0);
		return 0;
	}
	/* check for intersection with a sphere or triangle */
	t1 = ray_hit_object(e, d, t, INFINITY, surface_normal, triangles, 
			&hit_tri, last_triangle, spheres, &hit_sphere, 
			last_sphere);

	/* do lighting */
	if (!hit_tri && !hit_sphere) 
		return 0;

	/* p = e + t*d */
	multiply_vector(d, -t1, p);
	add_vector(e, p, p);

	/* pick the fill of the object that was hit */
	if (hit_tri)
		fill = hit_tri->element.triangle_fill;
	else
		fill = hit_sphere->element.sphere_fill;

	/* assume it's a shadow */
	SET_COLOR(object_color, 0.0, 0.0, 0.0);

	/* go through each light, calculate a vector pointing
	 * to it and see if it's intersected by any objects */
	for (light = lights; light != NULL; light = light->next) {
		/* calculate the vector pointing at the light */
		subtract_vector(p, light->element.position, l);

		/* check for intersection with an object. use ignore_me 
		 * because we don't care about this normal */
		ray_hit_object(p, l, EPSILON, INFINITY, ignore_me,
				triangles, &light_hit_tri, hit_tri,
				spheres, &light_hit_sphere, hit_sphere);

		/* the light wasn't blocked by any objects... */
		if (light_hit_tri || light_hit_sphere) 
			continue;

		compute_specular_and_diffuse(&diffuse, &specular, d, l,
		       			surface_normal, fill.phong_power);

		calculate_local_color(object_color, light->element.light_color,
					diffuse, specular, &fill);

		/* totalColor = localColor + Ks*reflection + T*refraction */
		/* spawn our reflection ray */
		if (fill.Ks > 0) {
			calculate_reflection_ray(r, d, surface_normal);

			/* if we hit something, add the color
			 * that's a result of that */
			if (ray_color(p, EPSILON, r, triangles, spheres,
					       	lights, reflection_part,
						bounces_left - 1, 
						hit_tri, hit_sphere)) {
				multiply_vector(reflection_part, fill.Ks, 
						reflection_part);
				add_vector(object_color, reflection_part,
						object_color);
			}
		}

		/* calculate refraction ray */
		if (fill.T > 0.0 && fill.index_of_refraction > 0.0) {
			ior_f = 1.0;
			if (last_triangle)
				ior_f = last_triangle->element.triangle_fill.index_of_refraction;
			else if (last_sphere)
				ior_f = last_sphere->element.sphere_fill.index_of_refraction;
			calculate_refraction_ray(r, d, surface_normal, ior_f, 
					fill.index_of_refraction);

			/* for some reason I noticed that removing the -t1
			* from this code above caused refraction to work,
			* so here it is */
			multiply_vector(d, t1, p);
			add_vector(e, p, p);

			if (ray_color(p, EPSILON, r, triangles, spheres, 
						lights, refraction_part,
						bounces_left - 1, hit_tri, 
						hit_sphere)) {
				multiply_vector(refraction_part, fill.T, 
						refraction_part);
				add_vector(object_color, refraction_part,
						object_color);
			}

		}

		protect_color_overflow(object_color);
	}

	return 1;
}

/* print a whole column on the sphere and check normals */

/* e = eye position
 * g = gaze direction
 * t = view-up vector */
static void calculate_basis_vectors(point3 u, point3 v, point3 w, 
		const viewpoint *view)
{
	/* w = -g */
	subtract_vector(view->from, view->at, w);
	normalize(w);

	/* u = (t x w) / (|t x w|) */
	cross_product(view->up, w, u);
	normalize(u);

	/* v = w x u */
	cross_product(w, u, v);
	normalize(v);
}


void ray_trace(unsigned char *pixels, unsigned int height, 
		unsigned int width, color background_color, 
		triangle_node triangles, sphere_node spheres, 
		light_node lights, const viewpoint *view)
{
	int i, j;
	point3 u, v, w, d;
	color object_color = {0.0, 0.0, 0.0};

	/* calculate u, v, w */
	calculate_basis_vectors(u, v, w, view);

	multiply_vector(u, -1.0, u);

	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {
			compute_viewing_ray(d, u, v, w, i, j, view);

			if (ray_color(view->from, 0.0, d, triangles, spheres, 
						lights, object_color, 
						MAX_REFLECTION_BOUNCES, NULL, 
						NULL)) {
				pixels[((i+(j*width))*3)+0] = object_color[0]*255;
				pixels[((i+(j*width))*3)+1] = object_color[1]*255;
				pixels[((i+(j*width))*3)+2] = object_color[2]*255;
			} else {
				pixels[((i+(j*width))*3)+0] = background_color[0]*255;
				pixels[((i+(j*width))*3)+1] = background_color[1]*255;
				pixels[((i+(j*width))*3)+2] = background_color[2]*255;
			}
		}
	}
}

