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

#ifndef INCL_SYNC4JCLIENT
    #define INCL_SYNC4JCLIENT

    #include "base/fscapi.h"
    #include "base/Log.h"
    #include "base/ErrorHandler.h"
    #include "base/util/ArrayList.h"
    #include "client/DMTClientConfig.h"
    #include "spds/SyncSource.h"
    #include "spds/constants.h"

    class SyncManager;

    /**
     * This class wrap the common operations requested by the client to
     * handle a synchronization. It uses the signleton pattern to make
     * sure that only one instance of the client is active on the client
     * program. 
     */
    class Sync4jClient {

        public:
            // get and release singleton instance
            static Sync4jClient& getInstance();
            static void dispose();
            // set the default client configuration (using context root)
            void setDMConfig(const BCHAR* root);
            // set a client-provided DMTree config
            void setDMConfig(DMTClientConfig& c);

            // get the defined error handler
            ErrorHandler& getErrorHandler();
            // set a custom error handler
            void setErrorHandler(ErrorHandler& e);

            /*
             * Start a synchronization on the specified sources.
             *
             * @param sources - NULL terminated array of sources to sync.
             *
             * @return - 0 on success, an error otherwise
             */
            int sync(SyncSource** sources);

            /*
             * Used to start the sync process. The argument is an array of SyncSource
             * that have to be synched
             *
             * @param sources the SyncSource& array
             */
            int sync(ArrayList& sources);

        private:

            static Sync4jClient* instance;

            SyncManager* syncManager;
            ErrorHandler* errorHandler;

            // private constructor and destructor
            Sync4jClient();
            ~Sync4jClient();

    };

#endif

