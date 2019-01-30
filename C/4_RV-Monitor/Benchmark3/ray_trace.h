/********************************************************************
 * File: ray_trace.h
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
 * This file contains prototypes for a function implementing a ray 
 * tracer.  ray_trace() is the main algorithm which should be called for
 * doing ray tracing.
 ********************************************************************/
#ifndef __RAY_TRACE_H
#define __RAY_TRACE_H

#include "nff_entities.h"
#include "triangle_list.h"
#include "sphere_list.h"
#include "light_list.h"


void ray_trace(unsigned char *pixels, unsigned int height, 
		unsigned int width, color background_color, 
		triangle_node triangles, sphere_node spheres,
		light_node lights, const viewpoint *view);

#endif

