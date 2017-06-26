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
#include "images/images.h"

namespace graipe {

/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new Models and new ViewControllers.
 */
class ImagesModule
:   public Module
{
	public:
        /**
         * Default constructor of the ImagesModule.
         */
		ImagesModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the ImagesModule.
         * This may be used to make new images of any kind.
         *
         * \return the ModelFactory of the ImagesModule.
         */
		ModelFactory modelFactory() const
		{
			ModelFactory model_factory;
			
			//Model/controller
			ModelFactoryItem model_item;
            
			model_item.model_type = "Image";
			model_item.model_fptr = &createImage;
			model_factory.push_back(model_item);
            
			model_item.model_type = "IntImage";
			model_item.model_fptr = &createIntImage;
			model_factory.push_back(model_item);
            
			model_item.model_type = "ByteImage";
			model_item.model_fptr = &createByteImage;
			model_factory.push_back(model_item);
			
			return model_factory;
		}
        /**
         * Returns the ViewControllerFactory of the ImagesModule.
         * This may be used to show and control images of any kind.
         *
         * \return the ViewControllerFactory of the ImagesModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			ViewControllerFactory vc_factory;
            
            ViewControllerFactoryItem vc_item;
            
			//View/Controller
			vc_item.model_type = "Image";
			vc_item.viewController_name = "ImageSingleBandViewController";
			vc_item.viewController_fptr = &createImageSingleBandViewController;
            vc_factory.push_back(vc_item);
            
			vc_item.model_type = "Intmage";
			vc_item.viewController_name = "IntImageSingleBandViewController";
			vc_item.viewController_fptr = &createIntImageSingleBandViewController;
            vc_factory.push_back(vc_item);
            
			vc_item.model_type = "ByteImage";
			vc_item.viewController_name = "ByteImageSingleBandViewController";
			vc_item.viewController_fptr = &createByteImageSingleBandViewController;
            vc_factory.push_back(vc_item);
            
			vc_item.model_type = "Image";
			vc_item.viewController_name = "ImageRGBViewController";
			vc_item.viewController_fptr = &createImageRGBViewController;
            vc_factory.push_back(vc_item);
            
			vc_item.model_type = "IntImage";
			vc_item.viewController_name = "IntImageRGBViewController";
			vc_item.viewController_fptr = &createIntImageRGBViewController;
            vc_factory.push_back(vc_item);
            
			vc_item.model_type = "ByteImage";
			vc_item.viewController_name = "ByteImageRGBViewController";
			vc_item.viewController_fptr = &createByteImageRGBViewController;
            vc_factory.push_back(vc_item);
            
			return vc_factory;
		}
        /**
         * Returns the AlgorithmFactory of the FeatureModule.
         * This module does not provide any ral algorithms, but promotes the import/export
         * of images into other formats by means of two algorithms, with the special topics
         * "Import" and "Export".
         *
         * \return An AlgorithmFactory containing the import/export algorithms.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
		
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Import";
			
			//Import images
			alg_item.algorithm_name = "Import image";
            alg_item.algorithm_type = "ImageImporter";
			alg_item.algorithm_fptr =  &createImageImporter;
			alg_factory.push_back(alg_item);
			
			alg_item.topic_name = "Export";
			
			//Import images
			alg_item.algorithm_name = "Export image";
            alg_item.algorithm_type = "ImageExporter";
			alg_item.algorithm_fptr =  &createImageExporter;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "ImagesModule"
         */
		QString name() const
        {
            return "ImagesModule";
        }
    
    private:
        /**
         * Factory function to create a new Image<float> aka "Image", which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new Image<float>.
         */
        static Model* createImage(Environment* env)
        {
            return new Image<float>(env);
        }
            
        /**
         * Factory function to create a new ImageSingleBandViewController<float>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageSingleBandViewController<float>.
         */
        static ViewController* createImageSingleBandViewController(Model * model)
        
        {
            if(model->typeName() == "Image")
            {
                return new ImageSingleBandViewController<float>(static_cast<Image<float>*>(model));
            }
            return NULL;
        }
            
        /**
         * Factory function to create a new ImageRGGViewController<float>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageRGGViewController<float>.
         */
        static ViewController* createImageRGBViewController(Model * model)
        
        {
            if(model->typeName() == "Image"){
                return new ImageRGBViewController<float>(static_cast<Image<float>*> (model));
            }
            return NULL;
        }
    
    
        /**
         * Factory function to create a new Image<int> aka "IntImage", which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new Image<int>.
         */
        static Model* createIntImage(Environment* env)
        {
            return new Image<int>(env);
        }
    
        /**
         * Factory function to create a new ImageSingleBandViewController<int>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageSingleBandViewController<int>.
         */
        static ViewController* createIntImageSingleBandViewController(Model * model)
        
        {
            if(model->typeName() == "IntImage")
            {
                return new ImageSingleBandViewController<int>(static_cast<Image<int>*>(model));
            }
            return NULL;
        }
    
        /**
         * Factory function to create a new ImageRGBViewController<int>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageRGBViewController<int>.
         */
        static ViewController* createIntImageRGBViewController(Model * model)
        
        {
            if(model->typeName() == "IntImage"){
                return new ImageRGBViewController<int>(static_cast<Image<int>*> (model));
            }
            return NULL;
        }
    
        /**
         * Factory function to create a new Image<unsigned char> aka "ByteImage", which
         * is returned by means of a general model pointer, but may be
         * up-casted anyhow.
         *
         * \return A new Image<unsigned char>.
         */
        static Model* createByteImage(Environment* env)
        {
            return new Image<unsigned char>(env);
        }
    
        /**
         * Factory function to create a new ImageSingleBandViewController<unsigned char>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageSingleBandViewController<unsigned char>.
         */
        static ViewController* createByteImageSingleBandViewController(Model * model)
        {
            if(model->typeName() == "ByteImage")
            {
                return new ImageSingleBandViewController<unsigned char>(static_cast<Image<unsigned char>*>(model));
            }
            return NULL;
        }
    
        /**
         * Factory function to create a new ImageRGBViewController<unsigned char>, which
         * is returned by means of a general ViewController pointer, but may be
         * up-casted anyhow.
         *
         * \return A new ImageRGBViewController<unsigned char>.
         */
        static ViewController* createByteImageRGBViewController(Model * model)
        {
            if(model->typeName() == "ByteImage"){
                return new ImageRGBViewController<unsigned char>(static_cast<Image<unsigned char>*> (model));
            }
            return NULL;
        }
    
        /**
         * Creates a new algorithm for image import.
         *
         * \return A new ImageImporter.
         */
        static Algorithm* createImageImporter(Environment* env)
        {
            return new ImageImporter(env);
        }

        /**
         * Creates a new algorithm for image export.
         *
         * \return A new ImageExporter.
         */
        static Algorithm* createImageExporter(Environment* env)
        {
            return new ImageExporter(env);
        }
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  ImagesModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::ImagesModule;
	}
}





