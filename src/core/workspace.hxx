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

#ifndef GRAIPE_CORE_ENVIRONMENT_HXX
#define GRAIPE_CORE_ENVIRONMENT_HXX

#include "core/factories.hxx"
#include "core/model.hxx"
#include "core/module.hxx"
#include "core/viewcontroller.hxx"

#include <QDir>
#include <QCoreApplication>
#include <QLibrary>

#include <vector>

/**
 * @file
 * @brief This file holds all global data structures
 * @addtogroup core
 * @{
 */
 
namespace graipe {

class GRAIPE_CORE_EXPORT Workspace
: public Serializable
{
    public:
        /**
         * Constructor: Creates an empty workspace
         * and auto-loads all modules, that are in same dir as the core-module.
         * Calls loadModules with different paths:
         * Under Mac OS at the place of the executable file (not the app-Bundle) and at
         * the location of the .app-bundle.
         *
         * \param auto_load If true, it will auto-load all modules. False by default
         */
        Workspace();
    
        /**
         * Virtual destructor of an workspace
         */
        virtual ~Workspace();
    
        /**
         * Copy Constructor: Creates an workspace from another one.
         * This contructor copies all the data from the workspace, but uses the same
         * (identical factories). If you want to have new Factories, you need to set the 
         * reload_factories flag to true, or call loadModules() after the copy.
         * This will return a clean-copy version, without other's models and viewControllers.
         *
         * \param reload_factories If true, it reload all modules and thus use new factories.
         *                         Else, it will use the other environment's factories.
         *                         False by default
         */
        Workspace(const Workspace& wsp, bool reload_factories=false);

        /**
         * The typename of this class.
         *
         * \return Always "Workspace" as a QString
         */
        QString typeName() const
        {
            return "Workspace";
        }
    
        /**
         * Deserialization of an workspace from an xml file.
         *
         * \param xmlReader The QXmlStreamReader, where we read from.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QXmlStreamReader& xmlReader);
    
        /**
         * Serialization on to an output device.
         *
         * \param xmlWriter The QXmlStreamWriter on which we want to serialize.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;

        /**
         * Clear all data structures, lie models and viewControllers,
         * but keeps the modules by means of the factories.
         */
        void clearContents();
    
        /**
         * Resets this evironment and clears all data structures, 
         * deletes all objects etc. Also deletes all modules and facrtories.
         */
        void reset();
    
        //Three global variables for the factories:
        const ModelFactory& modelFactory() const
        {
            return m_modelFactory;
        }
        const ViewControllerFactory& viewControllerFactory() const
        {
            return m_viewControllerFactory;
        }
        const AlgorithmFactory& algorithmFactory() const
        {
            return m_algorithmFactory;
        }
    
        /**
         * Basic import procedure for all types, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        Model* loadModel(const QString & filename);
    
        /**
         * Basic import procedure for all types, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        Model* loadModel(QXmlStreamReader & xmlReader);
    
        /**
         * Basic import procedure for all types, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        ViewController* loadViewController(const QString & filename);

        /**
         * Basic import procedure of a settings dictionary from a given QString
         * A dictionary is defined by means of a mapping from QString keys
         * to QString values.
         *
         * \param contents The input QString.
         * \param separator The seaparator, which will be used to split the key/value pairs, default is ": "
         */
        ViewController* loadViewController(QXmlStreamReader & xmlReader);
    
        /**
         * Basic import procedure for all algorithms, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        Algorithm* loadAlgorithm(const QString & filename);
    
        /**
         * Basic import procedure for all algorithms, which implement the serializable interface
         *
         * \param filename The filename of the stored object.
         * \param object   The object, which shall be deserialized.
         * \param compress If true, the file will be read using the GZip decompressor.
         * \return True, if the loading of the object was successful.
         */
        Algorithm* loadAlgorithm(QXmlStreamReader & xmlReader);
    
        //One global algorithm mutex:
        QMutex global_algorithm_mutex;

        //And two containers holding all currently available Models and ViewControllers:
        std::vector<Model*> models;
        std::vector<ViewController*> viewControllers;
    
        //The filenames of each loaded module:
        const QStringList& modules_names() const
        {
            return m_modules_names;
        }
        //The status messages of each loaded module
        const QStringList& modules_status() const
        {
            return m_modules_status;
        }
    
        Model* currentModel()
        {
            return m_currentModel;
        }
        void setCurrentModel(Model * model)
        {
            for(Model* m : models)
            {
                if(model == m)
                {
                    m_currentModel = m;
                }
            }
        }
    
    
        ViewController* currentViewController()
        {
            return m_currentViewController;
        }
        void setCurrentViewController(ViewController * viewController)
        {
            for(ViewController* vc : viewControllers)
            {
                if(viewController == vc)
                {
                    m_currentViewController = vc;
                }
            }
        }
    
    protected:
        /**
         * Auto-loads all modules, that are in same dir as the core-module.
         * Calls loadModules with different paths:
         * Under Mac OS at the place of the executable file (not the app-Bundle) and at
         * the location of the .app-bundle.
         */
        void findAndLoadModules();
    
        /**
         * Find all available modules in a directory and fill the corresponding registries with their
         * contributions. SymLinks are not loaded to avoid double loading. Updates the report
         * property of this class.
         *
         * \param dir The directory to search for modules.
         */
        void loadModules(QDir dir);

        /**
         * Load one module and fill the corresponding registries with its contributions.
         * Also (incrementally updates the report property of this class.
         *
         * \param file The filename of the module.
         */
        void loadModule(QString file);
    
    private:
        //The filenames of each loaded module:
        QStringList m_modules_names;
        //The status messages of each loaded module
        QStringList m_modules_status;
    
        //Three variables for the factories:
        ModelFactory m_modelFactory;
        ViewControllerFactory m_viewControllerFactory;
        AlgorithmFactory m_algorithmFactory;
    
        Model* m_currentModel;
        ViewController* m_currentViewController;
};

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_FACTORIES_HXX
