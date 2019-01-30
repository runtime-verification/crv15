/*
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
#ifndef __READ_NFF_H
#define __READ_NFF_H

#include "nff_entities.h"
#include "triangle_list.h"
#include "light_list.h"


void parse_nff(FILE *fp, viewpoint *view, color background_color, 
		triangle_node *triangles, sphere_node *spheres,
		light_node *lights);

#endif

