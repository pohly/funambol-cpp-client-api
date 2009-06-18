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
#include "mail/MailAccountManager.h"
#include "base/globalsdef.h"

USE_NAMESPACE



MailAccountManager::MailAccountManager(MailSyncSourceConfig& ssc) : config(ssc) {}

MailAccountManager::~MailAccountManager() {}


int MailAccountManager::createAccount(MailAccount& account) {

    // Create the account on Client
    int ret = 0;
    if (ret) {
        LOG.error("Error creating email account, code %i", ret);
        return ret;
    }

    // TODO: Set account settings in config
    //config.createEmailAccount(account.name);

    return ret;
}

int MailAccountManager::updateAccount(const MailAccount& account) {

    // Update the account on Client
    int ret = 0;
    if (ret) {
        LOG.error("Error updating email account, code %i", ret);
        return ret;
    }

    // TODO: Update account settings in config (TODO)

    return ret;
}

int MailAccountManager::deleteAccount(const StringBuffer& accountID) {

    // Deletes the account on Client
    int ret = 0;
    if (ret) {
        LOG.error("Error deleting email account, code %i", ret);
        return ret;
    }

    // TODO: Remove account settings in config

    return ret;
}


int MailAccountManager::createFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("createFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    //int ret = createClientFolder(folder);
    int ret = 0;
    if (ret) {
        LOG.error("Error creating email folder, code %i", ret);
        return ret;
    }

    // TODO: Create folder account settings in config

    return ret;
}

int MailAccountManager::updateFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("updateFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    //int ret = updateClientFolder(folder);
    int ret = 0;
    if (ret) {
        LOG.error("Error updating email folder, code %i", ret);
        return ret;
    }

    // TODO: Update folder account settings in config

    return ret;
}

int MailAccountManager::deleteFolder(const FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }

    if ( !accountExists(folder.getParent()) ) {
        LOG.error("deleteFolder error: parent account '%s' not found", folder.getParent());
        return -1;
    }

    //int ret = deleteClientFolder(folder);
    int ret = 0;
    if (ret) {
        LOG.error("Error deleting email folder, code %i", ret);
        return ret;
    }

    // TODO: Remove folder account settings in config

    return ret;
}



int MailAccountManager::getAccountNumber() {
    // TODO: read from config
    return 0;
}

bool MailAccountManager::accountExists(const StringBuffer& accountID) {
    // TODO: read from config
    return true;
}

int MailAccountManager::readAccount(MailAccount& account) {

    // TODO: read from config
    return 0;
}

int MailAccountManager::readFolder(FolderData& folder) {

    // Safe check
    if (folder.getName().empty() || folder.getParent().empty()) {
        return -2;
    }
    
    // TODO: read from config
    return 0;
}


