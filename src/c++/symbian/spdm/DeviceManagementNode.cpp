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


#include <stdio.h>
#include <ctype.h>
#include <f32file.h>
#include <utf.h>

#include "base/util/utils.h"
#include "base/fscapi.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"

#define CONFIG_DIR      ".config"
#define SYNC4J_DIR      ".sync4j"

//static StringBuffer DeviceManagementNode::defaultPath;

StringBuffer DeviceManagementNode::configPath; 
StringBuffer DeviceManagementNode::configFile = "config.ini";

DeviceManagementNode::DeviceManagementNode(const char* parent,
                                           const char *leafName)
                            : ManagementNode(parent, leafName),
                              lines(new ArrayList),
                              modified(false)
{
    update(true);
}

DeviceManagementNode::DeviceManagementNode(const char *node)
    : ManagementNode(node),
      lines(new ArrayList),
      modified(false)
{
    update(true);
    if (configPath.empty()) {
        currentDir = ".\\\\";
    } else {
        currentDir = configPath;
    }
}

DeviceManagementNode::DeviceManagementNode(const DeviceManagementNode &other)
    : ManagementNode(other) {

    lines = other.lines->clone();
    currentDir = other.currentDir;
    modified = other.modified;
}

DeviceManagementNode::~DeviceManagementNode() {
    if (modified) {
        update(false);
    }
    delete lines;
}

#if 0
bool checkConfigurationPath(StringBuffer path) {

    int val = chdir(path);
    if (val == 0){
        return true;
    }else{
        return false;
    }
}
#endif

void DeviceManagementNode::update(bool read) {
    if (!read && !modified) {
        // no work to be done
        return;
    }

    StringBuffer fileName(currentDir);
    fileName.append("config.ini");
    FILE* file = fopen(fileName.c_str(), "r");

    if (file) {
        // Open a temp file in writing mode if we must update the config
        if (!read) {
            fclose(file);
            fileName.append(".tmp");
            file = fopen(fileName, "w");
        }

        if (read) {
            char buffer[512];

            lines->clear();
            while (fgets(buffer, sizeof(buffer), file)) {
                char *eol = strchr(buffer, '\n');
                if (eol) {
                    *eol = 0;
                }
                line newline(buffer);
                lines->add(newline);
            }
        } else {
            int i = 0;

            while (true) {
                line *curr = (line *)lines->get(i);
                if (!curr) {
                    break;
                }
                fprintf(file, "%s\n", curr->getLine());

                i++;
            }
            fflush(file);
            if (!ferror(file)) {
                StringBuffer tmpConfig = configFile;
                tmpConfig += ".tmp"; 
#if 0
                // TODO FIXME
                rename( tmpConfig.c_str(), configFile.c_str());
#endif
            }
        }
        fclose(file);
    }
}

int DeviceManagementNode::strnicmp( const char *a, const char *b, int len ) {
    while (--len >= 0) {
        if (toupper(*a) != toupper(*b)) {
            return 1;
        }
        a++;
        b++;
    }
    return 0;
}


/*
 * Returns the value of the given property
 * the value is returned as a new char array and must be fred by the user
 *
 * @param property - the property name
 */
char* DeviceManagementNode::readPropertyValue(const char* property) {
    int i = 0;

    while (true) {
        line *curr = (line *)lines->get(i);
        if (!curr) {
            break;
        }

        const char *value = curr->getLine();
        while (*value && isspace(*value)) {
            value++;
        }
        if (!strnicmp(value, property, strlen(property))) {
            value = strchr(value, '=');
            if (value) {
                value++;
                while (*value && isspace(*value)) {
                    value++;
                }
                char *res = stringdup(value);   // FOUND :)

                // remove trailing white space: usually it is
                // added accidentally by users
                char *tmp = res + strlen(res) - 1;
                while (tmp > res) {
                    if (!isspace(*tmp)) {
                        break;
                    }
                    tmp--;
                }
                tmp[1] = 0;

                return res;
            }
        }
        i++;
    }
    // Not found, return an empty string
    return stringdup("");
}

// TEMP TODO FIXME
#include "base/util/symbianUtils.h"

