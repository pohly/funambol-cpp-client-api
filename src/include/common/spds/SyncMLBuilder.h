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

#ifndef INCL_SYNCML_BUILDER
#define INCL_SYNCML_BUILDER

#include "spds/SyncSource.h"
#include "spds/SyncMap.h"
#include "syncml/core/TagNames.h"
#include "syncml/core/ObjectDel.h"
#include "syncml/formatter/Formatter.h"

class SyncMLBuilder {

    public:
        SyncMLBuilder() EXTRA_SECTION_01;
        ~SyncMLBuilder() EXTRA_SECTION_01;
        SyncMLBuilder(wchar_t* t, wchar_t* d, unsigned long msgSize) EXTRA_SECTION_01;
        
        /*
         * Convert the SyncML object into an xml message
         */
        wchar_t* prepareMsg(SyncML* syncml) EXTRA_SECTION_01;        
        
        /*
         * Convert the SyncML object into an xml message
         */       
        SyncML*  prepareSyncML(ArrayList* commands, BOOL final) EXTRA_SECTION_01;
         
        /*
        * Set init parameters
        */
        void     set(wchar_t* t, wchar_t* d, unsigned long msgSize) EXTRA_SECTION_01;
        
        /*
        * Prepare the init SyncML* message with credential and db alert to sync
        */
        SyncML*  prepareInitObject(Cred* cred, ArrayList* alerts, ArrayList* commands) EXTRA_SECTION_01;
        
        /*
        * Prepare the SyncHdr message with credential if not null
        */        
        SyncHdr* prepareSyncHdr(Cred* cred) EXTRA_SECTION_01;
        
        /*
        * Prepare the init alert
        */
        Alert*   prepareInitAlert(SyncSource& source) EXTRA_SECTION_01;
        
        /*
        * Prepare the special init alert for Address Change Notification
        */
        Alert*   prepareAddrChangeAlert(SyncSource& source) EXTRA_SECTION_01;
        
        /*
        * Prepare alert with 222 code to request the server items
        */ 
        Alert*   prepareRequestAlert(SyncSource& source) EXTRA_SECTION_01;
        
        /*
        * Prepare the status for Sync Header
        */ 
        Status*  prepareSyncHdrStatus(Chal* chal, int data) EXTRA_SECTION_01;
        
        /*
        * Prepare the status for Alert command
        */
        Status*  prepareAlertStatus(SyncSource& s,  ArrayList* alert, int authStatusCode) EXTRA_SECTION_01;

        /*
        * Prepare the status for Item command. COMMAND could be ADD, REPLACE, DEL
        */
        Status*  prepareItemStatus(wchar_t* COMMAND, wchar_t* key, wchar_t* cmdRef, int code) EXTRA_SECTION_01;
        
        /*
        * Add the status to the corrent list of commands. It is responsible to collapse the status if needed
        */ 
        void     addItemStatus(ArrayList* previousStatus, Status* status) EXTRA_SECTION_01;

        /*
        * Prepare the status for Sync command
        */
        Status*  prepareSyncStatus(SyncSource& source, Sync* sync) EXTRA_SECTION_01;
                        
        /*
        * Prepare the Sync object. It doesn't contain any items. It is to prepare the insert of items
        */
        Sync*    prepareSyncCommand(SyncSource& source) EXTRA_SECTION_01;
        
        /*
        * Prepare the Map object. It doesn't containt anything. It is to prepare the insert of map items
        */
        Map*     prepareMapCommand(SyncSource& source)EXTRA_SECTION_01;
        
        /*
        * Prepare the MapItem. It could contain only one MapItem
        */
        MapItem* prepareMapItem(SyncMap* syncMap) EXTRA_SECTION_01;
        
        /*
        * Add the MapItem to the Map command.
        */
        void     addMapItem(Map* map, MapItem* mapItem) EXTRA_SECTION_01;
        
        /*
        * Prepare a ADD, REPLACE, DEL command using the syncItem passed by the source
        */ 
        ModificationCommand* prepareModificationCommand(wchar_t* COMMAND, SyncItem* item, const wchar_t* type) EXTRA_SECTION_01;
        
        /*
        * Add a SyncItem into the modificationCommand. It is responsible to collapse if needed
        */
        void addItem(ModificationCommand* modificationCommand, wchar_t* COMMAND, SyncItem* syncItem, const wchar_t* type) EXTRA_SECTION_01;
        
        /*
        * Reset the cmdID counter
        */
        void resetCommandID() EXTRA_SECTION_01;

        /*
        * increase the msgID counter
        */
        void increaseMsgRef() EXTRA_SECTION_01;
        
        /*
        * reset MsgId counter
        */
        
        void resetMessageID() EXTRA_SECTION_01;

    private:

        void initialize();

        wchar_t* target;
        wchar_t* device;
        unsigned long maxMsgSize;

        unsigned long sessionID;
        unsigned int  msgID    ;
        unsigned int  cmdID    ;
        unsigned int  msgRef   ;

};

#endif
