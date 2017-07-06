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

#include "gui/mainwindow.hxx"
#include "gui/memorystatus.hxx"

#include "core/updatechecker.hxx"
#include "core/workspace.hxx"

#include <QDir>
#include <QLibrary>
#include <QThread>
#include <QMessageBox>
#include <QPrintDialog>
#include <QFileDialog>
#include <QSvgGenerator>
#include <QSettings>
#include <QScrollBar>
#include <QTimer>

namespace graipe {

/**
 * Constructor of main window.
 *
 * \param parent The parent of this window. None by default.
 * \param name The name of this window. None by default.
 * \param flags Additional flags of this window. None by default.
 */
MainWindow::MainWindow(QWidget* parent, const char* name, Qt::WindowFlags f) :
    QMainWindow(parent,f),
	m_scene(NULL),
	m_view(NULL),
	m_modSignalMapper(new QSignalMapper),
	m_algSignalMapper(new QSignalMapper),
	m_status_window(new StatusWindow),
    m_lblMemoryUsage(new QLabel("(Memory: 0 MB, max: 0 MB)")),
    m_recentFileCount(10),
    m_workspace(new Workspace)
{	
    m_ui.setupUi(this);
    
    //init the factories from the workspace
    initializeFactories();
	
    m_ui.scrModelParameters->setWidgetResizable(true);
    m_ui.scrViewParameters->setWidgetResizable(true);
    
	/*--------- File menu --------*/
	connect( m_ui.actionLoad_data, SIGNAL(triggered()), this, SLOT(loadModel()));
	
	connect( m_ui.actionSave_current_data,      SIGNAL(triggered()), this, SLOT(saveCurrentModel()));
	connect( m_ui.actionShow_current_data,      SIGNAL(triggered()), this, SLOT(showCurrentModel()));
	connect( m_ui.actionUnload_current_data,    SIGNAL(triggered()), this, SLOT(removeCurrentModel()));
    
    connect( m_ui.actionSave_workspace,     SIGNAL(triggered()), this, SLOT(saveWorkspace()));
    connect( m_ui.actionRestore_workspace,  SIGNAL(triggered()), this, SLOT(restoreWorkspace()));
    
	connect( m_ui.actionPrint,              SIGNAL(triggered()), this, SLOT(print()));
	connect(m_ui.actionExport_View_to_PDF,  SIGNAL(triggered()), this, SLOT(saveAsPDF()));
	connect(m_ui.actionExport_View_to_SVG,  SIGNAL(triggered()), this, SLOT(saveAsSVG()));
		
	connect( m_ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	
	/*--------- View menu --------*/
	connect( m_ui.actionZoom_in,        SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect( m_ui.actionZoom_out,       SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect( m_ui.actionNormal_size,    SIGNAL(triggered()), this, SLOT(normalSize()));

	
	/*--------- Help menu --------*/
	connect( m_ui.actionHelp,   SIGNAL(triggered()), this, SLOT(help()));
	connect( m_ui.actionAbout,  SIGNAL(triggered()), this, SLOT(about()));
	
	
	//Views view
	m_ui.btnCreateItem->setMenu(m_ui.menuCreate);
    
	connect(m_ui.btnLoadItem,	SIGNAL(clicked()), this, SLOT(loadModel()));
	connect(m_ui.btnSaveItem,	SIGNAL(clicked()), this, SLOT(saveCurrentModel()));
	connect(m_ui.btnRemoveItem, SIGNAL(clicked()), this, SLOT(removeCurrentModel()));
	
	connect(m_ui.btnRemoveItemView,     SIGNAL(clicked()), this, SLOT(removeCurrentViewController()));
    connect(m_ui.btnCenterOnModelView,  SIGNAL(clicked()), this, SLOT(centerOnCurrentView()));
	connect(m_ui.btnWorldView,          SIGNAL(clicked()), this, SLOT(updateView()));
	
    //Notbehelf, weil die modelUpdate Nachrichten irgendwie nicht immer ankommen... Update manuell per Klick
	connect( m_ui.listViews, SIGNAL(itemClicked ( QListWidgetItem *)), this, SLOT(currentViewControllerChanged(QListWidgetItem *)));
	connect( m_ui.listViews, SIGNAL(currentItemChanged ( QListWidgetItem *, QListWidgetItem * )), this, SLOT(currentViewControllerChanged(QListWidgetItem *)));

    //Notbehelf, weil die modelUpdate Nachrichten irgendwie nicht immer ankommen... Update manuell per Klick
	connect( m_ui.listModels, SIGNAL(itemClicked ( QListWidgetItem *)), this, SLOT(currentModelChanged(QListWidgetItem *)));
	connect( m_ui.listModels, SIGNAL(currentItemChanged ( QListWidgetItem *, QListWidgetItem * )), this, SLOT(currentModelChanged(QListWidgetItem *)));
            
	connect(m_ui.btnShowModel, SIGNAL(clicked()), this, SLOT(showCurrentModel()));
	
	m_ui.listViews->installEventFilter(this);
	
	//Initialize other main-window stuff
    m_scene = new QGraphicsScene;
    m_view  = new QGraphicsView(m_scene, this);
    m_view->setMinimumSize(400, 300);
    m_view->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	m_view->setInteractive(true);
    m_view->setMouseTracking(true);
    m_view->setRenderHint(QPainter::Antialiasing, true);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform, false);
    
    m_printer = NULL;
	
	m_displayMode = ImageMode;
    
    //Prepare the recent file menu:
    QAction* recentFileAction = NULL;
    for(int i = 0; i < m_recentFileCount; i++)
    {
        recentFileAction = new QAction(this);
        recentFileAction->setVisible(false);
        QObject::connect(recentFileAction, SIGNAL(triggered()), this, SLOT(loadRecentModel()));
        m_recentFileActions.append(recentFileAction);
        m_ui.menuLoad_recent_data->addAction(recentFileAction);
    }
    
    this->setWindowTitle("GRAIPE v." + version_name);
    
    //Set Stylesheet for info label:
    m_ui.lblStatusInformation->setStyleSheet(
        QString("QLabel table {     width: 100%;   border: 1px solid black; } ")
            + "QLabel th {  padding: 5px;     border: 1px solid black; } "
            + "QLabel td {  text-align: right; padding: 5px; border: 1px solid black; } "
        );
    
    //load the recent files from the "graipe.ini" file
    updateRecentActionList();
    
    //default opening dir for the file dialogs:
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);
    m_default_dir = settings.value("defaultDir").toString();
    
    //Restore the other vierport and window state settings:
    m_ui.btnWorldView->setChecked(settings.value("geoMode", false).toBool());
        
    //Restore view transform
    QTransform vc_t;
        
    vc_t.setMatrix(
            settings.value("view_m11", 1.0).toFloat(), settings.value("view_m12", 0.0).toFloat(), settings.value("view_m13", 0.0).toFloat(),
            settings.value("view_m21", 0.0).toFloat(), settings.value("view_m22", 1.0).toFloat(), settings.value("view_m23", 0.0).toFloat(),
            settings.value("view_m31", 0.0).toFloat(), settings.value("view_m32", 0.0).toFloat(), settings.value("view_m33", 1.0).toFloat());
    m_view->setTransform(vc_t);
        
    //Restore window geometry & dockWidgets state
    restoreGeometry( settings.value("window_geometry").toByteArray());
    restoreState( settings.value("window_state").toByteArray());
        
    m_view->horizontalScrollBar()->setValue(settings.value("view_hscroll").toInt());
    m_view->verticalScrollBar()->setValue(settings.value("view_vscroll").toInt());

    //Set the GRAIPE settings directory to homeDir()/.graipe/
    //Or, if the folder cannot be created: /tmp/.graipe/
    //Same as for the logger...
    m_settings_dir = "/tmp/";
    QDir dir;
    QString preferredDirname = QDir::homePath() + "/.graipe/";
    if( dir.mkpath(preferredDirname))
    {
        m_settings_dir = preferredDirname;
    }
    //Give the GUI time build up before restoring the last Workspace (if wanted)
    QTimer::singleShot(100, this, SLOT(restoreLastWorkspace()));
    
    //Check for updates in another thread
    UpdateChecker* checker = new UpdateChecker;
    
    QThread *checkerThread = new QThread();
    checker->moveToThread(checkerThread);
    connect(checkerThread, SIGNAL(started()), checker, SLOT(checkForUpdates()));
    connect(checker, SIGNAL(finished()), checkerThread, SLOT(quit()));
    checkerThread->start();
    
	statusBar();
    this->statusBar()->addPermanentWidget(m_lblMemoryUsage);
    updateMemoryUsage();
    
	setWindowTitle(tr(name));
    setCentralWidget(m_view);
	
}


/** 
 * Destructor of the main window
 * Here we need to free all alocated memory
 */
MainWindow::~MainWindow()
{
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);

    settings.setValue("geoMode", m_displayMode == GeographicMode);
    
    settings.setValue("window_geometry", saveGeometry());
    settings.setValue("window_state", saveState());
    
    settings.setValue("view_hscroll",  m_view->horizontalScrollBar()->value());
    settings.setValue("view_vscroll",  m_view->verticalScrollBar()->value());
    
    //save the scene's settings to an ini-file
    QTransform vc_t = m_view->transform();
    
    settings.setValue("view_m11", vc_t.m11());    settings.setValue("view_m12", vc_t.m12());    settings.setValue("view_m13", vc_t.m13());
    settings.setValue("view_m21", vc_t.m21());    settings.setValue("view_m22", vc_t.m22());    settings.setValue("view_m23", vc_t.m23());
    settings.setValue("view_m31", vc_t.m31());    settings.setValue("view_m32", vc_t.m32());    settings.setValue("view_m33", vc_t.m33());


    if( QMessageBox::question(this,
                              "Save workspace?",
                             "Do you want to save the current workspace for the next start of GRAIPE?",
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::Yes)
            == QMessageBox::Yes)
    {
        saveWorkspace(m_settings_dir + "workspace.xgz");
    }
    delete m_lblMemoryUsage;
    delete m_view;
    delete m_printer;
    delete m_status_window;
    delete m_workspace;
}

/**
 * Resets the main window to an empty state witout any ViewControllers or Models
 */
void MainWindow::reset()
{
    //If at least one model is locked, give up:
    bool locked=false;
    for(int i=0;  i < m_ui.listModels->count(); ++i)
    {
        QListWidgetModelItem* model_item = static_cast<QListWidgetModelItem*>(m_ui.listModels->item(i));
        if(model_item && model_item->model())
        {
            Model* model = model_item->model();
            locked |= model->locked();
        }
        if (locked)
            throw std::runtime_error("At least one model is locked, reset is not possible");
    }
    
    //Else start with removing of the views:
    m_ui.listViews->clear();
    
    //and proceed with the models:
    m_ui.listModels->clear();
    
    //Also clear models and viewControllers in the Workspace
    m_workspace->clear();
}

/**
 * This slot is called to load a Model from file system.
 */
void MainWindow::loadModel()
{	
	QFileDialog dialog(this, "Load models", m_default_dir, "GRAIPE models (*.xgz *.xml)");
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setViewMode(QFileDialog::Detail);
	
	QStringList fileNames;
	if (dialog.exec())
    {
		fileNames = dialog.selectedFiles();
	}
    
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);
    
