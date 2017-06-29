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

#include "core/core.h"
#include "features2d/features2d.h"

namespace graipe {

/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new Models and new ViewControllers.
 */
class Features2DModule
:   public Module
{
	public:
        /**
         * Default constructor of the FeatureModule.
         */
		Features2DModule()
		{}
	
        /**
         * Returns the ModelFactory of the FeatureModule.
         * This may be used to make new featurelists of any kind.
         *
         * \return the ModelFactory of the FeatureModule.
         */
		ModelFactory modelFactory() const
		{
			ModelFactory model_factory;
			ModelFactoryItem model_item;
            
			model_item.model_fptr = &createPointFeatureList2D;
			model_item.model_type = "PointFeatureList2D";
			model_factory.push_back(model_item);
			
			model_item.model_fptr = &createWeightedPointFeatureList2D;
			model_item.model_type = "WeightedPointFeatureList2D";
			model_factory.push_back(model_item);
			
			model_item.model_fptr = &createEdgelFeatureList2D;
			model_item.model_type = "EdgelFeatureList2D";
			model_factory.push_back(model_item);
			
			model_item.model_fptr = &createSIFTFeatureList2D;
			model_item.model_type = "SIFTFeatureList2D";
            model_factory.push_back(model_item);
            
            model_item.model_fptr = &createPolygonList2D;
            model_item.model_type = "PolygonList2D";
            model_factory.push_back(model_item);
            
            model_item.model_fptr = &createWeightedPolygonList2D;
            model_item.model_type = "WeightedPolygonList2D";
            model_factory.push_back(model_item);
            
            model_item.model_fptr = &createCubicSplineList2D;
            model_item.model_type = "CubicSplineList2D";
            model_factory.push_back(model_item);
            
            model_item.model_fptr = &createWeightedCubicSplineList2D;
            model_item.model_type = "WeightedCubicSplineList2D";
            model_factory.push_back(model_item);
            
			return model_factory;
		}
		
        /**
         * Returns the ViewControllerFactory of the FeatureModule.
         * This may be used to show and control featurelists of any kind.
         *
         * \return the ViewControllerFactory of the FeatureModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			ViewControllerFactory view_factory;
			ViewControllerFactoryItem item;
			
			item.model_type = "PointFeatureList2D";
			item.viewController_name = "PointFeatureList2DViewController";
			item.viewController_fptr = &createPointFeatureList2DViewController;
			view_factory.push_back(item);
			
			item.model_type = "WeightedPointFeatureList2D";
			item.viewController_name = "WeightedPointFeatureList2DViewController";
			item.viewController_fptr = &createWeightedPointFeatureList2DViewController;
			view_factory.push_back(item);
			
			item.model_type = "EdgelFeatureList2D";
			item.viewController_name = "EdgelFeatureList2DViewController";
			item.viewController_fptr = &createEdgelFeatureList2DViewController;
			view_factory.push_back(item);
			
			item.model_type = "SIFTFeatureList2D";
			item.viewController_name = "SIFTFeatureList2DViewController";
			item.viewController_fptr = &createSIFTFeatureList2DViewController;
			view_factory.push_back(item);
            
            item.model_type = "PolygonList2D";
            item.viewController_name = "PolygonList2DViewController";
            item.viewController_fptr = &createPolygonList2DViewController;
            view_factory.push_back(item);
            
            item.model_type = "WeightedPolygonList2D";
            item.viewController_name = "WeightedPolygonList2DViewController";
            item.viewController_fptr = &createWeightedPolygonList2DViewController;
            view_factory.push_back(item);
            
            item.model_type = "CubicSplineList2D";
            item.viewController_name = "CubicSplineList2DViewController";
            item.viewController_fptr = &createCubicSplineList2DViewController;
            view_factory.push_back(item);
            
            item.model_type = "WeightedCubicSplineList2D";
            item.viewController_name = "WeightedCubicSplineList2DViewController";
            item.viewController_fptr = &createWeightedCubicSplineList2DViewController;
            view_factory.push_back(item);
			
			return view_factory;
		}
		
        /**
         * Returns the AlgorithmFactory of the FeatureModule.
         * Since this module does not provide any algorithms, this results an
         * empty factory.
         *
         * \return An empty AlgorithmFactory.
         */
        AlgorithmFactory algorithmFactory() const
        {
            AlgorithmFactory alg_factory;
            
            return alg_factory;
        }
    
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "Features2DModule"
         */
		QString name() const
        {
            return "Features2DModule";
        }
	
    private:
        
        /**
         * Factory function to create a new PointFeatureList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new PointFeatureList2D.
         */
        static Model* createPointFeatureList2D(Workspace* wsp)
        {
            return new PointFeatureList2D(wsp);
        }
        
        /**
         * Factory function to create a new PointFeatureList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new PointFeatureList2DViewController.
         */
        static ViewController* createPointFeatureList2DViewController(Model * model){
            if(model->typeName() == "PointFeatureList2D")
            {
                return new PointFeatureList2DViewController(static_cast<PointFeatureList2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new WeightedPointFeatureList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedPointFeatureList2D.
         */
        static Model* createWeightedPointFeatureList2D(Workspace* wsp)
        {
            return new WeightedPointFeatureList2D(wsp);
        }
                
        /**
         * Factory function to create a new WeightedPointFeatureList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedPointFeatureList2DViewController.
         */
        static ViewController* createWeightedPointFeatureList2DViewController(Model * model){
            if(model->typeName() == "WeightedPointFeatureList2D")
            {
                return new WeightedPointFeatureList2DViewController(static_cast<WeightedPointFeatureList2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new EdgelFeatureList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new EdgelFeatureList2D.
         */
        static Model* createEdgelFeatureList2D(Workspace* wsp)
        {
            return new EdgelFeatureList2D(wsp);
        }
            
        /**
         * Factory function to create a new SIFTFeatureList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SIFTFeatureList2DViewController.
         */
        static ViewController* createEdgelFeatureList2DViewController(Model * model){
            if(model->typeName() == "EdgelFeatureList2D")
            {
                return new EdgelFeatureList2DViewController(static_cast<EdgelFeatureList2D*>(model));
            }
            return NULL;
        }
            
        /**
         * Factory function to create a new SIFTFeatureList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SIFTFeatureList2D.
         */
        static Model* createSIFTFeatureList2D(Workspace* wsp)
        {
            return new SIFTFeatureList2D(wsp);
        }
        
        /**
         * Factory function to create a new SIFTFeatureList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SIFTFeatureList2DViewController.
         */
        static ViewController* createSIFTFeatureList2DViewController(Model * model){
            if(model->typeName() == "SIFTFeatureList2D")
            {
                return new SIFTFeatureList2DViewController(static_cast<SIFTFeatureList2D*>(model));
            }
            return NULL;
        }
        
        
        
        
        /**
         * Factory function to create a new PolygonList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new PolygonList2D.
         */
        static Model* createPolygonList2D(Workspace* wsp)
        {
            return new PolygonList2D(wsp);
        }
        
        /**
         * Factory function to create a new PolygonList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new PolygonList2DViewController.
         */
        static ViewController* createPolygonList2DViewController(Model * model){
            if(model->typeName() == "PolygonList2D")
            {
                return new PolygonList2DViewController(static_cast<PolygonList2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new WeightedPolygonList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedPolygonList2D.
         */
        static Model* createWeightedPolygonList2D(Workspace* wsp)
        {
            return new WeightedPolygonList2D(wsp);
        }
        
        /**
         * Factory function to create a new WeightedPolygonList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedPolygonList2DViewController.
         */
        static ViewController* createWeightedPolygonList2DViewController(Model * model){
            if(model->typeName() == "WeightedPolygonList2D")
            {
                return new WeightedPolygonList2DViewController(static_cast<WeightedPolygonList2D*>(model));
            }
            return NULL;
        }
        
        
        
        
        /**
         * Factory function to create a new CubicSplineList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new CubicSplineList2D.
         */
        static Model* createCubicSplineList2D(Workspace* wsp)
        {
            return new CubicSplineList2D(wsp);
        }
        
        /**
         * Factory function to create a new CubicSplineList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new CubicSplineList2DViewController.
         */
        static ViewController* createCubicSplineList2DViewController(Model * model){
            if(model->typeName() == "CubicSplineList2D")
            {
                return new CubicSplineList2DViewController(static_cast<CubicSplineList2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new WeightedCubicSplineList2D, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedCubicSplineList2D.
         */
        static Model* createWeightedCubicSplineList2D(Workspace* wsp)
        {
            return new WeightedCubicSplineList2D(wsp);
        }
        
        /**
         * Factory function to create a new WeightedCubicSplineList2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new WeightedCubicSplineList2DViewController.
         */
        static ViewController* createWeightedCubicSplineList2DViewController(Model * model){
            if(model->typeName() == "WeightedCubicSplineList2D")
            {
                return new WeightedCubicSplineList2DViewController(static_cast<WeightedCubicSplineList2D*>(model));
            }
            return NULL;
        }
};

} //End of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  ImageModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return the pointer to the module 
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::Features2DModule;
	}
}




