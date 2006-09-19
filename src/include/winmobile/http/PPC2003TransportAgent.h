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


/**

 How to test SSL connections on WindowsMobile 2003 device/emulator
 -----------------------------------------------------------------

 On the server:
 1) create the keystore:
    %JAVA_HOME%\bin\keytool -genkey -alias tomcat -keyalg RSA
 2) In $CATALINA_HOME/conf/server.xml uncomment the lines:
    <Connector className="org.apache.catalina.connector.http.HttpConnector"
               port="8443" minProcessors="5" maxProcessors="75"
               enableLookups="true"
               acceptCount="10" debug="0" scheme="https" secure="true">
      <Factory className="org.apache.catalina.net.SSLServerSocketFactory" clientAuth="false" protocol="TLS"/>
    </Connector>
 2) Export the certificate from the key store:
    %JAVA_HOME%\bin\keytool -export -alias tomcat -file myroot.cer  

 On the emulator:
 1) Copy myroot.cer in a device/emulator directory
 2) Click on it to import the certificate as a trusted CA

**/

#ifndef INCL_PPC2003_TRANSPORT_AGENT
    #define INCL_PPC2003_TRANSPORT_AGENT

    #include "base/fscapi.h"

    #include "http/URL.h"
    #include "http/Proxy.h"
    #include "http/TransportAgent.h"


    /*
     * This class is the transport agent responsible for messages exchange
     * over an HTTP connection.
     * This is a generic abtract class which is not bound to any paltform
     */

    class PPC2003TransportAgent : public TransportAgent {


    public:
        PPC2003TransportAgent();
        PPC2003TransportAgent(URL& url, Proxy& proxy, unsigned int responseTimeout = DEFAULT_MAX_TIMEOUT, unsigned int maxmsgsize = DEFAULT_MAX_MSG_SIZE);
        ~PPC2003TransportAgent();

        /*
         * Sends the given SyncML message to the server specified
         * by the install property 'url'. Returns the server's response.
         * The response string has to be freed with delete [].
         * In case of an error, NULL is returned and lastErrorCode/Msg
         * is set.
         */
        char*  sendMessage(const char*  msg);
    };

#endif