	for(const QString& filename: fileNames)
	{
        m_default_dir = QFileInfo(filename).absoluteDir().path();
        settings.setValue("defaultDir",m_default_dir);
        
        try
        {
            loadModel(filename);
        }
        catch (...)
        {
			QMessageBox::about(this, tr("Error"), filename + "\n was not loaded!\n");
        }
	}		
}

/**
 * This slot is called to load a Model from the "Recent Models" list.
 */
void MainWindow::loadRecentModel()
{	
    QAction *action = qobject_cast<QAction *>(sender());
    
    if (action)
    {
        try
        {
            loadModel(action->data().toString());
        }
        catch (...)
        {
			QMessageBox::about(this, tr("Error"), action->data().toString() + "\n was not loaded!\n");
        }
    }
}

/**
 * This slot is called by menu item: File->Print.
 */
void MainWindow::print()
{
    if ( !m_printer )
		m_printer = new QPrinter(QPrinter::HighResolution);
	
	QPrintDialog *dialog = new QPrintDialog(m_printer, this);
	dialog->setWindowTitle(tr("Print out canvas"));
	
    if ( dialog->exec() == QDialog::Accepted )
	{
		QPainter painter(m_printer);
		painter.setRenderHint(QPainter::Antialiasing);
		m_scene->render(&painter);
    }
}

