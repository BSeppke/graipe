/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
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

#ifndef GRAIPE_CORE_MODULE_HXX
#define GRAIPE_CORE_MODULE_HXX

#include "core/config.hxx"
#include "core/factories.hxx"

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the Module class
 */

/**
 * This defines a module - the general extension structure of the
 * GRAIPE framework. You may extend the framework using:
 *  
 *     1. new models (with associated properties),
 *     2. new views  (with associated properties) and
 *     3. new algorithms.
 */
class GRAIPE_CORE_EXPORT Module
{	
    public:
        /**
         * Default constructor of the Module class.
         */
        Module();

        /**
         * Default destructor of the Module class.
         */
        virtual ~Module();

        /**
         * Returns the model factory of this module. It can be used
         * to introduce new datatypes into the GRAIPE framework
         *
         * \return The model factory
         */
        virtual ModelFactory modelFactory() const = 0;

        /**
         * Returns the view factory of this module. It can be used
         * to introduce new views and their corresponding properties
         * into the GRAIPE framework.
         *
         * \return The view factory
         */
        virtual ViewControllerFactory  viewControllerFactory() const = 0;

        /**
         * Returns the algorithm factory of this module. It can be used
         * to introduce new algorithms into the GRAIPE framework.
         *
         * \return The algorithm factory
         */
        virtual AlgorithmFactory algorithmFactory() const = 0;

        /**
         * Returns the name of this module.
         *
         * \return The name of this module.
         */
        virtual QString name() const = 0;
};

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_CORE_MODULE_HXX
