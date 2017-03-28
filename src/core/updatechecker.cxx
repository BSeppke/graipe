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

#include "updatechecker.hxx"

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QMessageBox>

/**
 * @file
 * @brief Implementation file for the UpdateChecker class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This method/slot checks for updates up to 1.0 (=10) higher than the currently
 * installed version. In addition, a minimal filesize need to be given (defaults to
 * 10 MB) If both is found, it presents a MsgBox, where the user  can either click
 * on the <Download> link or simply ignore it.
 *
 * \param max_version_jump Maximum version jump to search for: major_jump*10+minor_jump.
 * \param min_filesize Minimum size of the installer package, if found at search url.
 */
void UpdateChecker::checkForUpdates(unsigned int max_version_jump, unsigned int min_filesize)
{
    //Binaries are only provided for Mac OS X and Windows (64bit)
    //So this pattern might be empty for other versions, and we can avoid searching...
    if(!update_urlpattern.isEmpty())
    {
        //uint of a version: 1.7 -> 17 etc.
        unsigned int version = version_major*10 + version_minor;
        
        QNetworkAccessManager NAManager;

        //Search up to 10 revisions: Ex. 1.7 -> search up to 2.7!
        unsigned int new_version = version+max_version_jump;
        QUrl url;

        while (new_version > version)
        {
            url= QUrl(update_urlpattern.arg(0.1*new_version,0,'f',1));
            
            QNetworkRequest request(url);
            QNetworkReply *reply = NAManager.get(request);
            QEventLoop eventLoop;
            QObject::connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            eventLoop.exec();

            if (   reply->error() == QNetworkReply::NoError									//No Error
                && reply->header(QNetworkRequest::ContentLengthHeader).toUInt() > min_filesize)	//Large enough to be an installer (> 1MB)
            {
                break;
            }
            new_version--;
        }

        if (new_version > version)
        {
            QString new_version_name = QString::number(0.1*new_version,'f', 1);

            QMessageBox::information(NULL, "Update available", 
                "<p><b>A new version of GRAIPE is available!</b></p>"
                "<p>Your version: " + version_name + "<br/>"
                "New version: " + new_version_name + "</p>"
                "<p>You may download the new version from:<br/>"
                "<a href='" + url.toString() + "'>" + url.toString() + "</a></p>");
        }
    }
    emit finished();
}

} //end of namespace graipe

/**
 * @}
 */
