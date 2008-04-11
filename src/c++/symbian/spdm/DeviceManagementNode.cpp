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
#include "base/util/stringUtils.h"
#include "base/util/StringBuffer.h"
#include "base/fscapi.h"
#include "base/SymbianLog.h"
#include "spdm/spdmutils.h"
#include "spdm/constants.h"
#include "spdm/ManagementNode.h"
#include "spdm/DeviceManagementNode.h"
#include "base/globalsdef.h"

USE_NAMESPACE

#define CONFIG_DIR      ".config"
#define SYNC4J_DIR      ".sync4j"

StringBuffer DeviceManagementNode::configPath; 
StringBuffer DeviceManagementNode::configFile = "config.ini";

DeviceManagementNode::DeviceManagementNode(const char* parent,
                                           const char *leafName)
                            : ManagementNode(parent, leafName),
                              lines(new ArrayList),
                              modified(false)
{
    initCurrentDir();
    update(true);
}

DeviceManagementNode::DeviceManagementNode(const char *node)
    : ManagementNode(node),
      lines(new ArrayList),
      modified(false)
{
    initCurrentDir();
    update(true);
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

void DeviceManagementNode::update(bool read) {
    if (!read && !modified) {
        // no work to be done
        return;
    }

    StringBuffer fileName(currentDir);
    concatDirs(fileName, configFile.c_str());
    const char* fname = fileName.c_str();
    FILE* file = fopen(fname, "r");

    if (file) {
        // Open a temp file in writing mode if we must update the config
        if (!read) {
            fclose(file);
            fileName.append(".tmp");
            file = fopen(fileName, "w");
        }

        if (read) {
            // Note: don't create local buffers too big, the Symbian stack size is limited!
            char buffer[128];

            lines->clear();
            while (fgets(buffer, sizeof(buffer), file)) {
                char *eol = strchr(buffer, '\r');
                if (!eol) {
                    eol = strchr(buffer, '\n');
                }
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

                renameFileInCwd( tmpConfig.c_str(), configFile.c_str());
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


int DeviceManagementNode::getChildrenMaxCount() {
    int count = 0;

    RFs fileSession;
    RFile file;
    int cleanupStackSize = 0;

    StringBuffer fileSpecSb(currentDir);
    concatDirs(fileSpecSb, "*.*");

    // TODO use utility function for string conversion
    TBuf8<DIM_MANAGEMENT_PATH> buf8((const unsigned char*)fileSpecSb.c_str());
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
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir|KEntryAttMatchExclusive,
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
    concatDirs(fileSpecSb, "*.*");

    // TODO use utility function for string conversion
    TBuf8<DIM_MANAGEMENT_PATH> buf8((const unsigned char*)fileSpecSb.c_str());
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
    TRAPD(err, fileSession.GetDir(*fileSpec, KEntryAttDir|KEntryAttMatchExclusive,
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
        TBuf<DIM_MANAGEMENT_PATH> fileName = (*dirList)[i].iName;

#if 0
        childrenName[i] = bufToNewChar(buf8);
#else
        // TODO use string utils
        HBufC8* buf8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(fileName);
        childrenName[i] = stringdup((const char*)buf8->Ptr(), buf8->Length());
        *(childrenName[i] + buf8->Length()) = (char)0;
        delete buf8;
#endif

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

    // In Symbian some of the synchronizations are performed using the Symbian
    // SyncML APIs. These APIs use their own configuration, but we want this to
    // be invisible to our clients. Therefore we handle it here and update the
    // Symbian SyncML configuration here. There is a potential problem as we
    // save the SyncML config here while the DMTree is not flushed until the
    // node is closed. This may result in configuration being out of sync, but
    // this should only happen only on error cases. To fix this problem (and
    // other source of unaligned configurations) we shall push the DM
    // configuration into the SyncML config on startup.
#if defined(UPDATE_NATIVE_CONFIG)
    pushSymbianSyncMLConfigParameter(property, newvalue);
#endif

    modified = true;
    delete [] newstr;
}

#if defined(UPDATE_NATIVE_CONFIG)

void DeviceManagementNode::pushSymbianSyncMLConfigParameter(const char* property,
                                                            const char* value)
{
     StringBuffer p(property);
     StringBuffer v(value);

     if (p == "") {
     } else if (p == "") {
     }

}
#endif

ArrayElement* DeviceManagementNode::clone()
{
    DeviceManagementNode* ret = new DeviceManagementNode(context, name);

    int n = children.size();

    for (int i = 0; i<n; ++i) {
        ret->addChild(*((ManagementNode*)children[i]));
    }

    return ret;
}

void DeviceManagementNode::concatDirs(StringBuffer& src1, const char* src2) {
 
    // If the src path terminates with \\ then there is no
    // need to add it again (this would be an illegal symbian path)
    // Unfortunately we cannot use StringBuffer directly for this check
    // there is something weird with \\ in StringBuffer (at least on symbian)
    const char* chars = src1.c_str();
    if (chars[strlen(chars)-1] != '\\') {
        src1.append("\\");
    }
    src1.append(src2);
}

void DeviceManagementNode::initCurrentDir() {

    if (configPath.empty()) {
        currentDir = ".\\";
    } else {
        currentDir = configPath;
    }
    if (context) {
        StringBuffer translatedContext = contextToPath(context);
        const char* tc = translatedContext.c_str();
        concatDirs(currentDir, tc);
    }
    if (name) {
        concatDirs(currentDir, name);
    }
}

StringBuffer DeviceManagementNode::contextToPath(const char* cont) {
    // Contextes are defined as: (string/)* and on Symbian
    // we map them to file. But Symbian uses backslashes as
    // directory separator.
    StringBuffer sb(cont);
    sb.replaceAll("/", "\\", 0);
    return sb;
}

int DeviceManagementNode::renameFileInCwd(const char* src, const char* dst) {

    RFs fileSession;
    RFile file;
    int cleanupStackSize = 0;

    StringBuffer srcSb(currentDir);
    concatDirs(srcSb, src);
    StringBuffer dstSb(currentDir);
    concatDirs(dstSb, dst);

    // TODO use utility function for string conversion
    TBuf8<DIM_MANAGEMENT_PATH> srcBuf8((const unsigned char*)srcSb.c_str());
    HBufC* srcDes = CnvUtfConverter::ConvertToUnicodeFromUtf8L(srcBuf8);
    ++cleanupStackSize;
    CleanupStack::PushL(srcDes);

    TBuf8<DIM_MANAGEMENT_PATH> dstBuf8((const unsigned char*)dstSb.c_str());
    HBufC* dstDes = CnvUtfConverter::ConvertToUnicodeFromUtf8L(dstBuf8);
    ++cleanupStackSize;
    CleanupStack::PushL(dstDes);

    //
    // Connect to the file server
    //
    fileSession.Connect();
    ++cleanupStackSize;
    CleanupClosePushL(fileSession);

    TRAPD(err, fileSession.Rename(*srcDes, *dstDes));

    CleanupStack::PopAndDestroy(cleanupStackSize);

    if (err != KErrNone) {
        return -1;
    } else {
        return 0;
    }
}

//#define UPDATE_NATIVE_CONFIG 1

#if defined(UPDATE_NATIVE_CONFIG)

#include <e32base.h>
#include <e32cmn.h>
#include <syncmlclient.h>
#include <syncmlclientds.h>
#include <syncmldef.h>
#include <syncmltransportproperties.h>
////////////////////////////////////////////////////////////////////

#include <CommDbConnPref.h>  // for IAP retrieval
#include <apselect.h>
#include <CDBPREFTABLE.H>



/**
 * Sync application enum.
 * The applications for which we enable/disable sync.They can 
 * be ESyncCalendar for the calendar application and ESyncContacts
 * for the contacts application.During the creation of the 
 * default Funambol profile they are both enabled.
 */
enum TSyncApp 
{
    ESyncCalendar,
    ESyncContacts
};

/**
 * Errors.
 * Errors specific to this wrapper
 * in addition to those listed into syncmlerr.h
 */
const TInt KErrProfileMgrErrorBase = -9250;
const TInt KErrNoContactsProvider = KErrProfileMgrErrorBase -1;
const TInt KErrNoCalendarProvider = KErrProfileMgrErrorBase -2;
const TInt KErrNoProfile = KErrProfileMgrErrorBase -3;
const TInt KErrWrongSyncApp = KErrProfileMgrErrorBase -4;


/*
 * SyncML profile manager.
 * Provide a small number of functions to create/delete a 
 * SyncML profile, change the username/password used to connect
 * to the sync servr, enable/disable the sync of calendar and 
 * contacts.Link against syncmlclientapi.lib, syncmldatafilter.lib, nsmltransporthandler.lib
 * commdb.lib apengine.lib
 */
class CSyncProfileManager: public CBase
{

public:

    static CSyncProfileManager* createNewInstance();
   
    /**
     *  Destructor.
     */
    ~CSyncProfileManager();

    bool createProfile(const StringBuffer& username, const StringBuffer& password,
                       const StringBuffer& serverURI, const StringBuffer& iapName);
    bool deleteProfile();
    bool setUsername(const StringBuffer& username);
    bool setPassword(const StringBuffer& password);
    bool setCredentials(const StringBuffer& username,
                        const StringBuffer& password);
    bool setIapIdForConnProperties(const StringBuffer& iapName);
    bool enableTask(TSyncApp aSyncApp);
    bool disableTask(TSyncApp aSyncApp);
    
private:

    /**
     * C++ default constructor can NOT contain any code, that might leave.
     */
    CSyncProfileManager();

    /**
     * Create default SyncML profile.
     * Create a default Funambol profile; both calendar and contacts 
     * are enabled for sync; server source for calendar is implicitly set
     * to "cal" and for contacts to "card".If a profile with the same
     * name already exists, it is overwritten.Can leave.
     * @param aUserName The username used to connect to the sync server.
     * @param aPassword The password used to connect to the sync server.
     * @param aServerURI The sync server URI.
     * @param aIapName The IAP to be used; "Ask" sets "Always ask";
     *                 "<IAPName>" sets the specified IAP, if it doesn't
     *                 exists sets to "Always ask"; "Default" set the
     *                 default IAP as in packet data conn prefs. NOTE:
     *                    if the user change the default IAP, SyncML profile
     *                 is not updated!!!
     */
    void createProfileL(const TDesC8& aUsername, const TDesC8& aPassword,
                        const TDesC8& aServerURI, const TDesC& aIapName);
    
    /**
     * Delete specified SyncML profile.
     * Delete specified SyncML profile.Can leave.
     */
    void deleteProfileL();
    
    /**
     * Set username.
     * Set the username to connect to sync server for the specified 
     * profile.Can leave.
     * @param aProfileName The SyncML profile name.
     * @param aUsername The username used to connect.
     */
    void setUsernameL(const TDesC8& aUsername);

    /**
     * Set password.
     * Set the password to connect to sync server for the specified 
     * profile.Can leave.
     * @param aProfileName The SyncML profile name.
     * @param aPassword The password used to connect.
     */
    void setPasswordL(const TDesC8& aPassword);
    
    /*
     * Enable sync application.
     * Enable the specified sync application for the specified 
     * profile.Can leave.
     * @param aProfileName The SyncML profile name.
     * @param aSyncApp The application to be enabled for sync.
     */
    void enableTaskL(TSyncApp aSyncApp, bool enable);

    /**
     * Set internal IAP id.
     * Find the IAP id of the specified IAP and set the internal iIapId.
     * @param aIapName The name of the IAP.
     */
    void setIapIdForConnPropertiesL(const TDesC& aIapName); 

    void setCredentialsL(const TDesC8* aUsername, const TDesC8* aPassword);

    /*
     * descriptor storing the IAP id, used by CreateProfileL.
     */
    TBuf8<32>   iIapId;

    static TBuf16<128> aProfileName;
};

TBuf16<128> CSyncProfileManager::aProfileName = _L("Funambol");

/**
 * CSyncProfileManager::NewL()
 * Two-phased constructor.
 */
CSyncProfileManager* CSyncProfileManager::createNewInstance()
{
    CSyncProfileManager* self = new(ELeave) CSyncProfileManager();
    return self;
}

/**
 * CSyncProfileManager::CSyncProfileManager()
 * C++ default constructor can NOT contain any code, that might leave.
 */
CSyncProfileManager::CSyncProfileManager() 
{
}

/**
 * CSyncProfileManager::~CSyncProfileManager()
 * Class destructor
 */
CSyncProfileManager::~CSyncProfileManager()
{
}

/**
 * CSyncProfileManager::CreateProfileL()
 * Create a default SyncML Funambol profile, calendar and contacts sync 
 * enabled.If a profile with the same name already exists, it is
 * overwritten.
 */
bool CSyncProfileManager::createProfile(const StringBuffer& username,
                                        const StringBuffer& password,
                                        const StringBuffer& serverURI,
                                        const StringBuffer& iapName)
{
    HBufC8* aUsername  = stringBufferToNewBuf8(username);
    HBufC8* aPassword  = stringBufferToNewBuf8(password);
    HBufC8* aServerURI = stringBufferToNewBuf8(serverURI);
    HBufC*  aIapName   = stringBufferToNewBuf(iapName);

    TRAPD(err, createProfileL(*aUsername, *aPassword, *aServerURI, *aIapName));

    delete aUsername;
    delete aPassword;
    delete aServerURI;
    delete aIapName;

    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::deleteProfile() {
    TRAPD(err, deleteProfileL());
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::enableTask(TSyncApp aSyncApp) {
    TRAPD(err, enableTaskL(aSyncApp, true));
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::disableTask(TSyncApp aSyncApp) {
    TRAPD(err, enableTaskL(aSyncApp, false));
    return err == KErrNone ? true : false;
}

/**
 * CSyncProfileManager::SetUsernameL()
 * Set the username to connect to sync server for the specified SyncML profile.
 */
bool CSyncProfileManager::setUsername(const StringBuffer& username) {
    if (username.null()) {
        return false;
    }
    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, (TDesC8*)NULL));
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setPassword(const StringBuffer& password) {
    if (password.null()) {
        return false;
    }
    HBufC8* aPassword = stringBufferToNewBuf8(password);
    TRAPD(err, setCredentialsL((TDesC8*)NULL, aPassword));
    delete aPassword;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setCredentials(const StringBuffer& username,
                                         const StringBuffer& password) {
    if (password.null() || username.null()) {
        return false;
    }
    HBufC8* aPassword = stringBufferToNewBuf8(password);
    HBufC8* aUsername = stringBufferToNewBuf8(username);
    TRAPD(err, setCredentialsL(aUsername, aPassword));
    delete aPassword;
    delete aUsername;
    return err == KErrNone ? true : false;
}

bool CSyncProfileManager::setIapIdForConnProperties(const StringBuffer& iapName) {
    if (iapName.empty()) {
        return false;
    }
    HBufC* aIapName = stringBufferToNewBuf(iapName);
    TRAPD(err, setIapIdForConnPropertiesL(*aIapName));
    delete aIapName;
    return err == KErrNone ? true : false;
}

/**
 * CSyncProfileManager::DeleteProfileL()
 * Delet the specified SyncML profile.
 */
void CSyncProfileManager::deleteProfileL()
{
    RSyncMLSession syncMLSession;
    TSmlProfileId  profileId = -1;
    TInt err=KErrNone;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
    
    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        RSyncMLDataSyncProfile syncProfile;
        
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenRead));
        User::LeaveIfError(err);
        if(syncProfile.DisplayName().Compare(aProfileName)== 0)
        {
            profileId = syncProfile.Identifier();
            syncProfile.Close();
            break;
        }
        syncProfile.Close();
    }
    
    if(profileId == -1)
        User::Leave(KErrNoProfile);
    // delete the profile
    TRAP(err,syncMLSession.DeleteProfileL(profileId));
    User::LeaveIfError(err);
    // close SyncML session
    syncMLSession.Close();
}

void CSyncProfileManager::createProfileL(const TDesC8& aUsername,
                                         const TDesC8& aPassword,
                                         const TDesC8& aServerURI,
                                         const TDesC& aIapName)
{
    RSyncMLSession syncMLSession;
    RSyncMLDataSyncProfile syncProfile;

    TInt err=KErrNone;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
    
    // create a profile
    TRAP(err,syncProfile.CreateL(syncMLSession));
    User::LeaveIfError(err);
    
    TRAP(err, {
        syncProfile.SetDisplayNameL(aProfileName);
        syncProfile.SetUserNameL(aUsername);
        syncProfile.SetPasswordL(aPassword);
        // protocol version, values are:
        // ESmlVersion1_1_2 = 1.1.2
        // ESmlVersion1_2 = 1.2
        syncProfile.SetProtocolVersionL(ESmlVersion1_1_2);
        // This is for accepting all sync requests
        syncProfile.SetSanUserInteractionL(ESmlEnableSync);
        syncProfile.DeleteAllowed();
        // save profile
        syncProfile.UpdateL();
    });
    User::LeaveIfError(err);

    // create and enable tasks (applications)
    // retrieve needed data provider ids
    TSmlDataProviderId contactsProvider = -1;
    TSmlDataProviderId calendarProvider = -1;
    
    RArray<TSmlDataProviderId> dataProvidersArr;
    TRAP(err,syncMLSession.ListDataProvidersL(dataProvidersArr));
    User::LeaveIfError(err);
    TInt count = dataProvidersArr.Count();
    
    for(int i=0;i<count;i++)
    {
        RSyncMLDataProvider dataProvider;
        TRAP(err,dataProvider.OpenL(syncMLSession,dataProvidersArr[i]));
        User::LeaveIfError(err);
        
        if(dataProvider.DisplayName().Compare(_L("Calendar"))==0)
        {
            calendarProvider = dataProvider.Identifier();
        }
        else if(dataProvider.DisplayName().Compare(_L("Contacts"))==0)
        {
            contactsProvider = dataProvider.Identifier();
        }
        dataProvider.Close();
    }
    
    if(contactsProvider == -1)
        User::Leave(KErrNoContactsProvider);
    // contacts
    // default Funambol server source is "card"
    RSyncMLTask contactsTask;
    TRAP(err,contactsTask.CreateL(syncProfile,contactsProvider,_L("card"),_L("C:Contacts.cdb")));
    User::LeaveIfError(err);
    TRAP(err, {
        contactsTask.SetEnabledL(ETrue);
        contactsTask.UpdateL();
    });
    User::LeaveIfError(err);
    contactsTask.Close();
    
    if(calendarProvider == -1)
        User::Leave(KErrNoCalendarProvider);
    // calendar 
    // default Funambol server source is "cal"
    RSyncMLTask calendarTask;
    TRAP(err,calendarTask.CreateL(syncProfile,calendarProvider,_L("cal"),_L("C:Calendar")));
    User::LeaveIfError(err);
    TRAP(err, {
        calendarTask.SetEnabledL(ETrue);
        calendarTask.UpdateL();
    });
    User::LeaveIfError(err);
    
    calendarTask.Close();
    
    // SAVE PROFILE AGAIN to save tasks!!!!
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);

    // set server URI and IapId
    // if NSmlIapId = -1, user is asked for IAP
    // otherwise it is set to the specified IapId
    // useful property names: NSmlIapId,NSmlIapId2,NSmlIapId3,NSmlIapId4,
    // NSmlUseRoaming,NSmlHTTPAuth,NSmlHTTPUsername,NSmlHTTPPassword
    RArray<TSmlConnectionId> connArr;
    TRAP(err,syncProfile.ListConnectionsByConnectionIdL(connArr));
    User::LeaveIfError(err);
    TInt count2 = connArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLConnection syncConnection;
        TRAP(err,syncConnection.OpenByConnectionIdL(syncProfile,connArr[i]));
        User::LeaveIfError(err);
        setIapIdForConnPropertiesL(aIapName);
        TRAP(err, {
            syncConnection.SetServerURIL(aServerURI);
            syncConnection.SetPropertyL(_L8("NSmlIapId"),iIapId);
            syncConnection.UpdateL();
        });
        User::LeaveIfError(err);
        syncConnection.Close();
    }
    
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);
    
    // close profile
    syncProfile.Close();
    
    // close SyncML session     
    syncMLSession.Close();

}


/**
 * CSyncProfileManager::SetUsernameL()
 * Set the username to connect to sync server for the specified SyncML profile.
 */
void CSyncProfileManager::setCredentialsL(const TDesC8* aUsername,
                                          const TDesC8* aPassword)
{
    RSyncMLSession syncMLSession;
    TInt err=KErrNone;

    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);

    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        RSyncMLDataSyncProfile syncProfile;
    
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenReadWrite));
        User::LeaveIfError(err);
        if(syncProfile.DisplayName().Compare(aProfileName)== 0)
        {
            TRAP(err, {
                if (aUsername) {
                    syncProfile.SetUserNameL(*aUsername);
                }
                if (aPassword) {
                    syncProfile.SetPasswordL(*aPassword);
                }
                syncProfile.UpdateL();
            });
            User::LeaveIfError(err);
            syncProfile.Close();
            break;
        }
        syncProfile.Close();
    }
    
    // close SyncML session
    syncMLSession.Close();
}