/**
 * This slot is called by menu item: File->"Save as PDF".
 */
void MainWindow::saveAsPDF()
{
	QString s = QFileDialog::getSaveFileName(this, QString("Save Image As PDF"),".",tr("Portable Document Format (*.pdf)"));
	
	if(!s.isNull())
	{
		try
		{
			QPrinter printer;
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setOutputFileName(s);
			printer.setFullPage(true);
			printer.setOrientation(QPrinter::Portrait);
			printer.setPaperSize(QPrinter::A4);
			
			QPainter painter(&printer);
			painter.setRenderHint(QPainter::Antialiasing);
			m_scene->render(&painter);
		}
		catch (std::exception & e)
		{
			QMessageBox::about(this, tr("Error"),s+QString(" \n was not printed because: \n")+QString(e.what()));
		}
	}	
}

/**
 * This slot is called by menu item: File->"Save as SVG".
 */
void MainWindow::saveAsSVG()
{
	QString s = QFileDialog::getSaveFileName(this, QString("Save Image As SVG"),".",tr("Scalable Vector Format (*.svg)"));
	
	if(!s.isNull())
	{
		try
		{   
			QSvgGenerator generator;
			generator.setFileName(s);
			generator.setSize(QSize(200, 200));
			generator.setViewBox(QRect(0, 0, 200, 200));
			generator.setTitle(tr("SVG Generator Example Drawing"));
			generator.setDescription(tr("An SVG drawing created by the SVG Generator "
										"Example provided with Qt."));
			
			QPainter painter;
			painter.begin(&generator);
			painter.setRenderHint(QPainter::Antialiasing);
			m_scene->render(&painter);
			painter.end();
		}
		catch (std::exception & e)
		{
			QMessageBox::about(this, tr("Error"),s+QString(" \n was not printed because: \n")+QString(e.what()));
		}
	}	
}

/**
 * This slot is called by menu item: View->"Zoom in".
 */
void MainWindow::zoomIn()
{
	m_view->scale( 1.1, 1.1);
}

/**
 * This slot is called by menu item: View->"Zoom in".
 */
void MainWindow::zoomOut()
{
	m_view->scale( 1.0/1.1, 1.0/1.1);
}

/**
 * This slot is called by menu item: View->"Normal size".
 */
void MainWindow::normalSize()
{	
	m_view->setTransform(QTransform());
}

/**
 * This slot is called by menu item: Help->Help.
 */
void MainWindow::help()
{
    m_status_window->show(); 
}

/**
 * This slot is called by menu item: Help->About.
 */
void MainWindow::about()
{
    QMessageBox::about(this, "About GRAIPE",
                "<p><b>About the GrAphical Image Processing Workspace<br/>"
                "a.k.a. GRAIPE</b></p>"
                "<p>Currently installed version: " + full_version_name + "<br/>"
                "GIT version (build): " + git_version + "<br />"
				"Build timestamp: " + __TIMESTAMP__ + "</p>"
                "<p>You may find more information about the currently loaded modules as well "
                "as the log under the menu item: Help.</p>");
}

/**
 * This slot is called every time a new data item / Model shall be created.
 *
 * \param inxed The model's index in the modelFactory.
 */
void MainWindow::newModel(int index)
{
    ModelFactoryItem item = m_workspace->modelFactory()[index];
    Model * new_model = item.model_fptr(m_workspace);
            
    //SHOW EDIT DIALOG
    ModelParameterSelection parameter_selection(this, new_model);
    parameter_selection.setWindowTitle(QString("Create new ")+ new_model->typeName());
    parameter_selection.setModal(true);

    if(parameter_selection.exec())
    {
        Model* copyModel = parameter_selection.useOtherModel();
        
        if(copyModel != NULL)
        {
            if(parameter_selection.cloneOtherModel())
            {
                copyModel->copyData(*new_model);
            }
            else
            {
                copyModel->copyMetadata(*new_model);
            }
        }
        
        addModelItemToList(new_model);
    }
}

