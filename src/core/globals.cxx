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

#include "core/globals.hxx"
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
 * Find all available modules and fill the corresponding registries with their
 * contributions. Calls loadFactoriesFromDirectory with different paths:
 * Under Mac OS at the place of the executable file (not the app-Bundle) and at
 * the location of the .app-bundle.
 */
GRAIPE_CORE_EXPORT Environment* loadModules(QString & report)
{
	//search paths for modules:
	QDir		current_dir		= QCoreApplication::applicationDirPath();
	
    //First search at the EXACT dir of the executable - under Mac OS X this means inside the bundle
    //     Graipe.app/Contents/MacOS
    Environment* env = loadModules(QCoreApplication::applicationDirPath(), report);
    
    //Since this only works after deploying the app, we will also search at the level of the .app directory
    //for loadable modules, iff we found none so far
    if(report.isEmpty())
    {
        delete env;
        env = loadModules(QDir::current(), report);
    }
    
    return env;
}

/**
 * Find all available modules in a directory and fill the corresponding registries with their
 * contributions.
 *
 * \param dir The directory to search for modules.
 * \param added_menus Menus added during the module load.
 */
GRAIPE_CORE_EXPORT Environment* loadModules(const QDir & current_dir, QString& report)
{
    Environment* env = new Environment;
    
    QString ss;
	
	QStringList all_files = current_dir.entryList(QDir::Files);
    
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
                        env->modelFactory.push_back(item);
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
                        env->viewControllerFactory.push_back(item);
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
                        env->algorithmFactory.push_back(item);
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
    report = ss;
    
    return env;
}


//Environment * environment = new Environment;

} //end of namespace graipe

/**
 * @}
 */
