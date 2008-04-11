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

#include <e32std.h>


#include "base/fscapi.h"
#include "push/FThread.h"
#include "base/SymbianLog.h"
#include "base/globalsdef.h"


USE_NAMESPACE

// Initialize statically the thread ID.
uint32_t FThread::id = 0;

FThread::FThread() : terminate(false),
                     isRunning(false)
{
}

FThread::~FThread() {
}

typedef void (FThread::*ThreadFunction)(void);

void FThread::start( FThread::Priority priority ) {

    RBuf threadId;
    threadId.CreateL(128);
    threadId.Format(_L("FThread-%d"), id++);

    TRAPD(err, sthread.Create(threadId, (TThreadFunction)symbianRunWrapper,
                              KDefaultStackSize, (RAllocator*)&User::Heap(), this));
    if (err == KErrNone) {
        LOG.debug("Created thread (id = %d)", id-1);
        if (priority == InheritPriority) {
            // TODO: how do we get the current thread priority?
            priority = NormalPriority;
        }
        sthread.SetPriority((TThreadPriority)priority);
        sthread.Resume();
    }
}

void FThread::wait() {
    TRequestStatus stat;
    sthread.Logon(stat);
    User::WaitForRequest(stat);
}

TRequestStatus timerStat;

bool FThread::wait(unsigned long timeout) {
    TRequestStatus stat;
    sthread.Logon(stat);

    RThread tthread;
    TBuf<128> tthreadId;
    tthreadId.Format(_L("FThread-timer-%d"), id);
    // Start the timer thread
    if (timeout) {
        this->timeout = timeout;
        TRAPD(err, tthread.Create(tthreadId, (TThreadFunction)symbianTimeoutWrapper,
                                  KDefaultStackSize, (RAllocator*)NULL, this));
        if (err == KErrNone) {
            tthread.Resume();
        }
        User::WaitForRequest(stat, timerStat);
    }
     
    if (this->timeout || timeout == 0) {
        if (timeout) {
            timer.Cancel();
        }
        return true;
    } else {
        return false;
    }
}

bool FThread::finished() const {
    return !isRunning;
}

bool FThread::running() const {
    return isRunning;
}

void FThread::softTerminate() {
    terminate = true;
    // Kill the thread
    sthread.Kill(0);
}

void FThread::sleep(long msec) {
    User::After((TTimeIntervalMicroSeconds32) (msec * 1000));
}

TInt FThread::startTimeout() {
    TRequestStatus status;
    timer.CreateLocal();
    timer.After(status, (TTimeIntervalMicroSeconds32)timeout);
    User::WaitForRequest(status);
    // If the thread is still running we must kill it
    if (isRunning) {
        // This will unlock the calling thread
#if 0
        User::RequestComplete(timerStat);
#endif
    }
    timeout = 0;
    return 0;
}

BEGIN_NAMESPACE

TInt symbianRunWrapper(TAny* thread) {
    
    // Install a new trap handler for the thread.
    CTrapCleanup* cleanupstack = CTrapCleanup::New();
    TInt err = KErrNone;

    // Mandatory!
    // To trap any internal call to "CleanupStack::PushL()"
    TRAP(err, 
         { FThread* t = (FThread*)thread;
           t->run();
         }
    )
    
    delete cleanupstack;
    return err;
}

TInt symbianTimeoutWrapper(TAny* thread) {

    FThread* t = (FThread*)thread;
    t->startTimeout();
    return 0;
}

END_NAMESPACE

