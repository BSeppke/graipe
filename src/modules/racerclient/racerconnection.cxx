/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
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

#include "racerclient/racerconnection.hxx"

namespace graipe {

/**
 * Default constructor. Connects to localhost (127.0.0.1) on port 8088
 */
RacerConnection::RacerConnection()
:	QObject(),
	m_tcpSocket(new QTcpSocket),
	m_ipAddress("127.0.0.1"),
	m_port(8088)
{
	/*
	// find out which IP to connect to
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address())
        {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
	
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	
	
    if (!ipAddress.isEmpty())
	{
		m_ipAddress = ipAddress.toStdString();
		m_port = 8088;
	}
	*/
	
	connectActions();	
}

/**
 * Constructor, which sets up the connection to a Racer server using a
 * given IP address and port number.
 *
 * \param ipAddress The IP address of the Racer server.
 * \param port The IP port number
 */
RacerConnection::RacerConnection(QString ipAddress, int port)
:	QObject(),
m_tcpSocket(new QTcpSocket),
m_ipAddress(ipAddress),
m_port(port)
{
	connectActions();
}

/**
 * (Virtual) destructor of a Racer connection.
 */
RacerConnection::~RacerConnection()
{
	m_tcpSocket->close();
	delete m_tcpSocket;
}

/**
 * Getter for the IP address of the current Racer server.
 *
 * \return the IP address of the currently used Racer server
 */
const QString & RacerConnection::ipAddress() const
{
	return m_ipAddress;
}

/**
 * Sets the IP address to a new one. Please note this does not force any 
 * re-connection. You should call disconnectFromServer and connectToServer
 * manually afterwards.
 * 
 * \param ipAddress The new IP address.
 */
void RacerConnection::setIpAddress(const QString & ipAddress)
{
	m_ipAddress = ipAddress;
}

/**
 * Getter for the IP port of the current Racer server.
 *
 * \return the IP port of the currently used Racer server
 */
int RacerConnection::port() const
{
	return m_port;
}

/**
 * Sets the IP port to a new one. Please note this does not force any
 * re-connection. You should call disconnectFromServer and connectToServer
 * manually afterwards.
 * 
 * \param port The new IP port.
 */
void RacerConnection::setPort(int port)
{
	m_port = port;
}

/**
 * Returns true, if there is no connection to the Racer server.
 * \return True, if there is no connection to the Racer server.
 */
bool RacerConnection::disconnected() const
{
	return (m_tcpSocket->state() & QAbstractSocket::UnconnectedState) || racerVersion().isEmpty();
}

/**
 * Returns true, if there is a connection to the Racer server.
 * \return True, if there is a connection to the Racer server.
 */
bool RacerConnection::connected() const
{
	return (m_tcpSocket->state() & QAbstractSocket::ConnectedState) && !racerVersion().isEmpty() ;
}

/**
 * Initializes a connection to the Racer server.
 * 
 * \param msecs Timout for the connection. Defaults to 30.000 msecs (= 30s).
 */
void RacerConnection::connectToServer(int msecs)
{
	m_tcpSocket->connectToHost(m_ipAddress, m_port);
	if (m_tcpSocket->waitForConnected(msecs))
	{
		qDebug("Connected!");
		m_racerVersion = send("(get-racer-version)");
	}
	else
	{
		qDebug("Connection failed!");
	}
}

/**
 * Closes a connection to the Racer server.
 * 
 * \param msecs Timout for the disconnection. Defaults to 30.000 msecs (= 30s).
 */
void RacerConnection::disconnectFromServer(int msecs)
{
	m_tcpSocket->disconnectFromHost();
	if (	m_tcpSocket->state() == QAbstractSocket::UnconnectedState
		||	m_tcpSocket->waitForDisconnected(msecs))
	{
		qDebug("Disconnected!");
	}
	else
	{
		qDebug("Disconnection failed!");
	}
}

/**
 * Sends a request to the Racer server, wait for a timeout and return the result
 * of the sent request.
 *
 * \param request The request string.
 * \param msecs Timout for the disconnection. Defaults to 30.000 msecs (= 30s).
 * \return the resulting string as got from the server.
 */
QString RacerConnection::send(const QString& request,int msecs)
{
	
	//qDebug() << QString("Sent Data") << qRequest;
	
	
	//Remove whitespaces
	QString request_simplified = request.simplified();
	if( request_simplified.isEmpty() )
	{
		//m_log.addToLog("Unable to send empty string to RACER","Error");
	}
	else
	{
		QByteArray array = request_simplified.toUtf8().data();
		
		long len =  m_tcpSocket->write(array);
		
		if (len != array.size())
			qDebug("Wrote less than I should...");

		//m_log.addToLog(QString("%1\n was send (%2/%3 bytes)").arg(request_simplified).arg(len).arg(array.size()),"Request");
		
		m_tcpSocket->write("\n");
		if (m_tcpSocket->waitForBytesWritten(msecs))
		{
			if(m_tcpSocket->waitForReadyRead(msecs))
			{
				QByteArray array = m_tcpSocket->readAll();
				
				//qDebug() << QString("Received Data");
				
				//try to split answers
				QStringList lst = QString(array).split(" ");
				if(lst.size()>= 2 && lst[0] == ":ok")
				{
					//qDebug("Got an ok");
					
					QString result = lst[2];
					for (int i=3; i<lst.size(); ++i)
					{
						result += " " + lst[i];
					}
					return result;
				}
				if(lst.size()>= 2 && lst[0] == ":error")
				{
					qDebug("Got an error");
					
					QString result = lst[2];
					for (int i=3; i<lst.size(); ++i)
					{
						result += " " + lst[i];
					}
					return result;
				}
				if(lst.size()>=2 && lst[0] == ":answer")
				{
					qDebug("Got an answer");
					
					//unescape strings (seems to be neccessary sometimes...
					QString s = lst[2];
					s.replace("\\\"","");
					if( s != lst[2])
						
						//Remove outer "-marks
						s = s.replace("\"","");
					
					return s;
				}
				else {
					qDebug() << "Got a strange result:" << QString(array);
					
				}

			}
			else 
			{
				qDebug("Timeout: Reading bytes from Racer not successful!");
			}
			
		}
		else 
		{
			qDebug("Timeout: Sending bytes to Racer not successful!");
		}

	}
	return "";
}

/**
 * If successfully connected, this returns the Racer version, which the
 * server is currently running.
 */
const QString & RacerConnection::racerVersion() const
{
	return m_racerVersion;
}

/**
 * This slot will be called by tcp Socket everytime an error occured.
 *
 * \param error The socket error.
 */
void RacerConnection::socketError(QAbstractSocket::SocketError error)
{
	switch (error) {
		case QAbstractSocket::RemoteHostClosedError:
			break;
			
		case QAbstractSocket::HostNotFoundError:
			//"The host was not found."
			//"Please check the host name and port settings."
			break;
			
		case QAbstractSocket::ConnectionRefusedError:
			//"The connection was refused by the peer. "
			//"Make sure the RACER server is running, "
			//"and check that the host name and port "
			//"settings are correct."
			break;
			
		default:
			break;
			//m_tcpSocket->errorString()
    }	
}

/**
 * Internally connects the error-actions of the TCP socket the the
 * functions of this class.
 */
void RacerConnection::connectActions()
{
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
	
}


} //end of namespace graipe