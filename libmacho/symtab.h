/**
 * GreenPois0n Absinthe - macho_symtab.h
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

#ifndef MACHO_SYMTAB_H_
#define MACHO_SYMTAB_H_

typedef struct macho_symtab_cmd_t {
	uint32_t cmd;		/* LC_SYMTAB */
	uint32_t cmdsize;	/* sizeof(struct macho_symtab_cmd_t) */
	uint32_t symoff;	/* symbol table offset */
	uint32_t nsyms;		/* number of symbol table entries */
	uint32_t stroff;	/* string table offset */
	uint32_t strsize;	/* string table size in bytes */
} macho_symtab_cmd_t;

typedef struct nlist {
	union {
		char *n_name; /* resolved symbol name */
		int32_t n_strx; /* index into the string table */
	} n_un;
	uint8_t n_type; /* type flag, see below */
	uint8_t n_sect; /* section number or NO_SECT */
	int16_t n_desc; /* see <mach-o/stab.h> */
	uint32_t n_value; /* value of this symbol (or stab offset) */
} nlist;

typedef struct macho_symtab_t {
	uint32_t nsyms;
	struct nlist* symbols;
	macho_symtab_cmd_t* cmd;
} macho_symtab_t;

/*
 * Mach-O Symtab Functions
 */
extern macho_symtab_t* macho_symtab_create(void);
extern macho_symtab_t* macho_symtab_load(unsigned char* cmd, unsigned char* data);
extern void macho_symtab_debug(macho_symtab_t* symtab);
extern void macho_symtab_free(macho_symtab_t* symtab);

/*
 * Mach-O Symtab Info Functions
 */
extern macho_symtab_cmd_t* macho_symtab_cmd_create(void);
extern macho_symtab_cmd_t* macho_symtab_cmd_load(unsigned char* data);
extern void macho_symtab_cmd_debug(macho_symtab_cmd_t* cmd);
extern void macho_symtab_cmd_free(macho_symtab_cmd_t* cmd);

#endif /* MACHO_SYMTAB_H_ */
