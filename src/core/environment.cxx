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

#include "core/environment.hxx"
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
 * Constructor: Creates an empty environment
 * and auto-loads all modules, that are in same dir as the core-module.
 * Calls loadModules with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle) and at
 * the location of the .app-bundle.
 */
Environment::Environment()
: m_currentModel(NULL),
  m_currentViewController(NULL)
{
    findAndLoadModules();
}

/**
 * Copy Constructor: Creates an environment from another one.
 * This contructor copies all the data from the environment, but uses the same
 * (identical factories). If you want to have new Factories, you need to set the 
 * reload_factories flag to true, or call loadModules() after the copy.
 * This will return a clean-copy version, without other's models and viewControllers.
 *
 * \param reload_factories If true, it reload all modules and thus use new factories.
 *                         Else, it will use the other environment's factories.
 *                         False by default
 */
Environment::Environment(const Environment& env, bool reload_factories)
: m_modules_names(env.modules_names()),
  m_modules_status(env.modules_status()),m_modelFactory(env.modelFactory()),
  m_viewControllerFactory(env.viewControllerFactory()),
  m_algorithmFactory(env.algorithmFactory()),
  m_currentModel(NULL),
  m_currentViewController(NULL)
{
    if(reload_factories)
    {
        findAndLoadModules();
    }
}

/**
 * Virtual destructor of an environment
 */
Environment::~Environment()
{
    //TODO: Avoid errors on reset call here: reset();
}

/**
 * Deserialization of an environment from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool Environment::deserialize(QXmlStreamReader& xmlReader)
{
    clearContents();
    
    try
    {
        if(xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == "Environment")
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
        qWarning() << "Environment was not restored!" <<  e.what();
    }
    catch (...)
    {
        qWarning() << "Environment was not restored!";
    }
    return false;
}

/**
 * Serialization on to an output device.
 *
 * \param xmlWriter The QXmlStreamWriter on which we want to serialize.
 */
void Environment::serialize(QXmlStreamWriter& xmlWriter) const
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
            
        xmlWriter.writeStartElement("Environment");
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
        qWarning() << "Environment was not saved!" << e.what();
    }
    catch (...)
    {
        qWarning() << "Environment was not saved!";
    }
}

/**
 * Clear all data structures, lie models and viewControllers,
 * but keeps the modules by means of the factories.
 */
void Environment::clearContents()
{
    for(ViewController* vc : viewControllers)
    {
        delete vc;
        vc=NULL;
    }
    viewControllers.clear();
    
    for(Model* m : models)
    {
        delete m;
        m=NULL;
    }
    models.clear();
}

/**
 * Resets this evironment and clears all data structures, 
 * deletes all objects etc. Also deletes all modules and facrtories.
 */
void Environment::reset()
{
    //resets and reloads all modules
    findAndLoadModules();
    
    //clears all contents
    clearContents();
}

/**
 * Auto-loads all modules, that are in same dir as the core-module.
 * Calls loadModules with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle) and at
 * the location of the .app-bundle.
 */
void Environment::findAndLoadModules()
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
void Environment::loadModules(QDir dir)
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
void Environment::loadModule(QString file)
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

/**
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Model* Environment::loadModel(const QString & filename)
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
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Model* Environment::loadModel(QXmlStreamReader& xmlReader)
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
            qWarning("Environment::loadModel: Model type was not found in modelFactory.");
            return NULL;
        }
        
        if(model->deserialize(xmlReader))
        {
            return model;
        }
        else
        {
            qWarning("Environment::loadModel: Deserialization of Model failed");
            delete model;
            return NULL;
        }
    }
    else
    {
        qWarning("Environment::loadModel: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}

/**
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
ViewController* Environment::loadViewController(const QString & filename)
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
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
ViewController* Environment::loadViewController(QXmlStreamReader & xmlReader)
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
            qWarning("Environment::loadViewController: Model was not found among available ones.");
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
            qWarning("Environment::loadViewController: ViewController type was not found in the factory.");
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
            qWarning("Environment::loadViewController: Deserialization of ViewController failed");
            delete vc;
            return NULL;
        }
    }
    else
    {
        qWarning("Environment::loadViewController: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}
/**
 * Basic import procedure for all types, which implements the serializable interface.
 *
 * \param filename The filename of the stored object.
 * \param object   The object, which shall be deserialized.
 * \param compress If true, the file will be read using the GZip decompressor.
 * \return True, if the loading of the object was successful.
 */
Algorithm* Environment::loadAlgorithm(const QString & filename)
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
 * Basic import procedure of a settings dictionary from a given QString
 * A dictionary is defined by means of a mapping from QString keys
 * to QString values.
 *
 * \param contents The input QString.
 * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
 */
Algorithm* Environment::loadAlgorithm(QXmlStreamReader & xmlReader)
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
            qWarning("Environment::loadAlgorithm: Algorithm was not found among available ones.");
            return NULL;
        }
        
        //4. Restore the parameters
        if(alg->deserialize(xmlReader))
        {
            return alg;
        }
        else
        {
            qWarning("Environment::loadAlgorithm: Deserialization of Algorithm failed");
            delete alg;
            return NULL;
        }
    }
    else
    {
        qWarning("Environment::loadAlgorithm: Could not find a single XML start element!");
        return NULL;
    }
    return NULL;
}

} //end of namespace graipe

/**
 * @}
 */
