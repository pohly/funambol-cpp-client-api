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
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
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

#ifndef INCL_CTP_SERVICE
#define INCL_CTP_SERVICE

/** @cond DEV */

#include "base/fscapi.h"

#include "push/FThread.h"
#include "push/FSocket.h"

#include "push/CTPMessage.h"
#include "push/CTPConfig.h"

/**< CTP Protocol version = 1.0 */
#define CTP_PROTOCOL_VERSION            0x10
/**< Each time the CTP connection is broken, we double the ctpRetry time */
#define CTP_RETRY_INCREASE_FACTOR       2
#include "base/globalsdef.h"

BEGIN_NAMESPACE


// Private Threads
class CTPThread : public FThread {

public:
    CTPThread();
    void run();
    int32_t getErrorCode() { return errorCode; }

private:
    int32_t errorCode;
    bool saveNonceParam(CTPMessage* authStatusMsg);
};

class ReceiverThread : public FThread {
public:
    ReceiverThread();
    void run();
    int32_t getErrorCode() { return errorCode; }

private:
    int32_t errorCode;
};

class HeartbeatThread : public FThread {
public:
    HeartbeatThread();
    void run();
    int32_t getErrorCode() { return errorCode; }

private:
    int32_t errorCode;

};

class CmdTimeoutThread : public FThread {

public:
    CmdTimeoutThread();
    void run();
};

/**
 * The class to manage the CTP connection, singleton.
 */
class CTPService {

public:
    /**
     * The state of CTP connection.
     * State CTP_STATE_WAITING_RESPONSE is used to check the timeout (ctpCmdTimeout)
     * for which the client shall wait for the server response, after sending a command.
     */
    typedef enum {
        CTP_STATE_DISCONNECTED          = 0, 
        CTP_STATE_CONNECTING            = 1, 
        CTP_STATE_CONNECTED             = 2,
        CTP_STATE_AUTHENTICATING        = 3, 
        CTP_STATE_READY                 = 4, 
        CTP_STATE_WAITING_RESPONSE      = 5, 
        CTP_STATE_CLOSING               = 6
    } CtpState;

private:

    /// Pointer to CTPService instance
    static CTPService* pinstance;


    /// The configuration of CTP
    CTPConfig config;

    /// The state of CTP connection.
    CtpState ctpState;

    /// Used by threads to exit in a soft way.
    bool leaving;

    /// The socket used
    FSocket* ctpSocket;

    /**< Handle of main CTP thread, which implements the CTP process */
    CTPThread* ctpThread;                   
    /**< Handle of thread used to receive msg from Server */
    ReceiverThread* receiverThread;
    /**< Handle of thread used to send ready msg to Server */
    HeartbeatThread* heartbeatThread;
    /**< Handle of thread used to check if a response arrived in ctpCmdTimeout seconds */
    CmdTimeoutThread* cmdTimeoutThread;

    /// Store the received message from Server
    CTPMessage* receivedMsg;

    // For debugging
    int32_t totalBytesSent;
    int32_t totalBytesReceived;

    // Private methods:
    int32_t sendMsg(CTPMessage* message);
    StringBuffer createMD5Credentials();
    StringBuffer createErrorMsg(uint32_t errorCode = 0);


protected:

    // Constructor
    CTPService();


public:

    // Method to get the sole instance of CTPService
    static CTPService* getInstance();

    ~CTPService();

    FThread* startCTP();
    int32_t stopCTP();
    int32_t openConnection();
    int32_t closeConnection();
    int32_t receive();
    bool stopThread(FThread* thread);

    // Create and send messages through the socket.
    int32_t sendReadyMsg();
    int32_t sendAuthMsg();
    int32_t sendByeMsg();

    CTPMessage* receiveStatusMsg();

    /**
     * Returns pointer to (internally owned) CTPConfig
     */
    CTPConfig* getConfig() { return &config; }

    /// Get the ctpState member.
    CtpState getCtpState()    { return ctpState; }

    /// Set the ctpState member.
    void setCtpState(CtpState v) { ctpState = v; }

    /// Returns true if CTP is leaving.
    bool isLeaving() { return leaving; }

    /// Forces CTP to go in the "leaving" state. 
    /// It hallows to exit threads and close correctly CTP.
    void setLeaving(bool value) { leaving = value; }

private:
    void hexDump(char *buf, int len);
    int extractMsgLength(const char* package, int packageLen);
    bool saveNonceParam(CTPMessage* authStatusMsg);

};



END_NAMESPACE

/** @endcond */
#endif