/**
 * This slot is called by all registered algorithms. The parameter
 * (int id) corresponds to the index at the algorithm_factory.
 *
 * \param index The index of the algorithm at the algorithm_factory.
 */
void MainWindow::runAlgorithm(int index)
{
    using namespace ::std;
    
    AlgorithmFactoryItem alg_item = m_workspace->algorithmFactory()[index];
	
	Algorithm* alg = alg_item.algorithm_fptr(m_workspace);
    
	AlgorithmParameterSelection parameter_selection(this, alg);
	parameter_selection.setWindowTitle(alg_item.algorithm_name);
	parameter_selection.setModal(true);
	
	if(parameter_selection.exec())
	{
		//The result is true if and only if all parameters have been finalised
		//AND are available!
		if( parameter_selection.result()!=0 )
		{
			QThread *thr = new QThread;
			alg->moveToThread(thr);
			
			//Add alg. status item to models
			QListWidgetAlgorithmItem * alg_list_item = new QListWidgetAlgorithmItem(alg_item.algorithm_name, alg );
			alg_list_item->setToolTip(alg_item.algorithm_name);
			alg_list_item->setFlags(Qt::NoItemFlags);
			m_ui.listModels->addItem(alg_list_item);
						
			connect(thr, SIGNAL(started()), alg, SLOT(run()));
			
			connect(alg, SIGNAL(statusMessage(float, QString)), this, SLOT(algorithmStateChanged(float, QString)));
			connect(alg, SIGNAL(errorMessage(QString)), this, SLOT(algorithmErrorState(QString)));
			connect(alg, SIGNAL(finished()), this, SLOT(algorithmFinished()));
			
			connect(alg, SIGNAL(finished()), thr, SLOT(quit()));
			
			thr->start();
		}
		else 
		{
			QMessageBox::critical(this, "Error in Algorithm run",
								  QString("Not all necessary parameters have been set!"));
		}
	}
    else
    {
        for(Model* m : alg->results())
        {
            delete m;
            m=NULL;
        }
        alg->results().clear();
    }
}


/**
 * This slot is called by changing the current irem in the ListView of active views.
 *
 * \param item The item, which is links to the next active ViewController.
 */
void MainWindow::currentModelChanged(QListWidgetItem * item)
{
    if(!item || !item->flags())
		return;
	
	//try to down-cast to QListWidgetItem
	QListWidgetModelItem* model_item  = dynamic_cast<QListWidgetModelItem*>(item);
	if(model_item)
	{
        Model * model = model_item->model();
        
        if(model)
        {
            QWidget * model_parameter_widget = model->parameters()->delegate();
        
            if(model_parameter_widget != NULL)
            {
                m_workspace->setCurrentModel(model);
                
                m_ui.scrModelParameters->takeWidget();
                m_ui.dockModelParameters->setWindowTitle(model->parameters()->name());
                
                model_parameter_widget->setEnabled(!model->locked());
                model_parameter_widget->update();
                m_ui.scrModelParameters->setWidget(model_parameter_widget);
            }
        }
    }
}

/**
 * This slot is called by changing the current irem in the ListView of active views.
 *
 * \param item The item, which is links to the next active ViewController.
 */
void MainWindow::currentViewControllerChanged(QListWidgetItem * item)
{
    QListWidgetViewControllerItem* new_item  = static_cast<QListWidgetViewControllerItem*>(item);
    
    for(int i=0; i< m_ui.listViews->count(); ++i)
	{
        QListWidgetViewControllerItem* vc_item = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->item(i));
        if(vc_item)
        {
            ViewController* viewController = vc_item->viewController();
        
            if(vc_item == new_item)
            {
                m_workspace->setCurrentViewController(viewController);
                
                viewController->setAcceptHoverEvents(true);
                
                if (viewController && item->checkState()==Qt::Checked)
                {
                    viewController->show();
                }
                else
                {
                    viewController->hide();
                }
                
                m_ui.scrViewParameters->takeWidget();
                m_ui.dockViewParameters->setWindowTitle(viewController->parameters()->name());
                m_ui.scrViewParameters->setWidget(viewController->parameters()->delegate());
            }
            else
            {
                viewController->setAcceptHoverEvents(false);
            }
		}
	}
}

/**
 * This slot is called by layer drag and drop inside the ListView of active views.
 */
void MainWindow::layerPositionChange()
{
	for(int i=0; i< m_ui.listViews->count(); ++i)
	{
		QListWidgetViewControllerItem* vc_item = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->item(i));
		if(vc_item != NULL)
		{
			ViewController * viewController = vc_item->viewController();
		
			if (viewController != NULL)
            {
				viewController->setZValue(i);
		
                if(vc_item->checkState()==Qt::Checked)
                {
                    viewController->show();
                }
                else
                {
                    viewController->hide();
                }
            }
        }
	}
}

/**
 * This slot updates the names of the models, if changed from the Models' properties.
 */
void MainWindow::refreshModelNames()
{
    for(int i=0; i!=m_ui.listModels->count(); i++)
    {
        QListWidgetModelItem * model_item = dynamic_cast<QListWidgetModelItem *>(m_ui.listModels->item(i));
        
        //No update for Algorithm-Items
        if (model_item)
        {
            Model* model = model_item->model();
            
            if (model)
            {
                model_item->setText(QString(model->locked()?"Locked ":"") + model->name());
                model_item->setToolTip(model->description());
                
                model->parameters()->delegate()->setEnabled(!model->locked());
            }
        }
    }
    for(int i=0; i!=m_ui.listViews->count(); i++)
    {
        QListWidgetViewControllerItem * vc_item = static_cast<QListWidgetViewControllerItem *>(m_ui.listViews->item(i));
        
        //No update for Algorithm-Items
        if (vc_item)
        {
            ViewController* viewController = vc_item->viewController();
            
            if (viewController && viewController->model())
            {
                Model* model = viewController->model();
                
                vc_item->setText(viewController->typeName() + " of " + QString(model->locked()?"Locked ":"") + model->name());
                vc_item->setToolTip(viewController->typeName() + " of " + model->description());
                
            }
        }
    }
}

