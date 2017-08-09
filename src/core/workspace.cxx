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

#include "core/workspace.hxx"
#include "core/impex.hxx"
#include "core/module.hxx"

#include <QCoreApplication>
#include <QLibrary>

/**
 * @file
 * @brief Implementation file for the global vars
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Constructor: Creates an empty Workspace and calls the function
 * findAndloadModules() to load the Modules from the corresponding
 * directories.
 */
Workspace::Workspace()
: m_currentModel(NULL),
  m_currentViewController(NULL)
{
    findAndLoadModules();
}

/**
 * Copy Constructor: Creates an workspace from another one.
 * This contructor copies all the data from the workspace, but uses the same
 * (identical) factories. 
 * If you want to have new Factories, you  will need to set the
 * reload_factories flag to true, or call loadModules() after the copying.
 *
 * \param wsp The other workspace.
 * \param reload_factories If true, it reload all modules and thus use new factories.
 *                         Else, it will use the other environment's factories.
 *                         False by default
 */
Workspace::Workspace(const Workspace& wsp, bool reload_factories)
: m_modules_names(wsp.modules_names()),
  m_modules_status(wsp.modules_status()),m_modelFactory(wsp.modelFactory()),
  m_viewControllerFactory(wsp.viewControllerFactory()),
  m_algorithmFactory(wsp.algorithmFactory()),
  m_currentModel(NULL),
  m_currentViewController(NULL)
{
    if(reload_factories)
    {
        findAndLoadModules();
    }
}

/**
 * Virtual destructor of an workspace.
 */
Workspace::~Workspace()
{
    clear();
}

/**
 * Deserialization of a workspace from an xml file.
 * This will copy all the data from the workspace, but use the same
 * (identical) factories. If you want to have new Factories, you will 
 * need to call loadModules() after the copy manually. However, it checks
 * if all modules of the serialization are avaible in the current workspace.
 *      
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool Workspace::deserialize(QXmlStreamReader& xmlReader)
{
    clear();
    
    try
    {
        if(xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == "Workspace")
            {
                if(xmlReader.readNextStartElement())
                {
                    if(xmlReader.name() =="Header")
                    {
                        ParameterGroup w_settings;
                        
                        IntParameter* p_modules = new IntParameter("Module count:", 0, 1e10, 0);
                        w_settings.addParameter("modules", p_modules);
                        
                        IntParameter* p_models = new IntParameter("Model count:", 0, 1e10, 0);
                        w_settings.addParameter("models", p_models);
            
                        IntParameter* p_viewControllers = new IntParameter("ViewController count:", 0, 1e10, 0);
                        w_settings.addParameter("viewControllers", p_viewControllers);
                        
                        StringParameter* p_currentModel = new StringParameter("Current Model:","0");
                        w_settings.addParameter("currentModel", p_currentModel);
            
                        StringParameter* p_currentViewController = new StringParameter("Current ViewController:", "0");
                        w_settings.addParameter("currentViewController", p_currentViewController);
                
                        if(w_settings.deserialize(xmlReader))
                        {
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
                                        //1. Restor in all the modules
                                        if(xmlReader.name() =="Modules")
                                        {
                                            for(int i=0; i!=p_modules->value(); i++)
                                            {
                                                xmlReader.readNextStartElement();
                                                QString module_name = xmlReader.readElementText();
                                                
                                                if(!modules_names().contains(module_name))
                                                {
                                                    throw "Error: Module " + module_name + " was needed but not already loaded...";
                                                }
                                            }
                                            
                                            //Read until </Modules> comes....
                                            while(true)
                                            {
                                                if(xmlReader.isEndElement() && xmlReader.name() == "Modules")
                                                {
                                                    break;
                                                }
                                                if(!xmlReader.readNext())
                                                {
                                                    throw "Error: XML at end before Modules End-Tag";
                                                }
                                            }

                                            if(xmlReader.readNextStartElement())
                                            {
                                                //2. Read in all the saved models
                                                if(xmlReader.name() =="Models")
                                                {
                                                    for(int i=0; i!=p_models->value(); i++)
                                                    {
                                                        Model* m = loadModel(xmlReader);
                                                        if(m == NULL)
                                                        {
                                                            throw "did not load model!";
                                                        }
                                                        
                                                        if(m->id() == p_currentModel->value())
                                                        {
                                                            m_currentModel = m;
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
                                                        //3. Read in all the saved views
                                                        if(xmlReader.name() =="ViewControllers")
                                                        {
                                                            for(int i=0; i!=p_viewControllers->value(); i++)
                                                            {
                                                                ViewController* vc = loadViewController(xmlReader);
                                                                if(vc == NULL)
                                                                {
                                                                    throw "did not load viewController!";
                                                                }
                                                                
                                                                if(vc->id() == p_currentViewController->value())
                                                                {
                                                                    m_currentViewController = vc;
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                             }
                        }
                    }
                }
                return true;
            }
        }
    }
    catch (const std::exception & e)
    {
        qWarning() << "Workspace was not restored!" <<  e.what();
    }
    catch (...)
    {
        qWarning() << "Workspace was not restored!";
    }
    return false;
}

/**
 * Serialization on to an output device.
 * This performes an XML-serialization of all Modules (by their names),
 * all loaded Models and all ViewControllers.
 *
 * \param xmlWriter The QXmlStreamWriter on which we want to serialize.
 */
