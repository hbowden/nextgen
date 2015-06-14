/**
 * GreenPois0n Absinthe - mb2.h
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
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "macho.h"
#include "symtab.h"

static int get_file_size(int fd, off_t *fileSize)
{
    struct stat stbuf;
    
    if ((fstat(fd, &stbuf) != 0) || (!S_ISREG(stbuf.st_mode)))
    {
        perror("fstat");
        return -1;
    }
    
    *fileSize = stbuf.st_size;
    
    return 0;
}

/*
 * Mach-O Functions
 */
macho_t* macho_create() {
	macho_t* macho = (macho_t*) malloc(sizeof(macho_t));
	if (macho) {
		memset(macho, '\0', sizeof(*macho));
	}
	return macho;
}

macho_t* macho_load(unsigned char* data, unsigned int size) {
	unsigned int i;
	macho_t* macho = NULL;

	macho = macho_create();
	if (macho) {
		macho->offset = 0;
		macho->data = data;
		macho->size = size;

		//debug("Loading Mach-O header\n");
		macho->header = macho_header_load(macho);
		if (macho->header == NULL) {
			printf("Unable to load Mach-O header information\n");
			macho_free(macho);
			return NULL;
		}

		//debug("Loading Mach-O command\n");
		macho->command_count = macho->header->ncmds;
		macho->commands = macho_commands_load(macho);
		if (macho->commands == NULL) {
			printf("Unable to parse Mach-O load commands\n");
			macho_free(macho);
			return NULL;
		}

		// initialize the buffers for the different types.
		uint32_t seg_count = 0;
		uint32_t symtab_count = 0;
		uint32_t unk_count = 0;
		for (i = 0; i < macho->command_count; i++) {
			switch (macho->commands[i]->info->cmd) {
			case MACHO_CMD_SEGMENT:
				seg_count++;
				break;
			case MACHO_CMD_SYMTAB:
				symtab_count++;
				break;
			default:
				unk_count++;
				break;
			}
		}

		macho->segments = macho_segments_create(seg_count);
		macho->segment_count = 0;
		macho->symtabs = macho_symtabs_create(symtab_count);
		macho->symtab_count = 0;

		if (unk_count > 0) {
			printf("WARNING: %d unhandled Mach-O Commands\n", unk_count);
		}

		//debug("Handling %d Mach-O commands\n", macho->command_count);
		for (i = 0; i < macho->command_count; i++) {
			macho_handle_command(macho, macho->commands[i]);
		}
		// TODO: Remove the line below this once debugging is finished
		//macho_debug(macho);
	}
	return macho;
}

macho_t* macho_open(const char* path) {
	int fd, rtrn;
	macho_t* macho = NULL;
	unsigned int size = 0;
	unsigned char *data = NULL;

	fd = open(path, O_RDONLY);
    if(fd < 0)
    {
	    printf("Unable to read Mach-O file.\n");
		macho_free(macho);
		return NULL;
	}

    rtrn = get_file_size(fd, (off_t *)&size);
    if(rtrn < 0)
    {
	    printf("Unable get Mach-O file size.\n");
		macho_free(macho);
		return NULL;
	}

	rtrn = read(fd, data, size);
	if(rtrn < (int)size)
	{
		printf("Unable to read Mach-O file.\n");
		macho_free(macho);
		return NULL;
	}   

	macho = macho_load(data, size);
	if (macho == NULL)
	{
		printf("Unable to load Mach-O file\n");
		return NULL;
	}
	return macho;
}

uint32_t macho_lookup(macho_t* macho, const char* sym) {
	unsigned int i = 0;
	unsigned int j = 0;
	nlist* nl = NULL;
	macho_symtab_t* symtab = NULL;
	for (i = 0; i < macho->symtab_count; i++) {
		symtab = macho->symtabs[i];
		for (j = 0; j < symtab->nsyms; j++) {
			nl = &symtab->symbols[j];
			if (nl->n_un.n_name != NULL) {
				if (strcmp(sym, nl->n_un.n_name) == 0) {
					return nl->n_value;
				}
			}
		}
	}
	return 0;
}