/**
 * This slot is called to show a Model using a ViewController.
 */
void MainWindow::showCurrentModel()
{
    using namespace ::std;
    
	QListWidgetItem* current_item = m_ui.listModels->currentItem();
	
	if(current_item)
	{
		QListWidgetModelItem* model_item = static_cast<QListWidgetModelItem*> (current_item);
			
        if(model_item && model_item->model()->isViewable())
        {
            ViewControllerFactory vc_possibilities =  m_workspace->viewControllerFactory().filterByModelType(model_item->model());
            int vc_index = -1;
            
            //Select the view/controller
            
            //trivial if there exists only one
            if(vc_possibilities.size()==1)
            {
                vc_index=0;
            }
            //ask user if there are alternatives
            else if(vc_possibilities.size()>1)
            {
            
                QStringList possible_views;
                for (unsigned int i=0; i< vc_possibilities.size(); ++i)
                {
                    possible_views.push_back(vc_possibilities[i].viewController_name);
                }
                
                EnumParameter* choices = new EnumParameter("Views", possible_views,0,0);
                
                ParameterSelection parameter_selection(this, choices);
                parameter_selection.setModal(true);
                
                if(parameter_selection.exec())
                {
                    vc_index = choices->value();
                }
            }
            //if index found: add view/controller
            if(vc_index != -1)
            {
                ViewController* new_vc = vc_possibilities[vc_index].viewController_fptr(model_item->model());
                
                //Always show and make current for new ViewControllers
                new_vc->setVisible(true);
                m_workspace->setCurrentViewController(new_vc);
                addViewControllerItemToSceneAndList(new_vc);
            }
        }
    }
}

/**
 * This slot is called to save the current Model.
 */
void MainWindow::saveCurrentModel()
{
	using namespace std;
	
	Model* model = currentModel();
	
	if(model)
	{
		QString suggested_filename = model->name().replace(" ", "_");
		QString filename = QFileDialog::getSaveFileName(this, tr("Save Model to file"),
                           suggested_filename,
                            tr("Packed GRAIPE-models (*.xgz);;Unpacked GRAIPE-models (*.xml)"));
		
		if(!filename.isEmpty())
		{	
			ModelFactory model_possibilities =  m_workspace->modelFactory().filterByModelType(model);
			
			if(model_possibilities.size()==1)
			{
				bool compress =  (filename.right(2) == "gz");
				
				if(Impex::save(model, filename, compress))
                {
                    addToRecentActionList(filename);
                }
                else
				{
					QMessageBox::about(this, tr("Error"),filename + "\n was not saved!\n");
				}
			}
		}
	}
}

/**
 * This slot is called to remove the current Model.
 */
void MainWindow::removeCurrentModel()
{
	Model* model = currentModel();
	
	if(model)
	{
		bool found_model_view = false;
		
		//look if the model is displayed -> cannot delete in that case
		for(int i=0;  i < m_ui.listViews->count(); ++i)
		{
			QListWidgetViewControllerItem* vc_item = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->item(i));
			if(vc_item && vc_item->viewController()->model() == model)
			{
				found_model_view = true;
				break;
			}
		}
		
		if(!found_model_view && !model->locked())
		{
            m_ui.scrModelParameters->takeWidget();
            m_ui.listModels->takeItem(m_ui.listModels->currentRow());
            updateMemoryUsage();
            
            if (m_ui.listModels->count() != 0)
            {
                currentModelChanged(m_ui.listModels->currentItem());
            }
        }
	}
}

/**
 * This slot is called to center the graphicsview on the current ViewController.
 */
void MainWindow::centerOnCurrentView()
{
	ViewController* view = currentViewController();
    
	if(view != NULL)
	{
        m_view->centerOn(view);
    }
}
    
/**
 * This slot is called to remove the current Model ViewController.
 */
void MainWindow::removeCurrentViewController()
{
	ViewController* viewController = currentViewController();
	
	if(viewController != NULL)
	{
		//hide all graphical elements
		viewController->hide();
        m_ui.scrViewParameters->takeWidget();
		m_ui.listViews->takeItem(m_ui.listViews->currentRow());
		updateMemoryUsage();
            
        if (m_ui.listViews->count() != 0)
        {
            currentViewControllerChanged(m_ui.listViews->currentItem());
        }
	}
}

/**
 * This slot is called to update the Model ViewController.
 */
void MainWindow::updateView()
{
	for(int i=0;  i < m_ui.listViews->count(); ++i)
	{
		QListWidgetViewControllerItem* vc_item = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->item(i));
		
		ViewController* viewController = vc_item->viewController();
		
		viewController->hide();
		
		if(m_ui.btnWorldView->isChecked())
        {
            viewController->setTransform(viewController->model()->globalTransformation());
		
        }
        else
		{
            viewController->setTransform(viewController->model()->localTransformation());
        }
		if(vc_item->checkState() == Qt::Checked)
        {
            viewController->show();
        }
	}
	
	QRectF scene_rect;
	if(m_ui.btnWorldView->isChecked())
	{
		scene_rect = QRectF(-180,-90, 360,180);
	}
	else 
	{
		scene_rect = m_scene->itemsBoundingRect();
	}
	m_scene->setSceneRect(scene_rect);
	
	m_view->setSceneRect(scene_rect);
	m_view->centerOn(scene_rect.center());
	m_view->updateSceneRect(scene_rect);
}
    
