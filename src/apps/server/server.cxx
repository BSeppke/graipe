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

#include <stdlib.h>

namespace graipe {

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
    qDebug() << "Server knows factories: models " << modelFactory.size() << ", ViewControllers: " << viewControllerFactory.size() << ", algorithms: " << algorithmFactory.size();
    
    m_registered_users.push_back("test:test");
    m_registered_users.push_back("user:password");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug("New incoming connection");
    
    QString user;
    
    for(unsigned int i=0; i!=m_connected_sockets.size(); ++i)
    {
        //This socket has already registered itself as a valid user
        if(m_connected_sockets[i] == socketDescriptor)
        {
            user = m_connected_usernames[i];
            qDebug() << "User:" << user << "Socket:"<< socketDescriptor;
        }
    }
    WorkerThread *thread = new WorkerThread(socketDescriptor, user, this);
    connect(thread, SIGNAL(userRegistered(qintptr, QString, QString)), this, SLOT(registerSocket(qintptr, QString, QString)));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void Server::registerSocket(qintptr socketDescriptor, QString username, QString password)
{
    QString user = username + ":" + password;
    if(m_registered_users.contains(user))
    {
        qDebug() << "Login for user " << username << " complete";
        m_connected_sockets.push_back(socketDescriptor);
        m_connected_usernames.push_back(username);
    }
    else
    {
        qWarning() << "Attempt of illegal user login! User: " << username << ", Password: " << password;
    }
}

} //namespace graipe
