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

#include <NetMgr.h>
#include "http/palm/Sock.h"
#include "base/Log.h"
#include "base/debug.h"
#include "base/util/UTF8StringBuffer.h"


#define DIM_BUF 1024

Sock::Sock(unsigned long maxSize = MAX_MSG_SIZE)
{
    // gestione parametri
    netRefnum=0;
    socketRef=0;
    timeOut=120;                                 // in seconds
    AppNetTimeout=SysTicksPerSecond()*timeOut;  // in ticks

    // gestione address
    addr = new NetSocketAddrINType;
    addr->family=netSocketAddrINET;
    addr->port=23;
    addr->addr=NULL;

    content = NULL;
    contentLength = 0;

    maxMsgSize = maxSize;
}


Sock::~Sock()
{
    close();

    if (content) {
        delete [] content;
    }

    if (addr) {
        delete addr;
    }
}


void Sock::setTimeout(UInt16 x)
{
    timeOut=x;
    AppNetTimeout=SysTicksPerSecond()*timeOut;
}


UInt16 Sock::getTimeout()
{
    return timeOut;
}


UInt16 Sock::getTicks()
{
    return AppNetTimeout;
}


Err Sock::findLibrary()
{
    return SysLibFind("Net.lib",&netRefnum);
}


Err Sock::openNetLibrary()
{
    UInt16 ifErrs;
    Err err=NetLibOpen(netRefnum,&ifErrs);

    if (err==netErrAlreadyOpen) {

        // la connessione era stata interrotta dall'utente
        UInt16 countP=0;
        NetLibOpenCount(netRefnum,&countP);

        for (UInt16 i=0;i<=countP;i++)
            err=NetLibClose(netRefnum,true);

        if (err==netErrStillOpen)
            return err;

        err=NetLibOpen(netRefnum,&ifErrs);

        if (ifErrs==netErrUserCancel) {
            return netErrUserCancel;
        }
    }

    if (ifErrs==netErrUserCancel) {
        return netErrUserCancel;
    }

    return errNone;
}


Err Sock::closeNetLibrary()
{
    Err err = 0;

    UInt16 countP=0;
    NetLibOpenCount(netRefnum,&countP);

    for (UInt16 i=0;i<countP;i++) {
        err=NetLibClose(netRefnum,true);
    }

    return err;
}


Err Sock::open()
{
    Err error=0;
    socketRef=NetLibSocketOpen(netRefnum,netSocketAddrINET,netSocketTypeStream,netSocketProtoIPTCP,AppNetTimeout,&error);
    switch (error) {
        case netErrNotOpen:
            error=openNetLibrary();
            if (error==0)
                socketRef=NetLibSocketOpen(netRefnum,netSocketAddrINET,netSocketTypeStream,netSocketProtoIPTCP,AppNetTimeout,&error);
            break;
    }
    return error;
}


Err Sock::close()
{
    Err error=0;

    NetLibSocketClose(netRefnum,socketRef,AppNetTimeout,&error);

    return error;
}


Err Sock::connect(char* ipName,UInt16 port)
{
    NetIPAddr ipAddr;
    if (isDotted(ipName))
        ipAddr=NetLibAddrAToIN(netRefnum,ipName);
    else
        ipAddr=GetIPAddress(ipName,netRefnum,AppNetTimeout);

    Err error=0;

    addr->family=netSocketAddrINET;
    addr->addr=ipAddr;
    addr->port=port;

    Int16 result=NetLibSocketConnect(netRefnum,socketRef,(NetSocketAddrType*)addr,sizeof(addr),AppNetTimeout,&error);
    switch (error) {
        case netErrSocketAlreadyConnected:
            // prosecuzione normale del programma
            break;

        case netErrSocketNotOpen:
            // occorre tentare di aprire nuovamente il socket
            error=open();
            if (error==0)
                result=NetLibSocketConnect(netRefnum,socketRef,(NetSocketAddrType*)addr,sizeof(addr),AppNetTimeout,&error);
            break;

        case netErrNotOpen:
             // occorre tentare di aprire nuovamente la library
             error=openNetLibrary();
             if (error==0) {
                error=open();
                if (error==0)
                    result=NetLibSocketConnect(netRefnum,socketRef,(NetSocketAddrType*)addr,sizeof(addr),AppNetTimeout,&error);
             }
             break;

        default:
            return error;
            break;
    }

    return error;
}


