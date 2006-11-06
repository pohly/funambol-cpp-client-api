/**
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

#ifndef INCL_SYNCML_PROCESSOR
    #define INCL_SYNCML_PROCESSOR

    #include "base/util/ArrayList.h"
    #include "base/util/XMLProcessor.h"
    #include "spds/SyncSource.h"
    #include "spds/SyncItem.h"
    #include "spds/SyncItemStatus.h"
    #include "syncml/core/TagNames.h"
    #include "syncml/core/ObjectDel.h"
    #include "syncml/parser/Parser.h"
    #include "spds/SyncReport.h"

    /*
     * This class is responsible for the processing of the incoming messages.
     */

    class __declspec(dllexport) SyncMLProcessor : public XMLProcessor {

    private:
        
        /*
         * It include the common part of getSyncHdrStatusCode and getAlertStatusCode
         */
        int getStatusCode(SyncBody* syncBody, SyncSource* source, char*  commandName) EXTRA_SECTION_01;        
        /*
         * Returns the status code for the SyncHeader command included
         * in the message sent by the client.
         *
         * @param syncBody - the SyncBody content
         */
        int getSyncHeaderStatusCode(Status* s) EXTRA_SECTION_01; 
        /*
         * Returns the status code for the Alert relative to the given source.
         *
         * @param syncBody - the SyncBody content
         * @param sourceName - the name of the source
         */
        int getAlertStatusCode(Status* status, const char*  sourceName) EXTRA_SECTION_01;       
        
        /*
        * Return the command of the given commandName
        */
        AbstractCommand* getCommand(SyncBody* syncBody, char*  commandName, int index) EXTRA_SECTION_01;
        
         /*
        * To get a generic array element. It returns the <index> arrayElement it founds.
        * 0-based.
         */
        ArrayElement* getArrayElement(ArrayList* list, int index) EXTRA_SECTION_01;

    public:

        /*
         * Constructor
         */
        SyncMLProcessor();

        /*
        * Process a generic syncml message and return a SyncML object
        */
        SyncML* processMsg(char*  msg) EXTRA_SECTION_01;

        /*
         * Processes the initialization response. Returns 0 in case of success, an
         * error code in case of error.
         *
         * @param msg the response from the server
         */
        int processInitResponse(SyncSource& source, SyncML* syncml, Alert* alert) EXTRA_SECTION_01;

        int processSyncHdrStatus(SyncML* syncml) EXTRA_SECTION_01;
        int processAlertStatus(SyncSource& source, SyncML* syncml, ArrayList* alerts) EXTRA_SECTION_01;

        int processServerAlert(SyncSource& source, SyncML* syncml) EXTRA_SECTION_01;
        /*
        * Get the chal from a syncBody object. It is used to get the auth type and next nonce if needed
        */
        Chal* getChal(SyncBody* syncBody) EXTRA_SECTION_01;
        
        /*
        * Get server credential. It is used by the SyncManager to get the server credentials and check them
        */
        Cred* getServerCred(SyncHdr* syncHdr) EXTRA_SECTION_01;
        

        /*
         * Process the SyncBody and looks for the item status of the sent items.
         * It calls the setItemStatus method of the sync source. 
         */
        int processItemStatus(SyncSource& source, SyncBody* syncBody) EXTRA_SECTION_01;

        /*
         * Processes the response and get the Sync command of the given source
         *
         * @param source the source
         * @param syncml the syncML Object the response from the server
         */
        Sync* processSyncResponse(SyncSource& source, SyncML* syncml) EXTRA_SECTION_01;

        /*
         * Processes the map message response. Returns 0 in case of success. 
         * Currently it return always 0. TBD
         *
         * @param source the source
         * @param msg the response from the server
         */
        int processMapResponse(SyncSource& source, SyncBody* syncBody) EXTRA_SECTION_01;

        /*
         * Returns the SyncHeader/RespURI element of the given message. If the element is not
         * found it returns NULL. The returned respURI is allocated with the new operator
         * and must be discarded with delete by the caller.
         *
         * @param msg - the SyncHdr message - NOT NULL
         */
        char*  getRespURI(SyncHdr* msg) EXTRA_SECTION_01;

        /*
         * Returns an ArrayList containing the command given by commandName. It uses the getCommand method
         */
        ArrayList* getCommands(SyncBody* syncBody, char*  commandName) EXTRA_SECTION_01;
       
        /* To retrieve a (NULL terminated) list of source names from list of Alert commands from server.
         * @return: a new array of source names (NULL terminated) - must be freed by the caller.
         */
        char** getSortedSourcesFromServer(SyncML* syncml, int sourcesNumber);

        /* To retrieve a Sync pointer from ArrayList of Sync objects. 
         * It gets the order like the server sends
         */        
        Sync* getSyncResponse(SyncML* syncml, int index);
    };

#endif
