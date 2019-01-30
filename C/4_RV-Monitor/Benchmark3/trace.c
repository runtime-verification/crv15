/********************************************************************
 * File: trace.c
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
 *
 * This is the main part of my ray tracing program.  It does not get
 * called with any arguments and it takes it's input on stdin.  It's
 * input should be NFF-formatted geometry.
 ********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppm.h"
#include "nff_entities.h"
#include "read_nff.h"
#include "triangle_list.h"
#include "sphere_list.h"
#include "light_list.h"
#include "ray_trace.h"
#include "mem_alloc.h"

#define DEFAULT_OUTPUT_FILE	"trace.ppm"


/* returns whether or not the file handle needs to be closed */
static unsigned int check_args(int argc, char **argv, FILE **out)
{
	unsigned int need_to_close = 1;

	if (argc > 2 || (argc == 2 && (strcmp(argv[1], "-h") == 0
				|| strcmp(argv[1], "--help") == 0))) {
		fprintf(stderr, "Usage: %s [ file.ppm ]\n", argv[0]);
		fprintf(stderr, "\t If not specified file.ppm will default to \"trace.ppm\"\n");
		exit(1);
	} else if (argc == 2) {
		if (strcmp(argv[1], "-") == 0) {
			*out = stdout;
			need_to_close = 0;
		} else {
			*out = fopen(argv[1], "wb");
		}
	} else {
		*out = fopen(DEFAULT_OUTPUT_FILE, "wb");
	}

	if (!*out) {
		fprintf(stderr, "Could not open file\n");
		exit(2);
	}

	return need_to_close;
}


int main(int argc, char **argv)
{
	unsigned char *pixels;
	color background_color;
	triangle_node triangles = NULL;
	sphere_node spheres = NULL;
	light_node lights = NULL;
	viewpoint view;
	unsigned int do_close;
	FILE *output = NULL;

	/* check how we were called */
	do_close = check_args(argc, argv, &output);

	/* read our input */
	parse_nff(stdin, &view, background_color, &triangles, &spheres, 
			&lights);

	/* allocate by the given resolution */
	pixels = mem_alloc(sizeof(unsigned char) * view.resolution[0]
			* view.resolution[1] * 3);

	/* do the ray tracing with the given geometry */
	ray_trace(pixels, view.resolution[1], view.resolution[0], 
			background_color, triangles, spheres, lights, &view);

	/* write the results */
	write_ppm_file(output, pixels, view.resolution[1], view.resolution[0]);

	if (do_close)
		fclose(output);

	/* free the linked lists */
	delete_triangle_list(&triangles);
	delete_sphere_list(&spheres);
	delete_light_list(&lights);

	return 0;
}

