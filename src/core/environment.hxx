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

class GRAIPE_CORE_EXPORT Environment
{
    public:
        /**
         * Constructor: Creates an empty environment
         * or auto-loads all modules, that are in same dir as the core-module.
         * Calls loadModules with different paths:
         * Under Mac OS at the place of the executable file (not the app-Bundle) and at
         * the location of the .app-bundle.
         *
         * \param auto_load If true, it will auto-load all modules. False by default
         */
        Environment(bool auto_load=false);
        
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
        Environment(const Environment& env, bool reload_factories=false);

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
    
        //One status/report string
        QString status;
    
        //One global algorithm mutex:
        QMutex global_algorithm_mutex;

        //Three global variables for the factories:
        ModelFactory modelFactory;
        ViewControllerFactory viewControllerFactory;
        AlgorithmFactory algorithmFactory;

        //And two more holding all currently available Models and ViewControllers:
        std::vector<Model*> models;
        std::vector<ViewController*> viewControllers;
};

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_FACTORIES_HXX
