/********************************************************************
 * File: sphere_list.h
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
 * This file contains prototypes for a very restricted implementation
 * of a linked list for holding sphere-like objects.
 ********************************************************************/
#ifndef __SPHERE_LIST_H
#define __SPHERE_LIST_H

#include "nff_entities.h"


struct snode;

typedef struct snode *sphere_node;

struct snode {
	sphere element;
	sphere_node next;
};


void append_sphere(const sphere *X, sphere_node *list);

void delete_sphere_list(sphere_node *list);


#endif

