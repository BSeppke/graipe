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

#ifndef GRAIPE_CORE_LOGGING_HXX
#define GRAIPE_CORE_LOGGING_HXX

#include "core/config.hxx"

#include <QString>
#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

/**
 * @file
 * @brief Header file for the logging facilities
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This class defines everything, that is needed to add basic logging
 * facilities to Qt-Main-Apps. This class is also a Singleton, where all
 * public functions are static!
 */
class GRAIPE_CORE_EXPORT Logging
{ 
	public:    
        /**
         * Replaces the constructor by means of the Singleton design pattern (static)
         *
         * \return a new logger, if non existed before, else, the old one
         */
        static Logging* logger();
    
        /**
         * Returns the currently used filename (static)
         *
         * \return The complete path of the used filename
         */
        static QString filename();
    
        /**
         * returns the currently used textStream pointer (static)
         *
         * \return If existing, it returns the pointer for textstreaming, NULL otherwise
         */
        static QTextStream* textStream();
    
        /**
         * Basic message handler for the QtDebug interface (static)
         *
         * \param type The type of the incoming message.
         * \param context The context of the incoming message.
         * \param msg  The message itself.
         */
        static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    
    protected:
        /**
         * Default constructor for the Logging class (protected)
         */
        Logging();
            
        /**
         * Basic message handler for the QtDebug interface (non-static)
         *
         * \param type The type of the incoming message.
         * \param context The context of the incoming message.
         * \param msg  The message itself.
         */
        void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    
        /** The complete file* (also to return the name of the output file) **/
        QFile* m_file;
        
        /** The textstream, where we write onto **/
        QTextStream* m_textStream;
    
        /** Static pointer to this class's instance **/
        static Logging* m_this;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_LOGGING_HXX
