/* modified by Patrick Carroll <patrick@patrickomatic.com> 
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
 */

/*
 * ReadNFF.c - Simple NFF file importer.  Uses lib to output to
 * many different raytracer formats.
 *
 * Author:  Eduard [esp] Schwan
 *
 * input file parameter...
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "triangle_list.h"
#include "sphere_list.h"
#include "light_list.h"
#include "nff_entities.h"
#include "graphics_math.h"
#include "mem_alloc.h"
#include "__RVC_Instrument.h"

static void show_error(char *s)
{
	fprintf(stderr, "%s\n", s);
}

/*----------------------------------------------------------------------
Comment.  Description:
    "#" [ string ]

Format:
    # [ string ]

    As soon as a "#" character is detected, the rest of the line is considered
    a comment.
----------------------------------------------------------------------*/
static void do_comment(FILE *fp)
{
	char	*cp;
	char	comment[256];
	
	fgets(comment, 255, fp);
	/* strip out newline */
	cp = (char*)strchr(comment, '\n');
	if (cp != NULL)
		*cp = '\0';

	/* don't do anything with cp */
}


/*----------------------------------------------------------------------
Viewpoint location.  Description:
    "v"
    "from" Fx Fy Fz
    "at" Ax Ay Az
    "up" Ux Uy Uz
    "angle" angle
    "hither" hither
    "resolution" xres yres

Format:

    v
    from %g %g %g
    at %g %g %g
    up %g %g %g
    angle %g
    hither %g
    resolution %d %d

The parameters are:

    From:  the eye location in XYZ.
    At:    a position to be at the center of the image, in XYZ world
	   coordinates.  A.k.a. "lookat".
    Up:    a vector defining which direction is up, as an XYZ vector.
    Angle: in degrees, defined as from the center of top pixel row to
	   bottom pixel row and left column to right column.
    Resolution: in pixels, in x and in y.

  Note that no assumptions are made about normalizing the data (e.g. the
  from-at distance does not have to be 1).  Also, vectors are not
  required to be perpendicular to each other.

  For all databases some viewing parameters are always the same:
    Yon is "at infinity."
    Aspect ratio is 1.0.

  A view entity must be defined before any objects are defined (this
  requirement is so that NFF files can be used by hidden surface machines).
----------------------------------------------------------------------*/
static void do_view(FILE *fp, viewpoint *view)
{
	float    x,y,z;
	point3 from;
	point3 at;
	point3 up;
	point3 look_at;
	float fov_angle;
	float aspect_ratio;
	float hither;
	int resx;
	int resy;
	
	if (fscanf(fp, " from %f %f %f", &x, &y, &z) != 3)
		goto fmterr;
	SET_POINT3(from, x, y, z);
	
	if (fscanf(fp, " at %f %f %f", &x, &y, &z) != 3)
		goto fmterr;
	SET_POINT3(at, x, y, z);
	
	if (fscanf(fp, " up %f %f %f", &x, &y, &z) != 3)
		goto fmterr;
	SET_POINT3(up, x, y, z);
	
	if (fscanf(fp, " angle %f", &fov_angle) != 1)
		goto fmterr;
	
	fscanf(fp, " hither %f", &hither);
	
	aspect_ratio = (float)1.0;
	
	fscanf(fp, " resolution %d %d", &resx, &resy);
	
	/* save the view */
	COPY_POINT3(view->from, from);
	COPY_POINT3(view->at, at);
	COPY_POINT3(view->up, up);
	view->angle = fov_angle;
	view->aspect_ratio = aspect_ratio;
	view->hither = hither;
	view->resolution[0] = resx;
	view->resolution[1] = resy;
	subtract_vector(from, at, look_at);
	view->t = tan(degrees_to_radians(HALF(view->angle))) 
			* vector_length(look_at);
 	view->b = -view->t;
 	view->r = view->t * (view->resolution[0] / view->resolution[1]);
 	view->l = -view->r;
	
	return;
fmterr:
	show_error("NFF view syntax error");
	exit(1);
}


/*----------------------------------------------------------------------
Positional light.  A light is defined by XYZ position.  Description:
    "l" X Y Z

Format:
    l %g %g %g

    All light entities must be defined before any objects are defined (this
    requirement is so that NFF files can be used by hidden surface machines).
    Lights have a non-zero intensity of no particular value [this definition
    may change soon, with the addition of an intensity and/or color].
----------------------------------------------------------------------*/
static void do_light(FILE *fp, light_node *lights)
{
	float r, g, b;	
	float x, y, z;
	int num_read;
	light l;
	
	/* try to read 6 but might only read 3 */
	num_read = fscanf(fp, "%g %g %g %g %g %g", &x, &y, &z, &r, &g, &b);
	if (num_read != 3 && num_read != 6) {
		show_error("Light source syntax error");
		exit(1);
	}

	SET_POINT3(l.position, x, y, z);
	if (num_read == 6) {
		SET_COLOR(l.light_color, r, g, b);
	} else {	/* default to white */
		SET_COLOR(l.light_color, 1.0, 1.0, 1.0);
	}

	l.intensity = 0.0;

	append_light(&l, lights);
}


