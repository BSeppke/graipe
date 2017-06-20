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

class WorkerThread
:   public QThread
{
    Q_OBJECT

    public:
        WorkerThread(qintptr socketDescriptor, QVector<QString> registered_users, Environment* env, QObject *parent);
        void run() override;

    protected slots:
        void readyRead();
        void disconnected();
   
    protected:
        void readModel();
        void readAndRunAlgorithm();

    signals:
        void error(QTcpSocket::SocketError socketError);

    private:
        int m_socketDescriptor;
        QTcpSocket* m_tcpSocket;
    
        QVector<QString> m_registered_users;
    
        int m_state; // -1 : no logged in,
                     //  0 : logged in,
                     //  1 : busy (waiting for data to be received)
        int m_expected_bytes; //in case state==1 - how many bytes do we want to receive
        QByteArray m_buffer;
    
        Environment * m_environment;
};

} //namespace graipe

#endif
