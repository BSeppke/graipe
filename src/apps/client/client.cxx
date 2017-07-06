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

#include <QtWidgets>
#include <QtNetwork>
#include <QCryptographicHash>

#include "client.hxx"

#include "core/core.h"

namespace graipe {

Client::Client(QWidget *parent)
:   QMainWindow(parent),
    m_cmbHost(new QComboBox),
    m_lnePort(new QLineEdit),
    m_lneUser(new QLineEdit),
    m_lnePassword(new QLineEdit),
    m_btnLogin(new QPushButton(tr("Login"))),
    m_tcpSocket(new QTcpSocket(this)),
    m_algSignalMapper(new QSignalMapper),
    m_workspace(new Workspace)
{
    m_workspace->loadModel("/Users/seppke/Desktop/Lenna_face.xgz");
    
    menuBar();
    
    QMenu* mnuFile = menuBar()->addMenu("File");

    QMenu* mnuImport = mnuFile->addMenu("Import");
    QMenu* mnuExport = mnuFile->addMenu("Export");
    
    QMenu* mnuAlgs = menuBar()->addMenu("Algorithms");
    
    
    
    
    
    
    //Connect the algorithm factory to the GUI
	QList<QMenu*> added_menus;
    unsigned int i=0;
    for(const AlgorithmFactoryItem& item : m_workspace->algorithmFactory())
    {
        QAction* newAct = new QAction(item.algorithm_name, this);
        
        if (item.topic_name == "Import")
        {
            mnuImport->addAction(newAct);
        }
        else if (item.topic_name == "Export")
        {
            mnuExport->addAction(newAct);
        }
        else
        {
            //add Menu-entry "topic_name"
            QMenu* algorithm_menu = NULL;
            for(QMenu* m : added_menus)
            {
                if (m->title() == item.topic_name)
                {
                    algorithm_menu = m;
                    break;
                }
            }
            if (!algorithm_menu){
                algorithm_menu = new QMenu(item.topic_name);
                mnuAlgs->addMenu(algorithm_menu);

                added_menus.push_back(algorithm_menu);
            }
            algorithm_menu->addAction(newAct);
        }
        //connect everything
        connect(newAct, SIGNAL(triggered()), m_algSignalMapper, SLOT(map()));
        m_algSignalMapper->setMapping(newAct, i++);
    }
    
	connect(m_algSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(clickedAlgorithm(int)));
	connect(this, SIGNAL(clickedAlgorithm(int)), this, SLOT(runAlgorithm(int)));








    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    m_cmbHost->setEditable(true);
    // find out name of this machine
    QString name = QHostInfo::localHostName();
    if (!name.isEmpty()) {
        m_cmbHost->addItem(name);
        QString domain = QHostInfo::localDomainName();
        if (!domain.isEmpty())
            m_cmbHost->addItem(name + QChar('.') + domain);
    }
    if (name != QLatin1String("localhost"))
        m_cmbHost->addItem(QString("localhost"));
    // find out IP addresses of this machine
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // add non-localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (!ipAddressesList.at(i).isLoopback())
            m_cmbHost->addItem(ipAddressesList.at(i).toString());
    }
    // add localhost addresses
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i).isLoopback())
            m_cmbHost->addItem(ipAddressesList.at(i).toString());
    }

    m_lnePort->setValidator(new QIntValidator(1, 65535, this));

    QLabel *hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(m_cmbHost);
    QLabel *portLabel = new QLabel(tr("S&erver port:"));
    portLabel->setBuddy(m_lnePort);
    
    QLabel *userLabel = new QLabel(tr("&User:"));
    userLabel->setBuddy(m_lneUser);
    QLabel *passwordLabel = new QLabel(tr("&Password:"));
    passwordLabel->setBuddy(m_lnePassword);

    m_lblStatus = new QLabel(tr("This examples requires that you run the "
                                "Graipe Server example as well."));
    QPushButton *quitButton = new QPushButton(tr("Quit"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(m_btnLogin, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(m_btnLogin, &QAbstractButton::clicked, this, &Client::registerAtServer);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);
    connect(m_tcpSocket, &QIODevice::readyRead, this, &Client::readHandler);
    
    typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(m_tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error),
            this, &Client::displayError);
    
    this->setCentralWidget(new QWidget);

    QGridLayout *mainLayout = new QGridLayout(this->centralWidget());

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(m_cmbHost, 0, 1);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(m_lnePort, 1, 1);
    mainLayout->addWidget(userLabel, 2, 0);
    mainLayout->addWidget(m_lneUser, 2, 1);
    mainLayout->addWidget(passwordLabel, 3, 0);
    mainLayout->addWidget(m_lnePassword, 3, 1);
    mainLayout->addWidget(m_lblStatus, 4, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 5, 0, 1, 2);
    
    setWindowTitle(QGuiApplication::applicationDisplayName());
    m_lnePort->setFocus();

    m_lblStatus->setText(tr("This client requires that you run the "
                            "GraipeServer as well."));
}