void Workspace::serialize(QXmlStreamWriter& xmlWriter) const
{
    try
    {
        //Transform memory address to ID for models and viewControllers:
        for(Model* model : models)
        {
            model->setID(QString::number((long int) model));
        }
        for(ViewController* vc : viewControllers)
        {
            vc->setID(QString::number((long int) vc));
        }
        
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
            
        xmlWriter.writeStartElement("Workspace");
        xmlWriter.writeAttribute("ID", id());
            
            xmlWriter.writeStartElement("Header");
                ParameterGroup w_settings;
                w_settings.addParameter("modules", new IntParameter("Module count:", 0, 1e10, modules_names().size()));
                w_settings.addParameter("models", new IntParameter("Model count:", 0, 1e10, models.size()));
                w_settings.addParameter("viewControllers", new IntParameter("ViewController count:", 0, 1e10, viewControllers.size()));
                w_settings.addParameter("currentModel",
                                        new StringParameter("Current Model:", (m_currentModel == NULL) ? "0" : m_currentModel->id()));
                w_settings.addParameter("currentViewController",
                                        new StringParameter("Current ViewController:", (m_currentViewController == NULL) ? "0" : m_currentViewController->id()));
                w_settings.serialize(xmlWriter);
            xmlWriter.writeEndElement();
            
            xmlWriter.writeStartElement("Content");
        
                xmlWriter.writeStartElement("Modules");
                for(QString name : modules_names())
                {
                    xmlWriter.writeTextElement("Module", name);
                }
                xmlWriter.writeEndElement();
        
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
    }
    catch (std::exception & e)
    {
        qWarning() << "Workspace was not saved!" << e.what();
    }
    catch (...)
    {
        qWarning() << "Workspace was not saved!";
    }
}

/**
 * Clear all data structures, namely: Models and ViewControllers,
 * but keeps the Modules by means of the factories.
 * If you want to reload the factories, you will need to call loadModules
 * manually afterwards.
 */
void Workspace::clear()
{
    for(ViewController* vc : viewControllers)
    {
        vc->deleteLater();
    }
    viewControllers.clear();
    
    for(Model* m : models)
    {
        m->deleteLater();
    }
    models.clear();
}

/**
 * Import procedure for available Models from a filename.
 *
 * \param filename The filename of the stored Model.
 * \return A valid pointer to a new Model, if the loading of the Model was successful.
 *         else: a null pointer.
 */
