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

#include "FSyncConfig.h"
#include "FSyncOpt.h"

#include "base/fscapi.h"
#include "client/SyncClient.h"
#include "client/FileSyncSource.h"
#include "base/util/StringBuffer.h"
#include "base/Log.h"

//////// TODO: remove me
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
////////////////////////

#ifndef EXIT_SUCCESS
#	define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#	define EXIT_FAILURE 1
#endif

USE_NAMESPACE

static const char *progname = "fsync";

bool doSync(FSyncConfig& config, FSyncOpt& options) 
{
    // Create the SyncSource passing its name, the SyncSourceConfig 
    FileSyncSource fSource(TEXT(FSYNC_SOURCE_NAME), config.getSyncSourceConfig(FSYNC_SOURCE_NAME));    
    
    // Store the path to the folder to sync
    fSource.setDir(config.getSyncPath());

    // Initialize the SyncSource array to sync
    SyncSource* ssArray[] = { &fSource, NULL } ;

    // Create the SyncClient
    SyncClient fileClient;

    // SYNC!
    if (fileClient.sync(config, ssArray)) {
        LOG.error("Error in sync.");

        return false;
    }

    // Save the config
    config.save();

    return true;
}

// TODO: this should be implemented in some sort of directory handler class
bool createFolder(const char *path)
{
	if (path == NULL)
		return false;

#ifdef WIN32
    wchar_t* p = toWideChar(path);
    DWORD attr = CreateDirectory(p, NULL);
    delete [] p;
#else
    DIR* d = opendir(path);
    if (!d) {
        mkdir(path, 0777);
    } else {
        closedir(d);
    }
#endif

	return true;
}

int main(int argc, char** argv) 
{
	FSyncOpt opts(progname); // command line options handler

	// Option values
	const char *logPath = FSYNC_LOG_PATH;
	const char *logName = LOG_NAME;
	const char *logLevelName = "debug";
	LogLevel log_level = FSYNC_LOG_LEVEL;
	int verbose = 0; // verbosity levels: -1 quiet, 0 normal, 1 verbose

	if (opts.getopt(argc, const_cast<const char **>(argv)) == false) {
		fprintf(stderr, "error parsing options: %s\n", opts.getErr());

		exit(EXIT_FAILURE);
	}

	// On user help request exit smoothly without doing anything 
	if (opts.optionSet("help")) { 
		exit(EXIT_SUCCESS);
	}

	verbose = opts.getVerbosity();

	if (opts.optionSet("logpath")) { 
		logPath = opts.getLogPath();
	}

	if (opts.optionSet("logname")) { 
		logName = opts.getLogName();
	}

    // Init LOG
    LOG.setLogName(logName);
    LOG.setLogPath(logPath);
    LOG.reset(FSYNC_LOG_TITLE);

	// Configure log level from command line
	if (opts.optionSet("loglevel")) { 
		logLevelName = opts.getLogLevel();

		if (strcmp(logLevelName, "none") == 0) {
			log_level = LOG_LEVEL_NONE;
		} else if (strcmp(logLevelName, "info") == 0) {
			log_level = LOG_LEVEL_INFO;	
		} else if (strcmp(logLevelName, "debug") == 0) {
			log_level = LOG_LEVEL_DEBUG;
		} else {
			fprintf(stderr, "%s: unrecognized log level: '%s'\n",
				progname, logLevelName);

			exit(EXIT_FAILURE);
		}
	} 

    LOG.setLevel(log_level);
   
	if (verbose > 0) {
		printf("writing log to '%s' with %s level\n",
			logName, logLevelName);
	} 

    // Create and initialize the configuration.
    FSyncConfig config;

	if (verbose > 0) {
		printf("initalizing client configuration\n");
	}

    config.init();

    StringBuffer folder = config.getSyncPath();
	
    // Check the presence of the sync folder
    if (createFolder(folder.c_str()) == false) {
        exit(EXIT_FAILURE);
    }

	if (verbose >= 0) {
		printf("starting sync..\n");
	}

    if (doSync(config, opts) == false) {
		exit(EXIT_FAILURE);
	}

	if (verbose >= 0) {
		printf("sync ended\n");
	}

	exit(EXIT_SUCCESS);
}

