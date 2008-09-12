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

#include "base/fscapi.h"
#include "base/messages.h"
#include "base/Log.h"
#include "base/util/ArrayList.h"
#include "base/util/StringBuffer.h"
#include "spds/spdsutils.h"
#include "spds/constants.h"
#include "client/SyncClient.h"
#include "client/DMTClientConfig.h"
#include "spds/DefaultConfigFactory.h"
#include "client/FileSyncSource.h"

#include "FSyncConfig.h"

bool doSync(FSyncConfig& config, FSyncOpt& options) {

    // Create the SyncSource passing its name, the SyncSourceConfig 
    FileSyncSource fSource(TEXT(SOURCE_NAME), config.getSyncSourceConfig(SOURCE_NAME));    
    // store the path to the folder to sync
    fSource.setDir(config.getSyncPath());

    SyncSource* ssArray[] = { &fSource, NULL } ;

    // Create the SyncClient
    SyncClient fileClient;

    // SYNC!
    if( fileClient.sync(config, ssArray) ) {
        LOG.error("Error in sync.");
    }

    // Save config to registry.
    config.save();
}

int main(int argc, char** argv) {

    // Parse the command line
    FSyncOpt options;
    if (!options.getopt(argc, argv)) {
        LOG.error("Error processing command line.");
        fprintf(stderr,"Error processing command line.");
        exit(1);
    }

    // TODO: use cmdline options
    // Init LOG
    LOG.setLogName(LOG_NAME);
    LOG.setLogPath(LOG_PATH);
    LOG.reset(LOG_TITLE);
    LOG.setLevel(LOG_LEVEL_DEBUG);
    
    // Create and initialize the configuration.
    FSyncConfig config(APPLICATION_URI).init();

    briefcase = options.getPath() || config.getSyncPath ;

    // Check the presence of the sync folder
    if(createFolder(briefcase) != 0) {
        LOG.error("Error creating briefcase folder.");
        perror("filesync");
        exit(1);
    }

    doSync(config, options);

    return 0;
}

