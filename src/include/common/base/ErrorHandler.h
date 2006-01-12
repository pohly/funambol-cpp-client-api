/*
 * Copyright (C) 2005-2006 Funambol
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

#ifndef INCL_ERROR_HANDLER
#define INCL_ERROR_HANDLER

#include "base/fscapi.h"

class ErrorHandler {

    public:
        ErrorHandler();
        ~ErrorHandler();

        /**
         * @brief Handle a warning event.
         *        The sync engine continues the operations.
         *
         * @param code  An integer code defining the warning
         * @param msg   A message explaining the warning
         *
         * @return None.
         */
        virtual void warning(int code, const wchar_t* msg) = 0;

        /**
         * @brief Handle an error condition
         *        The sync engine may continue or not the operations,
         *        depending on client response.
         *
         * @param code  An integer code defining the error
         * @param msg   A message explaining the error
         *
         * @return true if the synchronization must be stopped.
         */
        virtual bool error(int code, const wchar_t* msg) = 0;
        
        /**
         * @brief Handle a fatal error
         *        The synchronization process cannot be
         *        completed.
         *
         * @param code  An integer code defining the error
         * @param msg   A message explaining the error
         *
         * @return None
         */
        virtual void fatalError(int code, const wchar_t* msg) = 0;

        /**
         * @brief clone the error handler object.
         *
         * @return None
         */
        virtual ErrorHandler* clone() = 0;

        /**
         * @brief Get last error
         *
         * @return Return the code of the last error occurred.
         */
         int getLastError();

        /**
         * @brief Reset last error attribute to the initial value
         *
         * @return Return the code of the previous value of last error.
         */
         int resetError();

    private:
        int lastError;
};

#endif

