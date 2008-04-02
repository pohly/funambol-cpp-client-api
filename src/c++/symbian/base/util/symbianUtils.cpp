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

#include <e32base.h>
#include <aknnotewrappers.h>
#include <apgtask.h>
#include <eikenv.h>
#include <apgcli.h>

#include "base/fscapi.h"
#include "base/util/symbianUtils.h"
#include "base/util/stringUtils.h"



void msgBox(const TDesC& aMsg, const TWarningLevel aLevel)
{
    // go to foreground and prompt
    TApaTask task(CEikonEnv::Static()->WsSession());
    task.SetWgId( CEikonEnv::Static()->RootWin().Identifier());
    task.BringToForeground();

    CAknResourceNoteDialog* note;
    switch (aLevel)
    {
        case EInfoLevel:
            note = new (ELeave)CAknInformationNote(ETrue);
            break;
        case EWarningLevel:
            note = new (ELeave)CAknWarningNote(ETrue);
            break;
        case EErrorLevel:
            note = new (ELeave)CAknErrorNote(ETrue);
            break;
        default:
            return;
    }

    note->ExecuteLD(aMsg);
}

void msgBox(const TDesC8& aMsg, const TWarningLevel aLevel)
{
    RBuf msg16;
    msg16.CreateL(aMsg.Length());
    CnvUtfConverter::ConvertToUnicodeFromUtf8(msg16, aMsg);

    msgBox(msg16, aLevel);
}

void msgBox(const char* aMsg, const TWarningLevel aLevel)
{
    RBuf16 buf16;
    buf16.Assign(charToNewBuf(aMsg));
    msgBox(buf16);

}
void msgBox(const wchar_t* aMsg, const TWarningLevel aLevel)
{
    RBuf16 buf16;
    buf16.Assign(wcharToNewBuf(aMsg));
    msgBox(buf16);
}





void showAlert(const TDesC& aMsg)
{
    CEikonEnv::Static()->AlertWin(aMsg);
}

void showAlert(const TDesC8& aMsg)
{
    RBuf msg16;
    msg16.CreateL(aMsg.Length());
    CnvUtfConverter::ConvertToUnicodeFromUtf8(msg16, aMsg);

    showAlert(msg16);
}

void showAlert(const char* aMsg)
{
    RBuf16 buf16;
    buf16.Assign(charToNewBuf(aMsg));
    showAlert(buf16);
}
void showAlert(const wchar_t* aMsg)
{
    RBuf16 buf16;
    buf16.Assign(wcharToNewBuf(aMsg));
    showAlert(buf16);
}
