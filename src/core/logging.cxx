/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
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
 * The "this" pointer's space (static)
 */
Logging* Logging::m_this;


/**
 * Replaces the constructor by means of the Singleton design pattern (static)
 *
 * \return a new logger, if non existed before, else, the old one
 */
Logging* Logging::logger()
{
    if (m_this == NULL)
    {
        m_this = new Logging();
    }
    return m_this;
}
 
/**
 * Returns the currently used filename (static)
 *
 * \return The complete path of the used filename
 */
QString Logging::filename()
{
    return logger()->m_file->fileName();
}

/**
 * returns the currently used textStream pointer (static)
 *
 * \return If existing, it returns the pointer for textstreaming, NULL otherwise
 */
QTextStream* Logging::textStream()
{
    return logger()->m_textStream;
}

/**
 * Basic message handler for the QtDebug interface (static)
 *
 * \param type The type of the incoming message.
 * \param msg  The message itself.
 */
void Logging::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    logger()->logMessage(type, context, msg);
}

/**
 * Default constructor for the Logging class (protected)
 */
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

/**
 * Basic message handler for the QtDebug interface (non-static)
 *
 * \param type The type of the incoming message.
 * \param msg  The message itself.
 */
void Logging::logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
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
}

}//end of namespace graipe