/*----------------------------------------------------------------------
Background color.  A color is simply RGB with values between 0 and 1:
    "b" R G B

Format:
    b %g %g %g

    If no background color is set, assume RGB = {0,0,0}.
----------------------------------------------------------------------*/
static void do_background(FILE *fp, color background_color)
{
	float r, g, b;
	
	if (fscanf(fp, "%f %f %f", &r, &g, &b) != 3) {
		show_error("background color syntax error");
		exit(1);
	}

	SET_COLOR(background_color, r, g, b);
}


/*----------------------------------------------------------------------
Fill color and shading parameters.  Description:
     "f" red green blue Kd Ks Shine T index_of_refraction

Format:
    f %g %g %g %g %g %g %g %g

    RGB is in terms of 0.0 to 1.0.

    Kd is the diffuse component, Ks the specular, Shine is the Phong cosine
    power for highlights, T is transmittance (fraction of light passed per
    unit).  Usually, 0 <= Kd <= 1 and 0 <= Ks <= 1, though it is not required
    that Kd + Ks == 1.  Note that transmitting objects ( T > 0 ) are considered
    to have two sides for algorithms that need these (normally objects have
    one side).

    The fill color is used to color the objects following it until a new color
    is assigned.
----------------------------------------------------------------------*/
static void do_fill(FILE *fp, object_fill *current_fill)
{
	float r, g, b, ka, kd, ks, ks_spec, phong_pow, ang, t, ior;
	
	if (fscanf(fp, "%f %f %f",&r, &g, &b) != 3) {
		show_error("fill color syntax error");
		exit(1);
	}
	SET_COLOR(current_fill->fill_color, r, g, b);
	
	if (fscanf(fp, "%f %f %f %f %f", &kd, &ks, &phong_pow, &t, &ior) != 5) {
		show_error("fill material syntax error");
		exit(1);
	}
	
	/* some parms not input in NFF, so hard-coded. */
	ka = (float)0.1;
	ks_spec = ks;
	/* convert phong_pow back into phong hilight angle. */
	/* reciprocal of formula in libpr1.c, lib_output_color() */
	if (phong_pow < 1.0)
		phong_pow = 1.0;
	ang = (float)((180.0 / PI) * acos(exp(log(0.5) / phong_pow)));

	/* XXX do i need to do anything with ang? */
	current_fill->Kd = kd;
	current_fill->Ks = ks;
	current_fill->T = t;
	current_fill->index_of_refraction = ior;
	current_fill->phong_power = phong_pow;
}


/*----------------------------------------------------------------------
Cylinder or cone.  A cylinder is defined as having a radius and an axis
    defined by two points, which also define the top and bottom edge of the
    cylinder.  A cone is defined similarly, the difference being that the apex
    and base radii are different.  The apex radius is defined as being smaller
    than the base radius.  Note that the surface exists without endcaps.  The
    cone or cylinder description:

    "c"
    base.x base.y base.z base_radius
    apex.x apex.y apex.z apex_radius

Format:
    c
    %g %g %g %g
    %g %g %g %g

    A negative value for both radii means that only the inside of the object is
    visible (objects are normally considered one sided, with the outside
    visible).  Note that the base and apex cannot be coincident for a cylinder
    or cone.
----------------------------------------------------------------------*/
static void do_cone(FILE *fp)
{
	point4 base_pt;
	point4 apex_pt;
	float x0, y0, z0, x1, y1, z1, r0, r1;
	
	if (fscanf(fp, " %f %f %f %f %f %f %f %f", &x0, &y0, &z0, &r0,
		&x1, &y1, &z1, &r1) != 8) {
		show_error("cylinder or cone syntax error");
		exit(1);
	}
	if (r0 < 0.0) {
		r0 = -r0;
		r1 = -r1;
	}
	SET_POINT4(base_pt, x0, y0, z0, r0);
	SET_POINT4(apex_pt, x1, y1, z1, r1);
	
	/* don't do anything with base_pt, apex_pt, output_format */
}


/*----------------------------------------------------------------------
Sphere.  A sphere is defined by a radius and center position:
    "s" center.x center.y center.z radius

Format:
    s %g %g %g %g

    If the radius is negative, then only the sphere's inside is visible
    (objects are normally considered one sided, with the outside visible).
----------------------------------------------------------------------*/
static void do_sphere(FILE *fp, sphere_node *spheres, const object_fill *fill)
{
	float x, y, z, r;
	sphere s;
	
	if (fscanf(fp, "%f %f %f %f", &x, &y, &z, &r) != 4) {
		show_error("sphere syntax error");
		exit(1);
	}
	
	COPY_OBJECT_FILL(s.sphere_fill, *fill);
	SET_POINT3(s.center, x, y, z);
	s.radius = r;
	append_sphere(&s, spheres);
}