/**
 * CSyncProfileManager::EnableTaskL()
 * Enable sync for the specified application, ESyncContacts or ESyncCalendar.
 */
void CSyncProfileManager::enableTaskL(TSyncApp aSyncApp, bool enable)
{

    RSyncMLSession syncMLSession;
    TSmlProfileId  profileId = -1;
    RSyncMLDataSyncProfile syncProfile;
    TInt err=KErrNone;
    
    TBuf<32> KSyncApp;
    switch(aSyncApp)
    {
        case ESyncCalendar:
        {
            KSyncApp.Copy(_L("C:Calendar")); //C:Calendar is the client source
            break;
        }
        case ESyncContacts:
        {
            KSyncApp.Copy(_L("C:Contacts.cdb")); //C:Contacts.cdb is the client source 
            break;
        }
        default:
        {
            User::Leave(KErrWrongSyncApp);
            break;
        }
    }
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
        
    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenRead));
        User::LeaveIfError(err);
        if(syncProfile.DisplayName().Compare(aProfileName)== 0)
        {
            profileId = syncProfile.Identifier();
            syncProfile.Close();
            break;
        }
        syncProfile.Close();
    }
    
    if(profileId == -1)
        User::Leave(KErrNoProfile);
    
    // find the specified task
    RArray<TSmlTaskId> taskArr;
    
    TRAP(err,syncProfile.OpenL(syncMLSession,profileId,ESmlOpenReadWrite));
    User::LeaveIfError(err);
    TRAP(err,syncProfile.ListTasksL(taskArr));
    User::LeaveIfError(err);
    
    TInt count2 = taskArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLTask syncTask;
        TRAP(err,syncTask.OpenL(syncProfile,taskArr[i]));
        User::LeaveIfError(err);
        if(syncTask.ClientDataSource().Compare(KSyncApp)==0)
        {
            // enable task
            TRAP(err, {
                syncTask.SetEnabledL(enable ? ETrue : EFalse);
                syncTask.UpdateL();
            });
            User::LeaveIfError(err);
            syncTask.Close();
            break;
        }
        syncTask.Close();
    }
    // REMEMBER to update profile or changes won't take effect!!
    TRAP(err,syncProfile.UpdateL());
    User::LeaveIfError(err);
    // close sync profile
    syncProfile.Close();
    // close SyncML session
    syncMLSession.Close();
}

