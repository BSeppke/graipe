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

#include "gui/statuswindow.hxx"
#include "core/logging.hxx"

#include <QScrollBar>

namespace graipe {

/**
 * @addtogroup graipe_gui
 * @{
 *     @file
 *     @brief Implementation file to the graipe gui statuswindow.
 * @}
 */
 
StatusWindow::StatusWindow(QWidget* parent, Qt::WindowFlags flags)
 :  QDialog(parent,flags),
    m_log_watcher(new QFileSystemWatcher)
{
    m_ui.setupUi(this);
	connect(m_ui.btnClose, SIGNAL(clicked()), this, SLOT(close()));
    
    m_log_watcher->addPath(Logging::filename());
    updateLog(Logging::filename());
    
	connect(m_log_watcher, SIGNAL(fileChanged(const QString &)), this, SLOT(updateLog(const QString &)));
}

StatusWindow::~StatusWindow()
{
    delete m_log_watcher;
}

void StatusWindow::updateStatus(const QString & str)
{
	m_ui.txtStatus->setText(str);
}

void StatusWindow::updateLog(const QString & path)
{
    QFile file(path);
    m_ui.txtLog->setText(QString("Error while opening the log-file:\n%1").arg(file.fileName()));

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        m_ui.txtLog->setFocus();
        m_ui.txtLog->setText(stream.readAll());
        m_ui.txtLog->verticalScrollBar()->setValue(m_ui.txtLog->verticalScrollBar()->maximum());
    }
}

}// end of namespace graipe