/*----------------------------------------------------------------------
Polygon.  A polygon is defined by a set of vertices.  With these databases,
    a polygon is defined to have all points coplanar.  A polygon has only
    one side, with the order of the vertices being counterclockwise as you
    face the polygon (right-handed coordinate system).  The first two edges
    must form a non-zero convex angle, so that the normal and side visibility
    can be determined.  Description:

    "p" total_vertices
    vert1.x vert1.y vert1.z
    [etc. for total_vertices vertices]

Format:
    p %d
    [ %g %g %g ] <-- for total_vertices vertices
----------------------------------------------------------------------
Polygonal patch.  A patch is defined by a set of vertices and their normals.
    With these databases, a patch is defined to have all points coplanar.
    A patch has only one side, with the order of the vertices being
    counterclockwise as you face the patch (right-handed coordinate system).
    The first two edges must form a non-zero convex angle, so that the normal
    and side visibility can be determined.  Description:

    "pp" total_vertices
    vert1.x vert1.y vert1.z norm1.x norm1.y norm1.z
    [etc. for total_vertices vertices]

Format:
    pp %d
    [ %g %g %g %g %g %g ] <-- for total_vertices vertices
----------------------------------------------------------------------*/
static void do_poly(FILE *fp, triangle_node *triangles, 
		const object_fill *current_fill) 
{
	int vertcount, number_vertices;
	point3 *vertices = NULL, *normals = NULL;
	triangle t;
	float x, y, z;
	unsigned int i, is_patch = 1;
	char c;
	
	c = getc(fp);
	if (c != 'p') {
		ungetc(c, fp);
		is_patch = 0;
	}
	
	if (fscanf(fp, "%d", &number_vertices) != 1)
		goto fmterr;

	vertices = mem_alloc(sizeof(point3) * number_vertices);
	
	if (is_patch) 
		normals = mem_alloc(sizeof(point3) * number_vertices);
	
	/* read all the vertices into temp array */
	for (vertcount = 0; vertcount < number_vertices; vertcount++) {
		if (fscanf(fp, " %f %f %f", &x, &y, &z) != 3)
			goto fmterr;

		SET_POINT3(vertices[vertcount], x, y, z);
		
		if (is_patch) {
			if (fscanf(fp, " %f %f %f", &x, &y, &z) != 3)
				goto fmterr;
			SET_POINT3(normals[vertcount], x, y, z);
		}
	}
	
	t.is_patch = is_patch;
	COPY_OBJECT_FILL(t.triangle_fill, *current_fill);

	/* use vertice[0] to make (num_vertices - 2) triangles */
	for (i = 1; i < number_vertices - 1; i++) {
		COPY_POINT3(t.vertices[0], vertices[0]);
		COPY_POINT3(t.vertices[1], vertices[i]);
		COPY_POINT3(t.vertices[2], vertices[i+1]);

		if (is_patch) {
			COPY_POINT3(t.vertice_normals[0], normals[0]);
			COPY_POINT3(t.vertice_normals[1], normals[i]);
			COPY_POINT3(t.vertice_normals[2], normals[i+1]);
		}

		triangle_normal_vector(t.normal, t.vertices[0], t.vertices[1],
				t.vertices[2]);

		/* put the triangle on our list */
		append_triangle(&t, triangles);
	}
	
	free(vertices);
	__RVC_free(vertices);
	if (is_patch) {
		free(normals);
		__RVC_free(normals);
	}

	return;
fmterr:
	show_error("polygon or patch syntax error");
	exit(1);
}


void parse_nff(FILE *fp, viewpoint *view, color background_color, 
		triangle_node *triangles, sphere_node *spheres,
		light_node *lights) 
{
	int c;
	object_fill current_fill;

	/* initialize this to make gcc happy */
	current_fill.fill_color[0] = 0.0; 
	current_fill.fill_color[1] = 0.0; 
	current_fill.fill_color[2] = 0.0; 
	current_fill.Kd = 0.0;
	current_fill.Ks = 0.0;
	current_fill.T = 0.0;
	current_fill.index_of_refraction = 0.0;
	current_fill.phong_power = 0.0;

	/* NFF-spec says to default to 0, 0, 0 */
	background_color[0] = 0; 
	background_color[1] = 0; 
	background_color[2] = 0; 
	
	while ((c = getc(fp)) != EOF) {
		switch (c) {
			case ' ':            /* white space */
			case '\t':
			case '\n':
			case '\f':
			case '\r':
				continue;
			case '#':            /* comment */
				do_comment(fp);
				break;
			case 'v':            /* view point */
				do_view(fp, view);
				break;
			case 'l':            /* light source */
				do_light(fp, lights);
				break;
			case 'b':            /* background color */
				do_background(fp, background_color);
				break;
			case 'f':            /* fill material */
				do_fill(fp, &current_fill);
				break;
			case 'c':            /* cylinder or cone */
				do_cone(fp);
				break;
			case 's':            /* sphere */
				do_sphere(fp, spheres, &current_fill);
				break;
			case 'p':            /* polygon or patch */
				do_poly(fp, triangles, &current_fill);
				break;
			default:            /* unknown */
				show_error("unknown NFF primitive code");
				exit(1);
		}
	}
} /* parse_nff */