/**
 * This slot is called to save the complete workspace as a folder to the file system.
 */
void MainWindow::saveWorkspace()
{
    QString suggested_xmlFilename = QString("workspace_") + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm") + ".xgz";
    QString xmlFilename = QFileDialog::getSaveFileName(this, tr("Save Workspace to file"), suggested_xmlFilename);
    
    if(!xmlFilename.isNull())
    {
        saveWorkspace(xmlFilename);
    }
}

/**
 * This slot is called to save the complete workspace as a folder to the file system.
 *
 * \param dirname The dirname of the Workspace serialization.
 */
void MainWindow::saveWorkspace(const QString& xmlFilename)
{
    try
    {
        QIODevice* device = Impex::openFile(xmlFilename, QIODevice::WriteOnly);
        
        if(device != NULL)
        {
            QXmlStreamWriter xmlWriter(device);
            xmlWriter.setAutoFormatting(true);
            
            m_workspace->serialize(xmlWriter);
            
            device->close();
        }
    }
    catch (std::exception & e)
    {
        QMessageBox::about(this, tr("Error"),QString("Workspace was not saved!\n") + QString::fromUtf8(e.what()));
    }
    catch (...)
    {
        QMessageBox::about(this, tr("Error"),QString("Workspace was not saved!\n"));
    }
}

/**
 * This slot is called to restore the last workspace from a folder in the file system.
 */
void MainWindow::restoreLastWorkspace()
{
    if(     QFile(m_settings_dir).exists()
        &&  QMessageBox::question(this,
                                  "Restore workspace?",
                                  "Do you want to restore the workspace of the last run of GRAIPE?",
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes)
                == QMessageBox::Yes)
    {
        QApplication::processEvents();
        restoreWorkspace(m_settings_dir + "workspace.xgz");

        //We will reload window settings here, and only here to make sure, that the geometry is updated accordingly
        QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);
        
        //Restore the other vierport and window state settings:
        m_ui.btnWorldView->setChecked(settings.value("geoMode", false).toBool());
            
        //Restore view transform
        QTransform vc_t;
            
        vc_t.setMatrix(
                settings.value("view_m11", 1.0).toFloat(), settings.value("view_m12", 0.0).toFloat(), settings.value("view_m13", 0.0).toFloat(),
                settings.value("view_m21", 0.0).toFloat(), settings.value("view_m22", 1.0).toFloat(), settings.value("view_m23", 0.0).toFloat(),
                settings.value("view_m31", 0.0).toFloat(), settings.value("view_m32", 0.0).toFloat(), settings.value("view_m33", 1.0).toFloat());
        m_view->setTransform(vc_t);
            
        //Restore window geometry & dockWidgets state
        restoreGeometry( settings.value("window_geometry").toByteArray());
        restoreState( settings.value("window_state").toByteArray());
            
        m_view->horizontalScrollBar()->setValue(settings.value("view_hscroll").toInt());
        m_view->verticalScrollBar()->setValue(settings.value("view_vscroll").toInt());

    }
}

/**
 * This slot is called to restore the complete workspace from a folder in the file system.
 */
void MainWindow::restoreWorkspace()
{
    QString xmlFilename = QFileDialog::getOpenFileName(this, tr("Open Workspace from file"));
    
    if(!xmlFilename.isNull())
    {
        restoreWorkspace(xmlFilename);
    }
}

/**
 * This slot is called to restore the complete  from a folder in the file system.
 *
 * \param dirname The dirname of the Workspace serialization.
 */
void MainWindow::restoreWorkspace(const QString& xmlFilename)
{
    try
    {
        reset();
        
        QIODevice* device = Impex::openFile(xmlFilename, QIODevice::ReadOnly);
        
        if(device != NULL)
        {
            QXmlStreamReader xmlReader(device);
            
            m_workspace->deserialize(xmlReader);
            
            for(Model* model : m_workspace->models)
            {
                addModelItemToList(model);
            }
            for(ViewController* vc : m_workspace->viewControllers)
            {
                addViewControllerItemToSceneAndList(vc);
            }

            device->close();
        }
    }
    catch (const std::exception & e)
    {
        QMessageBox::about(this, tr("Error"),QString("Workspace was not restored!\n") + QString::fromUtf8(e.what()));
    }
    catch (...)
    {
        QMessageBox::about(this, tr("Error"),QString("Workspace was not restored!\n"));
    }
}
    
/**
 * The slot creates a new Algorithm item on the Model list for an algorithm instance.
 * This item will persist until the processing is finished.
 *
 * \param alg An instance of teh algorithm to be performed.
 * \return The QListWidgetAlgorithmItem for the model list.
 */
QListWidgetAlgorithmItem* MainWindow::getAlgorithmItem(Algorithm* alg)
{
	for(int i=0; i<m_ui.listModels->count(); i++)
	{
		QListWidgetAlgorithmItem* item = dynamic_cast<QListWidgetAlgorithmItem*>( m_ui.listModels->item(i) );
		
        if (item && item->algorithm() == alg)
		{
        	return item;
        }
	}
	
	return NULL;	
}

/**
 * This slot is called from a running algorithm to update its percentage and
 * status text.
 *
 * \param p The current percentage of execution.
 * \param str Status text.
 */
void MainWindow::algorithmStateChanged(float p, QString str)
{
	Algorithm* alg = static_cast<Algorithm*> (sender());
	
	QListWidgetAlgorithmItem* item = getAlgorithmItem(alg);
   
	item->setText(QString("%1: %2 (%3%)").arg(item->toolTip()).arg(str).arg(p));
}

