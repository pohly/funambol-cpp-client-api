/*
 * Copyright (C) 2003-2007 Funambol, Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY, TITLE, NONINFRINGEMENT or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307  USA
 */

#ifndef INCL_WIN_ADAPTER
#define INCL_WIN_ADAPTER
/** @cond DEV */

#include <stdarg.h>
#include <stdio.h>


#ifdef _WIN32_WCE
#include <winsock.h>
#define __declspec(ignore)

// Map snprintf and vsnprintf to winmobile functions
#define snprintf _snprintf
#define vsnprintf _vsnprintf

// some of the code compares NULL against integers, which
// fails if NULL is defined as (void *)0
#undef NULL
#define NULL 0
#endif  // #ifdef _WIN32_WCE


typedef wchar_t WCHAR;

// Define this to the implementation of the sprintf variant with
// size check for wide char.
#define snwprintf _snwprintf


/**
 * Returns a rounded integer value from double.
 */
int round(double val);


#if defined(WIN32) && !defined(_WIN32_WCE)
void printMemLeaks();
#endif


// Enable warning: class has virtual functions, but destructor is not virtual
// When a class has virtual functions but a nonvirtual destructor, 
// objects of the type might not be destroyed properly when the class 
// is destroyed through a base class pointer.
#pragma warning(default : 4265)


/** @endcond */
#endif
