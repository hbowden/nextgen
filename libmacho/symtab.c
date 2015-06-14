/**
 * GreenPois0n Absinthe - macho_symtab.c
 * Copyright (C) 2011 Chronic-Dev Team
 * Copyright (C) 2011 Hanéne Samara
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

#include "symtab.h"

/*
 * Mach-O Symtab Functions
 */
macho_symtab_t* macho_symtab_create() {
	macho_symtab_t* symtab = malloc(sizeof(macho_symtab_t));
	if (symtab) {
		memset(symtab, '\0', sizeof(macho_symtab_t));
	}
	return symtab;
}

macho_symtab_t* macho_symtab_load(unsigned char* cmd, unsigned char* data) {
	macho_symtab_t* symtab = macho_symtab_create();
	if (symtab) {
		symtab->cmd = macho_symtab_cmd_load(cmd);
		if (!symtab->cmd) {
			macho_symtab_free(symtab);
			return NULL;
		}
		symtab->nsyms = symtab->cmd->nsyms;
		symtab->symbols = (struct nlist*)(data+symtab->cmd->symoff);
		int i;
		for (i = 0; i < symtab->nsyms; i++) {
			uint32_t off = symtab->symbols[i].n_un.n_strx;
			if (off >= symtab->cmd->strsize) {
				symtab->symbols[i].n_un.n_name = NULL;
			} else {
				symtab->symbols[i].n_un.n_name = (char*)(data+symtab->cmd->stroff + off);
			}
		}
		//macho_symtab_debug(symtab);
	}
	return symtab;
}

void macho_symtab_debug(macho_symtab_t* symtab) {
	int i;
	for (i = 0; i < symtab->nsyms; i++) {
		struct nlist sym = symtab->symbols[i];
		if (sym.n_un.n_name) {
			
		} else {
			
		}
	}
}

void macho_symtab_free(macho_symtab_t* symtab) {
	if (symtab) {
		if (symtab->cmd) {
			macho_symtab_cmd_free(symtab->cmd);
		}
		free(symtab);
	}
}

/*
 * Mach-O Symtab Info Functions
 */
macho_symtab_cmd_t* macho_symtab_cmd_create() {
	macho_symtab_cmd_t* info = malloc(sizeof(macho_symtab_cmd_t));
	if (info) {
		memset(info, '\0', sizeof(macho_symtab_cmd_t));
	}
	return info;
}

macho_symtab_cmd_t* macho_symtab_cmd_load(unsigned char* data) {
	macho_symtab_cmd_t* cmd = macho_symtab_cmd_create();
	if (cmd) {
		memcpy(cmd, data, sizeof(macho_symtab_cmd_t));
		//macho_symtab_cmd_debug(cmd);
	}
	return cmd;
}

void macho_symtab_cmd_debug(macho_symtab_cmd_t* cmd) {

}

void macho_symtab_cmd_free(macho_symtab_cmd_t* cmd) {
	if (cmd) {
		free(cmd);
	}
}
