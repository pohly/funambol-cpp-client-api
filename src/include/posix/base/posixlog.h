/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2003 - 2007 Funambol, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License version 3 as published by
 * the Free Software Foundation with the addition of the following permission 
 * added to Section 15 as permitted in Section 7(a): FOR ANY PART OF THE COVERED
 * WORK IN WHICH THE COPYRIGHT IS OWNED BY FUNAMBOL, FUNAMBOL DISCLAIMS THE 
 * WARRANTY OF NON INFRINGEMENT  OF THIRD PARTY RIGHTS.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Affero General Public License 
 * along with this program; if not, see http://www.gnu.org/licenses or write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA.
 * 
 * You can contact Funambol, Inc. headquarters at 643 Bair Island Road, Suite 
 * 305, Redwood City, CA 94063, USA, or at email address info@funambol.com.
 * 
 * The interactive user interfaces in modified source and object code versions
 * of this program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU Affero General Public License version 3.
 * 
 * In accordance with Section 7(b) of the GNU Affero General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Funambol" logo. If the display of the logo is not reasonably 
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Funambol".
 */

#ifndef INCL_AUTOTOOLS_LOG
# define INCL_AUTOTOOLS_LOG
/** @cond DEV */

#include <base/fscapi.h>

/*
 * Opens the specified file for logging of messages.
 *
 * By default the LOG instance of the Log class will
 * create the file specified via its set methods
 * as soon as the first message needs to be printed or
 * when explicitly asking for a reset.
 *
 * By calling this function instead one gets more detailed
 * control over logging and avoids the (currently) insecurely
 * implemented handling of file name strings in the Log class.
 *
 * @param path            directory where file is to be created, can be NULL
 * @param name            file name relative to path or "-" when asking for
 *                        logging to stdout
 * @param redirectStderr  if TRUE, then file descriptor 2 (stderr)
 *                        will also be redirected into the log file;
 *                        the original stderr is preserved and will be
 *                        restored when turning this redirection off
 */
void setLogFile(const char *path, const char* name, BOOL redirectStderr = FALSE);

/** traditional version of setLogFile() which writes in the current directory */
void setLogFile(const char* name, BOOL redirectStderr = FALSE);

/** @endcond */
#endif