/**
 * This slot is called from a running algorithm to indicate that it has reached a
 * critical error state.
 *
 * \param str The error text.
 */
void MainWindow::algorithmErrorState(QString str)
{
	Algorithm* alg = static_cast<Algorithm*> (sender());
	
	QListWidgetAlgorithmItem* item = getAlgorithmItem(alg);
	
	QMessageBox::critical(this, "Error in Algorithm run",
								QString("The algorithm run %1 produced one ore more errors during processing.<br>"
										"Error description: %2").arg(item->text()).arg(str));
	
    delete item;
}

/**
 * This slot is called from a running algorithm to indicate that it has
 * finished successfully.
 */
void MainWindow::algorithmFinished()
{
	Algorithm* alg = static_cast<Algorithm*> (sender());
	
	QListWidgetAlgorithmItem* item = getAlgorithmItem(alg);

	for(Model* model : alg->results())
	{
    	addModelItemToList(model);
	}
    
    delete item;
}

/**
 * This slot is called, whenever the text in the status bar needs to be updated.
 *
 * \param str The new text, to be displayed in the status bar.
 */
void MainWindow::updateStatusText(QString str)
{
	this->statusBar()->showMessage(str);
}

/**
 * This slot is called, whenever the text in the lower right dockWidget needs to be updated.
 *
 * \param str The new text, to be displayed in the lower right dockWidget.
 */
void MainWindow::updateStatusDescription(QString str)
{
	m_ui.lblStatusInformation->setText(str);
}

/**
 * Updates the display of the currently used memory.
 */
void MainWindow::updateMemoryUsage()
{
    m_lblMemoryUsage->setText(QString("%1 Models, %2 Views (Memory: %3 MB, max: %4 MB)").arg(m_workspace->models.size()).arg(m_workspace->viewControllers.size()).arg((float)(getCurrentRSS()>>10)/1024).arg((float)(getPeakRSS()>>10)/1024));
}

/**
 * Updates the recently used models list from GRAIPE settings file.
 */
void MainWindow::updateRecentActionList()
{
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);

    //1. Restore the recently opened files
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();

    int itEnd = 0;
    if(recentFilePaths.size() <= m_recentFileCount)
    {
        itEnd = recentFilePaths.size();
    }
    else
    {
        itEnd = m_recentFileCount;
    }
    
    for (int i = 0; i < itEnd; i++)
     {
        QString strippedName = QFileInfo(recentFilePaths.at(i)).fileName();
        m_recentFileActions.at(i)->setText(strippedName);
        m_recentFileActions.at(i)->setToolTip(recentFilePaths.at(i));
        m_recentFileActions.at(i)->setData(recentFilePaths.at(i));
        m_recentFileActions.at(i)->setVisible(true);
    }

    for (int i = itEnd; i < m_recentFileCount; i++)
    {
        m_recentFileActions.at(i)->setVisible(false);
    }
}

/**
 * Adds a filename to the recently used models list.
 * 
 * \param filename The filename of the model.
 */
void MainWindow::addToRecentActionList(const QString &filename)
{
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);
    QStringList recentFilePaths = settings.value("recentFiles").toStringList();
    
    recentFilePaths.removeAll(filename);
    recentFilePaths.prepend(filename);
    
    while (recentFilePaths.size() > m_recentFileCount)
    {
        recentFilePaths.removeLast();
    }
    
    settings.setValue("recentFiles", recentFilePaths);
    updateRecentActionList();
}

/**
 * Load a Model, given by its filename, from the file system.
 * 
 * \param filename The filename of the model.
 */
void MainWindow::loadModel(const QString& filename)
{
    if(!QFile(filename).exists())
    {
        throw std::runtime_error("Loading model from " + filename.toStdString() + " failed. File does not exists");
    }
    
	Model* model = m_workspace->loadModel(filename);
    
    if(model != NULL)
    {
        addModelItemToList(model);
        addToRecentActionList(filename);
    }
    else
    {
        throw std::runtime_error("Failed to load file '"  + filename.toStdString() + "' although it was found as a file!");
    }
}

/**
 * Uses the graipe::core function to find and load all modules into the global factories.
 * Afterwards, it connects to all loaded models and algorithms in the factories.
 */
void MainWindow::initializeFactories()
{
    QString status;
    
    for(QString str : m_workspace->modules_status())
    {
        status += str;
    }
    
    m_status_window->updateStatus(status);
    
    //Connect the model factory to the GUI
    unsigned int i=0;
    for(const ModelFactoryItem& item : m_workspace->modelFactory())
    {
        QAction* newAct = new QAction(item.model_type, this);
        m_ui.menuCreate->addAction(newAct);
        //connect everything
        connect(newAct, SIGNAL(triggered()), m_modSignalMapper, SLOT(map()));
        m_modSignalMapper->setMapping(newAct, i++);
    }
    
	connect(m_modSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(clickedNewModel(int)));
	connect(this, SIGNAL(clickedNewModel(int)), this, SLOT(newModel(int)));
    
    //Connect the algorithm factory to the GUI
	QList<QMenu*> added_menus;
    i=0;
    for(const AlgorithmFactoryItem& item : m_workspace->algorithmFactory())
    {
        QAction* newAct = new QAction(item.algorithm_name, this);
        
        if (item.topic_name == "Import")
        {
            m_ui.menuImport->addAction(newAct);
        }
        else if (item.topic_name == "Export")
        {
            m_ui.menuExport->addAction(newAct);
        }
        else
        {
            //add Menu-entry "topic_name"
            QMenu* algorithm_menu = NULL;
            for(QMenu* m : added_menus)
            {
                if (m->title() == item.topic_name)
                {
                    algorithm_menu = m;
                    break;
                }
            }
            if (!algorithm_menu){
                algorithm_menu = new QMenu(item.topic_name);
                m_ui.menuAlgorithms->addMenu(algorithm_menu);

                added_menus.push_back(algorithm_menu);
            }
            algorithm_menu->addAction(newAct);
        }
        //connect everything
        connect(newAct, SIGNAL(triggered()), m_algSignalMapper, SLOT(map()));
        m_algSignalMapper->setMapping(newAct, i++);
    }
    
	connect(m_algSignalMapper, SIGNAL(mapped(int)), this, SIGNAL(clickedAlgorithm(int)));
	connect(this, SIGNAL(clickedAlgorithm(int)), this, SLOT(runAlgorithm(int)));
}

