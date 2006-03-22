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

MailSourceManagementNode::MailSourceManagementNode(const BCHAR* context,
                                                   const BCHAR* name   )
    : DeviceManagementNode(context, name) {
}

MailSourceManagementNode::MailSourceManagementNode(const BCHAR*        context,
                                                   const BCHAR*        name   ,
                                                   MailSyncSourceConfig& c      )
    : DeviceManagementNode(context, name) {

    setMailSourceConfig(c);
}

MailSourceManagementNode::~MailSourceManagementNode() {
}

MailSyncSourceConfig& MailSourceManagementNode::getMailSourceConfig(BOOL refresh) {
    if (refresh) {
        BCHAR* c = NULL;

        config.setName(getPropertyValue(PROPERTY_SOURCE_NAME));
        config.setURI(getPropertyValue(PROPERTY_SOURCE_URI));
        config.setSyncModes(getPropertyValue(PROPERTY_SOURCE_SYNC_MODES));
        config.setSync(getPropertyValue(PROPERTY_SOURCE_SYNC));
        config.setType(getPropertyValue(PROPERTY_SOURCE_TYPE));

        config.setLast(bstrtol(getPropertyValue(PROPERTY_SOURCE_LAST_SYNC), &c, 10));
        config.setDownloadAge((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE), &c, 10));
        config.setBodySize((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_BODY_SIZE), &c, 10));
        config.setAttachSize((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE), &c, 10));

        config.setInbox((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_INBOX), &c, 10));
        config.setDraft((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_DRAFT), &c, 10));
        config.setTrash((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_TRASH), &c, 10));
        config.setOutbox((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_OUTBOX), &c, 10));
        config.setSent((int)bstrtol(getPropertyValue(PROPERTY_SOURCE_SENT), &c, 10));
    }

    return config;
}

void MailSourceManagementNode::setMailSourceConfig(MailSyncSourceConfig& c) {
    config.assign(c);

    BCHAR t[512];

    setPropertyValue(PROPERTY_SOURCE_NAME,       (BCHAR*)c.getName());
    setPropertyValue(PROPERTY_SOURCE_URI,        (BCHAR*)c.getURI());
    setPropertyValue(PROPERTY_SOURCE_SYNC_MODES, (BCHAR*)c.getSyncModes());
    setPropertyValue(PROPERTY_SOURCE_SYNC,       (BCHAR*)c.getSync());
    setPropertyValue(PROPERTY_SOURCE_TYPE,       (BCHAR*)c.getType());

    bsprintf(t, T("%ld"), c.getLast());
    setPropertyValue(PROPERTY_SOURCE_LAST_SYNC, t);
    bsprintf(t, T("%d"), c.getDownloadAge());
    setPropertyValue(PROPERTY_SOURCE_DOWNLOAD_AGE, t);
    bsprintf(t, T("%d"), c.getBodySize());
    setPropertyValue(PROPERTY_SOURCE_BODY_SIZE, t);
    bsprintf(t, T("%d"), c.getAttachSize());
    setPropertyValue(PROPERTY_SOURCE_ATTACH_SIZE, t);

    bsprintf(t, T("%d"), c.getInbox());
    setPropertyValue(PROPERTY_SOURCE_INBOX, t);
    bsprintf(t, T("%d"), c.getOutbox());
    setPropertyValue(PROPERTY_SOURCE_OUTBOX, t);
    bsprintf(t, T("%d"), c.getTrash());
    setPropertyValue(PROPERTY_SOURCE_TRASH, t);
    bsprintf(t, T("%d"), c.getSent());
    setPropertyValue(PROPERTY_SOURCE_SENT, t);
    bsprintf(t, T("%d"), c.getDraft());
    setPropertyValue(PROPERTY_SOURCE_DRAFT, t);

}


ArrayElement* MailSourceManagementNode::clone()  {
    return new MailSourceManagementNode(context, name, config);
}
