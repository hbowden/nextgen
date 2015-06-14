/**
 * GreenPois0n Absinthe - macho_segment.c
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

#include "segment.h"

/*
 * Mach-O Segment Functions
 */
macho_segment_t* macho_segment_create() {
	macho_segment_t* segment = (macho_segment_t*) malloc(sizeof(macho_segment_t));
	if(segment) {
		memset(segment, '\0', sizeof(macho_segment_t));
	}
	return segment;
}

macho_segment_t* macho_segment_load(unsigned char* data, unsigned int offset) {
	unsigned char* address = NULL;
	macho_segment_t* segment = macho_segment_create();
	if (segment) {
		segment->command = macho_segment_cmd_load(data, offset);
		if (!segment->command) {
			macho_segment_free(segment);
			return NULL;
		}
		segment->name = strdup(segment->command->segname);
		segment->size = segment->command->filesize;
		segment->offset = segment->command->fileoff;
		segment->address = segment->command->vmaddr;
		segment->data = NULL; // ????
		//segment->sections = malloc(segment->cmd->nsects * sizeof(macho_section_t*));
	}
	return segment;
}

void macho_segment_debug(macho_segment_t* segment) {

}

void macho_segment_free(macho_segment_t* segment) {
	if (segment) {
		if (segment->command) {
			macho_segment_cmd_free(segment->command);
		}
		if (segment->name) {
			free(segment->name);
		}
		free(segment);
	}
}

/*
 * Mach-O Segment Info Functions
 */
macho_segment_cmd_t* macho_segment_cmd_create() {
	macho_segment_cmd_t* info = malloc(sizeof(macho_segment_cmd_t));
	if (info) {
		memset(info, '\0', sizeof(macho_segment_cmd_t));
	}
	return info;
}

macho_segment_cmd_t* macho_segment_cmd_load(unsigned char* data, unsigned int offset) {
	macho_segment_cmd_t* cmd = macho_segment_cmd_create();
	if (cmd) {
		memcpy(cmd, data+offset, sizeof(macho_segment_cmd_t));
		//macho_segment_cmd_debug(cmd);
	}
	return cmd;
}

void macho_segment_cmd_debug(macho_segment_cmd_t* cmd) {
	
}

void macho_segment_cmd_free(macho_segment_cmd_t* cmd) {
	if (cmd) {
		free(cmd);
	}
}
