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

#ifndef GRAIPE_CORE_UPDATECHECKER_HXX
#define GRAIPE_CORE_UPDATECHECKER_HXX

#include "core/config.hxx"

#include <QObject>


/**
 * The UpdateChecker is supposed to run concurrently, in order to keep
 * processes free in their running, which the http[s] requests are being
 * performed. Thus is has to derive from QObject. It uses the current version
 * and the internet addresses given in <core/config.hxx>.
 * A typical call would look like:
 *
 *   #include "core/updatechecker.hxx"
 *
 *   //Check for updates in another thread
 *   UpdateChecker* checker = new UpdateChecker;
 *
 *   QThread *checkerThread = new QThread();
 *   checker->moveToThread(checkerThread);
 *   connect(checkerThread, SIGNAL(started()), checker, SLOT(checkForUpdates()));
 *   connect(checker, SIGNAL(finished()), checkerThread, SLOT(quit()));
 *   checkerThread->start();
 */


namespace graipe {

class GRAIPE_CORE_EXPORT UpdateChecker
:   public QObject
{
    Q_OBJECT
    
public slots:
    /**
     * This method/slot checks for updates up to 1.0 (=10) higher than the currently
     * installed version. In addition, a minimal filesize need to be given (defaults to
     * 10 MB) If both is found, it presents a MsgBox, where the user  can either click
     * on the <Download> link or simply ignore it.
     *
     * \param max_version_jump Maximum version jump to search for: major_jump*10+minor_jump.
     * \param min_filesize Minimum size of the installer package, if found at search url.
     */
    void checkForUpdates(unsigned int max_version_jump = 10, unsigned int min_filesize = 1000000);

signals:
    /**
     * This signal is emmitted, when the search for updates is finished. This is
     * neccessary, to exit the thread, in which the checker is running.
     */
    void finished();
};

} //end of namespace graipe

#endif //GRAIPE_CORE_UPDATECHECKER_HXX