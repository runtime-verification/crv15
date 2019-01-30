/********************************************************************
 * File: sphere_list.c
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
 * of a linked list for holding sphere-like objects.
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "nff_entities.h"
#include "sphere_list.h"
#include "mem_alloc.h"
#include "__RVC_Instrument.h"


void append_sphere(const sphere *X, sphere_node *list)
{
	sphere_node tmp, new;

	/* allocate the node */
	new = mem_alloc(sizeof(struct snode));

	/* copy the struct elements */
	COPY_OBJECT_FILL(new->element.sphere_fill, X->sphere_fill);

	new->element.radius = X->radius;
	COPY_POINT3(new->element.center, X->center);

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


void delete_sphere_list(sphere_node *list)
{
	sphere_node pos, tmp;

	pos = *list;
	while (pos != NULL) {
		tmp = pos->next;
		free(pos);
		__RVC_free(pos);
		pos = tmp;
	}

	*list = NULL;
}

