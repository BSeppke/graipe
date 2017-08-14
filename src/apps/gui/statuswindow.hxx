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

#ifndef GRAIPE_GUI_STATUSWINDOW_HXX
#define GRAIPE_GUI_STATUSWINDOW_HXX

#include "ui_statuswindow.h"

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QFileSystemWatcher>

namespace graipe {

/**
 * @addtogroup graipe_gui
 * @{
 *
 * @file
 * @brief Header file to the graipe gui statuswindow.
 */

/**
 * A very basic status window wich is able to display a progress bar in
 * conjunction with a status message on a QDialog.
 *
 * The GUI design is stored elsewhere, the base class is created using UIC.
 */
class StatusWindow
:   public QDialog
{
    Q_OBJECT
	
	public:
        /**
         * Default constructor of the status window.
         * 
         * \param parent The parent of the status window. If set,
         *               the window will be displayed modally.
         * \param flags  Other window flags.
         */
		StatusWindow(QWidget* parent=0, Qt::WindowFlags flags=0);
	
		/**
         * Default destructor. We need to destroy all "new"ly-created
         * pointers of this class in here.
         */
        ~StatusWindow();

	public slots:
		/**
         * Slot to set or update the module status of GRAIPE.
         *
         * \param str the new status string.
         */
		void updateStatus(const QString & str);
    
		/**
         * Slot to set or update the log of GRAIPE.
         *
         * \param path The path to the file, where the FileSystemWatcher
         *             detected a change. Should always be the systems log-file.
         */
		void updateLog(const QString & path);
	
	private:
		/** GUI elements of this window (auto-created by Qtdesigner) **/
		Ui::StatusWindowBase m_ui;
    
        /** Filesystem watcher to update the log view **/
        QFileSystemWatcher* m_log_watcher;
};

/**
 * @}
 */
 
} //end of namespace graipe

#endif //GRAIPE_GUI_STATUSWINDOW_HXX

