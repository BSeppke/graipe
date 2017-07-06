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

#include "workerthread.hxx"

#include "core/core.h"

#include <QtNetwork>
#include <QImage>
#include <QDir>

namespace graipe {

WorkerThread::WorkerThread(qintptr socketDescriptor, QVector<QString> registered_users, Workspace* wsp, QObject *parent)
:   QThread(parent),
    m_socketDescriptor(socketDescriptor),
    m_tcpSocket(NULL),
    m_registered_users(registered_users),
    m_state(-1),
    m_expected_bytes(0),
    m_workspace(new Workspace(*wsp))
{
    qDebug()    << "Server knows factories: models " << m_workspace->modelFactory().size()
                << ", ViewControllers: " << m_workspace->viewControllerFactory().size()
                << ", algorithms: " << m_workspace->algorithmFactory().size();
}

void WorkerThread::run()
{
    m_tcpSocket = new QTcpSocket;
    
    if (!m_tcpSocket->setSocketDescriptor(m_socketDescriptor))
    {
        emit error(m_tcpSocket->error());
        return;
    }
    
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));

    qDebug() << m_socketDescriptor << "--- connected";
    
    exec();
}

void WorkerThread::readyRead()
{
    qDebug()  << m_socketDescriptor << "--- state: " << m_state;
    
    if(m_state < 0)
    {
        QByteArray data = m_tcpSocket->readLine();
        qDebug() <<  m_socketDescriptor <<  "-->" << QString::fromLatin1(data);

        //Still waiting for login:
        QStringList split_data = QString::fromLatin1(data).trimmed().split(":");
        
        if( split_data.size() == 3 && split_data[0] == "Login")
        {
            QString account =  split_data[1] + ":" + split_data[2];
            
            if(m_registered_users.contains(account))
            {
                m_state = 0;
                qDebug() << m_socketDescriptor <<  "--- logged in unsing:" << account;
                
                //Tell the server
                emit connectionUserAuth(m_socketDescriptor, split_data[1]);
                
                //Tell the client:
                m_tcpSocket->write(QString("Login:OK").toLatin1());
                m_tcpSocket->flush();
                m_tcpSocket->waitForBytesWritten();
            }
        }
    }
    else if(m_state == 0)
    {
        QByteArray data = m_tcpSocket->readLine();
        qDebug() << m_socketDescriptor <<  "-->" << QString::fromLatin1(data);

        //Waiting for model or algorithm call
        QStringList split_data = QString::fromLatin1(data).trimmed().split(":");
        
        if(split_data.size() == 2)
        {
            if(split_data[0] == "Model")
            {
                m_state = 1;
                m_expected_bytes = split_data[1].toInt();
                m_buffer.clear();
                //Maybe some bytes already arrived?
                if(m_tcpSocket->bytesAvailable())
                {
                    readyRead();
                }
            }
            else if(split_data[0] == "Algorithm")
            {
                m_state = 2;
                m_expected_bytes = split_data[1].toInt();
                m_buffer.clear();
                //Maybe some bytes already arrived?
                if(m_tcpSocket->bytesAvailable())
                {
                    readyRead();
                }
            }
        }
    }
    else if(m_state == 1)
    {
        if(m_buffer.size() < m_expected_bytes)
        {
            m_buffer.append(m_tcpSocket->readAll());
        }
        
        if(m_buffer.size() == m_expected_bytes)
        {
            readModel();
            m_state = 0;
            m_expected_bytes = 0;
            m_buffer.clear();
        }
        else
        {
            qDebug()  << m_socketDescriptor << "--- Still waiting for more Model bytes";
        }
    }
    else if(m_state == 2)
    {
        if(m_buffer.size() < m_expected_bytes)
        {
            m_buffer.append(m_tcpSocket->readAll());
        }
        
        if(m_buffer.size() == m_expected_bytes)
        {
            readAndRunAlgorithm();
            m_state = 0;
            m_expected_bytes = 0;
            m_buffer.clear();
        }
        else
        {
            qDebug()  << m_socketDescriptor << "--- Still waiting for more Algorithm bytes";
        }
    }
}

void WorkerThread::disconnected()
{
    qDebug() << m_socketDescriptor << "--- disconnected";
    
    //Tell the server
    emit connectionTerminated(m_socketDescriptor);

    delete m_workspace;
    m_tcpSocket->deleteLater();
    exit(0);
}