/*
void CSyncProfileManager::TestL(void)
{
    //CSyncMLTransportPropertiesArray* propArr;
    
    RSyncMLSession syncMLSession;
    RSyncMLDataSyncProfile syncProfile;
    TSmlProfileId  profileId = -1;
    TInt err;
    
    // open SyncML session
    TRAP(err,syncMLSession.OpenL());
    User::LeaveIfError(err);
        
    // find the specified profile
    RArray<TSmlProfileId> profileArr;
    TRAP(err,syncMLSession.ListProfilesL(profileArr,ESmlDataSync));
    User::LeaveIfError(err);
    TInt count = profileArr.Count();
    for(int i=0;i<count;i++)
    {
        TRAP(err,syncProfile.OpenL(syncMLSession,profileArr[i],ESmlOpenRead));
        User::LeaveIfError(err);
        if(syncProfile.DisplayName().Compare(_L("Pava"))== 0)
        {
            CEikonEnv::InfoWinL(_L("Found profile!"),_L(""));
            profileId = syncProfile.Identifier();
            syncProfile.Close();
            break;
        }
        syncProfile.Close();
    }
    
    if(profileId == -1)
        User::Leave(KErrNoProfile);
    
    syncProfile.OpenL(syncMLSession,profileId,ESmlOpenRead);
    
    // find all connections
    RArray<TSmlTransportId> transportArr;
    TRAP(err,syncProfile.ListConnectionsL(transportArr));
    User::LeaveIfError(err);
    TInt count2 = transportArr.Count();
    for(int i=0;i<count2;i++)
    {
        RSyncMLTransport syncTransport;
        
        TRAP(err,syncTransport.OpenL(syncMLSession,transportArr[i]));
        User::LeaveIfError(err);
        
        CEikonEnv::InfoWinL(_L("transport name"),syncTransport.DisplayName());
        
        TInt count3 = syncTransport.Properties().Count();
        TBuf<32> propCount;
        propCount.AppendNum(count3);
        CEikonEnv::InfoWinL(_L("properties count"),propCount);
        for(i=0;i<count3;i++)
        {
            TBuf<KSmlMaxTransportPropertyNameLen> propName;
            propName.Copy(syncTransport.Properties().At(i).iName);
            CEikonEnv::InfoWinL(_L("name:"),propName);
            //
            // found properties:
            // NSmlIapId (-1: always ask; otherwise IapId)
            // NSmlIapId2
            // NSmlIapId3
            // NSmlIapId4
            // NSmlUseRoaming
            // NSmlHTTPAuth
            // NSmlHTTPUsername
            // NSmlHTTPPassword
            //
            
            if(propName.Compare(_L("NSmlIapId"))==0)
            {
                CEikonEnv::InfoWinL(_L("gotcha"),_L(""));
                
                RSyncMLConnection syncConnection;
                syncConnection.OpenL(syncProfile,syncTransport.Identifier());
                TBuf<KSmlMaxTransportPropertyNameLen> propValue;
                propValue.Copy(syncConnection.GetPropertyL(syncTransport.Properties().At(i).iName));
                CEikonEnv::InfoWinL(_L("NSmlIapId:"),propValue);
                syncConnection.Close(); 
            }
        }
        syncTransport.Close();
        
    }
    
    syncProfile.Close();
    syncMLSession.Close();
}
*/

