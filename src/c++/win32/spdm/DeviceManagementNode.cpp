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


#include "base/util/utils.h"
#include "base/fscapi.h"
#include "base/Log.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"


DeviceManagementNode::DeviceManagementNode(const wchar_t* parent, const wchar_t* name)
    : ManagementNode(parent, name), fullContext(0)
{
    setFullContext();
}

DeviceManagementNode::DeviceManagementNode(const wchar_t *node)
    : ManagementNode(node), fullContext(0)
{
    setFullContext();
}

DeviceManagementNode::~DeviceManagementNode()
{
    if (fullContext)
        delete [] fullContext;
}

/*
 * Returns the value of the given property
 * the value is returned as a new wchar_t array and must be free'd by the user
 *
 * @param property - the property name
 */
wchar_t* DeviceManagementNode::getPropertyValue(const wchar_t* property) {
    HKEY key = NULL;    
    DWORD res;	
    long err = 0;
    wchar_t *buf=NULL;

    ULONG dim = 0;

    RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, TEXT("Invalid context path: %s"), fullContext);
        goto finally;
    }

    err = RegQueryValueEx(
            key,
            property,
            NULL,
            NULL,  // we currently support only strings
            (UCHAR*)buf,
            &dim
            );

    if (buf == NULL) {
        buf = new wchar_t[dim + 1];

        err = RegQueryValueEx(
                key,
                property,
                NULL,
                NULL,  // we currently support only strings
                (UCHAR*)buf,
                &dim
                );
    }


    if (err == ERROR_SUCCESS) {
        ; //MessageBox(NULL,  TEXT("Correct"), TEXT("getConfigParameter"), MB_OK);
    } else {		
        ;//MessageBox(NULL,  TEXT("Error"), TEXT("getConfigParameter"), MB_OK);
        wsprintf(buf, TEXT(""));
    }

finally:

    if (key != 0) {
        RegCloseKey(key);
    }

    return buf;
}

int DeviceManagementNode::getChildrenMaxCount() {
    HKEY key;
    ULONG howMany = 0;

    RegOpenKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            KEY_READ,
            &key
            );

    if (key == 0) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, TEXT("Invalid context path: %s"), fullContext);

        goto finally;
    }

    RegQueryInfoKey(
            key, NULL, NULL, NULL, &howMany, NULL, NULL, NULL, NULL, NULL, NULL, NULL
            );


finally:

    if (key != 0) {
        RegCloseKey(key);
    }

    return howMany;
}


/*
 * Returns the children's name of the parent node. 
 *
 * @param children - the buffer where ManagementNode* must be stored
 * @param size - the size of the children buffer (number of ManagementNode*) in
 *               input; the number of children in output
 */
wchar_t **DeviceManagementNode::getChildrenNames() {
    ULONG dim = DIM_MANAGEMENT_PATH;
    HKEY key;
    wchar_t child[DIM_MANAGEMENT_PATH];
	wchar_t **childrenName = 0;
    ULONG i, howMany = 0;

    DWORD res;
    HRESULT ret;

    RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );

    if (key == 0) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, TEXT("Invalid context path: %s"), fullContext);
        goto finally;
    }

    //
    // First of all we need to know how many
    RegQueryInfoKey(
            key, NULL, NULL, NULL, &howMany, NULL, NULL, NULL,
			NULL, NULL, NULL, NULL
            );

	childrenName = new wchar_t *[howMany];

    for (i=0; i<howMany; ++i) {
        dim = DIM_MANAGEMENT_PATH*sizeof(wchar_t);
        child[0] = 0;

        ret = RegEnumKeyEx(key, i, child, &dim, NULL, NULL, NULL, NULL);
        if (ret != ERROR_SUCCESS) {
            if (ret == ERROR_NO_MORE_ITEMS) {
                break;
            }
            else {
                lastErrorCode = GetLastError();
                wcscpy(lastErrorMsg, TEXT("Error enumerating children nodes"));
                goto finally;
            }
        }
        childrenName[i] = stringdup(child);
    }


finally:

    if (key != 0) {
        RegCloseKey(key);
    }

	return childrenName;
}


/*
 * Sets a property value.
 *
 * @param property - the property name
 * @param value - the property value (zero terminated string)
 */
void DeviceManagementNode::setPropertyValue(const wchar_t* property, const wchar_t* value) {
    HKEY key;
    DWORD res;

    RegCreateKeyEx(
            HKEY_LOCAL_MACHINE,
            fullContext,
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_ALL_ACCESS,
            NULL,
            &key,
            &res
            );


    if (key == 0) {
        lastErrorCode = ERR_INVALID_CONTEXT;
        wsprintf(lastErrorMsg, TEXT("Invalid context path: %s"), fullContext);
        goto finally;
    }

    RegSetValueEx(
            key,
            property,
            NULL,
            REG_SZ,  // we currently support only strings
            (UCHAR*)value,
            (wcslen(value)+1)*sizeof(wchar_t*)
            );


finally:

    if (key != 0) {
        RegCloseKey(key);
    }
}

/*
 * Device management contexts are expressed as slash '/' separated
 * context path. This function converts the '/' chars in '\\' as
 * accepted by the registry API.
 *
 * @param str - the string to convert
 *
 * @return  a new-allocated string with the converted path
 */
wchar_t *DeviceManagementNode::convertSlashes(const wchar_t* str) {
    int i, len;

    len = wcslen(str);
    wchar_t *ret = new wchar_t[len+1];

    for (i=0; i<len; ++i) {
        if (str[i] == L'/') {
            ret[i] = L'\\';
        }
        else {
            ret[i] = str[i];
        }
    }
    ret[len] = 0;
    return ret;
}

void DeviceManagementNode::setFullContext() { 
    int len = wcslen(context)+wcslen(name)+1;
    wchar_t *ctx;
    const wchar_t swkey[] = TEXT("Software");

	if (fullContext)
		delete [] fullContext;

    ctx = convertSlashes(context);
    if (wcsstr(context, swkey) == NULL) {
        len += wcslen(swkey)+1;
        fullContext = new wchar_t[len+1];
        wsprintf(fullContext, TEXT("%s\\%s\\%s"), swkey, ctx, name);
		LOG.debug(fullContext);
    }
    else {
        fullContext = new wchar_t[len+1];
        wsprintf(fullContext, TEXT("%s\\%s"), ctx, name);
    }
    delete [] ctx;
}

/*
 * Creates a new ManagementNode with the exact content of this object.
 * The new instance MUST be created with the C++ new opertator.
 */
ArrayElement* DeviceManagementNode::clone() {
	DeviceManagementNode* ret = new DeviceManagementNode(context, name);
	
	int n = children.size();
	
	for (int i = 0; i<n; ++i) {
		ret->addChild(*((ManagementNode*)children[i]));
	}
	
	return ret;
}

