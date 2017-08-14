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

#include "server.hxx"
#include "workerthread.hxx"
#include "core/core.h"

#include <QDir>
#include <QCryptographicHash>

#include <stdlib.h>

namespace graipe {

Server::Server(Workspace* wsp, QObject *parent)
    : QTcpServer(parent),
    m_workspace(wsp)
{
    qDebug()    << "Server knows factories: models " << m_workspace->modelFactory().size()
                << ", ViewControllers: " << m_workspace->viewControllerFactory().size()
                << ", algorithms: " << m_workspace->algorithmFactory().size();
    
    QString user1 = "test";
    QString pass1 = "test";
    
    QString user2 = "user";
    QString pass2 = "password";
    
    m_registered_users.push_back(user1 + ":" + QCryptographicHash::hash(pass1.toLatin1(), QCryptographicHash::Algorithm::Md5).toHex());
    m_registered_users.push_back(user2 + ":" + QCryptographicHash::hash(pass2.toLatin1(), QCryptographicHash::Algorithm::Md5).toHex());
}

QVector<ConnectionInfo> Server::connectionInfo() const
{
    return m_connections;
}

void Server::connectionUserAuth(qintptr socketDescriptor, QString user)
{
    for(unsigned int i=0; i!=m_connections.size(); ++i)
    {
        if(m_connections[i].socketDescriptor == socketDescriptor)
        {
            m_connections[i].user = user;
            emit statusChanged();
            break;
        }
    }
    
}

void Server::connectionTerminated(qintptr socketDescriptor)
{
    for(unsigned int i=0; i!=m_connections.size(); ++i)
    {
        if(m_connections[i].socketDescriptor == socketDescriptor)
        {
            m_connections.remove(i);
            emit statusChanged();
            break;
        }
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New incoming connection for socket:" << socketDescriptor;
    
    WorkerThread *thread = new WorkerThread(socketDescriptor, m_registered_users, m_workspace, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(connectionUserAuth(qintptr, QString)), this, SLOT(connectionUserAuth(qintptr, QString)));
    connect(thread, SIGNAL(connectionTerminated(qintptr)), this, SLOT(connectionTerminated(qintptr)));
    
    connect(this, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
    
    ConnectionInfo ci;
    ci.socketDescriptor = socketDescriptor;
    m_connections.push_back(ci);
    emit statusChanged();
    
    thread->start();
}


} //namespace graipe