/**
 * CSyncProfileManager::SetIapIdForConnProperties()
 * Set the private member iIapId to the IAP id of the specified IAP.
 * It is used by CreateProfileL().
 * If aIapName is set to Ask, then the user will be asked every time;
 * if aIapName is set to <iapname>, then that particular IAP will be used,
 * if the IAP is not found, the default IAP is set;
 * if aIapName is set to Default, the default IAP set into packet data pref
 * is used. NOTE: the default set is a static set, if user change the
 * default IAP, the change is not propagated to the SyncML profile. 
 */
void CSyncProfileManager::setIapIdForConnPropertiesL(const TDesC& aIapName)
{
    
    if(aIapName.Compare(_L("Ask"))==0)
    {
        iIapId.Zero();
        iIapId.Copy(_L8("-1"));
    } 
    else if(aIapName.Compare(_L("Default"))==0)
    {
        // remember that this is a static set:
        // if the default IAP is changed, the change
        // is not reflected in the SyncML profile
        TUint32 iapId;
        // open the IAP communications database
        CCommsDatabase* commDB = CCommsDatabase::NewL();
        CleanupStack::PushL(commDB);
        // create a view in rank order
        CCommsDbConnectionPrefTableView* view;
        view = commDB->OpenConnectionPrefTableInRankOrderLC(ECommDbConnectionDirectionOutgoing);
        // Point to the first entry, the one set as default
        if (view->GotoFirstRecord() == KErrNone)
        {
            CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref pref;
            view->ReadConnectionPreferenceL(pref);
            iapId=pref.iBearer.iIapId;
        
            iIapId.Zero();
            iIapId.AppendNum(iapId);
        
        }
        CleanupStack::PopAndDestroy(); // view
        CleanupStack::PopAndDestroy(); // commDB
    }
    else
    {
        TBool found = EFalse;
        
        //find specified IAP id
        CCommsDatabase* commDb = CCommsDatabase::NewL(EDatabaseTypeIAP);
        CleanupStack::PushL(commDb);
        CApSelect* select = CApSelect::NewLC(*commDb, KEApIspTypeAll,
                                             EApBearerTypeAll,
                                             KEApSortUidAscending);
        TInt32 iapId= 0;
        TBool ok = select->MoveToFirst();
        for (TUint32 i = 0; ok&&(i<select->Count()); i++)
        {
            if ( select->Name() == aIapName)
            {
                found = ETrue;
                iapId = select->Uid();
                iIapId.Zero();
                iIapId.AppendNum(iapId);
            }
            else
            {
                ok = select->MoveNext ();
            }
        }
        CleanupStack::PopAndDestroy(2); //commdb and select
        if(!found)
            setIapIdForConnPropertiesL(_L("Default"));
    }
}
#endif
