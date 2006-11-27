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

#include "base/fscapi.h"
#include "base/errors.h"
#include "base/debug.h"
#include "base/Log.h"
#include "spdm/AccessManagementNode.h"
#include "spdm/ParentManagementNode.h"
#include "client/SourceManagementNode.h"
#include "spdm/utils.h"
#include "spdm/PalmDeviceManager.h"
#include "sps/errors.h"
#include "sps/DB.h"

PalmDeviceManager::PalmDeviceManager() : DeviceManager() {
    if (checkDatabase() == false) {
        createDatabase();
    }
    resetError();

    return;
}


ManagementNode* PalmDeviceManager::getManagementNode(const char*node) {
    if (node==NULL) {
        return NULL;
    }

    resetError();

    //
    // Let's separate node's name from context
    //
    WCHAR context[DIM_MANAGEMENT_PATH>>3];
    WCHAR leafName[DIM_MANAGEMENT_PATH>>3];

    getNodeContext(node, context, sizeof(context));
    getNodeName(node, leafName, sizeof(leafName));

    if (wcscmp(leafName, SYNCML) == 0) {
        wcsprintf(logmsg, DBG_LOADING_CONN_SETTINGS, node);
        LOG.debug(logmsg);

        AccessManagementNode* n = new AccessManagementNode(context, SYNCML);

        DeviceManagementRecord record;

        if (findNodeRecord(node, record)) {
            if (record.getAccessManagementNode(*n) == FALSE) {
                LOG.error(DBG_INVALID_ACCESS_SETTINGS);
            }

            if (LOG.isLoggable(LOG_LEVEL_DEBUG)) {
                n->getAccessConfig().getSyncURL(logmsg); LOG.debug(logmsg);
                n->getAccessConfig().getUsername(logmsg); LOG.debug(logmsg);
                n->getAccessConfig().getPassword(logmsg); LOG.debug(logmsg);
            }
        } else {
            lastErrorCode = ERR_INVALID_CONTEXT;
            wcsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, node);
            LOG.debug(DBG_NODE_NOT_FOUND);
        }

        return n;
    } else if (wcscmp(leafName, SOURCES) == 0) {
        //
        // This is the exact match on .../spdm/sources, which means that we
        // whant the children
        //
        wcsprintf(logmsg, DBG_LOADING_SOURCES, node);
        LOG.debug(logmsg);

        SourceManagementNode* s = NULL;
        ParentManagementNode* n = new ParentManagementNode(context, SOURCES);

        DeviceManagementRecord record;

        int pos = -1;
        WCHAR sourceName[DIM_MANAGEMENT_PATH>>3];
        while ((pos = findChildRecord(node, record, pos+1)) >= 0) {
            getNodeName(record.getField(1), sourceName, sizeof(sourceName));
            s = new SourceManagementNode(node, sourceName);
            if (record.getSourceManagementNode(*s) == FALSE) {
                LOG.error(DBG_INVALID_SOURCE_SETTINGS);
                continue;
            }
            if (LOG.isLoggable(LOG_LEVEL_DEBUG)) {
                s->getSourceConfig().getName(logmsg); LOG.debug(logmsg);
                s->getSourceConfig().getURI(logmsg);  LOG.debug(logmsg);
            }
            n->addChild(*s); delete s;
        }

        return n;
    } else {
        //
        // For any other leaf usa a generic ManagementObject
        //
        wcsprintf(logmsg, DBG_LOADING_MANAGEMENT_OBJECT, node);
        LOG.debug(logmsg);

        ManagementObject* o = NULL;

        DeviceManagementRecord record;
        ArrayList properties;

        WCHAR context[DIM_MANAGEMENT_PATH], name[DIM_MANAGEMENT_PATH];
        if (findNodeRecord(node, record)) {
            getNodeContext(node, context, sizeof(context));
            getNodeName(node, name, sizeof(name));
            o = new ManagementObject(context, name);
            if (record.getManagementObject(*o) == FALSE) {
                LOG.error(DBG_INVALID_ACCESS_SETTINGS);
            }
        } else {
            lastErrorCode = ERR_INVALID_CONTEXT;
            wcsprintf(lastErrorMsg, ERRMSG_INVALID_CONTEXT, node);
            LOG.debug(DBG_NODE_NOT_FOUND);
        }
        return o;
    }

    return NULL;
}

