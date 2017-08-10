/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the GrAphical Image Processing Enviroment.   */
/*    The GRAIPE Website may be found at:                               */
/*        https://github.com/bseppke/graipe                             */
/*    Please direct questions, bug reports, and contributions to        */
/*    the GitHub page and use the methods provided there.               */
/*                                                                      */
/*    Permission is hereby granted, free of charge, to any person       */
/*    obtaining a copy of this software and associated documentation    */
/*    files (the "Software"), to deal in the Software without           */
/*    restriction, including without limitation the rights to use,      */
/*    copy, modify, merge, publish, distribute, sublicense, and/or      */
/*    sell copies of the Software, and to permit persons to whom the    */
/*    Software is furnished to do so, subject to the following          */
/*    conditions:                                                       */
/*                                                                      */
/*    The above copyright notice and this permission notice shall be    */
/*    included in all copies or substantial portions of the             */
/*    Software.                                                         */
/*                                                                      */
/*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND    */
/*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES   */
/*    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND          */
/*    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT       */
/*    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,      */
/*    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      */
/*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR     */
/*    OTHER DEALINGS IN THE SOFTWARE.                                   */
/*                                                                      */
/************************************************************************/

#ifndef GRAIPE_RACERCLIENT_RACERCONNECTION_HXX
#define GRAIPE_RACERCLIENT_RACERCONNECTION_HXX

#include "racerclient/config.hxx"

#include <QObject>
#include <QtNetwork>

namespace graipe {

class GRAIPE_RACERCLIENT_EXPORT RacerConnection
:   public QObject
{
	Q_OBJECT
    
    public:
        /**
         * Default constructor. Connects to localhost (127.0.0.1) on port 8088
         */
        RacerConnection();
    
        /**
         * Constructor, which sets up the connection to a Racer server using a
         * given IP address and port number.
         *
         * \param ipAddress The IP address of the Racer server.
         * \param port The IP port number
         */
        RacerConnection(QString ipAddress, int port);
    
        /** 
         * (Virtual) destructor of a Racer connection.
         */
        ~RacerConnection();
    
        /**
         * Getter for the IP address of the current Racer server.
         *
         * \return the IP address of the currently used Racer server
         */
        const QString & ipAddress() const;
    
        /**
         * Sets the IP address to a new one. Please note this does not force any 
         * re-connection. You should call disconnectFromServer and connectToServer
         * manually afterwards.
         * 
         * \param ipAddress The new IP address.
         */
        void setIpAddress(const QString & ipAddress);
    
        /**
         * Getter for the IP port of the current Racer server.
         *
         * \return the IP port of the currently used Racer server
         */
        int port() const;
        
        /**
         * Sets the IP port to a new one. Please note this does not force any
         * re-connection. You should call disconnectFromServer and connectToServer
         * manually afterwards.
         * 
         * \param port The new IP port.
         */
        void setPort(int port);
    
        /** 
         * Returns true, if there is no connection to the Racer server.
         * \return True, if there is no connection to the Racer server.
         */
        bool disconnected() const;
    
        /** 
         * Returns true, if there is a connection to the Racer server.
         * \return True, if there is a connection to the Racer server.
         */
        bool connected() const;
    
        /**
         * Initializes a connection to the Racer server.
         * 
         * \param msecs Timout for the connection. Defaults to 30.000 msecs (= 30s).
         */
        void connectToServer(int msecs=30000);
    
        /**
         * Closes a connection to the Racer server.
         * 
         * \param msecs Timout for the disconnection. Defaults to 30.000 msecs (= 30s).
         */
        void disconnectFromServer(int msecs=30000);
    
        /**
         * If successfully connected, this returns the Racer version, which the
         * server is currently running.
         */
        const QString & racerVersion() const;

        /**
         * Sends a request to the Racer server, wait for a timeout and return the result
         * of the sent request.
         *
         * \param request The request string.
         * \param msecs Timout for the disconnection. Defaults to 30.000 msecs (= 30s).
         * \return the resulting string as got from the server.
         */
        QString send(const QString& request, int msecs=30000);

    protected slots:
        /**
         * This slot will be called by tcp Socket everytime an error occured.
         * 
         * \param error The socket error.
         */
        void socketError(QAbstractSocket::SocketError error);

    protected:
        /**
         * Internally connects the error-actions of the TCP socket the the
         * functions of this class.
         */
        void connectActions();
    
        //Member variables:
        qint16             m_blockSize;
        QTcpSocket *       m_tcpSocket;
        QString m_ipAddress, m_racerVersion;
        int m_port;

};

} //end of namespace graipe

#endif //GRAIPE_RACERCLIENT_RACERCONNECTION_HXX
