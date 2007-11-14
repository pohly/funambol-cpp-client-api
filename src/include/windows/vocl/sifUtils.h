 /*
 * Copyright (C) 2007 Funambol, Inc.
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

#ifndef INCL_SIF_UTILS
#define INCL_SIF_UTILS

/** @cond API */
/** @addtogroup win_adapter */
/** @{ */



/**
 * Adds a tag "<propertyName>propertyValue</propertyName>" into 'sif' string.
 * Escapes all the "&", "<", ">" characters inside propertyValue.
 *
 * @param sif            [IN-OUT] the SIF string where we append data
 * @param propertyName   name of the SIF field to append
 * @param propertyValue  value of the SIF field to append
 */
void addPropertyToSIF(std::wstring& sif, const std::wstring propertyName, std::wstring propertyValue);

/**
 * Format a date like yyyyMMdd in yyyy-MM-dd
 */
std::wstring formatDateWithMinus(const std::wstring& stringDate);


/**
 * Trims the string passed.
 */
std::wstring trim(const std::wstring& str);



/** @} */
/** @endcond */
#endif