Model* Workspace::loadModel(const QString & filename)
{
   QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
   Model* model = NULL;

    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        model = loadModel(xmlReader);
        device->close();
    }
    
    return model;
}

/**
 * Basic import procedure for available Models from an XMLStream.
 *
 * \param xmlReader The QXmlStreamReader of the stored Model.
 * \return A valid pointer to a new Model, if the loading of the Model was successful.
 *         else: a null pointer.
 */
Model* Workspace::loadModel(QXmlStreamReader& xmlReader)
{
    //1. Read the name of the xml root
    if(xmlReader.readNextStartElement())
    {
        //First start element: ViewController's name
        QString mod_type = xmlReader.name().toString();

        //3. Create a model using the mod_type and the modelFactory:
        Model* model = NULL;
        
        for(unsigned int i=0; i<modelFactory().size(); ++i)
        {
            if(modelFactory()[i].model_type==mod_type)
            {
                model = modelFactory()[i].model_fptr(this);
                break;
            }
        }
        
        //  If it was not found: Indicate error
        if(model == NULL)
        {
            qWarning("Workspace::loadModel: Model type was not found in modelFactory.");
            return NULL;
        }
        
        if(model->deserialize(xmlReader))
        {
            return model;
        }
        else
        {
            qWarning("Workspace::loadModel: Deserialization of Model failed");
            delete model;
            return NULL;
        }
    }
    else
    {
        qWarning("Workspace::loadModel: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}

/**
 * Import procedure for available ViewControllers from a filename.
 *
 * \param filename The filename of the stored ViewController.
 * \return A valid pointer to a new ViewController, if the loading of the ViewController was successful.
 *         else: a null pointer.
 */
ViewController* Workspace::loadViewController(const QString & filename)
{
    QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
    ViewController * vc = NULL;
    
    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        vc = loadViewController(xmlReader);
        device->close();
    }
    
    return vc;
}

/**
 * Import procedure for available ViewControllers from an XMLStream.
 *
 * \param xmlReader The QXmlStreamReader of the stored ViewController.
 * \return A valid pointer to a new ViewController, if the loading of the ViewController was successful.
 *         else: a null pointer.
 */
ViewController* Workspace::loadViewController(QXmlStreamReader & xmlReader)
{
    ViewController * vc = NULL;

    //1. Read the root attributes of the xml node as well as the name of the root
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.attributes().hasAttribute("ModelID")
        &&  xmlReader.attributes().hasAttribute("ZOrder"))
    {
        //First start element: ViewController's name
        QString vc_type = xmlReader.name().toString();
        QString vc_modelID = xmlReader.attributes().value("ModelID").toString();
        int vc_zorder = xmlReader.attributes().value("ZOrder").toInt();

        //2. Find the associated model at the model_list (if it was already loaded)
        Model* vc_model = NULL;
        for(Model* mod : models)
        {
            if(     mod
                &&  mod->id() == vc_modelID)
            {
                vc_model = mod;
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc_model == NULL)
        {
            qWarning("Workspace::loadViewController: Model was not found among available ones.");
            return NULL;
        }
        
         //3. Create a controller using the vc_type and the model found above:
        for(unsigned int i=0; i<viewControllerFactory().size(); ++i)
        {
            if(viewControllerFactory()[i].viewController_name==vc_type)
            {
                vc = viewControllerFactory()[i].viewController_fptr(vc_model);
                break;
            }
        }
        //  If it was not found: Indicate error
        if(vc == NULL)
        {
            qWarning("Workspace::loadViewController: ViewController type was not found in the factory.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(vc->deserialize(xmlReader))
        {
            vc->setZValue(vc_zorder);
            return vc;
        }
        else
        {
            qWarning("Workspace::loadViewController: Deserialization of ViewController failed");
            delete vc;
            return NULL;
        }
    }
    else
    {
        qWarning("Workspace::loadViewController: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}
    
/**
 * Import procedure for available Algorithms from a filename.
 *
 * \param filename The filename of the stored Algorithm.
 * \return A valid pointer to a new Algorithm, if the loading of the Algorithm was successful.
 *         else: a null pointer.
 */
Algorithm* Workspace::loadAlgorithm(const QString & filename)
{
    QIODevice* device = Impex::openFile(filename, QIODevice::ReadOnly);
    Algorithm * alg = NULL;
    
    if(device != NULL)
    {
        QXmlStreamReader xmlReader(device);
        alg = loadAlgorithm(xmlReader);
        device->close();
    }
    
    return alg;
}

/**
 * Import procedure for available Algorithms from an XMLStream.
 *
 * \param xmlReader The QXmlStreamReader of the stored Algorithms.
 * \return A valid pointer to a new Algorithm, if the loading of the Algorithm was successful.
 *         else: a null pointer.
 */
Algorithm* Workspace::loadAlgorithm(QXmlStreamReader & xmlReader)
{
    Algorithm * alg = NULL;

    //1. Read the root attributes of the xml node as well as the name of the root
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.attributes().hasAttribute("ID"))
    {
        //First start element: ViewController's name
        QString alg_type = xmlReader.name().toString();

        //Create an algorithm using the alg_type:
        for(unsigned int i=0; i<algorithmFactory().size(); ++i)
        {
            if(algorithmFactory()[i].algorithm_type==alg_type)
            {
                alg = algorithmFactory()[i].algorithm_fptr(this);
                break;
            }
        }
        //If it was not found: Indicate error
        if(alg == NULL)
        {
            qWarning("Workspace::loadAlgorithm: Algorithm was not found among available ones.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(alg->deserialize(xmlReader))
        {
            return alg;
        }
        else
        {
            qWarning("Workspace::loadAlgorithm: Deserialization of Algorithm failed");
            delete alg;
            return NULL;
        }
    }
    else
    {
        qWarning("Workspace::loadAlgorithm: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}
/**
 * Returns the currently active Model. Change it using 
 * setCurrentModel(model).
 * 
 * \return A valid pointer to a Model, if any was set as a current model.
 *         Else: a null pointer.
 */
Model* Workspace::currentModel()
{
    for(Model* m: models)
    {
        if(m_currentModel == m)
        {
            return m_currentModel;
        }
    }
    
    //Model not found, reset the local pointer:
    m_currentModel=NULL;
    
    //And return it:
    return m_currentModel;
}

/**
 * Sets the currently active Model. Does nothing if the given model is not
 * among the the available ones.
 *
 * \param model The Model, which shall become the next current model.
 */
void Workspace::setCurrentModel(Model * model)
{
    for(Model* m : models)
    {
        if(model == m)
        {
            m_currentModel = m;
        }
    }
}

/**
 * Returns the currently active ViewController. Change it using 
 * setCurrentViewController(vc).
 * 
 * \return A valid pointer to a ViewController, if any was set as a 
 *         current ViewController.
 *         Else: a null pointer.
 */
ViewController* Workspace::currentViewController()
{
    for(ViewController* vc : viewControllers)
    {
        if(m_currentViewController == vc)
        {
            return m_currentViewController;
        }
    }
    
    //ViewController not found, reset the local pointer:
    m_currentViewController=NULL;
    
    //And return it:
    return m_currentViewController;
}

/**
 * Sets the currently active ViewController. Does nothing if the given 
 * ViewController is not among the the available ones.
 *
 * \param viewController The ViewController, which shall become the next
 *                       current ViewController.
 */
void Workspace::setCurrentViewController(ViewController * viewController)
{
    for(ViewController* vc : viewControllers)
    {
        if(viewController == vc)
        {
            m_currentViewController = vc;
        }
    }
}

/**
 * Auto-loads all modules, that are in same dir as the core-module.
 * Calls loadModules with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle).
 * If there were no Modules found there, it also searches at the location
 * of the .app-bundle itself.
 */
void Workspace::findAndLoadModules()
{
    //search paths for modules:
    QDir current_dir = QCoreApplication::applicationDirPath();

    //First search at the EXACT dir of the executable - under Mac OS X this means inside the bundle
    //     Graipe.app/Contents/MacOS
    loadModules(QCoreApplication::applicationDirPath());

    //Since this only works after deploying the app, we will also search at the level of the .app directory
    //for loadable modules, iff we found none so far
    if(modules_names().empty())
    {
        loadModules(QDir::current());
    }
}

/**
 * Find all available modules in a directory and fill the corresponding registries with their
 * contributions. SymLinks are not loaded to avoid double loading. Updates the report
 * property of this class.
 *
 * \param dir The directory to search for modules.
 */
void Workspace::loadModules(QDir dir)
{
    m_modules_names.clear();
    m_modules_status.clear();
    
    m_modelFactory.clear();
    m_viewControllerFactory.clear();
    m_algorithmFactory.clear();
    
    for(const QString& file : dir.entryList(QDir::Files))
    {
        //ignore symlinks
        QString symTarget = QFile::symLinkTarget(file);
        
        if(symTarget.isEmpty() && QLibrary::isLibrary(file))
        {
            loadModule(dir.absoluteFilePath(file));
        }
    }
}

/**
 * Load one module and fill the corresponding registries with its contributions.
 * Also (incrementally updates the report property of this class.
 *
 * \param file The filename of the module.
 */
void Workspace::loadModule(QString file)
{
    typedef  Module* (*Initialize_f) ();
    Initialize_f module_initialize = (Initialize_f) QLibrary::resolve(file, "initialize");
    
    QString status;
    QString name;
    
    if (module_initialize)
    {
        Module* module = module_initialize();
        
        name = module->name();
        
        status  += "<h3>" + module->name() + "</h3>\n"
                + "<ul>\n"
                + "  <li>loaded from: " + file + "</li>\n";
        
        ModelFactory model_items = module->modelFactory();
        
        if(model_items.size() != 0)
        {
            status += "  <li>Models:</li>\n"
                      "    <ul>\n";
            
            for(const ModelFactoryItem& item : model_items)
            {
                status += "      <li>" + item.model_type  + "</li>\n";
                m_modelFactory.push_back(item);
            }
            
            status += "    </ul>\n";
        }
        else
        {
            status += "<li>No models.</li>\n";
        }
        
        ViewControllerFactory vc_items = module->viewControllerFactory();
        
        if(vc_items.size() != 0)
        {
            status += "  <li>ViewControllers:</li>\n"
                  "    <ul>\n";
            
            for(const ViewControllerFactoryItem& item : vc_items)
            {
                status += "      <li>" + item.viewController_name + " <i>(for model: " + item.model_type + ")</i></li>\n";
                m_viewControllerFactory.push_back(item);
            }
            
            status += "    </ul>\n";
        }
        else
        {
            status += "<li>No ViewControllers.</li>\n";
        }
        
        std::vector<AlgorithmFactoryItem> alg_items = module->algorithmFactory();
        
        if(alg_items.size() != 0)
        {
            status += "  <li>Algorithms:</li>\n"
                  "    <ul>\n";
            
            for(const AlgorithmFactoryItem& item : alg_items)
            {
                status += "      <li>" + item.algorithm_name + " <i>(for topic: " + item.topic_name  + ")</i></li>\n";
                m_algorithmFactory.push_back(item);
            }
            status += "    </ul>\n";
        }
        else
        {
            status += "<li>No algorithms.</li>\n";
        }
        
        status  += "</ul>\n\n";
        
        m_modules_names.push_back(name);
        m_modules_status.push_back(status);
    }
}

} //end of namespace graipe

/**
 * @}
 */