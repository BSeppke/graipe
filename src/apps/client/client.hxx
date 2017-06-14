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

#ifndef GRAIPE_CLIENT_CLIENT_HXX
#define GRAIPE_CLIENT_CLIENT_HXX

#include <QMainWindow>
#include <QTcpSocket>
#include <QSignalMapper>

#include "core/core.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTcpSocket;
class QNetworkSession;

namespace graipe {

class Client : public QMainWindow
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = Q_NULLPTR);

signals:
    void clickedAlgorithm(int);

private slots:
    void loadAndSendModel();
    void sendModel(Model* m);
    void sendAlgorithm(Algorithm* alg);
    
    void readHandler();
    void displayError(QAbstractSocket::SocketError socketError);
    void enableSendRequestButton();
    void registerAtServer();
    
    /**
     * This slot is called by all registered algorithms. The parameter
     * (int id) corresponds to the index at the algorithm_factory.
     *
     * \param index The index of the algorithm at the algorithm_factory.
     */
    void runAlgorithm(int index);

private:
    inline void readModel(int bytesToRead);
    QComboBox *m_cmbHost;
    QLineEdit *m_lnePort;
    QLineEdit *m_lneRequest;
    QLabel *m_lblStatus;
    QPushButton *m_btnLogin;
    QPushButton *m_btnSend;

    QTcpSocket *m_tcpSocket;
    
    //signal mapping for dynamically loaded algorithms (and their dynamically created actions)
    QSignalMapper* m_algSignalMapper;
};

} //namespace graipe

#endif
