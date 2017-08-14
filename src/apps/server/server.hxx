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

#ifndef GRAIPE_SERVER_SERVER_HXX
#define GRAIPE_SERVER_SERVER_HXX

#include <QStringList>
#include <QTcpServer>
#include "core/workspace.hxx"

namespace graipe {

/** 
 * Brief struct for a connection info
 */
struct ConnectionInfo
{
    /** a unique socket descriptor **/
    qintptr socketDescriptor;
    /** the user name, who logged in on this socket **/
    QString user;
};

/**
 * Implementation of a threaded server, who fulfilles graipe requests. Each user's
 * request will be served in its own (new) thread.
 */
class Server
:   public QTcpServer
{
    Q_OBJECT

public:
    /**
     * Starts a new server on a given GRAIPE workspace
     *
     * \param wsp The workspace of this server
     * \param parent Pointer to the parent object, defaults to NULL
     */
    Server(Workspace* wsp, QObject *parent = NULL);
    
    /**
     * Get statistics of the server.
     *
     * \return all current connections by means of their socket ids and usernames
     */
    QVector<ConnectionInfo> connectionInfo() const;

public slots:
    /**
     * This slot is called on each user's authentification try.
     *
     * \param socketDescriptor the socket descriptor.
     * \param user the user, who is requesting access.
     */
    void connectionUserAuth(qintptr socketDescriptor, QString user);
    
    /**
     * This slot is called on each socket's termination request.
     *
     * \param socketDescriptor the socket descriptor, who will be terminated.
     */
    void connectionTerminated(qintptr socketDescriptor);

protected:
        
    /**
     * This function is called on each socket's incoming connection request.
     *
     * \param socketDescriptor the socket descriptor, who yield the incomming connection.
     */
    void incomingConnection(qintptr socketDescriptor) override;
    
signals:
    /** 
     * This signal is emitted everytime, the status changed of the server.
     */
    void statusChanged();

private:
    /** A list of all registered users **/
    QVector<QString> m_registered_users;
    
    /** A global mutex for mutual exclusive server operations **/
    QMutex m_global_mutex;
    
    /** The workspace of this server **/
    Workspace* m_workspace;
    
    /** All active connections **/
    QVector<ConnectionInfo> m_connections;
};

} //namespace graipe

#endif //GRAIPE_SERVER_SERVER_HXX
