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


static void fillContentTypeInfoList(ArrayList &l, const char*  types);


//
// This is the core class which encodes the flow of messages between
// client and server throughout a session. It is configured via the
// DMTClientConfig with which it is constructed by the
// SyncClient::setDMConfig() and the (optional) DevInf provided
// to it by the client.
//
class SyncManager {

    public:
        /**
         * Initialize a new sync manager. Parameters provided to it
         * have to remain valid while this sync manager exists.
         *
         * @param config     required configuration
         */
        SyncManager(SyncManagerConfig& config) EXTRA_SECTION_01;
        ~SyncManager() EXTRA_SECTION_01;

        int prepareSync(SyncSource** sources) EXTRA_SECTION_01;
        
        int sync() EXTRA_SECTION_01;
        
        int endSync() EXTRA_SECTION_01;

        /**
         * Gathers the various bits and pieces known about the client and
         * its sources and builds a SyncML devinfo 1.1 instance.
         *
         * For simplicity reasons this function is called for the currently
         * active sync sources, changing them between runs thus causes
         * a (valid!) retransmission of the device info.
         *
         * @return device infos, to be deleted by caller, or NULL if unavailable
         */
        virtual DevInf *createDeviceInfo() EXTRA_SECTION_01;

    private:

        // Struct used to pass command info to the method processSyncItem
        struct CommandInfo {
            char*  commandName;
            char *cmdRef;
            char*  format;
            char*  dataType;
            long size;
        };

        DevInf* devInf;
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
        
        char syncURL [512];
        char deviceId[32];  
        unsigned int maxMsgSize;    // the max message size. If 0 it is not set. Setting it implies LargeObject support.
        unsigned int maxObjSize;    // The maximum object size. The server gets this in the Meta init message and should obey it.
        BOOL loSupport;             // enable support for large objects - without it large outgoing items are not split
        unsigned int maxModPerMsg;  // the max modification per message
        unsigned int readBufferSize; // the size of the buffer to store chunk of incoming stream.
        char  credentialInfo[256]; // used to store info for the des;b64 encription

        // Handling of incomplete incoming objects by processSyncItem().
        // Always active, even if Large Object support is off,
        // just in case the server happens to rely on it.
        // 
        class IncomingSyncItem : public SyncItem {
          public:
            IncomingSyncItem(const WCHAR* key,
                             const CommandInfo &cmdInfo,
                             int currentSource) :
                SyncItem(key),
                offset(0),
                cmdName(cmdInfo.commandName),
                cmdRef(cmdInfo.cmdRef),
                sourceIndex(currentSource) {
            }
            
            long offset;                // number of bytes already received, append at this point
            const StringBuffer cmdName; // name of the command which started the incomplete item
            const StringBuffer cmdRef;  // reference of the command which started the incomplete item
            const int sourceIndex;      // the index of the source to which the incomplete item belongs
        } *incomingItem;       // sync item which is not complete yet, more data expected
    
        void initialize() EXTRA_SECTION_01;
        BOOL readSyncSourceDefinition(SyncSource& source) EXTRA_SECTION_01;
        BOOL commitChanges(SyncSource& source) EXTRA_SECTION_01;
        int assignSources(SyncSource** sources) EXTRA_SECTION_01;
        
        Status *processSyncItem(Item* item, const CommandInfo &cmdInfo, SyncMLBuilder &syncMLBuilder) EXTRA_SECTION_01;
        char* processItemContent(const char*  data, const char*  encodings, long* size) EXTRA_SECTION_01;
        void decodeSyncItemContent(char** c, TransformationInfo& info, const char*  encoding) EXTRA_SECTION_01;
        BOOL checkForServerChanges(SyncML* syncml, ArrayList &statusList) EXTRA_SECTION_01;

        const char*  getUserAgent(SyncManagerConfig& config) EXTRA_SECTION_01;
};

#endif