int DeviceManagementNode::getChildrenMaxCount() {
    int count = 0;

    RFs fileSession;
    RFile file;
    int cleanupStackSize = 0;

    StringBuffer fileSpecSb(currentDir);

    msgBox(currentDir.c_str());

    // If the config path terminates with \\ then there is no
    // need to add it again (this causes the GetDir to fail)
    size_t backslashPos = fileSpecSb.rfind("\\\\");
    if (backslashPos != StringBuffer::npos &&
        backslashPos == fileSpecSb.length()-2)
    {
        fileSpecSb.append("*.*");
    } else {
        //TODO FIXME fileSpecSb.append("\\*.*");
        fileSpecSb.append("*.*");
    }
    TBuf8<255> buf8((const unsigned char*)fileSpecSb.c_str());
    HBufC* fileSpec = CnvUtfConverter::ConvertToUnicodeFromUtf8L(buf8);
    ++cleanupStackSize;
    CleanupStack::PushL(fileSpec);

    //
    // Connect to the file server
    //
    fileSession.Connect();
    ++cleanupStackSize;
    CleanupClosePushL(fileSession);

    //
    // Get the directories list, sorted by name
    // (Leave if an error occurs)
    //
    CDir* dirList;
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir,
                                  ESortByName, dirList));
    if (err != KErrNone || dirList == NULL) {
        goto finally;
    }
    ++cleanupStackSize;
    CleanupStack::PushL(dirList);

    count = dirList->Count(); 
finally:
    //
    // Close the connection with the file server
    // and destroy dirList
    //
    fileSession.Close();
    CleanupStack::PopAndDestroy(cleanupStackSize);
    return count;
}



char **DeviceManagementNode::getChildrenNames() {
    char **childrenName = 0;
    int cleanupStackSize = 0;
    RFs fileSession;
    RFile file;

    StringBuffer fileSpecSb(currentDir);
    size_t backslashPos = fileSpecSb.rfind("\\");
    if (backslashPos != StringBuffer::npos &&
        backslashPos == fileSpecSb.length()-2)
    {
        fileSpecSb.append("*.*");
    } else {
        //TODO FIXME fileSpecSb.append("\\*.*");
        fileSpecSb.append("*.*");
    }
    TBuf8<255> buf8((const unsigned char*)fileSpecSb.c_str());
    HBufC* fileSpec = CnvUtfConverter::ConvertToUnicodeFromUtf8L(buf8);
    ++cleanupStackSize;
    CleanupStack::PushL(fileSpec);

    //
    // Connect to the file server
    //
    fileSession.Connect();
    ++cleanupStackSize;
    CleanupClosePushL(fileSession);

    //
    // Get the directories list, sorted by name
    // (Leave if an error occurs)
    //
    CDir* dirList;
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir,
                                  ESortByName, dirList));
    if (err != KErrNone || dirList == NULL) {
        goto finally;
    }
    ++cleanupStackSize;
    CleanupStack::PushL(dirList);

    //
    // Process each entry
    //
    childrenName = new char*[dirList->Count()];
    TInt i;
    for (i=0;i<dirList->Count();i++)
    {
        TBuf<255> fileName = (*dirList)[i].iName;
        HBufC8* buf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(fileName);
        childrenName[i] = strdup((const char*)buf8->Ptr());
        delete buf8;
    }
    fileSession.Close();

finally:
    //
    // Close the connection with the file server
    // and destroy dirList
    //
    CleanupStack::PopAndDestroy(cleanupStackSize);
    return childrenName;
}

/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
void DeviceManagementNode::setPropertyValue(const char* property, const char* newvalue) {
    int i = 0;

    while (true) {
        line *curr = (line *)lines->get(i);
        if (!curr) {
            break;
        }

        const char *start = curr->getLine();
        const char *value = start;

        while (*value && isspace(*value)) {
            value++;
        }
        if (!strnicmp(value, property, strlen(property))) {
            value = strchr(value, '=');
            if (value) {
                value++;
                while (*value && isspace(*value)) {
                    value++;
                }
                if (strcmp(value, newvalue)) {
                    // preserve indention and property name from original config
                    char *newstr = new char[(value - start) + strlen(newvalue) + 1];
                    strncpy(newstr, start, value - start);
                    strcpy(newstr + (value - start), newvalue);
                    curr->setLine(newstr);
                    delete [] newstr;
                    modified = true;
                }
                return;
            }
        }

        i++;
    }

    char *newstr = new char[strlen(property) + 3 + strlen(newvalue) + 1];
    sprintf(newstr, "%s = %s", property, newvalue);
    line newline(newstr);
    lines->add(newline);
    modified = true;
    delete [] newstr;
}

ArrayElement* DeviceManagementNode::clone()
{
    DeviceManagementNode* ret = new DeviceManagementNode(context, name);

    int n = children.size();

    for (int i = 0; i<n; ++i) {
        ret->addChild(*((ManagementNode*)children[i]));
    }

    return ret;
}

