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
#include "base/Log.h"
#include "email/EmailAccountManager.h"
#include "base/globalsdef.h"

USE_NAMESPACE



EmailAccountManager::EmailAccountManager(MailSyncSourceConfig& ssc) : config(ssc) {}

EmailAccountManager::~EmailAccountManager() {}


int EmailAccountManager::createAccount(MailAccount& account) {

    // Create the account on Client
    int ret = createClientAccount(account);
    if (ret) {
        LOG.error("Error creating email account, code %i", ret);
        return ret;
    }

    // TODO: Set account settings in config
    //config.createEmailAccount(account.name);

    return ret;
}

int EmailAccountManager::updateAccount(const MailAccount& account) {

    // Update the account on Client
    int ret = updateClientAccount(account);
    if (ret) {
        LOG.error("Error updating email account, code %i", ret);
        return ret;
    }

    // TODO: Update account settings in config (TODO)

    return ret;
}

int EmailAccountManager::deleteAccount(const StringBuffer& accountID) {

    // Deletes the account on Client
    int ret = deleteClientAccount(accountID);
    if (ret) {
        LOG.error("Error deleting email account, code %i", ret);
        return ret;
    }

    // TODO: Remove account settings in config

    return ret;
}


int EmailAccountManager::createFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("createFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    int ret = createClientFolder(folder);
    if (ret) {
        LOG.error("Error creating email folder, code %i", ret);
        return ret;
    }

    // TODO: Create folder account settings in config

    return ret;
}

int EmailAccountManager::updateFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("updateFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    int ret = updateClientFolder(folder);
    if (ret) {
        LOG.error("Error updating email folder, code %i", ret);
        return ret;
    }

    // TODO: Update folder account settings in config

    return ret;
}

int EmailAccountManager::deleteFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("deleteFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    int ret = deleteClientFolder(folder);
    if (ret) {
        LOG.error("Error deleting email folder, code %i", ret);
        return ret;
    }

    // TODO: Remove folder account settings in config

    return ret;
}



int EmailAccountManager::getAccountNumber() {
    // TODO: read from config
    return 0;
}

bool EmailAccountManager::accountExists(const StringBuffer& accountID) {
    // TODO: read from config
    return true;
}

int EmailAccountManager::readAccount(MailAccount& account) {

    // TODO: read from config
    return 0;
}

int EmailAccountManager::readFolder(FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }
    
    // TODO: read from config
    return 0;
}


