/**
 * GreenPois0n Absinthe - macho_section.h
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

#ifndef MACHO_SECTION_H_
#define MACHO_SECTION_H_

typedef struct macho_section_info_t {
	uint32_t magic;
} macho_section_info_t;

typedef struct macho_section_t {
	macho_section_info_t* info;
} macho_section_t;

/*
 * Mach-O Segment Functions
 */
macho_section_t* macho_section_create(void);
macho_section_t* macho_section_load(unsigned char* data, unsigned int offset);
void macho_section_debug(macho_section_t* section);
void macho_section_free(macho_section_t* section);

/*
 * Mach-O Segment Info Functions
 */
macho_section_info_t* macho_section_info_create(void);
macho_section_info_t* macho_section_info_load(unsigned char* data, unsigned int offset);
void macho_section_info_debug(macho_section_info_t* info);
void macho_section_info_free(macho_section_info_t* info);

#endif /* MACHO_SECTION_H_ */
