/********************************************************************
 * File: triangle_list.c
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
 * This file contains functions for a very restricted implementation
 * of a linked list for holding triangle-like objects.
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "nff_entities.h"
#include "triangle_list.h"
#include "mem_alloc.h"
#include "__RVC_Instrument.h"


void append_triangle(const triangle *X, triangle_node *list)
{
	triangle_node tmp, new;
	int i;

	/* allocate the node */
	new = mem_alloc(sizeof(struct tnode));

	new->element.is_patch = X->is_patch;

	/* copy the struct elements */
	COPY_OBJECT_FILL(new->element.triangle_fill, X->triangle_fill);

	for (i = 0; i < 3; i++) {
		COPY_POINT3(new->element.vertices[i], X->vertices[i]);

		if (X->is_patch)
			COPY_POINT3(new->element.vertice_normals[i], 
					X->vertice_normals[i]);
		COPY_POINT3(new->element.normal, X->normal);
	}

	new->next = NULL;
	if (*list == NULL) { /* nothing in it yet */
		*list = new;
	} else {
		/* move to the end */
		for (tmp = *list; tmp->next != NULL; tmp = tmp->next)
		       ;
		tmp->next = new;
	}
}


void delete_triangle_list(triangle_node *list)
{
	triangle_node pos, tmp;

	pos = *list;
	while (pos != NULL) {
		tmp = pos->next;
		free(pos);
		__RVC_free(pos);
		pos = tmp;
	}

	*list = NULL;
}