void WorkerThread::readModel()
{
    try
    {
        QByteArray in_model_data = m_buffer;
        
        QBuffer in_buf(&in_model_data);
        
        //Always use compressed transfer
        QIOCompressor* in_compressor = new QIOCompressor(&in_buf);
        in_compressor->setStreamFormat(QIOCompressor::GzipFormat);

        if (!in_compressor->open(QIODevice::ReadOnly))
        {
            qWarning()  << m_socketDescriptor << "--- Did not open compressor (gz) on tcpSocket";
            throw "Error";
        }
        
        QXmlStreamReader xmlReader(in_compressor);
        
        m_workspace->global_algorithm_mutex.lock();
        Model* new_model = m_workspace->loadModel(xmlReader);
        m_workspace->global_algorithm_mutex.unlock();
        
        if(new_model == NULL)
        {
            qWarning() << m_socketDescriptor << "--- Did not load a model over the tcpSocket";
            throw "Error";
            
        }
        
        qDebug() << m_socketDescriptor << "--- Model loaded and added sucessfully!";
        qDebug() << m_socketDescriptor << "--- Now: " << m_workspace->models.size() << " models available!";
        
        m_tcpSocket->write(QString("Success:0").toLatin1());
        m_tcpSocket->flush();
        m_tcpSocket->waitForBytesWritten();
    }
    catch(...)
    {
        if(m_tcpSocket && m_tcpSocket->state() == QTcpSocket::ConnectedState)
        {
            m_tcpSocket->write(QString("Error:0").toLatin1());
            m_tcpSocket->flush();
            m_tcpSocket->waitForBytesWritten();
        }
    }
}

void WorkerThread::readAndRunAlgorithm()
{
    try
    {
        QByteArray in_alg_data = m_buffer;
        
        qDebug()  << m_socketDescriptor << "--> \"Algorithm data\".";
        QBuffer in_buf(&in_alg_data);
        
        //Always use compressed transfer
        QIOCompressor* in_compressor = new QIOCompressor(&in_buf);
        in_compressor->setStreamFormat(QIOCompressor::GzipFormat);

        if (!in_compressor->open(QIODevice::ReadOnly))
        {
            qWarning() << m_socketDescriptor << "--- Did not open compressor (gz) on tcpSocket";
            throw "Error";
        }
        
        QXmlStreamReader xmlReader(in_compressor);
        Algorithm* new_alg = m_workspace->loadAlgorithm(xmlReader);
        
        if(new_alg == NULL)
        {
            qWarning() << m_socketDescriptor << "--- Did not load a algorithm over the tcpSocket";
            throw "Error";
            
        }
        
        qDebug() << m_socketDescriptor << "--- Algorithm loaded sucessfully!";
        new_alg->run();
        qDebug() << m_socketDescriptor << "--- Algorithm ran sucessfully!";
        
        for(Model* model : new_alg->results())
        {
            QByteArray out_model_data;
            QBuffer out_buf(&out_model_data);
            
            //Always use compressed transfer
            QIOCompressor* out_compressor = new QIOCompressor(&out_buf);
            out_compressor->setStreamFormat(QIOCompressor::GzipFormat);

            if (!out_compressor->open(QIODevice::WriteOnly))
            {
                qWarning()  << m_socketDescriptor << "--- Did not open compressor (gz) on tcpSocket";
                throw "Error";
            }
            
            QXmlStreamWriter xmlWriter(out_compressor);
            model->serialize(xmlWriter);
            out_compressor->close();
            
            QString request = QString("Model:%1\n").arg(out_model_data.size());

            qDebug()  << m_socketDescriptor << "<-- " << request;
            //First: Write the type (Image) and the number of bytes (of the image) to the socket"
            m_tcpSocket->write(request.toLatin1());
            m_tcpSocket->flush();
            m_tcpSocket->waitForBytesWritten();
            
            qDebug()  << m_socketDescriptor << "<-- \"Model data\".";
            //Then submit the data of the model:
            m_tcpSocket->write(out_model_data);
            m_tcpSocket->flush();
            m_tcpSocket->waitForBytesWritten();
        }
    }
    catch(...)
    {
        if(m_tcpSocket && m_tcpSocket->state() == QTcpSocket::ConnectedState)
        {
            m_tcpSocket->write(QString("Error:0").toLatin1());
            m_tcpSocket->flush();
            m_tcpSocket->waitForBytesWritten();
        }
    }
}

} //namespace graipe
