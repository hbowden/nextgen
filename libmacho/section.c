/**
 * GreenPois0n Absinthe - macho_section.c
 * Copyright (C) 2010 Chronic-Dev Team
 * Copyright (C) 2010 Joshua Hill
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "section.h"

/*
 * Mach-O Segment Functions
 */
macho_section_t* macho_section_create() {
	macho_section_t* section = NULL;
	return section;
}

macho_section_t* macho_section_load(unsigned char* data, unsigned int offset) {
	macho_section_t* section = macho_section_create();
	return section;
}

void macho_section_debug(macho_section_t* section) {

}

void macho_section_free(macho_section_t* section) {

}

/*
 * Mach-O Segment Info Functions
 */
macho_section_info_t* macho_section_info_create() {
	macho_section_info_t* info = NULL;
	return info;
}

macho_section_info_t* macho_section_info_load(unsigned char* data, unsigned int offset) {
	macho_section_info_t* info = macho_section_info_create();
	return info;
}

void macho_section_info_debug(macho_section_info_t* info) {

}

void macho_section_info_free(macho_section_info_t* info) {

}
