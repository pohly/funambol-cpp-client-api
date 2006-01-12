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


#ifndef INCL_RESPONSE_COMMAND
#define INCL_RESPONSE_COMMAND

#include "base/fscapi.h"
#include "syncml/core/ItemizedCommand.h"


class ResponseCommand : public ItemizedCommand {
    
     // ------------------------------------------------------------ Protected data
    protected:
        /**
         * Message reference
         */
        wchar_t* msgRef;
    
        /** 
         * Command reference
         */
        wchar_t* cmdRef;
    
        /**
         * Target references
         */
        ArrayList* targetRef; // TargetRef[]
    
        /**
         * Source references
         */
        ArrayList* sourceRef; // SourceRef[]                        
    
    
    // ---------------------------------------------------------- Public data    
    public:

        ResponseCommand();
        ~ResponseCommand();
        
        /**
         * Creates a new ResponseCommand object.
         *
         * @param cmdID the command idendifier  - NOT NULL
         * @param msgRef message reference
         * @param cmdRef command reference - NOT NULL
         * @param targetRefs target references
         * @param sourceRefs source references
         * @param items command items
         * if any of the NOT NULL parameter is null
         */
        ResponseCommand(
                CmdID*              cmdID     ,
                wchar_t*            msgRef    ,
                wchar_t*            cmdRef    ,
                ArrayList*          targetRefs,
                ArrayList*          sourceRefs,
                ArrayList*          items      );
            
    
        /**
         * Returns the message reference
         *
         * @return the message reference
         *
         */
        wchar_t* getMsgRef(wchar_t* retGetMsgRef);
    
        /**
         * Sets the message reference
         *
         * @param msgRef message reference
         */
        void setMsgRef(wchar_t* msgRef);
    
        /**
         * Returns the command reference
         *
         * @return the command reference
         *
         */
        wchar_t* getCmdRef(wchar_t* retGetCmdRef);
    
        /**
         * Sets the command reference
         *
         * @param cmdRef commandreference - NOT NULL
         *
         */
        void setCmdRef(wchar_t* cmdRef);
    
        /**
         * Returns the target references
         *
         * @return the target references
         *
         */
        ArrayList* getTargetRef();
    
        /**
         * Sets the target references
         *
         * @param targetRefs target refrences
         */
        void setTargetRef(ArrayList* targetRefs);
    
        /**
         * Returns the source references
         *
         * @return the source references
         *
         */
        ArrayList* getSourceRef();
    
        /**
         * Sets the source references
         *
         * @param sourceRefs source refrences
         */
        void setSourceRef(ArrayList* sourceRefs);
    
        /**
         * Returns the command name. It must be redefined by subclasses.
         *
         * @return the command name
         */
        virtual wchar_t* getName() = 0;

        virtual ArrayElement* clone() = 0;
   
};

#endif
