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
 * or auto-loads all modules, that are in same dir as the core-module.
 * Calls loadModules with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle) and at
 * the location of the .app-bundle.
 *
 * \param auto_load If true, it will auto-load all modules. False by default
 */
Environment::Environment(bool auto_load)
{
    if(auto_load)
    {
        //search paths for modules:
        QDir current_dir = QCoreApplication::applicationDirPath();

        //First search at the EXACT dir of the executable - under Mac OS X this means inside the bundle
        //     Graipe.app/Contents/MacOS
        loadModules(QCoreApplication::applicationDirPath());

        //Since this only works after deploying the app, we will also search at the level of the .app directory
        //for loadable modules, iff we found none so far
        if(modules_filenames.empty())
        {
            loadModules(QDir::current());
        }
    }
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
: modelFactory(env.modelFactory),
  viewControllerFactory(env.viewControllerFactory),
  algorithmFactory(env.algorithmFactory),
  modules_filenames(env.modules_filenames),
  modules_status(env.modules_status)
{
    if(reload_factories)
    {
        modules_filenames.clear();
        modules_status.clear();
  
        for(QString file : env.modules_filenames)
        {
            loadModule(file);
        }
    }
}

/**
 * Virtual destructor of an environment
 */
Environment::~Environment()
{
    //TODO do something!
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
    modules_filenames.clear();
    modules_status.clear();
    
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
    
    if (module_initialize)
    {
        
        Module* module = module_initialize();
        
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
                modelFactory.push_back(item);
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
                viewControllerFactory.push_back(item);
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
                algorithmFactory.push_back(item);
            }
            status += "    </ul>\n";
        }
        else
        {
            status += "<li>No algorithms.</li>\n";
        }
        
        status  += "</ul>\n\n";
        
        modules_filenames.push_back(file);
        modules_status.push_back(status);
    }
}


/**
 * Deserialization of an environment from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool Environment::deserialize(QXmlStreamReader& xmlReader)
{
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
                        IntParameter* p_models = new IntParameter("Model count:", 0, 1e10, 0);
                        w_settings.addParameter("models", p_models);
            
                        IntParameter* p_viewControllers = new IntParameter("ViewController count:", 0, 1e10, 0);
                        w_settings.addParameter("viewControllers", p_viewControllers);
                
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
                                        //1. Read in all the saved models
                                        if(xmlReader.name() =="Models")
                                        {
                                            for(int i=0; i!=p_models->value(); i++)
                                            {
                                                Model* m = Impex::loadModel(xmlReader, this);
                                                if(m == NULL)
                                                {
                                                    throw "did not load model!";
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
                                                    for(int i=0; i!=p_viewControllers->value(); i++)
                                                    {
                                                        ViewController* vc = Impex::loadViewController(xmlReader, this);
                                                        if(vc == NULL)
                                                        {
                                                            throw "did not load viewController!";
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
{    try
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
                w_settings.addParameter("models", new IntParameter("Model count:", 0, 1e10, models.size()));
                w_settings.addParameter("viewControllers", new IntParameter("ViewController count:", 0, 1e10, viewControllers.size()));
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

} //end of namespace graipe

/**
 * @}
 */