void Client::sendModel(Model* model)
{
    QByteArray model_data;
    QBuffer out_buf(&model_data);
    
    //Always use compressed transfer
    QIOCompressor* compressor = new QIOCompressor(&out_buf);
    compressor->setStreamFormat(QIOCompressor::GzipFormat);

    if (!compressor->open(QIODevice::WriteOnly))
    {
        qWarning("Did not open compressor (gz) on tcpSocket");
        throw "Error";
    }
    
    QXmlStreamWriter xmlWriter(compressor);
    model->setID(QString::number((long int)model));
    model->serialize(xmlWriter);
    compressor->close();
    
    QString request1 = QString("Model:%1\n").arg(model_data.size());
    
    qDebug() << "--> " << request1;
    m_tcpSocket->write(request1.toLatin1());
    m_tcpSocket->flush();
    m_tcpSocket->waitForBytesWritten();
    
    while( m_tcpSocket->bytesToWrite() != 0)
    {
        QCoreApplication::processEvents();
    }
    
    qDebug() << "--> \"Compressed Model Data\"";
    m_tcpSocket->write(model_data);
    m_tcpSocket->flush();
    m_tcpSocket->waitForBytesWritten();
    
    while( m_tcpSocket->bytesToWrite() != 0)
    {
        QCoreApplication::processEvents();
    }
    
    m_tcpSocket->waitForReadyRead();
}

void Client::registerAtServer()
{
    m_btnLogin->setEnabled(false);
    
    if(m_btnLogin->text() == "Logout")
    {
        m_tcpSocket->abort();
        m_btnLogin->setText("Login");
        m_lblStatus->setText("Successfully logged out!");
    }
    else
    {
        m_tcpSocket->connectToHost( m_cmbHost->currentText(),
                                    m_lnePort->text().toInt());
        
        m_tcpSocket->waitForConnected();
        
        QString account = m_lneUser->text() + ":" + QCryptographicHash::hash(m_lnePassword->text().toLatin1(), QCryptographicHash::Algorithm::Md5).toHex();
        QString request1("Login:" + account + "\r\n");
        
        qDebug() << "--> " << request1;
        m_tcpSocket->write(request1.toLatin1());
        m_tcpSocket->flush();
        m_tcpSocket->waitForBytesWritten();
    }
    
    m_btnLogin->setEnabled(true);
}

void Client::sendAlgorithm(Algorithm* alg)
{
    QByteArray alg_data;
    QBuffer out_buf(&alg_data);
    
    //Always use compressed transfer
    QIOCompressor* compressor = new QIOCompressor(&out_buf);
    compressor->setStreamFormat(QIOCompressor::GzipFormat);

    if (!compressor->open(QIODevice::WriteOnly))
    {
        qWarning("Did not open compressor (gz) on tcpSocket");
        throw "Error";
    }
    
    QXmlStreamWriter xmlWriter(compressor);
    alg->serialize(xmlWriter);
    compressor->close();
    
    QString request1 = QString("Algorithm:%1\r\n").arg(alg_data.size());
    
    qDebug() << "--> " << request1;
    m_tcpSocket->write(request1.toLatin1());
    m_tcpSocket->flush();
    m_tcpSocket->waitForBytesWritten();
    
    while( m_tcpSocket->bytesToWrite() != 0)
    {
        QCoreApplication::processEvents();
    }
    
    qDebug() << "--> \"Compressed Algorithm Data\"";
    m_tcpSocket->write(alg_data);
    m_tcpSocket->flush();
    m_tcpSocket->waitForBytesWritten();
    
    while( m_tcpSocket->bytesToWrite() != 0)
    {
        QCoreApplication::processEvents();
    }
}

