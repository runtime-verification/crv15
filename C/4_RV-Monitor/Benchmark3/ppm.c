/********************************************************************
 * File: ppm.c
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
 * This file contains a function for writing a PPM-format image file
 ********************************************************************/
#include <stdio.h>

#include "ppm.h"


void write_ppm_file(FILE *output, unsigned char *pixels, int height, int width)
{
	fprintf(output, "P6\n%d %d\n%d\n", width, height, 255);
	fwrite(pixels, 1, height * width * 3, output);
}

