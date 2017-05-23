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

#ifndef GRAIPE_GUI_MAINWINDOW_HXX
#define GRAIPE_GUI_MAINWINDOW_HXX

#include "core/core.h"
#include "gui/qlistwidgetitems.hxx"
#include "gui/statuswindow.hxx"

#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSignalMapper>
#include <QPrinter>
#include <QList>
#include <QMenu>
#include <QString>
#include <QScrollBar>


namespace graipe {

/**
 * This is the MainWindow, the overall GUI controller. Here nearly 
 * everything is handled. At startup, it looks for importable
 * modules in the same directory and loads them if possible.
 *
 * The GUI design is stored elsewhere, the base class is created using UIC.
 */
class MainWindow
    : public QMainWindow
{
	Q_OBJECT

public:
    /**
     * Constructor of main window.
     *
     * \param parent The parent of this window. None by default.
     * \param name The name of this window. None by default.
     * \param flags Additional flags of this window. None by default.
     */
    MainWindow(QWidget* parent=NULL, const char* name=NULL, Qt::WindowFlags f=0);

    /**
     * Destructor of the main window.
     * We need to destroy all "new"-created pointers in here.
     */
    ~MainWindow();

    /** 
     * Resets the main window to an empty state witout any ViewControllers or Models
     */
    void reset();

signals:
    /**
     * This signal is mapped to call the right algorithm each time the
     * corresponding (menu-)action is triggered.
     */
    void clickedAlgorithm(int);

protected slots:
    /**
     * This slot is called to load a Model from file system.
     */
    void loadModel();
    
    /**
     * This slot is called to load a Model from the "Recent Models" list.
     */
    void loadRecentModel();
    
    /**
     * This slot is called by menu item: File->Print.
     */
    void print();

    /**
     * This slot is called by menu item: File->"Save as PDF".
     */
    void saveAsPDF();

    /**
     * This slot is called by menu item: File->"Save as SVG".
     */
    void saveAsSVG();

    /**
     * This slot is called by menu item: View->"Zoom in".
     */
    void zoomIn();
    
    /**
     * This slot is called by menu item: View->"Zoom out".
     */
    void zoomOut();
    
    /**
     * This slot is called by menu item: View->"Normal size".
     */
    void normalSize();
    
    /**
     * This slot is called by menu item: Help->Help.
     */
    void help();
    
    /**
     * This slot is called by menu item: Help->About.
     */
    void about();
    
    /**
     * This slot is called every tiem a new data item / Model shall be created.
     */
    void newModel();
    
    /**
     * This slot is called by all registered algorithms. The parameter
     * (int id) corresponds to the index at the algorithm_factory.
     *
     * \param index The index of the algorithm at the algorithm_factory.
     */
    void runAlgorithm(int index);
    
    /**
     * This slot is called by changing the current irem in the ListView of active models.
     *
     * \param item The item, which is links to the next active Model.
     */
    void currentModelChanged(QListWidgetItem *);
    
    /**
     * This slot is called by changing the current irem in the ListView of active views.
     *
     * \param item The item, which is links to the next active ViewController.
     */
    void currentViewControllerChanged(QListWidgetItem * item);
    
    /**
     * This slot is called by layer drag and drop inside the ListView of active views.
     */
    void layerPositionChange( );
    
    /**
     * This slot updates the names of the models, if changed from the Models' properties.
     */
    void refreshModelNames();
    
    /**
     * This slot is called to show a Model using a ViewController.
     */
    void showCurrentModel();
    
    /**
     * This slot is called to save the current Model.
     */
    void saveCurrentModel();
    
    /**
     * This slot is called to remove the current Model.
     */
    void removeCurrentModel();
    
    /**
     * This slot is called to center the graphicsview on the current ViewController.
     */
    void centerOnCurrentView();
    
    /**
     * This slot is called to remove the current Model ViewController.
     */
    void removeCurrentViewController();
    
    /**
     * This slot is called to update the Model ViewController.
     */
    void updateView();
    
    /**
     * This slot is called to save the complete workspace as a folder to the file system.
     */
    void saveWorkspace();
    
    /**
     * This slot is called to save the complete workspace as a folder to the file system.
     *
     * \param dirname The dirname of the Workspace serialization.
     */
    void saveWorkspace(const QString& dirname);

    
    /**
     * This slot is called to restore the last workspace from a folder in the file system.
     */
    void restoreLastWorkspace();
    
    /**
     * This slot is called to restore the complete workspace from a folder in the file system.
     */
    void restoreWorkspace();
    
    /**
     * This slot is called to restore the complete  from a folder in the file system.
     *
     * \param dirname The dirname of the Workspace serialization.
     */
    void restoreWorkspace(const QString& dirname);
    
    /**
     * This slot is called to load a view and maybe the depending objects from file system.
     *
     * \param filename The filename of the ViewController serialization.
     */
    void loadViewController(const QString& filename);
    
    /**
     * The slot creates a new Algorithm item on the Model list for an algorithm instance.
     * This item will persist until the processing is finished.
     *
     * \param alg An instance of teh algorithm to be performed.
     * \return The QListWidgetAlgorithmItem for the model list.
     */
    QListWidgetAlgorithmItem* getAlgorithmItem(Algorithm* alg);

public slots:
    /**
     * This slot is called from a running algorithm to update its percentage and
     * status text.
     *
     * \param p The current percentage of execution.
     * \param str Status text.
     */
    void algorithmStateChanged(float p, QString str);
    
    /**
     * This slot is called from a running algorithm to indicate that it has reached a
     * critical error state.
     *
     * \param str The error text.
     */
    void algorithmErrorState(QString str);
    
    /**
     * This slot is called from a running algorithm to indicate that it has
     * finished successfully.
     */
    void algorithmFinished();
    
    /**
     * This slot is called, whenever the text in the status bar needs to be updated.
     *
     * \param str The new text, to be displayed in the status bar.
     */
    void updateStatusText(QString str);
    
    /**
     * This slot is called, whenever the text in the lower right dockWidget needs to be updated.
     *
     * \param str The new text, to be displayed in the lower right dockWidget.
     */
    void updateStatusDescription(QString str);		

protected:
    /**
     * Updates the display of the currently used memory.
     */
    void updateMemoryUsage();
    
    /**
     * Updates the recently used models list from GRAIPE settings file.
     */
    void updateRecentActionList();
    
    /**
     * Adds a filename to the recently used models list.
     * 
     * \param filename The filename of the model.
     */
    void addToRecentActionList(const QString& filename);
    
    /**
     * Load a Model, given by its filename, from the file system.
     * 
     * \param filename The filename of the model.
     */
    void loadModel(const QString& filename);

    /**
     * Find all available modules and fill the corresponding registries with their
     * contributions. Calls loadFactoriesFromDirectory with different paths:
     * Under Mac OS at the place of the executable file (not the app-Bundle) and at
     * the location of the .app-bundle.
     */
    void loadFactories();
    
    /**
     * Find all available modules in a directory and fill the corresponding registries with their
     * contributions.
     *
     * \param dir The directory to search for modules.
     * \param added_menus Menus added during the module load.
     * \return An html-formatted report of the loading. Empty if no module was found.
     */
    QString loadFactoriesFromDirectory(const QDir & dir, QList<QMenu*> & added_menus);

    /**
     * Accessor for the current Model.
     *
     * \return Pointer of the current model. NULL if none.
     */
    Model*  currentModel();
    
    /**
     * The type of the current Model.
     *
     * \return The type of the current model. Empty if none.
     */
    QString currentModelType();

    /**
     * Accessor for the current ViewController.
     *
     * \return Pointer of the current ViewController. NULL if none.
     */
    ViewController* currentViewController();
    
    /**
     * Accessor for the Model of the current ViewController.
     *
     * \return Pointer of the Model of the current ViewController. NULL if none.
     */
    Model*          currentViewControllerModel();
    
    /**
     * The type of the Model of the current ViewController.
     *
     * \return The type of the Model of the current ViewController. Empty if none.
     */
    QString         currentViewControllerModelType();

    /**
     * Adds a given Model to the list of models.
     * 
     * \param model The model, which shall be added.
     */
    void addModelItemToList(Model* model);
    
    /**
     * Adds a given ViewController to the list of models.
     * 
     * \param viewController The ViewController, which shall be added.
     */
    void addViewControllerItemToList(ViewController* viewController);
    
	/**
     * To enable drag and drop of listwidget-entries, we use a global EventFilter
     * and the global timer, which checks for drags and drops.
	 * From the design side, this is really ugly - but it works...
     *
     * \param evt The event itself
     * \return Always false.
     */
     bool eventFilter(QObject *, QEvent * evt);

private:
    /* 
     *  Member variables
     *****************************************************
     */

    //all GUI elements of this window (auto-created by Qtdesigner)
    Ui::MainWindowBase m_ui;

    //the scene where the visual elements are placed at
    QGraphicsScene*	m_scene;
    
    //a view to the scene above
    QGraphicsView* m_view;

    //a printer (of the view)
    QPrinter* m_printer;

    //signal mapping for dynamically loaded algorithms (and their dynamically created actions)
    QSignalMapper* m_signalMapper;

    //Enum for easy handling of current view type:
    enum DisplayMode
    {
        ImageMode,
        GeographicMode
    }
    m_displayMode;
    
    StatusWindow* m_status_window;

    QString m_settings_dir;
    QString m_default_dir;
    
    QLabel * m_lblMemoryUsage;
    
    QList<QAction*> m_recentFileActions;
    const int       m_recentFileCount;
};

} //end of namespace graipe

#endif //GRAIPE_GUI_MAINWINDOW_HXX