Err Sock::disconnect()
{
    Err error = 0;
    NetLibSocketShutdown(netRefnum,socketRef,netSocketDirBoth,AppNetTimeout,&error);
    return error;
}


Err Sock::sendData(char* data)
{
    Err error = 0;
    UInt16 bytesToSend = StrLen(data);
    Int16 bytesSent = 0;

    while (bytesToSend > 0) {
        bytesSent = NetLibSend(
            netRefnum,
            socketRef,
            data,
            (bytesToSend > DIM_BUF) ? DIM_BUF : bytesToSend,
            0,
            NULL,
            0,
            AppNetTimeout,
            &error
        );

        if (bytesSent <= 0) {
            break;
        }
        data += bytesSent;
        bytesToSend -= bytesSent;
     }

    return error;
}

Err Sock::receiveData() {
    Err error = errNone;

    if (content == NULL) {
        content = new char[maxMsgSize];
    }

    wcsprintf(logmsg, DBG_READING_CONTENT, (long)(maxMsgSize));
    LOG.debug(logmsg);

    UInt32 c = 0;
    UInt32 bytesRead=0;
    while (true) {
        LOG.debug(DBG_READING);
        //
        // Note: we read incombing bytes a chunk at a time,
        // since reading too much in once seems confuse the
        // emulator (at least).
        //
        bytesRead = NetLibReceive(
                        netRefnum,
                        socketRef,
                        content+c,
                        1023,
                        0,
                        NULL,
                        0,
                        AppNetTimeout,
                        &error
                    );

        wcsprintf(logmsg, DBG_READING_BUFFER, bytesRead);

        LOG.debug(logmsg);
        if ((bytesRead<=0) || (error != errNone)) {
            break;
        }

        c += bytesRead;
    }

    LOG.debug(DBG_READING_DONE);

finally:
    if (c>0) {
        content[c] = 0;
        contentLength = c;
    } else {
        content[0] = 0;
        contentLength = 0;
    }

    return error;
}


UInt16 Sock::getContentLength()
{
    return contentLength;
}


NetIPAddr GetIPAddress(Char* hostName,UInt16 netRefNum,Int32 timeOut)
{
    Err error=0;
    NetIPAddr address=0;

    NetHostInfoBufPtr hostInfoBufP=(NetHostInfoBufPtr)MemPtrNew(sizeof(NetHostInfoBufType));

    if (!hostInfoBufP)
        return 0;

    NetHostInfoPtr hostInfoP=NetLibGetHostByName(netRefNum,hostName,hostInfoBufP,timeOut,&error);
    if (error==0) {
        NetIPAddr* addressP=(NetIPAddr*)hostInfoP->addrListP[0];
        address=NetNToHL(*addressP);
    } else {
        // azione da intraprendere x segnalare l'errore
    }

    if (hostInfoBufP)
        MemPtrFree(hostInfoBufP);
    return address;

}


char* Sock::getContent()
{
    return content;
}


Boolean Sock::isDotted(char* hostName)
{
    if ((StrStr(hostName,"1")!=NULL)||
        (StrStr(hostName,"2")!=NULL)||
        (StrStr(hostName,"3")!=NULL)||
        (StrStr(hostName,"4")!=NULL)||
        (StrStr(hostName,"5")!=NULL)||
        (StrStr(hostName,"6")!=NULL)||
        (StrStr(hostName,"7")!=NULL)||
        (StrStr(hostName,"8")!=NULL)||
        (StrStr(hostName,"9")!=NULL)||
        (StrStr(hostName,"0")!=NULL))
    return true;
    else return false;
}