void macho_list_symbols(macho_t* macho, void (*print_func)(const char*, uint32_t, void*), void* userdata) {
	int i = 0;
	int j = 0;
	nlist* nl = NULL;
	macho_symtab_t* symtab = NULL;
	for (i = 0; i < macho->symtab_count; i++) {
		symtab = macho->symtabs[i];
		for (j = 0; j < symtab->nsyms; j++) {
			nl = &symtab->symbols[j];
			if ((nl->n_un.n_name != NULL) && (nl->n_value != 0)) {
				print_func(nl->n_un.n_name, nl->n_value, userdata);
			}
		}
	}
}

void macho_debug(macho_t* macho) {
	if (macho) {
		if (macho->header)
			macho_header_debug(macho->header);
		if (macho->commands)
			macho_commands_debug(macho->commands);
		if (macho->segments)
			macho_segments_debug(macho->segments);
	}
}

void macho_free(macho_t* macho) {
	if (macho) {
		if (macho->header) {
			macho_header_free(macho->header);
			macho->header = NULL;
		}
		if (macho->commands) {
			macho_commands_free(macho->commands);
			macho->commands = NULL;
		}

		if (macho->segments) {
			macho_segments_free(macho->segments);
			macho->segments = NULL;
		}
		if (macho->symtabs) {
			macho_symtabs_free(macho->symtabs);
			macho->symtabs = NULL;
		}

		if (macho->data) {
			macho->size = 0;
			macho->offset = 0;
			macho->data = NULL;
		}

		free(macho);
	}
}

/*
 * Mach-O Header Functions
 */
macho_header_t* macho_header_create() {
	macho_header_t* header = (macho_header_t*) malloc(sizeof(macho_header_t));
	if (header) {
		memset(header, '\0', sizeof(macho_header_t));
	}
	return header;
}

macho_header_t* macho_header_load(macho_t* macho) {
	unsigned int size = 0;
	unsigned int offset = 0;
	unsigned char* data = NULL;
	macho_header_t* header = NULL;
	if (macho) {
		data = macho->data;
		size = macho->size;
		offset = macho->offset;
		header = macho_header_create();
		if (header) {
			memcpy(header, &data[offset], sizeof(macho_header_t));
			macho->offset += sizeof(macho_header_t);
		}
	}
	return header;
}

void macho_header_debug(macho_header_t* header) {
	if (header) {
		
	}
}

void macho_header_free(macho_header_t* header) {
	if (header) {
		free(header);
	}
}

int macho_handle_command(macho_t* macho, macho_command_t* command) {
	int ret = 0;
	if (macho) {
		//printf("handle command %x\n", command->info->cmd);
		// If load command is a segment command, then load a segment
		//  if a symbol table, then load a symbol table... etc...
		switch (command->info->cmd) {
		case MACHO_CMD_SEGMENT:
			// segment of this file to be mapped
		{
			macho_segment_t* seg = macho_segment_load(macho->data,
					command->offset);
			if (seg) {
				macho->segments[macho->segment_count++] = seg;
			} else {
				printf("Could not load segment at offset 0x%x\n", command->offset);
			}
		}
			break;
		case MACHO_CMD_SYMTAB:
			// link-edit stab symbol table info
		{
			macho_symtab_t* symtab = macho_symtab_load(macho->data+command->offset, macho->data);
			if (symtab) {
				macho->symtabs[macho->symtab_count++] = symtab;
			} else {
				printf(
						"Could not load symtab at offset 0x%x\n", command->offset);
			}
		}
			break;
		default:
			ret = -1;
			break;
		}
	}
	return ret;
}

/*
 * Mach-O Commands Functions
 */
