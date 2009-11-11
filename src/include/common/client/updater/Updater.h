/*
 * Funambol is a mobile platform developed by Funambol, Inc. 
 * Copyright (C) 2009 Funambol, Inc.
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

#ifndef INCL_UPDATER
#define INCL_UPDATER

#include "base/fscapi.h"
#include "http/TransportAgent.h"
#include "base/util/StringBuffer.h"

BEGIN_FUNAMBOL_NAMESPACE

class Updater {
protected:

    Updater() {
        clientName = NULL;
        currentVersion = NULL;
        response = NULL;
        updateUrl = NULL;
        updateTa = NULL;
    }

public:
    static const int UPDATE_UNAVAILABLE = 0;
    static const int UPDATE_AVAILABLE   = 1;
    static const int UPDATE_REQUIRED    = 2;
    
    /**
     * Set the name of the client that is requesting update information.
     *
     * @param char* name The name of the client checking for an update
     */
    void setClientName(const char* name) {
        clientName = name;
    }


    /**
     * Set the current version of the client. This is sent to the update URL to
     * help determine whether or not an update is available.
     *
     * @param char* version A string containing the dot (.) delimited
     *                                version number
     */
    void setCurrentVersion(const char* version) {
        currentVersion = version;
    }

    /**
     * Set the TransportAgent that will be used to contact the update URL in order
     * to check for an update.
     *
     * @param TransportAgent* A TransportAgent to use to check for updates to the
     *                            client.
     */
    void setTransportAgent(TransportAgent* ta) {
        updateTa = ta;
    }

    /**
     * Reset the update checker
     */
    void reset() {
        response = NULL;
    }
    
    /**
     * Get an update flag that indicates the type of update that is (or is not)
     * available.
     *
     * @return An integer that describes the type of update available
     */
    int getUpdateFlag() {
        int result = 0;
        
        if (isUpdateAvailable()) {
            if (isUpdateRequired()) {
                result = UPDATE_REQUIRED;
            } else {
                result = UPDATE_AVAILABLE;
            }
        } else {
            result = UPDATE_UNAVAILABLE;
        }
        
        return result;
    }

    /**
     * Is there an update available?
     *
     * @return whether or not the response from the update server contained an
     *            available update
     */
    virtual bool isUpdateAvailable() = 0;

    /**
     * Is the update required?
     *
     * @return whether or not the response from the update server contained a
     *            required update
     */
    virtual bool isUpdateRequired() = 0;

    /**
     * Get the url where the update is located
     *
     * @return The url in a string (null if not available)
     */
    virtual StringBuffer getUpdateUrl() = 0;

    /**
     * Get the available version
     *
     * @return The version number (null if not available)
     */
    virtual StringBuffer getAvailableVersion() = 0;

    virtual ~Updater() {
    }

private:
    /**
     * Query the set URL for an update and store its response.
     *
     */
    virtual void checkForUpdate() = 0;

protected:
    StringBuffer clientName;
    StringBuffer currentVersion;
    StringBuffer response;
    StringBuffer updateUrl;
    TransportAgent* updateTa;
};

END_FUNAMBOL_NAMESPACE

#endif

