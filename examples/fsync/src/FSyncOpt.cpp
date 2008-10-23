/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2008 Funambol, Inc.
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

#include "base/fscapi.h"
#include "FSyncOpt.h"

USE_NAMESPACE

FSyncOpt::FSyncOpt(const char *progname) : parser(progname), verbose(NORMAL)
{
    parser.addOption('s', "server", "set server url", true);
    parser.addOption('d', "dir", "set the local folder to sync", true);
	parser.addOption('l', "logname", "set log name", true);
	parser.addOption('L', "loglevel", "set log level [none, info, debug]", true);
	parser.addOption('v', "verbose", "increase verbosity");
	parser.addOption('q', "quiet", "decrease verbosity");

	/* TODO
	 * parser.addArgument("ServerURL", "the URL of the sync server, or the one in the config if not specified", false);
	 * parser.addArgument("LocalFolder", "the local folder to sync, or the one in the config if not specified", false);
     */
}

bool FSyncOpt::getopt(int args_num, const char** args_val) 
{
	if (parser.parse(args_num, args_val, opts, args) == false) {
		return false;
	}

	if (optionSet("verbose")) {
		verbose = VERBOSE;
	} else if (optionSet("quiet")) {
		verbose = QUIET;
	}

    return true;
}

const char* FSyncOpt::getOptionVal(const char *ln)
{ 
	const StringBuffer &optValue = opts.get(ln);

	return optValue.c_str();
}

