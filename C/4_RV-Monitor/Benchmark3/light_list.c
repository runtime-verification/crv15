/********************************************************************
 * File: light_list.c
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
 * of a linked list for holding light objects.
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "nff_entities.h"
#include "light_list.h"
#include "mem_alloc.h"
#include "__RVC_Instrument.h"

void append_light(const light *X, light_node *list)
{
	light_node tmp, new;

	/* allocate the lnode */
	new = mem_alloc(sizeof(struct lnode));

	COPY_POINT3(new->element.position, X->position);
	COPY_COLOR(new->element.light_color, X->light_color);
	new->element.intensity = X->intensity;

	/* insert it */
	new->next = NULL;
	if (*list == NULL) { /* nothing in it yet */
		*list = new;
	} else {
		/* move to the end */
		for (tmp = *list; tmp->next != NULL; tmp = tmp->next) {
			__RVC_access(tmp);
		}
		tmp->next = new;
	}
}


void delete_light_list(light_node *list)
{
	light_node pos, tmp;

	pos = *list;
	while (pos != NULL) {
		tmp = pos->next;
		__RVC_access(pos);
		free(pos);
		__RVC_free(pos);
		pos = tmp;
	}

	*list = NULL;
}

