/*
 * Copyright (C) 2003-2007 Funambol
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

#ifndef INCL_CONFIG_H
#define INCL_CONFIG_H
/** @cond DEV */

// Use these defines to activate the desired test code

// Set this to create a test DM tree using the code in settings.cpp
// Warning: the config will be overridden at each run !
#define DEBUG_SETTINGS       1

// Test a simple sync with one sync source
#define TEST_SYNCSOURCE      1

// Test the encryption routines
//#define TEST_SYNC_ENCRYPTION 1

// Test the character encoding conversion
//#define TEST_ENCODE          1

// Test SyncML config filter
//#define TEST_CONFIG_FILTER   1


// Test the Event Handling Framework
//#define TEST_EVENT_HANDLING 1

// Test filters
//#define TEST_FILTER 1

// Test XMLProcessor class
//#define TEST_XMLPROCESSOR 1

/** @endcond */
#endif // INCL_CONFIG_H
