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

#include "client.hxx"

#include "core/core.h"

namespace graipe {

Client::Client(QWidget *parent)
:   QMainWindow(parent),
    m_cmbHost(new QComboBox),
    m_lnePort(new QLineEdit),
    m_lneRequest(new QLineEdit("/Users/seppke/Desktop/Lenna_face.xgz")),
    m_btnSend(new QPushButton(tr("Send Request"))),
    m_tcpSocket(new QTcpSocket(this)),
    m_networkSession(Q_NULLPTR),
    m_algSignalMapper(new QSignalMapper)
{
    //init graipe
    loadModules();
    Impex::loadModel(m_lneRequest->text());
    
    menuBar();
    
    QMenu* mnuFile = menuBar()->addMenu("File");

    QMenu* mnuImport = mnuFile->addMenu("Import");
    QMenu* mnuExport = mnuFile->addMenu("Export");
    
    QMenu* mnuAlgs = menuBar()->addMenu("Algorithms");
    
    //Connect the algorithm factory to the GUI
	QList<QMenu*> added_menus;
    unsigned int i=0;
    for(const AlgorithmFactoryItem& item : algorithmFactory)
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
    QLabel *requestLabel = new QLabel(tr("&Request:"));
    requestLabel->setBuddy(m_lneRequest);

    m_lblStatus = new QLabel(tr("This examples requires that you run the "
                                "Graipe Server example as well."));

    m_btnSend->setDefault(true);
    m_btnSend->setEnabled(false);

    QPushButton *quitButton = new QPushButton(tr("Quit"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(m_btnSend, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    connect(m_cmbHost, &QComboBox::editTextChanged,
            this, &Client::enableSendRequestButton);
    connect(m_lnePort, &QLineEdit::textChanged,
            this, &Client::enableSendRequestButton);
    connect(m_lneRequest, &QLineEdit::textChanged,
            this, &Client::enableSendRequestButton);
    connect(m_btnSend, &QAbstractButton::clicked,
            this, &Client::loadAndSendModel);
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
    mainLayout->addWidget(requestLabel, 2, 0);
    mainLayout->addWidget(m_lneRequest, 2, 1);
    mainLayout->addWidget(m_lblStatus, 3, 0, 1, 2);
    mainLayout->addWidget(buttonBox, 4, 0, 1, 2);
    
    setWindowTitle(QGuiApplication::applicationDisplayName());
    m_lnePort->setFocus();

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        m_networkSession = new QNetworkSession(config, this);
        connect(m_networkSession, &QNetworkSession::opened, this, &Client::sessionOpened);

        m_btnSend->setEnabled(false);
        m_lblStatus->setText(tr("Opening network session."));
        m_networkSession->open();
    }
}

void Client::loadAndSendModel()
{
    Model* model = Impex::loadModel(m_lneRequest->text());
    
    if (model != NULL)
    {
        sendModel(model);
    }
}

void Client::sendModel(Model* model)
{
    m_btnSend->setEnabled(false);
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost( m_cmbHost->currentText(),
                                m_lnePort->text().toInt());
    
    m_tcpSocket->waitForConnected();

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
    model->serialize(xmlWriter);
    compressor->close();
    delete model;
    
    QString request1 = QString("Model:%1\n").arg(model_data.size());
    
    qDebug() << "--> " << request1;
    m_tcpSocket->write(request1.toLatin1());
    m_tcpSocket->waitForBytesWritten();
    m_tcpSocket->flush();
    
    
    qDebug() << "--> \"Compressed Model Data\"";
    m_tcpSocket->write(model_data);
    m_tcpSocket->waitForBytesWritten();
    m_tcpSocket->flush();
    
    m_btnSend->setEnabled(true);
}
void Client::sendAlgorithm(Algorithm* alg)
{
    m_btnSend->setEnabled(false);
    m_tcpSocket->abort();
    m_tcpSocket->connectToHost( m_cmbHost->currentText(),
                                m_lnePort->text().toInt());
    
    m_tcpSocket->waitForConnected();

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
    
    QString request1 = QString("Algorithm:%1\n").arg(alg_data.size());
    
    qDebug() << "--> " << request1;
    m_tcpSocket->write(request1.toLatin1());
    m_tcpSocket->waitForBytesWritten();
    m_tcpSocket->flush();
    
    
    qDebug() << "--> \"Compressed Algorithm Data\"";
    m_tcpSocket->write(alg_data);
    m_tcpSocket->waitForBytesWritten();
    m_tcpSocket->flush();
    
    m_btnSend->setEnabled(true);
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
        Model* new_model = Impex::loadModel(xmlReader);
        
        if(new_model == NULL)
        {
            qWarning("Did not load a model over the tcpSocket");
            throw "Error";
            
        }
        
        qDebug("    Model loaded and added sucessfully!");
        qDebug() << "Now: " << models.size() << " models available!";
        
        m_lblStatus->setText(QString("Models: %1, latest model: %2, type:%3, ID:%4, descripton:%5").arg(models.size()).arg(new_model->name()).arg(new_model->typeName()).arg(new_model->id()).arg(new_model->description()));
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
    m_btnSend->setEnabled(false);
    
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
        int bytesToRead = data_split[1].toInt();

        if(message_type == "Model")
        {
            readModel(bytesToRead);
        }
        if(message_type == "Error")
        {
            m_lblStatus->setText(QString("Error number: %1 occured!").arg(bytesToRead));
        }
    }
    m_btnSend->setEnabled(true);
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

    m_btnSend->setEnabled(true);
}

void Client::enableSendRequestButton()
{
    m_btnSend->setEnabled((!m_networkSession || m_networkSession->isOpen()) &&
                                 !m_cmbHost->currentText().isEmpty() &&
                                 !m_lnePort->text().isEmpty());

}

void Client::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = m_networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice)
        id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    else
        id = config.identifier();

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();

    m_lblStatus->setText(tr("This examples requires that you run the "
                            "Image Server example as well."));

    enableSendRequestButton();
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
    
    AlgorithmFactoryItem alg_item = algorithmFactory[index];
	
	Algorithm* alg = alg_item.algorithm_fptr();
    
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
