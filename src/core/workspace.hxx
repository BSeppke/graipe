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

#ifndef GRAIPE_CORE_WORKSPACE_HXX
#define GRAIPE_CORE_WORKSPACE_HXX

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
 * @brief This file holds all Workspace information
 * @addtogroup core
 * @{
 */
 
namespace graipe {

/**
 * This is the Workspace class.
 * Before you start working with Graipe, you should always create a workspace.
 * A workspace is the entity, which loads and provides the Modules, the Models,
 * the ViewControllers and the Algorithms by means of the corresponding Factories
 * and the collections of currently available Models and ViewControllers.
 * Moreover, a workspace might be serialized/deserialized to hibernate the current
 * state  - at least of the active Models and ViewControllers. Currently running
 * Algorithms as not supported for de/serialization.
 */
class GRAIPE_CORE_EXPORT Workspace
: public Serializable
{
    public:
        /**
         * Constructor: Creates an empty Workspace and calls the function
         * findAndloadModules() to load the Modules from the corresponding
         * directories.
         */
        Workspace();
    
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
        Workspace(const Workspace& wsp, bool reload_factories=false);
    
        /**
         * Virtual destructor of a workspace.
         */
        virtual ~Workspace();
    

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
         * Deserialization of a workspace from an xml file.
         * This will copy all the data from the workspace, but use the same
         * (identical) factories. If you want to have new Factories, you will 
         * need to call loadModules() after the copy manually. However, it checks
         * if all modules of the serialization are avaible in the current workspace.
         *      
         * \param xmlReader The QXmlStreamReader, where we read from.
         * \return True, if the deserialization was successful, else false.
         */
        bool deserialize(QXmlStreamReader& xmlReader);
    
        /**
         * Serialization on to an output device.
         * This performes an XML-serialization of all Modules (by their names),
         * all loaded Models and all ViewControllers.
         *
         * \param xmlWriter The QXmlStreamWriter on which we want to serialize.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;

        /**
         * Clear all data structures, namely: Models and ViewControllers,
         * but keeps the Modules by means of the factories.
         * If you want to reload the factories, you will need to call loadModules
         * manually afterwards.
         */
        void clear();
    
        /**
         * Read-only access to the modelFactory, which may be used for the creation
         * of new Models.
         *
         * \return the currently loaded modelFactory.
         */
        const ModelFactory& modelFactory() const
        {
            return m_modelFactory;
        }
    
        /**
         * Read-only access to the viewControllerFactory, which may be used for the creation
         * of new ViewControllers.
         *
         * \return the currently loaded viewController.
         */
        const ViewControllerFactory& viewControllerFactory() const
        {
            return m_viewControllerFactory;
        }
    
        /**
         * Read-only access to the algorithmFactory, which may be used for the creation
         * and running of new Algorithms
         *
         * \return the currently loaded algorithmFactory.
         */
        const AlgorithmFactory& algorithmFactory() const
        {
            return m_algorithmFactory;
        }
    
        /**
         * Import procedure for available Models from a filename.
         *
         * \param filename The filename of the stored Model.
         * \return A valid pointer to a new Model, if the loading of the Model was successful.
         *         else: a null pointer.
         */
        Model* loadModel(const QString & filename);
    
    
        /**
         * Basic import procedure for available Models from an XMLStream.
         *
         * \param xmlReader The QXmlStreamReader of the stored Model.
         * \return A valid pointer to a new Model, if the loading of the Model was successful.
         *         else: a null pointer.
         */
        Model* loadModel(QXmlStreamReader & xmlReader);
    
        /**
         * Import procedure for available ViewControllers from a filename.
         *
         * \param filename The filename of the stored ViewController.
         * \return A valid pointer to a new ViewController, if the loading of the ViewController was successful.
         *         else: a null pointer.
         */
        ViewController* loadViewController(const QString & filename);

        /**
         * Import procedure for available ViewControllers from an XMLStream.
         *
         * \param xmlReader The QXmlStreamReader of the stored ViewController.
         * \return A valid pointer to a new ViewController, if the loading of the ViewController was successful.
         *         else: a null pointer.
         */
        ViewController* loadViewController(QXmlStreamReader & xmlReader);
    
        /**
         * Import procedure for available Algorithms from a filename.
         *
         * \param filename The filename of the stored Algorithm.
         * \return A valid pointer to a new Algorithm, if the loading of the Algorithm was successful.
         *         else: a null pointer.
         */
        Algorithm* loadAlgorithm(const QString & filename);
    
        /**
         * Import procedure for available Algorithms from an XMLStream.
         *
         * \param xmlReader The QXmlStreamReader of the stored Algorithms.
         * \return A valid pointer to a new Algorithm, if the loading of the Algorithm was successful.
         *         else: a null pointer.
         */
        Algorithm* loadAlgorithm(QXmlStreamReader & xmlReader);
    
        /**
         * Returns the names of all loaded Modules. The names are stored in a
         * member variable, which is updated on each loadModule(..) call.
         *
         * \return a list of QStrings containing all loaded module names.
         */
        const QStringList& modules_names() const
        {
            return m_modules_names;
        }
    
        /**
         * Returns more detailled info for all loaded Modules. 
         * This ifo is stored in a member variable, which is updated on each
         * loadModule(..) call.
         *
         * \return a list of QStrings containing all loaded module statuses.
         */
        const QStringList& modules_status() const
        {
            return m_modules_status;
        }

        /**
         * Public global algorithm mutex.
         */
        QMutex global_algorithm_mutex;
    
        /**
         * A public container holding all loaded Models.
         */
        std::vector<Model*> models;
    
        /**
         * A public container holding all loaded ViewControllers.
         */
        std::vector<ViewController*> viewControllers;
    
    
        /**
         * Returns the currently active Model. Change it using 
         * setCurrentModel(model).
         * 
         * \return A valid pointer to a Model, if any was set as a current model.
         *         Else: a null pointer.
         */
        Model* currentModel();
    
        /**
         * Sets the currently active Model. Does nothing if the given model is not
         * among the the available ones.
         *
         * \param model The Model, which shall become the next current model.
         */
        void setCurrentModel(Model * model);
    
        /**
         * Returns the currently active ViewController. Change it using 
         * setCurrentViewController(vc).
         * 
         * \return A valid pointer to a ViewController, if any was set as a 
         *         current ViewController.
         *         Else: a null pointer.
         */
        ViewController* currentViewController();
    
        /**
         * Sets the currently active ViewController. Does nothing if the given 
         * ViewController is not among the the available ones.
         *
         * \param viewController The ViewController, which shall become the next
         *                       current ViewController.
         */
        void setCurrentViewController(ViewController * viewController);
    
    protected:
        /**
         * Auto-loads all modules, that are in same dir as the core-module.
         * Calls loadModules with different paths:
         * Under Mac OS at the place of the executable file (not the app-Bundle).
         * If there were no Modules found there, it also searches at the location
         * of the .app-bundle itself.
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
    
        //The current Model and ViewController
        Model* m_currentModel;
        ViewController* m_currentViewController;
};

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_WORKSPACE_HXX
