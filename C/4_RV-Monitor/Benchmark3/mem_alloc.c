/********************************************************************
 * File: mem_alloc.c
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
 * This file contains a function for allocating memory and checking
 * the return value of malloc().
 ********************************************************************/
#include <stdlib.h>
#include <stdio.h>

#include "__RVC_Instrument.h"

/* always die when out of memory.  there's nothing we can do so there's
 * no point in trying to be graceful */
void *mem_alloc(size_t n)
{
	void *tmp = malloc(n);
	if (!tmp) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}

	__RVC_malloc(tmp);
	return tmp;
}
