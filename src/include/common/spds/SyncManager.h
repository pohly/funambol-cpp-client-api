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

#ifndef INCL_SYNC_MANAGER
#define INCL_SYNC_MANAGER

#include "base/util/ArrayList.h"
#include "http/TransportAgent.h"
#include "spds/constants.h"
#include "spds/SyncManagerConfig.h"
#include "spds/SyncSource.h"
#include "spds/SyncMLBuilder.h"
#include "spds/SyncMLProcessor.h"
#include "spds/CredentialHandler.h"
#include "spds/CredentialHandler.h"



typedef enum {
                STATE_START        = 0,
                STATE_PKG1_SENDING = 1,
                STATE_PKG1_SENT    = 2,
                STATE_PKG3_SENDING = 3,
                STATE_PKG3_SENT    = 4,
                STATE_PKG5_SENDING = 5,
                STATE_PKG5_SENT    = 6
             } SyncManagerState ;


class SyncManager {

    public:
        SyncManager(SyncManagerConfig* config) EXTRA_SECTION_01;
        SyncManager(SyncManagerConfig& config) EXTRA_SECTION_01;
        ~SyncManager() EXTRA_SECTION_01;
        
        int prepareSync(SyncSource** sources) EXTRA_SECTION_01;
        
        int sync() EXTRA_SECTION_01;
        
        int endSync() EXTRA_SECTION_01;

    private:

        SyncManagerConfig& config;
        CredentialHandler credentialHandler;
        SyncMLBuilder syncMLBuilder;
        SyncMLProcessor syncMLProcessor;
        TransportAgent* transportAgent;

        SyncManagerState currentState;
        SyncSource** sources;
        ArrayList* commands;
        ArrayList** mappings;

        int* check;
        int  sourcesNumber;
        int  count;
        
		ArrayList** allItemsList;
        
        wchar_t syncURL [512];
        wchar_t deviceId[32];  
                
        unsigned int maxMsgSize;    // the max message size. If 0 it is not set
        unsigned int maxModPerMsg;  // the max modification per message

        BOOL readSyncSourceDefinition(SyncSource& source) EXTRA_SECTION_01;
        BOOL commitChanges(SyncSource& source) EXTRA_SECTION_01;
        void initialize() EXTRA_SECTION_01;
        int assignSources(SyncSource** sources) EXTRA_SECTION_01;
        

};

#endif

