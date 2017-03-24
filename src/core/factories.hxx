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

#ifndef GRAIPE_CORE_FACTORIES_HXX
#define GRAIPE_CORE_FACTORIES_HXX

#include "core/model.hxx"
#include "core/algorithm.hxx"
#include "core/viewcontroller.hxx"

#include <vector>
#include <algorithm>

#include <QString>


/**
 * This file holds all data structures, which are needed for the building
 * block system of GRAIPE to work. GRAIPE can be defined/extended by the
 * following items:
 *
 *     - New Models (with respective properties definition)
 *     - (Special) import/export of known or introduced models
 *     - Model views (with respective view properties)
 *     - Any kind of algorithms.
 * 
 * To store the data of all the items different types of registries are
 * defined (all specializing the std::vector).
 *
 * Since this a header only file, we need no export definitions here!
 */


namespace graipe {

/**
 * Structure for new Models (with respective parameters).
 * We need:
 * - a name for the model type to catalogize it and
 * - a function pointer (without arguments), which creates such a model.
 */
struct ModelFactoryItem
{
	QString model_type;
	Model* (*model_fptr)();
};

/**
 * Structure for model views (with respective view properties).
 * We need:
 * - a name for the model type to catalogize it,
 * - a name for the type of the view to catalogize it, and
 * - a view function pointer with three arguments:
 *   + a GraphicsScene, where the view shall be created,
 *   + a Model pointer,
 *   + an integer indicating the z-order of the view.
 */
struct ViewControllerFactoryItem
{
	QString model_type;
	QString viewController_name;
	ViewController* (*viewController_fptr)(QGraphicsScene*, Model *, int);
};

/**
 * Structure for new Algorithms (with respective parameters).
 * We need:
 * - a name for the algorithm's topic and
 * - a name for the alogithm itself to catalogize it and
 * - a function pointer (without arguments), which creates such an algorithm.
 */
struct AlgorithmFactoryItem
{
	QString  topic_name;
	QString  algorithm_name;
	Algorithm* (*algorithm_fptr)();
};

/**
 * Generic Factory to store all the items of the structures defined
 * Generally said, this is just a collection for each of the above item types.
 * A Factory can create as many different items as there have been stored inside.
 * Additionally, one can filter a factory by model_type.
 */

 
template <class FactoryItemType>
class Factory
:	public std::vector<FactoryItemType>
{
	public:
        /**
         * Filters a factory by a given model type
         * All items, where model_type == model->typeName() will be returned 
         * by means of a new factory
         */
		Factory<FactoryItemType> filterByModelType(const Model* model)
		{
            using namespace ::std;
            
			Factory<FactoryItemType> filtered_factory;
			            
            std::copy_if(this->begin(), this->end(),
                         std::back_inserter(filtered_factory),
                         [&](const FactoryItemType & item){return item.model_type == model->typeName();});
            
			return filtered_factory;
		}	
};
    
//Promote the three mostly needed types by own names inside the graipe namespace:
/** A Factory for Models **/
typedef Factory<ModelFactoryItem>     ModelFactory;
/** A Factory for ViewControllers **/
typedef Factory<ViewControllerFactoryItem>      ViewControllerFactory;
/** A Factory for Algorithms **/
typedef Factory<AlgorithmFactoryItem> AlgorithmFactory;

}//end of namespace graipe

#endif //GRAIPE_CORE_FACTORIES_HXX