void Client::readModel(int bytesToRead)
{
    try
    {
        QByteArray model_data;
        
        //Second: Receive all the data for the model
        m_tcpSocket->waitForReadyRead();
        
        while(m_tcpSocket->bytesAvailable() && model_data.size() < bytesToRead)
        {
            model_data.append(m_tcpSocket->readAll());
            if(model_data.size() == bytesToRead)
            {
                break;
            }
            else
            {
                m_tcpSocket->waitForReadyRead();
            }
        }
        
        if(model_data.size()!= bytesToRead)
        {
            qWarning() << "Did not receive the full model data, but only "<< model_data.size()  <<" of " << bytesToRead << "bytes";
            throw "Error";
        }
        
        qDebug() << "<-- \"Compressed Model data\".";
        QBuffer buf(&model_data);
        
        //Always use compressed transfer
        QIOCompressor* compressor = new QIOCompressor(&buf);
        compressor->setStreamFormat(QIOCompressor::GzipFormat);

        if (!compressor->open(QIODevice::ReadOnly))
        {
            qWarning("Did not open compressor (gz) on tcpSocket");
            throw "Error";
        }
        
        QXmlStreamReader xmlReader(compressor);
        Model* new_model = m_workspace->loadModel(xmlReader);
        
        if(new_model == NULL)
        {
            qWarning("Did not load a model over the tcpSocket");
            throw "Error";
            
        }
        
        qDebug("    Model loaded and added sucessfully!");
        qDebug() << "Now: " << m_workspace->models.size() << " models available!";
        
        m_lblStatus->setText(QString("Models: %1, latest model: %2, type:%3, ID:%4, descripton:%5").arg(m_workspace->models.size()).arg(new_model->name()).arg(new_model->typeName()).arg(new_model->id()).arg(new_model->description()));
    }
    catch(...)
    {
        qDebug("Error occured!");
    }
}

/**
 * Manages the reading of newly arrived data on the socket
 */
void Client::readHandler()
{
    QString data = QString::fromLatin1(m_tcpSocket->readLine());
    
    qDebug() << "<-- " << data << ".";
    
    QStringList data_split = data.split(":");
    
    if(data_split.size() != 2)
    {
        qWarning() << "Did not data in the right format. Expected MESSAGE_TYPE:SIZE, but got: " << data << ".";
    }
    else
    {
        QString message_type = data_split[0];

        if(message_type == "Model")
        {
            int bytesToRead = data_split[1].toInt();
            readModel(bytesToRead);
        }
        if(message_type == "Error")
        {
            int bytesToRead = data_split[1].toInt();
            m_lblStatus->setText(QString("Error number: %1 occured!").arg(bytesToRead));
        }
        if(message_type == "Sucess")
        {
            
            m_lblStatus->setText("Success!" + data_split[1]);
        }
        if(message_type == "Login" && data_split[1] =="OK")
        {
            m_lblStatus->setText(QString("Successfully logged in!"));
            m_btnLogin->setText("Logout");
        }
    }
}

void Client::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Graipe Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Graipe Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Graipe Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(m_tcpSocket->errorString()));
    }

    if (m_tcpSocket->state() == QTcpSocket::ConnectedState)
    {
        m_btnLogin->setText("Logout");
    }
    else
    {
        m_btnLogin->setText("Login");
    }
}

/**
 * This slot is called by all registered algorithms. The parameter
 * (int id) corresponds to the index at the algorithm_factory.
 *
 * \param index The index of the algorithm at the algorithm_factory.
 */
void Client::runAlgorithm(int index)
{
    using namespace ::std;
    
    AlgorithmFactoryItem alg_item = m_workspace->algorithmFactory()[index];
	
	Algorithm* alg = alg_item.algorithm_fptr(m_workspace);
    
	AlgorithmParameterSelection parameter_selection(this, alg);
	parameter_selection.setWindowTitle(alg_item.algorithm_name);
	parameter_selection.setModal(true);
    
	if(parameter_selection.exec())
	{
		//The result is true if and only if all parameters have been finalised
		//AND are available!
		if( parameter_selection.result()!=0 )
		{
			//Collect all models and transmit them:
            std::vector<Model*> neededModels = alg->parameters()->needsModels();
            
            for(Model* m : neededModels)
            {
                sendModel(m);
            }
            sendAlgorithm(alg);
		}
		else 
		{
			QMessageBox::critical(this, "Error in Algorithm run",
								  QString("Not all necessary parameters have been set!"));
		}
	}
    else
    {
        for(Model* m : alg->results())
        {
            delete m;
            m=NULL;
        }
        alg->results().clear();
    }
}
} //namespace graipe
