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
#include "core/globals.hxx"

#include <QDir>
#include <QMutex>
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

static QMutex global_algorithm_mutex;

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
	m_signalMapper(new QSignalMapper),
	m_status_window(new StatusWindow),
    m_lblMemoryUsage(new QLabel("(Memory: 0 MB, max: 0 MB)")),
    m_recentFileCount(10)
{	
    m_ui.setupUi(this);
	
    m_ui.scrModelParameters->setWidgetResizable(true);
    m_ui.scrViewParameters->setWidgetResizable(true);
    
	/*--------- File menu --------*/
	connect( m_ui.actionLoad_data, SIGNAL(triggered()), this, SLOT(loadModel()));
	
	connect( m_ui.actionSave_current_data, SIGNAL(triggered()), this, SLOT(saveCurrentModel()));
	connect( m_ui.actionShow_current_data, SIGNAL(triggered()), this, SLOT(showCurrentModel()));
	connect( m_ui.actionUnload_current_data, SIGNAL(triggered()), this, SLOT(removeCurrentModel()));
    
    connect( m_ui.actionSave_workspace, SIGNAL(triggered()), this, SLOT(saveWorkspace()));
    connect( m_ui.actionRestore_workspace, SIGNAL(triggered()), this, SLOT(restoreWorkspace()));
    
	connect( m_ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(m_ui.actionExport_View_to_PDF, SIGNAL(triggered()), this, SLOT(saveAsPDF()));
	connect(m_ui.actionExport_View_to_SVG, SIGNAL(triggered()), this, SLOT(saveAsSVG()));
		
	connect( m_ui.actionQuit, SIGNAL(triggered()), this, SLOT(close()));
	
	/*--------- View menu --------*/
	connect( m_ui.actionZoom_in, SIGNAL(triggered()), this, SLOT(zoomIn()));
	connect( m_ui.actionZoom_out, SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect( m_ui.actionNormal_size, SIGNAL(triggered()), this, SLOT(normalSize()));

	
	/*--------- Help menu --------*/
	connect( m_ui.actionHelp, SIGNAL(triggered()), this, SLOT(help()));
	connect( m_ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	
	
	//Views view
	m_ui.btnCreateItem->setMenu(m_ui.menuCreate);
    
	connect(m_ui.btnLoadItem,	SIGNAL(clicked()), this, SLOT(loadModel()));
	connect(m_ui.btnSaveItem,	SIGNAL(clicked()), this, SLOT(saveCurrentModel()));
	connect(m_ui.btnRemoveItem, SIGNAL(clicked()), this, SLOT(removeCurrentModel()));
	
	connect(m_ui.btnRemoveItemView, SIGNAL(clicked()), this, SLOT(removeCurrentViewController()));
    connect(m_ui.btnCenterOnModelView, SIGNAL(clicked()), this, SLOT(centerOnCurrentView()));
	connect(m_ui.btnWorldView,		SIGNAL(clicked()), this, SLOT(updateView()));
	
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
    
    
	statusBar();
    this->statusBar()->addPermanentWidget(m_lblMemoryUsage);
    updateMemoryUsage();
    
	setWindowTitle(tr(name));
    setCentralWidget(m_view);
	
    m_printer = NULL;
	
	m_displayMode = ImageMode;
	
	loadFactories();
    
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
}


/** 
 * Destructor of the main window
 * Here we need to free all alocated memory
 */
MainWindow::~MainWindow()
{
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
                "<p><b>About the GrAphical Image Processing Environment<br/>"
                "a.k.a. GRAIPE</b></p>"
                "<p>Currently installed version: " + full_version_name + "<br/>"
                "GIT version (build): " + git_version + "<br />"
				"Build timestamp: " + __TIMESTAMP__ + "</p>"
                "<p>You may find more information about the currently loaded modules as well "
                "as the log under the menu item: Help.</p>");
}

/**
 * This slot is called every tiem a new data item / Model shall be created.
 */
void MainWindow::newModel()
{
    QAction * act = static_cast<QAction *>(sender());
    QString newModel_typeName = act->text();
    
    for (ModelFactoryItem item : modelFactory)
    {
        if(item.model_type == newModel_typeName)
        {
            Model * new_model = item.model_fptr();
            
            //SHOW EDIT DIALOG
            ModelParameterSelection parameter_selection(this, new_model);
            parameter_selection.setWindowTitle(QString("Create new ")+ act->text());
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
                break;
            }
        }
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
    
    AlgorithmFactoryItem alg_item = algorithmFactory[index];
	
	Algorithm* alg = alg_item.algorithm_fptr();
	alg->setGlobalAlgorithmMutex(&global_algorithm_mutex);
    
	//Create temporary model list for algorithm
	vector<Model*>* alg_object_stack = new vector<Model*>;

	for(int i=0; i!=m_ui.listModels->count(); i++)
	{
		QListWidgetModelItem * model_item = dynamic_cast<QListWidgetModelItem *>(m_ui.listModels->item(i));
		if (model_item && model_item->model() ) 
		{
			alg_object_stack->push_back( model_item->model() );
		}
	}
	
	//Refresh object stack for all params
	alg->parameters()->refresh();
    
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
	
	//After all, we can erase the temporary model stack:
	delete alg_object_stack;
	alg_object_stack = NULL;
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
            ViewControllerFactory vc_possibilities =  viewControllerFactory.filterByModelType(model_item->model());
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
                ViewController* new_vc = vc_possibilities[vc_index].viewController_fptr(m_scene, model_item->model(), m_ui.listViews->count());
                addViewControllerItemToList(new_vc);
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
			ModelFactory model_possibilities =  modelFactory.filterByModelType(model);
			
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
            QString currentModelID;
            
            for(int i=0; i<m_ui.listModels->count(); ++i)
            {
                QListWidgetModelItem* model_item = static_cast<QListWidgetModelItem*>(m_ui.listModels->item(i));
                if(model_item && model_item->model())
                {
                    Model* model = model_item->model();
                    model->setFilename(xmlFilename + QString("/model%1").arg(i));
                    
                    if(model == currentModel())
                    {
                        currentModelID = model->filename();
                    }
                }
            }
            QString currentViewControllerID;
            QStringList visibleViewControllerIDs;
            
            for(int i=0; i<m_ui.listViews->count(); ++i)
            {
                QListWidgetViewControllerItem* vc_item = static_cast<QListWidgetViewControllerItem*>(m_ui.listViews->item(i));
                if(vc_item && vc_item->viewController())
                {
                    ViewController* vc = vc_item->viewController();
                
                    vc->setFilename(xmlFilename + QString("/viewController%1").arg(i));
                
                    if(vc == currentViewController())
                    {
                        currentViewControllerID = vc->filename();
                    }
                    if(vc_item->checkState() == Qt::CheckState::Checked)
                    {
                        visibleViewControllerIDs.append(vc->filename());
                    }
                }
            }
            
            QXmlStreamWriter xmlWriter(device);
            
            xmlWriter.setAutoFormatting(true);
            
            ParameterGroup w_settings;
            
            w_settings.addParameter("geoMode", new BoolParameter("Geographic Mode:",m_displayMode == GeographicMode));
            
            w_settings.addParameter("winGeometry", new LongStringParameter("Window Geometry:",QString(saveGeometry().toBase64())));
            w_settings.addParameter("winState", new LongStringParameter("Window State:",QString(saveState().toBase64())));
            
            QPoint scrolling(m_view->horizontalScrollBar()->value(),m_view->verticalScrollBar()->value());
            w_settings.addParameter("viewScroll", new PointParameter("Viewport Scrolling:",QPoint(-1e10,-1e10), QPoint(1e10,1e10), scrolling));
            
            w_settings.addParameter("viewTrans", new TransformParameter("Viewport transformation", m_view->transform()));
            w_settings.addParameter("models", new IntParameter("Model count:", 0, 1e10, m_ui.listModels->count()));
            w_settings.addParameter("viewControllers", new IntParameter("ViewController count:", 0, 1e10, m_ui.listViews->count()));
            w_settings.addParameter("currentModel", new FilenameParameter("Current model:", currentModelID));
            w_settings.addParameter("currentVC", new FilenameParameter("Current viewController:", currentViewControllerID));
            w_settings.addParameter("activeVCs", new LongStringParameter("Active viewControllers:",visibleViewControllerIDs.join(", ")));
            
            xmlWriter.writeStartDocument();
            
            xmlWriter.writeStartElement("Workspace");
            xmlWriter.writeAttribute("ID", xmlFilename);
            
                xmlWriter.writeStartElement("Header");
                    w_settings.serialize(xmlWriter);
                xmlWriter.writeEndElement();
            
                xmlWriter.writeStartElement("Content");
                    xmlWriter.writeStartElement("Models");
                        for(Model* m : models)
                        {
                            m->serialize(xmlWriter);
                        }
                    xmlWriter.writeEndElement();
                    xmlWriter.writeStartElement("ViewControllers");
                        for(ViewController* vc : viewControllers)
                        {
                            vc->serialize(xmlWriter);
                        }
                    xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
                
            xmlWriter.writeEndElement();
            xmlWriter.writeEndDocument();
            
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
            ParameterGroup w_settings;
            BoolParameter* p_geoMode = new BoolParameter("Geographic Mode:",m_displayMode == GeographicMode);
            w_settings.addParameter("geoMode", p_geoMode);
            
            LongStringParameter* p_winGeo = new LongStringParameter("Window Geometry:",QString(saveGeometry().toBase64()));
            w_settings.addParameter("winGeometry", p_winGeo);
            
            LongStringParameter* p_winState = new LongStringParameter("Window State:",QString(saveState().toBase64()));
            w_settings.addParameter("winState", p_winState);
            
            QPoint scrolling(m_view->horizontalScrollBar()->value(),m_view->verticalScrollBar()->value());
            PointParameter* p_viewScroll = new PointParameter("Viewport Scrolling:",QPoint(-1e10,-1e10), QPoint(1e10,1e10), QPoint(0,0));
            w_settings.addParameter("viewScroll", p_viewScroll);
            
            TransformParameter* p_viewTrans = new TransformParameter("Viewport transformation", m_view->transform());
            w_settings.addParameter("viewTrans", p_viewTrans);
            
            IntParameter* p_models = new IntParameter("Model count:", 0, 1e10, 0);
            w_settings.addParameter("models", p_models);
            
            IntParameter* p_viewControllers = new IntParameter("ViewController count:", 0, 1e10, 0);
            w_settings.addParameter("viewControllers", p_viewControllers);
            
            FilenameParameter* p_currentModel = new FilenameParameter("Current model:", "");
            w_settings.addParameter("currentModel", p_currentModel);
            
            FilenameParameter* p_currentVC = new FilenameParameter("Current viewController:", "");
            w_settings.addParameter("currentVC", p_currentVC);
            
            LongStringParameter* p_activeVCs = new LongStringParameter("Active viewControllers:","");
            w_settings.addParameter("activeVCs",p_activeVCs);
        
            QXmlStreamReader xmlReader(device);
        
            if(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() =="Workspace")
                {
                    if(xmlReader.readNextStartElement())
                    {
                        if(xmlReader.name() =="Header")
                        {
                            w_settings.deserialize(xmlReader);
                        }
                        //Read until </Header> comes....
                        while(true)
                        {
                            if(!xmlReader.readNext())
                            {
                                throw "Error: XML at end before Header End-Tag";
                            }
                            
                            if(xmlReader.isEndElement() && xmlReader.name() == "Header")
                            {
                                break;
                            }
                        }
                        if(xmlReader.readNextStartElement())
                        {
                            if(xmlReader.name() =="Content")
                            {
                                if(xmlReader.readNextStartElement())
                                {
                                    //1. Read in all the saved models
                                    if(xmlReader.name() =="Models")
                                    {
                                        for(int i=0; i!=p_models->value(); i++)
                                        {
                                            Model* m = Impex::loadModel(xmlReader);
                                            if(m != NULL)
                                            {
                                                addModelItemToList(m);
                                                if(m->filename() == p_currentModel->value())
                                                {
                                                    m_ui.listModels->setCurrentRow(m_ui.listModels->count()-1);
                                                }
                                            }
                                        }
                                        
                                        //Read until </Models> comes....
                                        while(true)
                                        {
                                            if(xmlReader.isEndElement() && xmlReader.name() == "Models")
                                            {
                                                break;
                                            }
                                            if(!xmlReader.readNext())
                                            {
                                                throw "Error: XML at end before Models End-Tag";
                                            }
                                        }
                                        if(xmlReader.readNextStartElement())
                                        {
                                            //2. Read in all the saved views
                                            if(xmlReader.name() =="ViewControllers")
                                            {
                                                QStringList activeVCs = p_activeVCs->value().split(", ");
                                            
                                                for(int i=0; i!=p_viewControllers->value(); i++)
                                                {
                                                    ViewController* vc = Impex::loadViewController(xmlReader, m_scene);
                                                    if(vc != NULL)
                                                    {
                                                        addViewControllerItemToList(vc);
                                                        
                                                        if(vc->filename() == p_currentVC->value())
                                                        {
                                                            m_ui.listViews->setCurrentRow(m_ui.listViews->count()-1);
                                                        }
                                                        Qt::CheckState state = Qt::CheckState::Unchecked;
                                                        if(activeVCs.contains(vc->filename()))
                                                        {
                                                            state = Qt::CheckState::Checked;
                                                        }
                                                        m_ui.listViews->item(m_ui.listViews->count()-1)->setCheckState(state);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                         }
                    }
                    //3. Restore the other vierport and window state settings:
                    m_ui.btnWorldView->setChecked(p_geoMode->value());
                    
                    m_view->setTransform(p_viewTrans->value());
                    
                    QByteArray block;
                    block.append(p_winGeo->value());
                    restoreGeometry(QByteArray::fromBase64(block));
                    
                    block.clear();
                    block.append(p_winState->value());
                    restoreState(QByteArray::fromBase64(block));
        
                    m_view->horizontalScrollBar()->setValue(p_viewScroll->value().x());
                    m_view->verticalScrollBar()->setValue(p_viewScroll->value().y());
                }
            }
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
 * This slot is called to load a view and maybe the depending objects from file system.
 *
 * \param filename The filename of the ViewController serialization.
 */
void MainWindow::loadViewController(const QString& filename)
{
    //1. Collect models in vector
    std::vector<Model*> all_models;
    for(int i=0;  i!=m_ui.listModels->count(); ++i)
    {
        QListWidgetModelItem* model_item = static_cast<QListWidgetModelItem*>(m_ui.listModels->item(i));
        
        if(model_item  &&  model_item->model())
        {
            all_models.push_back(model_item->model());
        }
    }
    
    ViewController * vc = Impex::loadViewController(filename, m_scene);
    
    //  If it was not found: Indicate error
    if(vc != NULL)
    {
        if(m_ui.btnWorldView->isChecked())
        {
            vc->setTransform(vc->model()->globalTransformation());
        }
        else
        {
            vc->setTransform(vc->model()->localTransformation());
            m_scene->setSceneRect(m_scene->itemsBoundingRect());
        }
        connect(vc, SIGNAL(updateStatusText(QString)), this, SLOT(updateStatusText(QString)));
        connect(vc, SIGNAL(updateStatusDescription(QString)), this,	SLOT(updateStatusDescription(QString)));
        
        addViewControllerItemToList(vc);
    }
    else
    {
        throw std::runtime_error("ViewController type could not be loaded.");
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
	//qDebug() << str << " percentage: " << p; 
	
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
	//qDebug() << "Error state reached: " << str; 
	
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
    m_lblMemoryUsage->setText(QString("(Memory: %1 MB, max: %2 MB)").arg((float)(getCurrentRSS()>>10)/1024).arg((float)(getPeakRSS()>>10)/1024));
}

/**
 * Updates the recently used models list from GRAIPE settings file.
 */
void MainWindow::updateRecentActionList()
{
    QSettings settings(m_settings_dir + "graipe.ini",QSettings::IniFormat);
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
    
	Model* model = Impex::loadModel(filename);
    
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
 * Find all available modules and fill the corresponding registries with their
 * contributions. Calls loadFactoriesFromDirectory with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle) and at
 * the location of the .app-bundle.
 */
void MainWindow::loadFactories()
{
	//search paths for modules:
	QDir		current_dir		= QCoreApplication::applicationDirPath();
	QList<QMenu*> added_menus;
	
    //First search at the EXACT dir of the executable - under Mac OS X this means inside the bundle
    //     Graipe.app/Contents/MacOS
    QString report = loadFactoriesFromDirectory(QCoreApplication::applicationDirPath(), added_menus);
    
    //Since this only works after deploying the app, we will also search at the level of the .app directory
    //for loadable modules, iff we found none so far
    if(report.isEmpty())
    {
        report = loadFactoriesFromDirectory(QDir::current(), added_menus);
    }
    m_status_window->updateStatus(report);
    
	connect(m_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(clickedAlgorithm(int)));
	connect(this, SIGNAL(clickedAlgorithm(int)), this, SLOT(runAlgorithm(int)));	
}

/**
 * Find all available modules in a directory and fill the corresponding registries with their
 * contributions.
 *
 * \param dir The directory to search for modules.
 * \param added_menus Menus added during the module load.
 */
QString MainWindow::loadFactoriesFromDirectory(const QDir & current_dir, QList<QMenu*> & added_menus)
{
    QString ss;
	
	QStringList all_files		= current_dir.entryList(QDir::Files);
    
    for(const QString& file : all_files)
	{
		//ignore symlinks
		QString symTarget = QFile::symLinkTarget(file);
		
		if(symTarget.isEmpty() && QLibrary::isLibrary(file))
		{
			typedef  Module* (*Initialize_f) ();
			Initialize_f module_initialize = (Initialize_f) QLibrary::resolve(current_dir.absoluteFilePath(file), "initialize");
			
			if (module_initialize)
			{
				Module* module = module_initialize();
				
				ss  += "<h3>" + module->name() + "</h3>\n"
                     + "<ul>\n"
                     + "  <li>loaded from: " + current_dir.absoluteFilePath(file) + "</li>\n";
				
				ModelFactory model_items = module->modelFactory();
				
                if(model_items.size() != 0)
                {
                    ss += "  <li>Models:</li>\n"
                          "    <ul>\n";
                    
                    for(const ModelFactoryItem& item : model_items)
                    {
                        ss += "      <li>" + item.model_type  + "</li>\n";
                        modelFactory.push_back(item);
                        
                        QAction* newAct = new QAction(item.model_type, this);
                        m_ui.menuCreate->addAction(newAct);
                        connect(newAct, SIGNAL(triggered()), this, SLOT(newModel()));
                    }
                    
                    ss += "    </ul>\n";
                }
                else
                {
                    ss += "<li>No models.</li>\n";
                }
                
				ViewControllerFactory vc_items = module->viewControllerFactory();
                
                if(vc_items.size() != 0)
                {
                    ss += "  <li>ViewControllers:</li>\n"
                          "    <ul>\n";
                    
                    for(const ViewControllerFactoryItem& item : vc_items)
                    {
                        ss += "      <li>" + item.viewController_name + " <i>(for model: " + item.model_type + ")</i></li>\n";
                        viewControllerFactory.push_back(item);
                    }
                    
                    ss += "    </ul>\n";
                }
                else
                {
                    ss += "<li>No ViewControllers.</li>\n";
                }
                
				std::vector<AlgorithmFactoryItem> alg_items = module->algorithmFactory();
                
                if(alg_items.size() != 0)
                {
                    ss += "  <li>Algorithms:</li>\n"
                          "    <ul>\n";
                    
                    for(const AlgorithmFactoryItem& item : alg_items)
                    {
                        ss += "      <li>" + item.algorithm_name + " <i>(for topic: " + item.topic_name  + ")</i></li>\n";
                        algorithmFactory.push_back(item);
                        
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
                        connect(newAct, SIGNAL(triggered()), m_signalMapper, SLOT(map()));
                        m_signalMapper->setMapping(newAct, (unsigned int)(algorithmFactory.size()-1));
                    }
                    ss += "    </ul>\n";
                }
                else
                {
                    ss += "<li>No algorithms.</li>\n";
                }
                
                ss  += "</ul>\n\n";
				
			}
		}			
	}
    return ss;
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
		return item->model();
	else
		return NULL;
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
		return model->typeName();
	else
		return QString("");
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
		return vc_item->viewController();
	else
		return NULL;
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
		return vc_item->viewController()->model();
	else
		return NULL;
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
		return model->typeName();
	else
		return QString("");
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
		QListWidgetModelItem * model_item = new QListWidgetModelItem(model->name(), model );
		model_item->setToolTip(model->description());
		m_ui.listModels->addItem(model_item);
		connect(model, SIGNAL(modelChanged()), this, SLOT(refreshModelNames()));
        
        //m_ui.listModels->setCurrentItem(model_item);
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
void MainWindow::addViewControllerItemToList(ViewController* viewController)
{
    if (viewController && viewController->model())
    {
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
		vc_item->setCheckState(Qt::Checked);
		vc_item->setToolTip(viewController->typeName() + " of " + model->description());
		m_ui.listViews->addItem(vc_item);
        m_ui.listViews->setCurrentItem(vc_item);
        //m_view->centerOn(viewController);
        currentViewControllerChanged(vc_item);
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
