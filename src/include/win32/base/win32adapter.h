/*
 * Copyright (C) 2003-2006 Funambol
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef INCL_WIN32_ADAPTER
#define INCL_WIN32_ADAPTER

#include <stdarg.h>
#include <stdio.h>

#define EXTRA_SECTION_00
#define EXTRA_SECTION_01
#define EXTRA_SECTION_02
#define EXTRA_SECTION_03
#define EXTRA_SECTION_04
#define EXTRA_SECTION_05
#define EXTRA_SECTION_06

#define T(_x) _x
#define CHR(_x)  _x

#define BCHAR char
//typedef char BCHAR;
typedef wchar_t WCHAR;

#define bsprintf sprintf 
#define bstrlen strlen
#define bstrcpy strcpy
#define bstrcat strcat
#define bstrstr strstr
#define bstrchr strchr
#define bstrrchr strrchr
#define bscanf scanf
#define bstrcmp strcmp
#define bstricmp _stricmp
#define bstrncmp strncmp

#define bstrncpy strncpy
#define bstrtol strtol
#define bstrtoul strtoul

void printMemLeaks();

#endif