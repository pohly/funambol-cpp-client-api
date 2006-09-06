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

#ifndef INCL_SYNCCLIENT
    #define INCL_SYNCCLIENT

    #include "base/fscapi.h"
    #include "base/Log.h"
    #include "base/ErrorHandler.h"
    #include "base/util/ArrayList.h"
    #include "client/DMTClientConfig.h"
    #include "spds/SyncSource.h"
    #include "spdm/ManagementNode.h"
    #include "spds/constants.h"

    class SyncManager;
    class AccessConfig;
    class DevInf;

    /**
     * This class wraps the common operations executed by a typical
     * client. It contains default implementations for calls invoked
     * by the library core to parameterize or control the
     * synchronization. A client can override these calls by
     * subclassing SyncClient and using an instance of its own class
     * instead.
     * 
     * Warning: the library contains global data in several different
     * places. Therefore it is not possible to execute multiple
     * sessions in parallel.
     */
    class SyncClient {
      public:
        /*
         * Constructor for SyncClient.
         *
         * @param  c    a reference to SyncManager configuration.
         */
        SyncClient(SyncManagerConfig& c);
        virtual ~SyncClient();
        
        // Set a client-provided configuration.
        // The caller owns the configuration and is responsible for
        // saving the (possibly modified) configuration after the
        // synchronization
        virtual void setConfig(SyncManagerConfig& c);


        /*
         * Execute a synchronization on the specified sources.
         * The sources will be configured automatically using the
         * client configuration set earlier in the constructor or
         * with the setConfig() method.
         *
         * @param sources - NULL terminated array of sources to sync.
         *
         * @return - 0 on success, an error otherwise
         */
        virtual int sync(SyncSource** sources);

        /**
         * Execute a synchronization with sources that are chosen based
         * on the sourceNames param or, if sourceNames is NULL, based
         * on the configuration: in this case all available sources will be used.
         *
         * - calls prepareSync() to give a derived class the chance to
         *   setup syncing and get ready for creating sources
         * - iterates over sources to sync, asks for the corresponding
         *   client source with createSource()
         * - calls beginSync() to give the derived class a class to do
         *   after all sources have been configured successfully
         * - executes the synchronization: call sync(sources**)
         *
         * This version of the call is easier to use for clients whose
         * sync sources are determined by the configuration or by an array
         * of desired sources to sync
         *
         * @param  sourceNames: optional, a NULL terminated array of source names that
         *                      we want to sync. If NULL, sources to sync are chosen
         *                      from the configuration.
         * @return 0 on success, an error otherwise
         */
        virtual int sync(BCHAR** sourceNames = NULL);


      protected:
        /**
         * A callback into a derived class which is called after
         * reading the SyncML configuration and before creating sources.
         * Not used by the sync(SyncSource**) call.
         *
         * @param config   a reference to SyncManager configuration
         *                 can be used to read client specific properties
         * @return 0       for success, an error code otherwise - an error code
         *                 aborts the whole synchronization
         */
        virtual int prepareSync(SyncManagerConfig& config) {
            return ERR_NONE;
        }

        /**
         * A factory method that is used by the sync() call to create
         * the sync sources that are to be synchronized. Not used by the
         * sync(SyncSource**) call.
         *
         * @param name     name of the sync source
         * @param pos      position of the SyncSource in the SSConfig array
         *                 (index: 0 - numSources-1)
         * @param config   a copy of the source's configuration: this
         *                 includes all properties that the client library
         *                 knows and uses itself (only valid during this call)
         * @retval source  the sync source created by the client or NULL if
         *                 there is no sync source currently associated with
         *                 the config or it is inactive; instance is a new SyncSource*
         *                 and it's freed by the caller (see finally)
         * @return 0 for success, an error code otherwise - an error code
         *         aborts the whole synchronization and the value of *source is
         *         ignored
         */
        virtual int createSyncSource(const char *name, const int pos,
                                     const SyncSourceConfig &config,
                                     SyncSource **source) {
            *source = NULL;
            return ERR_UNSPECIFIED;
        }

        /**
         * Callback invoked after creating all sources.
         * @return 0 for success, an error code otherwise - an error code stops
         *         immediately
         */
        virtual int beginSync(SyncSource **source) {
            return ERR_NONE;
        }

        /**
         * Callback invoked after the sync process.
         * @return: 0 for success, an error code otherwise
         */
        virtual int endSync(SyncSource **source) {
            return ERR_NONE;
        }


        // configuration of this client
        SyncManagerConfig& config;
    };


#endif

