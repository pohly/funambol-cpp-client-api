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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "base/fscapi.h"
#include "base/constants.h"
#include "base/util/utils.h"
#include "spdm/constants.h"
#include "client/MailSourceManagementNode.h"

MailSourceManagementNode::MailSourceManagementNode(const wchar_t*        context,
                                                   const wchar_t*        name   )
    : DeviceManagementNode(context, name) {
}

MailSourceManagementNode::MailSourceManagementNode(const wchar_t*        context,
                                                   const wchar_t*        name   ,
                                                   MailSyncSourceConfig& c      )
    : DeviceManagementNode(context, name) {

    setMailSourceConfig(c);
}

MailSourceManagementNode::~MailSourceManagementNode() {
}

MailSyncSourceConfig& MailSourceManagementNode::getMailSourceConfig(BOOL refresh) {
    if (refresh) {
        wchar_t* c = NULL;

        config.setName(getPropertyValue(PROPERTY_SOURCE_NAME));
        config.setURI(getPropertyValue(PROPERTY_SOURCE_URI));
        config.setSyncModes(getPropertyValue(PROPERTY_SOURCE_SYNC_MODES));
        config.setSync(getPropertyValue(PROPERTY_SOURCE_SYNC));
        config.setType(getPropertyValue(PROPERTY_SOURCE_TYPE));

        config.setLast(wcstol(getPropertyValue(PROPERTY_SOURCE_LAST_SYNC), &c, 10));
        config.setDownloadAge((int)wcstol(getPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE), &c, 10));
        config.setBodySize((int)wcstol(getPropertyValue(PROPERTY_SOURCE_BODY_SIZE), &c, 10));
        config.setAttachSize((int)wcstol(getPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE), &c, 10));

        config.setInbox((int)wcstol(getPropertyValue(PROPERTY_SOURCE_INBOX), &c, 10));
        config.setDraft((int)wcstol(getPropertyValue(PROPERTY_SOURCE_DRAFT), &c, 10));
        config.setTrash((int)wcstol(getPropertyValue(PROPERTY_SOURCE_TRASH), &c, 10));
        config.setOutbox((int)wcstol(getPropertyValue(PROPERTY_SOURCE_OUTBOX), &c, 10));
        config.setSent((int)wcstol(getPropertyValue(PROPERTY_SOURCE_SENT), &c, 10));
    }

    return config;
}

void MailSourceManagementNode::setMailSourceConfig(MailSyncSourceConfig& c) {
    config.assign(c);

    wchar_t t[512];

    setPropertyValue(PROPERTY_SOURCE_NAME,       (wchar_t*)c.getName());
    setPropertyValue(PROPERTY_SOURCE_URI,        (wchar_t*)c.getURI());
    setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, (wchar_t*)c.getSyncModes());
    setPropertyValue(PROPERTY_SOURCE_SYNC,       (wchar_t*)c.getSync());
    setPropertyValue(PROPERTY_SOURCE_TYPE,       (wchar_t*)c.getType());

    wcsprintf(t, TEXT("%ld"), c.getLast());
    setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, t);
    wcsprintf(t, TEXT("%d"), c.getDownloadAge());
    setPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE, t);
    wcsprintf(t, TEXT("%d"), c.getBodySize());
    setPropertyValue(PROPERTY_SOURCE_BODY_SIZE, t);
    wcsprintf(t, TEXT("%d"), c.getAttachSize());
    setPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE, t);

    wcsprintf(t, TEXT("%d"), c.getInbox());
    setPropertyValue(PROPERTY_SOURCE_INBOX, t);
    wcsprintf(t, TEXT("%d"), c.getOutbox());
    setPropertyValue(PROPERTY_SOURCE_OUTBOX, t);
    wcsprintf(t, TEXT("%d"), c.getTrash());
    setPropertyValue(PROPERTY_SOURCE_TRASH, t);
    wcsprintf(t, TEXT("%d"), c.getSent());
    setPropertyValue(PROPERTY_SOURCE_SENT, t);
    wcsprintf(t, TEXT("%d"), c.getDraft());
    setPropertyValue(PROPERTY_SOURCE_DRAFT, t);

}


ArrayElement* MailSourceManagementNode::clone()  {
    return new MailSourceManagementNode(context, name, config);
}
