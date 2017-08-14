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

#ifndef GRAIPE_SERVER_MAINDIALOG_HXX
#define GRAIPE_SERVER_MAINDIALOG_HXX

#include <QWidget>
#include "server.hxx"

class QPushButton;
class QLabel;

namespace graipe {

/**
 * GUI class for the server.
 */
class MainDialog
:   public QWidget
{
    Q_OBJECT

public:
    /** 
     * Creates a new GUI class for the server.
     * 
     * \param parent the parent of this dialog. NULL by default.
     */
    MainDialog(QWidget *parent = 0);

public slots:
    /**
     * This slot is called on every update of a client (e.g. after a successful login).
     */
    void updateClientStatus();
    
    /**
     * This slot updates the server log.
     */
    void updateLog();

private:
    /** 
     * @{
     *
     * GUI Widgets
     */
    QLabel* m_lblClientStatus;
    QTextEdit* m_txtLog;
    
    QPushButton *m_btnQuit;
    /**
     * @}
     */
    
    /** The server **/
    Server* m_server;
};

} //namespace graipe

#endif
