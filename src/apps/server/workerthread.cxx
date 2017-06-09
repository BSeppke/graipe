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

WorkerThread::WorkerThread(int socketDescriptor, const QString &image_dir, QObject *parent)
:   QThread(parent),
    socketDescriptor(socketDescriptor)
    
{
}

void WorkerThread::run()
{
    try
    {
        tcpSocket = new QTcpSocket;
    
        tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        
        if (!tcpSocket->setSocketDescriptor(socketDescriptor))
        {
            emit error(tcpSocket->error());
            throw "Error";
        }
        
        //Wait for the request
        tcpSocket->waitForReadyRead();
        
        //Get the request
        QString data = QString::fromLatin1(tcpSocket->readLine());
        qDebug() << "-->" << data << ".";
        
        QStringList data_split = data.split(":");
        
        if(data_split.size() != 2)
        {
            qWarning() << "Did not get data in the right format. Expected TypeName:Size, but got: " << data << ".";
            throw "Error";
        }
        
        QString message_type = data_split[0];
        int bytesToRead = data_split[1].toInt();
        
        if(message_type == "Model")
        {
            readModel(bytesToRead);
        }
        else if(message_type == "Algorithm")
        {
            readAndRunAlgorithm(bytesToRead);
        }
    }
    catch (...)
    {
        QString errorString = "Error:0";
        
        //First: Write the type (Image) and the number of bytes (of the image) to the socket"
        tcpSocket->write(errorString.toLatin1());
        tcpSocket->waitForBytesWritten();
    }
    
    tcpSocket->close();
    
    //Finish by disconnect
    tcpSocket->disconnectFromHost();
    if(tcpSocket->state() != QTcpSocket::UnconnectedState)
    {
        tcpSocket->waitForDisconnected();
    }
    
}

void WorkerThread::readModel(int bytesToRead)
{
    QByteArray in_model_data;
    
    //Second: Receive all the data for the model
    tcpSocket->waitForReadyRead();
    
    while(tcpSocket->bytesAvailable() && in_model_data.size() < bytesToRead)
    {
        in_model_data.append(tcpSocket->readAll());
        if(in_model_data.size() == bytesToRead)
        {
            break;
        }
        else
        {
            tcpSocket->waitForReadyRead();
        }
    }
    
    if(in_model_data.size()!= bytesToRead)
    {
        qWarning() << "Did not receive the full model data, but only "<< in_model_data.size()  <<" of " << bytesToRead << "bytes";
        throw "Error";
    }
    
    qDebug() << "--> \"Model data\".";
    QBuffer in_buf(&in_model_data);
    
    //Always use compressed transfer
    QIOCompressor* in_compressor = new QIOCompressor(&in_buf);
    in_compressor->setStreamFormat(QIOCompressor::GzipFormat);

    if (!in_compressor->open(QIODevice::ReadOnly))
    {
        qWarning("Did not open compressor (gz) on tcpSocket");
        throw "Error";
    }
    
    QXmlStreamReader xmlReader(in_compressor);
    Model* new_model = Impex::loadModel(xmlReader);
    
    if(new_model == NULL)
    {
        qWarning("Did not load a model over the tcpSocket");
        throw "Error";
        
    }
    
    qDebug("    Model loaded and added sucessfully!");
    qDebug() << "Now: " << models.size() << " models available!";
    
    
    QByteArray out_model_data;
    QBuffer out_buf(&out_model_data);
    
    //Always use compressed transfer
    QIOCompressor* out_compressor = new QIOCompressor(&out_buf);
    out_compressor->setStreamFormat(QIOCompressor::GzipFormat);

    if (!out_compressor->open(QIODevice::WriteOnly))
    {
        qWarning("Did not open compressor (gz) on tcpSocket");
        throw "Error";
    }
    
    QXmlStreamWriter xmlWriter(out_compressor);
    new_model->setName("Served: " + new_model->name());
    new_model->serialize(xmlWriter);
    out_compressor->close();
    
    QString request = QString("Model:%1\n").arg(out_model_data.size());

    qDebug() << "<-- " << request;
    //First: Write the type (Image) and the number of bytes (of the image) to the socket"
    tcpSocket->write(request.toLatin1());
    tcpSocket->waitForBytesWritten();
    
    qDebug() << "<-- \"Model data\".";
    //Then submit the data of the model:
    tcpSocket->write(out_model_data);
    tcpSocket->waitForBytesWritten();

}

void WorkerThread::readAndRunAlgorithm(int bytesToRead)
{
    QByteArray in_alg_data;
    
    //Second: Receive all the data for the model
    tcpSocket->waitForReadyRead();
    
    while(tcpSocket->bytesAvailable() && in_alg_data.size() < bytesToRead)
    {
        in_alg_data.append(tcpSocket->readAll());
        if(in_alg_data.size() == bytesToRead)
        {
            break;
        }
        else
        {
            tcpSocket->waitForReadyRead();
        }
    }
    
    if(in_alg_data.size()!= bytesToRead)
    {
        qWarning() << "Did not receive the full algorithm data, but only "<< in_alg_data.size()  <<" of " << bytesToRead << "bytes";
        throw "Error";
    }
    
    qDebug() << "--> \"Algorithm data\".";
    QBuffer in_buf(&in_alg_data);
    
    //Always use compressed transfer
    QIOCompressor* in_compressor = new QIOCompressor(&in_buf);
    in_compressor->setStreamFormat(QIOCompressor::GzipFormat);

    if (!in_compressor->open(QIODevice::ReadOnly))
    {
        qWarning("Did not open compressor (gz) on tcpSocket");
        throw "Error";
    }
    
    QXmlStreamReader xmlReader(in_compressor);
    Algorithm* new_alg = Impex::loadAlgorithm(xmlReader);
    
    if(new_alg == NULL)
    {
        qWarning("Did not load a algorithm over the tcpSocket");
        throw "Error";
        
    }
    
    qDebug("    Algorithm loaded sucessfully!");
    new_alg->run();
    qDebug("    Algorithm ran sucessfully!");
    
    for(Model* model : new_alg->results())
    {
        QByteArray out_model_data;
        QBuffer out_buf(&out_model_data);
        
        //Always use compressed transfer
        QIOCompressor* out_compressor = new QIOCompressor(&out_buf);
        out_compressor->setStreamFormat(QIOCompressor::GzipFormat);

        if (!out_compressor->open(QIODevice::WriteOnly))
        {
            qWarning("Did not open compressor (gz) on tcpSocket");
            throw "Error";
        }
        
        QXmlStreamWriter xmlWriter(out_compressor);
        model->serialize(xmlWriter);
        out_compressor->close();
        
        QString request = QString("Model:%1\n").arg(out_model_data.size());

        qDebug() << "<-- " << request;
        //First: Write the type (Image) and the number of bytes (of the image) to the socket"
        tcpSocket->write(request.toLatin1());
        tcpSocket->waitForBytesWritten();
        
        qDebug() << "<-- \"Model data\".";
        //Then submit the data of the model:
        tcpSocket->write(out_model_data);
        tcpSocket->waitForBytesWritten();

    }
}

} //namespace graipe
