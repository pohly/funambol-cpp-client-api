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

#include "client/updater/XmlUpdater.h"
#include "http/URL.h"

USE_FUNAMBOL_NAMESPACE

XmlUpdater::XmlUpdater() : Updater() {
}

XmlUpdater::~XmlUpdater() {
    if (response) {
        delete response;
        response = NULL;
    }
}

bool XmlUpdater::isUpdateAvailable() {
    bool available = false;
    StringBuffer version;

    // Check for a cached response first. If there is not one, we need to
    // query the update URL.
    if (response.null()) {
        checkForUpdate();
    }

    version = extractXmlNodeValue(response, "version");
    if (!version.null() && version != currentVersion) {
        available = true;
    }

    return available;
}

bool XmlUpdater::isUpdateRequired() {
    bool required = false;
    StringBuffer req_str;

    // Check for a cached response first. If there is not one, we need to
    // query the update URL.
    if (!response) {
        checkForUpdate();
    }

    req_str = XmlUpdater::extractXmlNodeValue(response, "required");
    if (!req_str.null() && req_str == "1") {
        required = true;
    }

    return required;
}

StringBuffer XmlUpdater::getUpdateUrl() {
    // Check for a cached response first. If there is not one, we need to
    // query the update URL.
    if (!response) {
        checkForUpdate();
    }

    StringBuffer url = XmlUpdater::extractXmlNodeValue(response, "url");

    return url;
}

StringBuffer XmlUpdater::getAvailableVersion() {
    // Check for a cached response first. If there is not one, we need to
    // query the update URL.
    if (!response) {
        checkForUpdate();
    }

    StringBuffer version = XmlUpdater::extractXmlNodeValue(response, "version");

    return version;
}

void XmlUpdater::checkForUpdate() {
    if (clientName.null()) {
        // TODO
    }

    if (!currentVersion.null()) {
        // TODO
    }

    if (!updateTa) {
        // TODO
    }
    
    StringBuffer message;
    message = "component=";
    message.append(clientName.c_str()).append("&").append("version=").append(currentVersion.c_str()).append("&").append("format=xml");

    response = updateTa->sendMessage(message.c_str());
}

StringBuffer XmlUpdater::extractXmlNodeValue(const StringBuffer & xml, const StringBuffer & node) {

    if (xml.null()) {
        return NULL;
    }

    size_t start;
    size_t end;
    StringBuffer temp;

    // Find the start of the node.
    temp.sprintf("<%s>", node.c_str());
    start = xml.find(temp.c_str());
    if (start == StringBuffer::npos) {
        return NULL;
    }
    start = start + temp.length();

    // Find the end of the node.
    temp.sprintf("</%s>", node.c_str());
    end = xml.find(temp.c_str());
    if (end == StringBuffer::npos) {
        return NULL;
    }

    return xml.substr(start, end - start);
}

