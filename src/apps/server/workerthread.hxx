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

#ifndef GRAIPE_SERVER_WORKERTHREAD_HXX
#define GRAIPE_SERVER_WORKERTHREAD_HXX

#include "core/model.hxx"

#include <QThread>
#include <QTcpSocket>
#include <QByteArray>
#include <QVector>

namespace graipe {

/**
 * This class defines a working task of the server. Each user will get it's 
 * own WorkerThread started by the server.
 */
class WorkerThread
:   public QThread
{
    Q_OBJECT

    public:
        /** 
         * Creates a new worker thread for the server.
         *
         * \param socketDescriptor The unique socketDescriptor of the client
         * \param registered_users A list of all registered users
         * \param wsp              The workspace of this client
         * \param parent           A pointer to the parent. Here: the server.
         */
        WorkerThread(qintptr socketDescriptor, QVector<QString> registered_users, Workspace* wsp, QObject *parent);
    
        /**
         * Running phase of the thread
         */
        void run() override;

    protected slots:
        /**
         * This slot will be called, if there are bytes available to read on the socket.
         */
        void readyRead();
    
        /** 
         * This slot will be called on disconnection of the client.
         */
        void disconnected();
   
    protected:
        /** 
         * Function to read a model.
         */
        void readModel();
        /**
         * Funciton to read and run an algorithm.
         */
        void readAndRunAlgorithm();

    signals:
        /**
         * This signal is emitted on every socket error.
         */
        void error(QTcpSocket::SocketError socketError);
        /**
         * This signal is emitted on every successful user authentification.
         */
        void connectionUserAuth(qintptr socketDescriptor, QString user);
        /**
         * This signal is emitted on every user termination.
         */
        void connectionTerminated(qintptr socketDescriptor);

    private:
        /** The socket descriptor of the client **/
        int m_socketDescriptor;
    
        /** The socket itself **/
        QTcpSocket* m_tcpSocket;
    
        /** The registered users' list **/
        QVector<QString> m_registered_users;
    
        /**
         * Coding of the state model of this class:
         * \verbatim
           -1 : no logged in, \n
            0 : logged in,\n
            1 : busy (waiting for data to be received)
           \endverbatim
         */
        int m_state;
    
        /** If state==1 - how many bytes do we want to receive **/
        int m_expected_bytes;
    
        /** Buffer for data exchange **/
        QByteArray m_buffer;
    
        /** The workspace of this thread **/
        Workspace * m_workspace;
};

} //namespace graipe

#endif //GRAIPE_SERVER_WORKERTHREAD_HXX
