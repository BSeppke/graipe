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

#include "vectorfields/vectorfields.h"
#include "core/core.h"

namespace graipe {


/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new Models and new ViewControllers.
 */
class VectorfieldsModule
:   public Module
{
	public:
        /**
         * Default constructor of the VectorfieldsModule.
         */
		VectorfieldsModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the VectorfieldsModule.
         * This may be used to make new vectorfields of any kind.
         *
         * \return the ModelFactory of the VectorfieldsModule.
         */
		ModelFactory modelFactory() const
		{
			ModelFactory model_factory;
			ModelFactoryItem model_item;
			
			//Models
			model_item.model_type = "SparseVectorfield2D";
			model_item.model_fptr = &createSparseVectorfield2D;
			model_factory.push_back(model_item);
			
			model_item.model_type = "SparseWeightedVectorfield2D";
			model_item.model_fptr = &createSparseWeightedVectorfield2D;
			model_factory.push_back(model_item);
			
			model_item.model_type = "SparseMultiVectorfield2D";
			model_item.model_fptr = &createSparseMultiVectorfield2D;
			model_factory.push_back(model_item);
			
			model_item.model_type = "SparseWeightedMultiVectorfield2D";
			model_item.model_fptr = &createSparseWeightedMultiVectorfield2D;
			model_factory.push_back(model_item);
			
			model_item.model_type = "DenseVectorfield2D";
			model_item.model_fptr = &createDenseVectorfield2D;
			model_factory.push_back(model_item);
			
			model_item.model_type = "DenseWeightedVectorfield2D";
			model_item.model_fptr = &createDenseWeightedVectorfield2D;
			model_factory.push_back(model_item);
			
			return model_factory;
		}
		
        /**
         * Returns the ViewControllerFactory of the VectorfieldsModule.
         * This may be used to show and control vectorfields of any kind.
         *
         * \return the ViewControllerFactory of the VectorfieldsModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			ViewControllerFactory vc_factory;
			
			ViewControllerFactoryItem vc_item;
			
			vc_item.model_type = "SparseVectorfield2D";
			vc_item.viewController_name = "SparseVectorfield2DViewController";
			vc_item.viewController_fptr = &createSparseVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "SparseWeightedVectorfield2D";
			vc_item.viewController_name = "SparseWeightedVectorfield2DViewController";
			vc_item.viewController_fptr = &createSparseWeightedVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "SparseMultiVectorfield2D";
			vc_item.viewController_name = "SparseMultiVectorfield2DViewController";
			vc_item.viewController_fptr = &createSparseMultiVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "SparseWeightedMultiVectorfield2D";
			vc_item.viewController_name = "SparseWeightedMultiVectorfield2DViewController";
			vc_item.viewController_fptr = &createSparseWeightedMultiVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "DenseVectorfield2D";
			vc_item.viewController_name = "DenseVectorfield2DViewController";
			vc_item.viewController_fptr = &createDenseVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "DenseVectorfield2D";
			vc_item.viewController_name = "DenseVectorfield2DParticleView";
			vc_item.viewController_fptr = &createDenseVectorfield2DParticleView;
			vc_factory.push_back(vc_item);
            
			vc_item.model_type = "DenseWeightedVectorfield2D";
			vc_item.viewController_name = "DenseWeightedVectorfield2DViewController";
			vc_item.viewController_fptr = &createDenseWeightedVectorfield2DViewController;
			vc_factory.push_back(vc_item);
			
			vc_item.model_type = "DenseWeightedVectorfield2D";
			vc_item.viewController_name = "DenseWeightedVectorfield2DParticleView";
			vc_item.viewController_fptr = &createDenseWeightedVectorfield2DParticleView;
			vc_factory.push_back(vc_item);
            
			return vc_factory;
		}
    
        /**
         * Returns the AlgorithmFactory of the VectorfieldsModule.
         * Since this module does not provide any algorithms, this results an
         * empty factory. For algorithms, look in the AnalysisModule.
         *
         * \return An empty AlgorithmFactory.
         */
		AlgorithmFactory algorithmFactory() const
        {
            AlgorithmFactory alg_factory;
		
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Import";
			
			//Import dense vectorfields
			alg_item.algorithm_name = "Import dense vectorfield (.flo)";
			alg_item.algorithm_type = "DenseVectorfieldImporter";
			alg_item.algorithm_fptr =  &createDenseVectorfieldImporter;
			alg_factory.push_back(alg_item);
			
			alg_item.topic_name = "Export";
			
			//Import dense vectorfields
			alg_item.algorithm_name = "Export dense vectorfield (.flo)";
			alg_item.algorithm_type = "DenseVectorfieldExporter";
			alg_item.algorithm_fptr =  &createDenseVectorfieldExporter;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
    
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "VectorfieldModule"
         */
        QString name() const
        {
            return "VectorfieldModule";
        }
	
    private:
        /**
         * Factory function to create a new sparse vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseVectorfield2D.
         */
        static Model* createSparseVectorfield2D(Workspace* wsp)
        {
            return new SparseVectorfield2D(wsp);
        }
    
        /**
         * Factory function to create a new SparseVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseVectorfield2DViewController.
         */
        static ViewController* createSparseVectorfield2DViewController(Model * model){
            if(model->typeName() == "SparseVectorfield2D")
            {
                return new SparseVectorfield2DViewController(static_cast<SparseVectorfield2D*> (model));
            }
            return NULL;
        }
    
        
        /**
         * Factory function to create a new sparse weighted vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseWeightedVectorfield2D.
         */
        static Model* createSparseWeightedVectorfield2D(Workspace* wsp)
        {
            return new SparseWeightedVectorfield2D(wsp);
        }
    
        /**
         * Factory function to create a new SparseWeightedVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseWeightedVectorfield2DViewController.
         */
        static ViewController* createSparseWeightedVectorfield2DViewController(Model * model){
            if(model->typeName() == "SparseWeightedVectorfield2D")
            {
                return new SparseWeightedVectorfield2DViewController(static_cast<SparseWeightedVectorfield2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new sparse multi vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseMultiVectorfield2D.
         */
        static Model* createSparseMultiVectorfield2D(Workspace* wsp)
        {
            return new SparseMultiVectorfield2D(wsp);
        }
        
        /**
         * Factory function to create a new SparseMultiVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseMultiVectorfield2DViewController.
         */
        static ViewController* createSparseMultiVectorfield2DViewController(Model * model){
            if(model->typeName() == "SparseMultiVectorfield2D")
            {
                return new SparseMultiVectorfield2DViewController(static_cast<SparseMultiVectorfield2D*>(model));
            }
            return NULL;
        }
    
        /**
         * Factory function to create a new sparse weighted multi vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseWeightedMultiVectorfield2D.
         */
        static Model* createSparseWeightedMultiVectorfield2D(Workspace* wsp)
        {
            return new SparseWeightedMultiVectorfield2D(wsp);
        }
        
        /**
         * Factory function to create a new SparseWeightedMultiVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new SparseWeightedMultiVectorfield2DViewController.
         */
        static ViewController* createSparseWeightedMultiVectorfield2DViewController(Model * model){
            if(model->typeName() == "SparseWeightedMultiVectorfield2D")
            {
                return new SparseWeightedMultiVectorfield2DViewController(static_cast<SparseWeightedMultiVectorfield2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new dense vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseVectorfield2D.
         */
        static Model* createDenseVectorfield2D(Workspace* wsp)
        {
            return new DenseVectorfield2D(wsp);
        }
    
        /**
         * Factory function to create a new DenseVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseVectorfield2DViewController.
         */
        static ViewController* createDenseVectorfield2DViewController(Model * model){
            if(model->typeName() == "DenseVectorfield2D")
            {
                return new DenseVectorfield2DViewController(static_cast<DenseVectorfield2D*>(model));
            }
            return NULL;
        }
    
        /**
         * Factory function to create a new DenseVectorfield2DParticleViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseVectorfield2DParticleViewController.
         */
        static ViewController* createDenseVectorfield2DParticleView(Model * model){
            if(model->typeName() == "DenseVectorfield2D")
            {
                return new DenseVectorfield2DParticleViewController(static_cast<DenseVectorfield2D*>(model));
            }
            return NULL;
        }
        
        
        /**
         * Factory function to create a new dense weighted vectorfield, which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseWeightedVectorfield2D.
         */
        static Model* createDenseWeightedVectorfield2D(Workspace* wsp)
        {
            return new DenseWeightedVectorfield2D(wsp);
        }    
    
        /**
         * Factory function to create a new DenseWeightedVectorfield2DViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseWeightedVectorfield2DViewController.
         */
        static ViewController* createDenseWeightedVectorfield2DViewController(Model * model){
            if(model->typeName() == "DenseWeightedVectorfield2D")
            {
                return new DenseWeightedVectorfield2DViewController(static_cast<DenseWeightedVectorfield2D*>(model));
            }
            return NULL;
        }
        
        /**
         * Factory function to create a new DenseWeightedVectorfield2DParticleViewController, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new DenseWeightedVectorfield2DParticleViewController.
         */
        static ViewController* createDenseWeightedVectorfield2DParticleView(Model * model){
            if(model->typeName() == "DenseWeightedVectorfield2D")
            {
                return new DenseWeightedVectorfield2DParticleViewController(static_cast<DenseWeightedVectorfield2D*>(model));
            }
            return NULL;
        }
        /**
         * Creates a new algorithm for dense vectorfields import.
         *
         * \return A new DenseVectorfieldImporter.
         */
        static Algorithm* createDenseVectorfieldImporter(Workspace* wsp)
        {
            return new DenseVectorfieldImporter(wsp);
        }

        /**
         * Creates a new algorithm for dense vectorfields export.
         *
         * \return A new DenseVectorfieldExporter.
         */
        static Algorithm* createDenseVectorfieldExporter(Workspace* wsp)
        {
            return new DenseVectorfieldExporter(wsp);
        }
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  VectorfieldsModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::VectorfieldsModule;
	}	
}