macho_command_t** macho_commands_create(uint32_t count) {
	uint32_t size = (count + 1) * sizeof(macho_command_t*);
	macho_command_t** commands = (macho_command_t**) malloc(size);
	if (commands) {
		memset(commands, '\0', size);
	}
	return commands;
}

macho_command_t** macho_commands_load(macho_t* macho) {
	int i = 0;
	uint32_t count = 0;
	macho_command_t** commands = NULL;
	if (macho) {
		count = macho->command_count;
		//debug("Creating Mach-O commands array\n");
		commands = macho_commands_create(count);
		if (commands == NULL) {
			printf("Unable to create Mach-O commands array\n");
			return NULL;
		}

		//debug("Loading Mach-O commands array\n");
		for (i = 0; i < count; i++) {
			//debug("Loading Mach-O command %d from offset 0x%x\n", i, macho->offset);
			commands[i] = macho_command_load(macho->data, macho->offset);
			if (commands[i] == NULL) {
				printf("Unable to parse Mach-O load command\n");
				macho_commands_free(commands);
				return NULL;
			}
			macho->offset += commands[i]->size;
		}
	}
	return commands;
}

void macho_commands_debug(macho_command_t** commands) {
	int i = 0;
	if (commands) {
		while (commands[i] != NULL) {
			macho_command_debug(commands[i++]);
		}
	}
}

void macho_commands_free(macho_command_t** commands) {
	int i = 0;
	if (commands) {
		while (commands[i] != NULL) {
			macho_command_free(commands[i]);
			commands[i] = NULL;
			i++;
		}
		free(commands);
	}
}

/*
 * Mach-O Segments Functions
 */
macho_segment_t** macho_segments_create(uint32_t count) {
	if (count == 0)
		return NULL;
	int size = (count + 1) * sizeof(macho_segment_t*);
	macho_segment_t** segments = (macho_segment_t**) malloc(size);
	if (segments) {
		memset(segments, '\0', size);
	}
	return segments;
}

macho_segment_t** macho_segments_load(macho_t* macho) {
	macho_segment_t** segments = macho_segments_create(0);
	return segments;
}

void macho_segments_debug(macho_segment_t** segments) {
	int i = 0;
	while (segments[i]) {
		macho_segment_debug(segments[i]);
		i++;
	}
}

void macho_segments_free(macho_segment_t** segments) {
	if (segments) {
		int i = 0;
		while (segments[i]) {
			macho_segment_free(segments[i]);
			i++;
		}
		free(segments);
	}
}

/*
 * Mach-O Symtab Functions
 */
macho_symtab_t** macho_symtabs_create(uint32_t count) {
	if (count == 0)
		return NULL;
	int size = (count + 1) * sizeof(macho_symtab_t*);
	macho_symtab_t** symtabs = (macho_symtab_t**) malloc(size);
	if (symtabs) {
		memset(symtabs, '\0', size);
	}
	return symtabs;
}

void macho_symtabs_debug(macho_symtab_t** symtabs) {
	int i = 0;
	while (symtabs[i]) {
		macho_symtab_debug(symtabs[i]);
		i++;
	}
}

void macho_symtabs_free(macho_symtab_t** symtabs) {
	if (symtabs) {
		int i = 0;
		while (symtabs[i]) {
			macho_symtab_free(symtabs[i]);
			i++;
		}
		free(symtabs);
	}
}

/*
 * Mach-O Sections Functions
 */
macho_section_t** macho_sections_create(uint32_t count) {
	macho_section_t** sections = NULL;
	return sections;
}

macho_section_t** macho_sections_load(macho_t* macho) {
	macho_section_t** sections = macho_sections_create(0);
	return sections;
}

void macho_sections_debug(macho_section_t** sections) {

}

void macho_sections_free(macho_section_t** sections) {
	// TODO: Loop through and free each item
	if (sections) {
		free(sections);
	}
}