/**
 * Accessor for the current Model.
 *
 * \return Pointer of the current model. NULL if none.
 */
Model* MainWindow::currentModel()
{
	QListWidgetModelItem* item  = dynamic_cast<QListWidgetModelItem*>(m_ui.listModels->currentItem());
	
	if(item)
	{
    	return item->model();
	}
    else
	{
    	return NULL;
    }
}

/**
 * The type of the current Model.
 *
 * \return The type of the current model. Empt if none.
 */
QString MainWindow::currentModelType()
{
    Model * model = currentModel();
    
	if(model)
    {
		return model->typeName();
	}
    else
	{
    	return QString("");
    }
}

/**
 * Accessor for the current ViewController.
 *
 * \return Pointer of the current ViewController. NULL if none.
 */
ViewController* MainWindow::currentViewController()
{
	QListWidgetViewControllerItem* vc_item  = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->currentItem());
	
	if(vc_item)
	{
    	return vc_item->viewController();
	}
    else
	{
    	return NULL;
    }
}

/**
 * Accessor for the Model of the current ViewController.
 *
 * \return Pointer of the Model of the current ViewController. NULL if none.
 */
Model* MainWindow::currentViewControllerModel()
{
	QListWidgetViewControllerItem* vc_item  = dynamic_cast<QListWidgetViewControllerItem*>(m_ui.listViews->currentItem());
	
	if(vc_item)
	{
    	return vc_item->viewController()->model();
	}
    else
	{
    	return NULL;
    }
}

/**
 * The type of the Model of the current ViewController.
 *
 * \return  The type of the Model of the current ViewController. Empty if none.
 */
QString MainWindow::currentViewControllerModelType()
{
    Model * model = currentViewControllerModel();

    if(model)
	{
    	return model->typeName();
	}
    else
	{
    	return QString("");
    }
}

/**
 * Adds a given Model to the list of models.
 * 
 * \param model The model, which shall be added.
 */
void MainWindow::addModelItemToList(Model* model)
{
	if(model)
	{
		QListWidgetModelItem * model_item = new QListWidgetModelItem(model->name(), model);
		model_item->setToolTip(model->description());
		m_ui.listModels->addItem(model_item);
		connect(model, SIGNAL(modelChanged()), this, SLOT(refreshModelNames()));
        
        if(m_workspace->currentModel() == model)
        {
            m_ui.listModels->setCurrentItem(model_item);
            currentModelChanged(model_item);
        }
        
        updateMemoryUsage();
	}
	else
	{
		qDebug("Error: Request to create Null-Pointer Model/Model-Controller\n");
	}
}

/**
 * Adds a given ViewController to the list of models.
 * 
 * \param viewController The ViewController, which shall be added.
 */
void MainWindow::addViewControllerItemToSceneAndList(ViewController* viewController)
{
    if (viewController && viewController->model())
    {
        //Add item to scene first
        m_scene->addItem(viewController);
        viewController->setZValue(m_ui.listViews->count());
        
        Model* model = viewController->model();
        
        if(m_ui.btnWorldView->isChecked())
        {
            viewController->setTransform(model->globalTransformation());
        }
        else
        {
            viewController->setTransform(model->localTransformation());
            m_scene->setSceneRect(m_scene->itemsBoundingRect());
        }
        connect(viewController, SIGNAL(updateStatusText(QString)), this, SLOT(updateStatusText(QString)));
        connect(viewController, SIGNAL(updateStatusDescription(QString)), this,	SLOT(updateStatusDescription(QString)));
	
        QListWidgetViewControllerItem * vc_item = new QListWidgetViewControllerItem(viewController->typeName() + " of " + model->name(), viewController);
		vc_item->setToolTip(viewController->typeName() + " of " + model->description());
		m_ui.listViews->addItem(vc_item);
        
        vc_item->setCheckState(viewController->isVisible() ? Qt::Checked : Qt::Unchecked);
        
        if(m_workspace->currentViewController() == viewController)
        {
            m_ui.listViews->setCurrentItem(vc_item);
            currentViewControllerChanged(vc_item);
        }
        
        updateMemoryUsage();
	}
	else
	{
		qDebug("Error: Request to create Null-Pointer View\n");
	}
}

/**
 * To enable drag and drop of listwidget-entries, we use a global EventFilter
 * and the global timer, which checks for drags and drops.
 * From the design side, this is really ugly - but it works...
 *
 * \param evt The event itself
 * \return Always false.
 */
 bool MainWindow::eventFilter(QObject *, QEvent *evt)
{
    if( evt->type() == QEvent::Timer)
    {
        layerPositionChange();
    }
    return false;
}

}//end of namespace graipe
