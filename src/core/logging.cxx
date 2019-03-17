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

#include "core/logging.hxx"

#include <QDateTime>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implmentation file for the logging facilities
 * @}
 */


/**
 * The "this" pointer's space (static)
 */
Logging* Logging::m_this;


Logging* Logging::logger()
{
    if (m_this == NULL)
    {
        m_this = new Logging();
    }
    return m_this;
}

Logging* Logging::logger(QString filename)
{
    if (m_this == NULL)
    {
        m_this = new Logging(filename);
    }
    if (m_this->m_file != NULL && m_this->m_file->fileName() != filename)
    {
        delete m_this;
        m_this = new Logging(filename);
    }
    return m_this;
}

QString Logging::filename()
{
    return logger()->m_file->fileName();
}

QTextStream* Logging::textStream()
{
    return logger()->m_textStream;
}

void Logging::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    logger()->logMessage(type, context, msg);
}

Logging::Logging()
: m_file(NULL),
  m_textStream(NULL)
{
    //Initialize static this pointer
    m_this = this;
    
    //1. Step: Find the correct path, either tmp or homeDir()/.graipe/
    QString outputDirname = "/tmp/";
    QString outputFilename = "graipe.log";
    
    QDir dir;
    QString preferredDirname = QDir::homePath() + "/.graipe/";
    if( dir.mkpath(preferredDirname))
    {
        outputDirname = preferredDirname;
    }
    
    
    //2. Assign class memebers
    m_file = new QFile(outputDirname+outputFilename);
    
    if(m_file->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_textStream = new QTextStream(m_file);
    }
}

Logging::Logging(QString filename)
: m_file(NULL),
  m_textStream(NULL)
{
    //Initialize static this pointer
    m_this = this;
    
    //2. Assign class memebers
    m_file = new QFile(filename);
    
    if(m_file->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        m_textStream = new QTextStream(m_file);
    }
}

void Logging::logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if(m_textStream == nullptr)
    {
        return;
    }
    
    QByteArray localMsg = msg.toLocal8Bit();
    
    QString txt = QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz"));
    
    switch (type)
    {
        case QtDebugMsg:
            txt += QString("Debug: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtInfoMsg:
            txt += QString("Info: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtWarningMsg:
            txt += QString("Warning: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtCriticalMsg:
            txt += QString("Critical: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
            break;
        case QtFatalMsg:
            txt += QString("Fatal: %1 (%2:%3, %4)").arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function);
            abort();
    }
    *m_textStream << txt << "\n";
    m_textStream->flush();
}

}//end of namespace graipe
