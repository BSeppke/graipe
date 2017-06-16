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
#include "core/viewcontroller.hxx"

#include <QDir>

#include <vector>

/**
 * @file
 * @brief This file holds all global data structures
 * @addtogroup core
 * @{
 */
 
namespace graipe {

GRAIPE_CORE_EXPORT Environment* loadModules(QString& report);
GRAIPE_CORE_EXPORT Environment* loadModules(const QDir & current_dir, QString& report);

class Environment
{
    public:
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

//extern Environment * environment;

}//end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_FACTORIES_HXX