void PalmDeviceManager::setManagementNode(ManagementNode& n) {
    DeviceManagementRecord record;

    WCHAR nodeName[DIM_MANAGEMENT_PATH];
    n.getFullName(nodeName, DIM_MANAGEMENT_PATH-1);

    resetError();

    if (StrStr(nodeName, SYNCML)) {
        if (findNodeRecord(nodeName, record)) {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((AccessManagementNode&)n);
            db.updateRecord(record.getIndex(), record.getData(), record.getSize());
        } else {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((AccessManagementNode&)n);
            db.appendRecord(record.getData(), record.getSize());
        }
    } else if (StrStr(nodeName, SOURCES)) {
        if (findNodeRecord(nodeName, record)) {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((SourceManagementNode&)n);
            db.updateRecord(record.getIndex(), record.getData(), record.getSize());
        } else {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((SourceManagementNode&)n);
            db.appendRecord(record.getData(), record.getSize());
        }
    } else {
        //
        // Generic ManagementObject
        //
        if (findNodeRecord(nodeName, record)) {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((ManagementObject&)n);
            db.updateRecord(record.getIndex(), record.getData(), record.getSize());
        } else {
            DB db = DB(DM_SYNCML_DB, TRUE);
            record.setData((ManagementObject&)n);
            db.appendRecord(record.getData(), record.getSize());
        }
    }
}


/**
 * Checks if the DM database is already present on the
 * system.
 *
 * @return true if the database already exists, false otherwise
 */
Boolean PalmDeviceManager::checkDatabase() {
    DB dmdb=DB(DM_SYNCML_DB);

    return (Boolean)(dmdb.find() == errNone);
}


/**
 *  Creates the DM database.
 */
Boolean PalmDeviceManager::createDatabase() {
    DB dmdb=DB(DM_SYNCML_DB);

    Err err = dmdb.create();

    if (err != errNone) {
        lastErrorCode = ERR_DM_TREE_NOT_AVAILABLE;
        StrCopy(lastErrorMsg, ERRMSG_DM_TREE_NOT_AVAILABLE);
        return false;
    }

    return true;
}

BOOL PalmDeviceManager::findNodeRecord(char* name, DeviceManagementRecord& record) {
    DB db = DB(DM_SYNCML_DB);

    resetError();

    if (db.openByName() != errNone) {
        return FALSE;
    }

    int n = db.getNumRecords();

    MemHandle recordHandle = NULL;
    char* r = NULL;

    int i=0;
    for(i=0; i<n; ++i) {
        recordHandle = db.getReadOnlyRecord(i);

        r = (char*)MemHandleLock(recordHandle);

        //
        // The first field is the context
        //
        if (strcmp(r, name) == 0) {
            MemHandleUnlock(recordHandle); recordHandle = NULL;
            recordHandle = db.getRecord(i);  // now is RW
            r = (char*)MemHandleLock(recordHandle);
            record.setIndex(i);
            record.setData(r, MemHandleSize(recordHandle));
            db.releaseRecord(i, true);
            MemHandleUnlock(recordHandle); recordHandle = NULL;
            return true;
        }

        MemHandleUnlock(recordHandle); recordHandle = NULL;
    }

    return false;
}

int PalmDeviceManager::findChildRecord( char*                   name  ,
                                        DeviceManagementRecord& record,
                                        unsigned int            pos   ) {
    DB db = DB(DM_SYNCML_DB);

    resetError();

    if (db.openByName() != errNone) {
        return FALSE;
    }

    int n = db.getNumRecords();

    MemHandle recordHandle = NULL;
    char* r = NULL;

    for(int i=pos; i<n; ++i) {
        recordHandle = db.getReadOnlyRecord(i);

        r = (char*)MemHandleLock(recordHandle);

        //
        // The first field is the context
        //
        if (StrStr(r, name)) {
            MemHandleUnlock(recordHandle); recordHandle = NULL;
            recordHandle = db.getRecord(i);  // now is RW
            r = (char*)MemHandleLock(recordHandle);
            record.setIndex(i);
            record.setData(r, MemHandleSize(recordHandle));
            db.releaseRecord(i, true);
            MemHandleUnlock(recordHandle); recordHandle = NULL;
            return i;
        }

        MemHandleUnlock(recordHandle); recordHandle = NULL;
    }

    //
    // No more records
    //
    return -1;
}