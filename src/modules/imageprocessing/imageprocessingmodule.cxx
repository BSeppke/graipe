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
#include "vectorfields/vectorfields.h"

#include <vigra/multi_convolution.hxx>
#include <vigra/convolution.hxx>
#include <vigra/recursiveconvolution.hxx>
#include <vigra/resizeimage.hxx>
#include <vigra/inspectimage.hxx>
#include <vigra/distancetransform.hxx>
#include <vigra/labelimage.hxx>
#include <vigra/multi_morphology.hxx>

namespace graipe {

/**
 * This algorithm allows the summation of any count of images.
 * It uses a multi-selection Model parameter to get the images.
 */
class AddImages
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        AddImages()
        {
            m_parameters->addParameter("images", new MultiModelParameter("Images",	"Image"));
        }
    
        QString typeName() const
        {
            return "AddImages";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    MultiModelParameter	* param_images = static_cast<MultiModelParameter*> ((*m_parameters)["images"]);
                    
                    std::vector<Model*> selected_images = param_images->value();
                    
                    if (selected_images.size() == 0)
                    {
                        emit errorMessage(QString("Explainable error occured: No images have been selected"));
                        
                    }
                    else
                    {
                    
                        //for each pair of images: add them				
                        emit statusMessage(1.0, QString("starting computation"));
                        
                        //take the first image as a master for size and channels
                        Image<float>* image = static_cast<Image<float>*>( selected_images[0] );
                        
                        //create new image to do the addition
                        Image<float>* new_image = new Image<float>(image->size(), image->numBands());
                        
                        //Copy all metadata from current image (will be overwritten later)
                        image->copyMetadata(*new_image);
                        new_image->setName(QString("addition of ") + param_images->toString());
                        
                        //iterate
                        for(unsigned int i = 0; i < selected_images.size(); ++i)
                        {
                            
                            image = static_cast<Image<float>*>( selected_images[i] );
                            
                            vigra_precondition(image->size() == new_image->size() && image->numBands() == new_image->numBands(), "images are of different size");
                            
                            for( unsigned int c=0; c < image->numBands(); c++)
                            {
                                using namespace vigra::functor;
                        
                                vigra::combineTwoImages(image->band(c),
                                                        new_image->band(c),
                                                        new_image->band(c),
                                                        Arg1()+Arg2());
                            }
                        }
                        
                        m_results.push_back(new_image);
                        
                        emit statusMessage(100.0, QString("finished computation"));
                        emit finished();
                    }
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

Algorithm* createAddImages()
{
	return new AddImages;
}




/**
 * This algorithm computes the Gaussian gradient at a certain scale of an image.
 * The result is returned by means of a dense vectorfield.
 */
class GaussianGradientCalculator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        GaussianGradientCalculator()
        {
            m_parameters->addParameter("image", new ImageBandParameter<float>("Image", NULL));
            m_parameters->addParameter("sigma", new FloatParameter("Gaussian gradient Scale", 0,9999999, 1.0));
        }
    
        QString typeName() const
        {
            return "GaussianGradientCalculator";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageband	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["image"]);
                    FloatParameter*	param_gradientSigma = static_cast<FloatParameter*>( (*m_parameters)["sigma"]);
                    
                    vigra::MultiArrayView<2,float> imageband = static_cast<vigra::MultiArrayView<2,float>> (param_imageband->value());
                    float sigma = param_gradientSigma->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    vigra::MultiArray<2, vigra::TinyVector<float, 2> > grad(imageband.width(), imageband.height());
                    
                    vigra::VectorComponentAccessor<vigra::FVector2Image::value_type> x_acc(0);
                    vigra::VectorComponentAccessor<vigra::FVector2Image::value_type> y_acc(1);	
                    
                    vigra::gaussianGradientMultiArray(imageband, grad, sigma);
                    
                    DenseVectorfield2D* new_gradient_vf = new DenseVectorfield2D(grad.bindElementChannel(0), grad.bindElementChannel(1));
                    
                    //Copy only geometry metadata from current image
                    ((Model*)param_imageband->image())->copyGeometry(*new_gradient_vf);
                    
                    new_gradient_vf->setName(QString("Gaussian gradient of ") + param_imageband->image()->name());
                    QString descr("The following parameters were used to determine the gaussian gradient:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_gradient_vf->setDescription(descr);
                
                    m_results.push_back(new_gradient_vf);
                    
                    emit statusMessage(100.0, QString("finished computation"));		
                    emit finished();
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new Gaussian gradient estimation algorithm to derive
 * first order derivatives of an image.
 *
 * \return A new instance of the GaussianGradientCalculator.
 */
Algorithm* createGaussianGradientCalculator()
{
	return new GaussianGradientCalculator;
}




/**
 * This algorithms computes a recursive smoothing at a certain scale of an image.
 */
class RecursiveSmoothingFilter : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        RecursiveSmoothingFilter()
        {
            m_parameters->addParameter("image", new ModelParameter("Image",	"Image"));
            m_parameters->addParameter("sigma", new FloatParameter("Scale sigma", 0.0, 50.0, 1.0));
            //m_parameters.push_back( new EnumParameter("Border treatment", border_treatment_modes(), 2));
            
        }
        QString typeName() const
        {
            return "RecursiveSmoothingFilter";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    FloatParameter	* param_scale      = static_cast<FloatParameter*>((*m_parameters)["sigma"]);
                    //EnumParameter		* param_btmode     = static_cast<EnumParameter*> ((*m_parameters)[2]);
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(current_image->size(), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Rec. smoothed ") + current_image->name());
                    
                    float scale = param_scale->value();
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        vigra::recursiveSmoothX(current_image->band(c), new_image->band(c), scale);// vigra::BorderTreatmentMode(param_btmode->value()));
                        vigra::recursiveSmoothY(new_image->band(c), new_image->band(c), scale);//, vigra::BorderTreatmentMode(param_btmode->value())));
                    }
                    QString descr("The following parameters were used for recursive smoothing:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new recursive smoothing filter to low-pass
 * the intensities of an image.
 *
 * \return A new instance of the RecursiveSmoothingFilter.
 */
Algorithm* createRecursiveSmoothingFilter()
{
	return new RecursiveSmoothingFilter;
}




/**
 * This algorithms computes a Gaussian smoothing at a certain scale of an image.
 */
class GaussianSmoothingFilter : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        GaussianSmoothingFilter()
        {
            m_parameters->addParameter("image", new ModelParameter("Image",	"Image"));
            m_parameters->addParameter("sigma", new FloatParameter("Scale sigma", 0.0, 50.0, 1.0));
        }
        QString typeName() const
        {
            return "GaussianSmoothingFilter";
        }
    
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    FloatParameter	* param_scale      = static_cast<FloatParameter*>((*m_parameters)["sigma"]);
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(current_image->size(), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Gaussian smoothed ") + current_image->name());
                    
                    float scale = param_scale->value();
                    
                    vigra::Kernel1D<double> gauss;
                    gauss.initGaussian(scale);
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        vigra::separableConvolveX(current_image->band(c), new_image->band(c), gauss);//, vigra::BorderTreatmentMode(param_btmode->value())) );
                        vigra::separableConvolveY(new_image->band(c), new_image->band(c), gauss);//, vigra::BorderTreatmentMode(param_btmode->value())));
                    }
                    QString descr("The following parameters were used for gaussian smoothing:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new Gaussian smoothing filter to low-pass
 * the intensities of an image.
 *
 * \return A new instance of the GaussianSmoothingFilter.
 */
Algorithm* createGaussianSmoothingFilter()
{
	return new GaussianSmoothingFilter;
}



/**
 * This algorithms computes a Gaussian smoothing at a certain scale of an image
 * with respect to a mask. This follows the approach by Knutsson, H. and Westin, C-F.:
 * <i>Normalized and differential convolution:
 *   Methods for Interpolation and Filtering of incomplete and uncertain data</i>.
 *   Proc. of the IEEE Conf. on Computer Vision and Pattern Recognition, 1993, 515-523.
 */
class NormalizedGaussianSmoothingFilter : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        NormalizedGaussianSmoothingFilter()
        {
            m_parameters->addParameter("image", new ModelParameter("Image",	"Image"));
            m_parameters->addParameter("sigma", new FloatParameter("Scale sigma", 0.0, 50.0, 1.0));
            m_parameters->addParameter("mask",  new ImageBandParameter<float>("Mask image Band"));
        }
        QString typeName() const
        {
            return "NormalizedGaussianSmoothingFilter";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    FloatParameter	* param_scale      = static_cast<FloatParameter*>((*m_parameters)["sigma"]);
                    ImageBandParameter<float>	* param_mask      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask"]);
                    
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    vigra::MultiArrayView<2,float> mask = param_mask->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(current_image->size(), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Norm. Gauss. smoothed ") + current_image->name());
                    
                    float scale = param_scale->value();
                    
                    vigra::Kernel1D<double> gauss;
                    gauss.initGaussian(scale);
                    
                    vigra::Kernel2D<double> gauss2d;
                    gauss2d.initSeparable(gauss,gauss);
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        vigra::normalizedConvolveImage(current_image->band(c),
                                                       mask,
                                                       new_image->band(c), gauss2d);
                    }
                    QString descr("The following parameters were used for normalized gaussian smoothing:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new normalized Gaussian smoothing filter to low-pass
 * the intensities of an image w.r.t. a mask of valid pixels.
 *
 * \return A new instance of the NormalizedGaussianSmoothingFilter.
 */
Algorithm* createNormalizedGaussianSmoothingFilter()
{
	return new NormalizedGaussianSmoothingFilter;
}




/**
 * This algorithm uses a mask to cut an image. Unter the mask, the image stays as it is.
 * Masked out areas will be marked with 0 pixel values.
 */
class ApplyMaskToImage
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ApplyMaskToImage()
        {
            m_parameters->addParameter("image", new ModelParameter("Image",	"Image"));
            m_parameters->addParameter("mask",  new ImageBandParameter<float>("Mask image Band"));
            
        }
        QString typeName() const
        {
            return "ApplyMaskToImage";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    
                    ModelParameter		* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    ImageBandParameter<float>	* param_mask      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask"]);
                    
                    
                    Image<float>* image = static_cast<Image<float>*>(  param_image->value() );	
                    vigra::MultiArrayView<2,float> mask = param_mask->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(image->size(), image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("masked ") + image->name());
                    
                    for( unsigned int c=0; c < image->numBands(); c++)
                    {
                        using namespace vigra::functor;
                        
                        vigra::combineTwoImages(image->band(c),
                                                mask,
                                                new_image->band(c),
                                                Arg1()*Arg2());
                    }
                    QString descr("The following parameters were used for masking:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for cutting out image areas using a mask.
 *
 * \return A new instance of the ApplyMaskToImage.
 */
Algorithm* createApplyMaskToImage()
{
	return new ApplyMaskToImage;
}




/**
 * This algorithm uses the morphological erosion operation on a mask to erode/shrinken the
 * area under the mask.
 */
class MaskErosion
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MaskErosion()
        {
            m_parameters->addParameter("mask",   new ImageBandParameter<float>("Mask image Band",NULL) );
            m_parameters->addParameter("radius", new IntParameter("Erosion radius", 1, 50, 1) );
            
        }
        QString typeName() const
        {
            return "MaskErosion";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    
                    ImageBandParameter<float>	* param_mask      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask"]);
                    IntParameter		* param_radius      = static_cast<IntParameter*> ((*m_parameters)["radius"]);
                    
                    
                    vigra::MultiArrayView<2,float> mask = param_mask->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(mask.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_mask->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Mask erosion: ") + param_mask->toString());
                    
                    vigra::multiBinaryErosion(mask,
                                       new_image->band(0),
                                       param_radius->value());
                    
                    QString descr("The following parameters were used for mask erosion:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for eroding a binary mask.
 *
 * \return A new instance of the MaskErosion.
 */
Algorithm* createMaskErosion()
{
	return new MaskErosion;
}




/**
 * This algorithm uses the morphological dilation operation on a mask to dilate/enlarge the
 * area under the mask.
 */
class MaskDilation
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MaskDilation()
        {
            m_parameters->addParameter("mask",   new ImageBandParameter<float>("Mask image Band",NULL) );
            m_parameters->addParameter("radius", new IntParameter("Dilation radius", 1, 50, 1) );
            
        }
        QString typeName() const
        {
            return "MaskDilation";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {        
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    
                    ImageBandParameter<float>	* param_mask      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask"]);
                    IntParameter		* param_radius    = static_cast<IntParameter*> ((*m_parameters)["radius"]);
                    
                    
                    vigra::MultiArrayView<2,float> mask = param_mask->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(mask.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_mask->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Mask dilation: ") + param_mask->toString());
                    
                    vigra::multiBinaryDilation(mask,
                                        new_image->band(0),
                                        param_radius->value());
                    
                    QString descr("The following parameters were used for mask dilation:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr );
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for dilating a binary mask.
 *
 * \return A new instance of the MaskDilation.
 */
Algorithm* createMaskDilation()
{
	return new MaskDilation;
}





/**
 * This algorithm computes the union of two masks.
 */
class MaskUnion
: public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MaskUnion()
        {
            m_parameters->addParameter("mask1", new ImageBandParameter<float>("First mask image band", NULL));
            m_parameters->addParameter("mask2", new ImageBandParameter<float>("Second mask image band",NULL));
            
        }
        QString typeName() const
        {
            return "MaskUnion";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                                
                    ImageBandParameter<float>	* param_mask1      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask1"]);
                    ImageBandParameter<float>	* param_mask2      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask2"]);
                    
                    
                    vigra::MultiArrayView<2,float> mask1 = param_mask1->value();
                    vigra::MultiArrayView<2,float> mask2 = param_mask2->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(mask1.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_mask1->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Mask union: ") + param_mask1->image()->name() + " and " + param_mask2->image()->name());
                    
                    using namespace vigra::functor;
                    
                    vigra::combineTwoImages(mask1,
                                            mask2,
                                            new_image->band(0),
                                            Arg1() || Arg2());
                    
                    QString descr("The following parameters were used for mask union:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                                
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the union of two binary masks.
 *
 * \return A new instance of the MaskUnion.
 */
Algorithm* createMaskUnion()
{
	return new MaskUnion;
}


/**
 * This algorithm computes the intersection of two masks.
 */
class MaskIntersection: public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MaskIntersection()
        {
            m_parameters->addParameter("mask1", new ImageBandParameter<float>("First mask image band", NULL));
            m_parameters->addParameter("mask2", new ImageBandParameter<float>("Second mask image band",NULL));
            
        }
        QString typeName() const
        {
            return "MaskIntersection";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_mask1      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask1"]);
                    ImageBandParameter<float>	* param_mask2      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask2"]);
                    
                    
                    vigra::MultiArrayView<2,float> mask1 = param_mask1->value();
                    vigra::MultiArrayView<2,float> mask2 = param_mask2->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(mask1.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_mask1->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Mask intersec.: ") + param_mask1->toString() + " and " + param_mask2->toString());
                    
                    using namespace vigra::functor;
                    
                    vigra::combineTwoImages(mask1,
                                            mask2,
                                            new_image->band(0),
                                            Arg1() && Arg2());
                    
                    QString descr("The following parameters were used for mask intersection:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the intesection of two binary masks.
 *
 * \return A new instance of the MaskIntersection.
 */
Algorithm* createMaskIntersection()
{
	return new MaskIntersection;
}




/**
 * This algorithm computes the difference of two masks.
 */
class MaskDifference: public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MaskDifference()
        {
            m_parameters->addParameter("mask1", new ImageBandParameter<float>("First mask image band", NULL));
            m_parameters->addParameter("mask2", new ImageBandParameter<float>("Second mask image band",NULL));
            
        }
        QString typeName() const
        {
            return "MaskDifference";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_mask1      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask1"]);
                    ImageBandParameter<float>	* param_mask2      = static_cast<ImageBandParameter<float>*> ((*m_parameters)["mask2"]);
                    
                    
                    vigra::MultiArrayView<2,float> mask1 = param_mask1->value();
                    vigra::MultiArrayView<2,float> mask2 = param_mask2->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(mask1.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_mask1->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Mask difference: ") + param_mask1->toString() + " and " + param_mask2->toString());
                    
                    using namespace vigra::functor;
                    
                    vigra::combineTwoImages(mask1,
                                            mask2,
                                            new_image->band(0),
                                            Arg1() && !Arg2());
                    
                    QString descr("The following parameters were used for mask difference:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the difference of two binary masks.
 *
 * \return A new instance of the MaskDifference.
 */
Algorithm* createMaskDifference()
{
	return new MaskDifference;
}




/**
 * This algorithm computes a cropped image using a new bounding rectangle for the image.
 * The local and global coordinates of the image are updated as well.
 */
class ImageCropper
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ImageCropper()
        {
            m_parameters->addParameter("image", new ModelParameter("Image", "Image"));
            m_parameters->addParameter("ul_x", new IntParameter("Upper Left x", 0,999999, 0));
            m_parameters->addParameter("ul_y", new IntParameter("Upper Left y", 0,999999, 0));
            m_parameters->addParameter("lr_x", new IntParameter("Lower Right x", 0,999999, 1000));
            m_parameters->addParameter("lr_y", new IntParameter("Lower Right y", 0,999999, 1000));
            
        }
        QString typeName() const
        {
            return "ImageCropper";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    
                    IntParameter	* param_ul_x      = static_cast<IntParameter*>((*m_parameters)["ul_x"]);
                    IntParameter	* param_ul_y      = static_cast<IntParameter*>((*m_parameters)["ul_y"]);
                    IntParameter	* param_lr_x      = static_cast<IntParameter*>((*m_parameters)["lr_x"]);
                    IntParameter	* param_lr_y      = static_cast<IntParameter*>((*m_parameters)["lr_y"]);
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    
                    unsigned int ul_x = param_ul_x->value(),
                                 ul_y = param_ul_y->value(),
                    
                                 lr_x = param_lr_x->value(),
                                 lr_y = param_lr_y->value();
                    
                    vigra_assert( ul_x< lr_x && ul_y < lr_y, "UpperLeft coords have to be smaller than lower right coords");
                    
                    vigra_assert( ul_x>=0 && ul_y>=0, "UpperLeft coords have to be >= (0,0)");
                    vigra_assert( lr_x<= current_image->width() && lr_y<=current_image->height(), "LowerRight coords have to be <= (width,height)");
                    
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(vigra::Shape2(lr_x - ul_x, lr_y - ul_y), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("cropped ") + current_image->name());
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        new_image->setBand(c, current_image->band(c));
                    }
                    
                    QString descr("The following parameters were used for cropping:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    //Set new geometry manually
                    //Local geometry
                    new_image->setLeft(current_image->left() + ul_x);	
                    new_image->setTop(current_image->top() + ul_y);
                    new_image->setRight(current_image->left() + lr_x);
                    new_image->setBottom(current_image->top() + lr_y);
                    new_image->setWidth(lr_x - ul_x);
                    new_image->setHeight(lr_y - ul_y);
                    //global geometry
                    new_image->setGlobalLeft( current_image->globalLeft() + ul_x/current_image->width() * (current_image->globalRight() - current_image->globalLeft()) );
                    new_image->setGlobalTop( current_image->globalTop()   + ul_y/current_image->height() * (current_image->globalBottom() - current_image->globalTop()) );
                    new_image->setGlobalRight( current_image->globalLeft() + lr_x/current_image->width() * (current_image->globalRight() - current_image->globalLeft()) );
                    new_image->setGlobalBottom( current_image->globalTop() + lr_y/current_image->height() * (current_image->globalBottom() - current_image->globalTop()) );
                    
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the cropping of an image.
 *
 * \return A new instance of the ImageCropper.
 */
Algorithm* createImageCropper()
{
	return new ImageCropper;
}



/**
 * This algorithm computes a resized image using a new width and height for the image.
 * The local and global coordinates of the image are not updated, so that is still covers 
 * the same area in the global view.
 */
class ImageResizer : public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ImageResizer()
        {
            m_parameters->addParameter("image",  new ModelParameter("Image", "Image"));
            m_parameters->addParameter("width",  new IntParameter("New width", 1,999999, 100));
            m_parameters->addParameter("height", new IntParameter("New height ", 1,999999, 100));
            m_parameters->addParameter("degree", new IntParameter("Spline-Interpolation degree ", 0,5, 1));
            
        }
        QString typeName() const
        {
            return "ImageResizer";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    
                    IntParameter	* param_width			= static_cast<IntParameter*>((*m_parameters)["width"]);
                    IntParameter	* param_height			   = static_cast<IntParameter*>((*m_parameters)["height"]);
                    IntParameter	* param_spline_degree      = static_cast<IntParameter*>((*m_parameters)["degree"]);
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    
                    int width  = param_width->value(),
                        height = param_height->value();
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(vigra::Shape2(width,height), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("resized ") + current_image->name());
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        switch (param_spline_degree->value())
                        {
                            case 5:
                                vigra::resizeImageSplineInterpolation(current_image->band(c),
                                                                      new_image->band(c),
                                                                      vigra::BSpline<5, float>());
                                break;
                            case 4:
                                vigra::resizeImageSplineInterpolation(current_image->band(c),
                                                                      new_image->band(c),
                                                                      vigra::BSpline<4, float>());
                                break;
                            case 3:
                                vigra::resizeImageSplineInterpolation(current_image->band(c),
                                                                      new_image->band(c),
                                                                      vigra::BSpline<3, float>());
                                break;
                            case 2:
                                vigra::resizeImageSplineInterpolation(current_image->band(c),
                                                                      new_image->band(c),
                                                                      vigra::BSpline<2, float>());
                                break;
                            case 1:
                                vigra::resizeImageLinearInterpolation(current_image->band(c),
                                                                      new_image->band(c));
                                break;
                            default:
                            case 0:
                                vigra::resizeImageNoInterpolation(current_image->band(c),
                                                                  new_image->band(c));
                                break;
                        }
                    }
                    QString descr("The following parameters were used for resizing:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    //overwrite width and heigt
                    new_image->setWidth(width);
                    new_image->setHeight(height);
                    
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the resizing of an image.
 *
 * \return A new instance of the ImageResizer.
 */
Algorithm* createImageResizer()
{
	return new ImageResizer;
}


class ImageInverter
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ImageInverter()
        {
            m_parameters->addParameter("image", new ModelParameter("Image", "Image"));
            m_parameters->addParameter("invert", new BoolParameter("Use maximum band value as inverting offset",true));
            m_parameters->addParameter("invert_offset", new IntParameter("Global inverting offset", 0,999999, 255, (*m_parameters)["invert"]));
            
        }
        QString typeName() const
        {
            return "ImageInverter";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ModelParameter	* param_image      = static_cast<ModelParameter*> ((*m_parameters)["image"]);
                    
                    BoolParameter	* param_use_maximum			= static_cast<BoolParameter*>((*m_parameters)["invert"]);
                    IntParameter	* param_offset			   = static_cast<IntParameter*>((*m_parameters)["invert_offset"]);
                    
                    Image<float>* current_image = static_cast<Image<float>*>(  param_image->value() );	
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(current_image->size(), current_image->numBands());
                    
                    //Copy all metadata from current image (will be overwritten later)
                    current_image->copyMetadata(*new_image);
                    
                    new_image->setName(QString("inverted ") + current_image->name());
                    
                    for( unsigned int c=0; c < current_image->numBands(); c++)
                    {
                        float offset = param_offset->value();
                        
                        if (param_use_maximum->value())
                        {
                            vigra::FindMinMax<vigra::FImage::PixelType> minmax;   // init functor
                            
                            vigra::inspectImage(current_image->band(c), minmax);
                            
                            offset =  minmax.max;
                        }
                        
                        using namespace vigra::functor;
                        
                        vigra::transformImage(current_image->band(c),
                                              new_image->band(c),
                                              Param(offset)-Arg1());
                        
                    }
                    QString descr("The following parameters were used for inverting:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                                
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the inverting of an image.
 *
 * \return A new instance of the ImageInverter.
 */
Algorithm* createImageInverter()
{
	return new ImageInverter;
}




/**
 * This algorithm computes a mask from an image using simple high-low thresholding.
 * If the pixel value is larger than low and smaller than high -> a mark is set on the 
 * resulting mask. Else it is set to 0.
 */
class ImageThresholder
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ImageThresholder()
        {
            m_parameters->addParameter("image", new ImageBandParameter<float>("Image band for thresh.",	NULL));
            m_parameters->addParameter("low",  new FloatParameter("Lower threshold", -999999,999999, 0));
            m_parameters->addParameter("hi",   new FloatParameter("Upper threshold", -999999,999999, 255));
            m_parameters->addParameter("no",    new FloatParameter("No Value Mark", 0,999999, 0));
            m_parameters->addParameter("yes",   new FloatParameter("Value Mark", 0,999999, 1));
            
        }
        QString typeName() const
        {
            return "ImageThresholder";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand	   = static_cast<ImageBandParameter<float>*>((*m_parameters)["image"]);
                
                    FloatParameter	* param_lowerT			   = static_cast<FloatParameter*>((*m_parameters)["low"]);
                    FloatParameter	* param_upperT			   = static_cast<FloatParameter*>((*m_parameters)["hi"]);
                    
                    FloatParameter	* param_mark0			   = static_cast<FloatParameter*>((*m_parameters)["no"]);
                    FloatParameter	* param_mark1			   = static_cast<FloatParameter*>((*m_parameters)["yes"]);
                    
                    
                    vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                    
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(imageband.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_imageBand->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("thresholded ") + param_imageBand->toString());
                    
                    using namespace vigra::functor;
                    
                    vigra::transformImage(imageband,
                                          new_image->band(0),
                                          ifThenElse(Arg1()<Param(param_lowerT->value()) || Arg1()>Param(param_upperT->value()),
                                                     Param(param_mark0->value()),
                                                     Param(param_mark1->value())));
                    
                    QString descr("The following parameters were used for thresholding:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the thresholding of an image.
 *
 * \return A new instance of the ImageThresholder.
 */
Algorithm* createImageThresholder()
{
	return new ImageThresholder;
}



/**
 * This algorithm computes a mask from an image using a floating high-low thresholding.
 * The local threshold varies with the pixels indices (either horizontally or vertically).
 * If the pixel value is larger than low and smaller than high -> a mark is set on the 
 * resulting mask. Else it is set to 0.
 */
class FloatingImageThresholder
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        FloatingImageThresholder()
        {
            m_parameters->addParameter("image",      new ImageBandParameter<float>("Image band for thresh.",	NULL));
            m_parameters->addParameter("lowS",       new FloatParameter("Lower threshold (at start)", -999999,999999, 0));
            m_parameters->addParameter("hiS",        new FloatParameter("Upper threshold (at start)", -999999,999999, 255));
            m_parameters->addParameter("lowE",       new FloatParameter("Lower threshold (at end)", -999999,999999, 0));
            m_parameters->addParameter("hiE",        new FloatParameter("Upper threshold (at end)", -999999,999999, 255));
            m_parameters->addParameter("no",         new FloatParameter("No Value Mark", 0,999999, 0));
            m_parameters->addParameter("yes",        new FloatParameter("Value Mark", 0,999999, 1));
            m_parameters->addParameter("horizontal", new BoolParameter("Float horizontally", true));
            
        }
        QString typeName() const
        {
            return "FloatingImageThresholder";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand	   = static_cast<ImageBandParameter<float>*>((*m_parameters)["image"]);
                    
                    FloatParameter	* param_lowerT_start	   = static_cast<FloatParameter*>((*m_parameters)["lowS"]);
                    FloatParameter	* param_upperT_start	   = static_cast<FloatParameter*>((*m_parameters)["hiS"]);
                    
                    FloatParameter	* param_lowerT_end		   = static_cast<FloatParameter*>((*m_parameters)["lowE"]);
                    FloatParameter	* param_upperT_end		   = static_cast<FloatParameter*>((*m_parameters)["hiE"]);
                    
                    FloatParameter	* param_mark0			   = static_cast<FloatParameter*>((*m_parameters)["no"]);
                    FloatParameter	* param_mark1			   = static_cast<FloatParameter*>((*m_parameters)["yes"]);
                    
                    BoolParameter		* param_horizontally	   = static_cast<BoolParameter*>((*m_parameters)["horizontal"]);

                    
                    vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                    
                    float temp_val, temp_lowerT, temp_upperT;
                    
                    vigra::MultiArray<2,float> res(imageband.shape());
                    
                    for(unsigned int y=0; y< imageband.height(); y++)
                    {
                        for(unsigned int x=0; x< imageband.width(); x++)
                        {
                            temp_val = imageband(x,y);
                            if( param_horizontally->value() )
                            {
                                temp_lowerT = param_lowerT_start->value() + float(x)/imageband.width()*(param_lowerT_end->value() - param_lowerT_start->value());
                                temp_upperT = param_upperT_start->value() + float(x)/imageband.width()*(param_upperT_end->value() - param_upperT_start->value());
                            }
                            else 
                            {							
                                temp_lowerT = param_lowerT_start->value() + float(y)/imageband.height()*(param_lowerT_end->value() - param_lowerT_start->value());
                                temp_upperT = param_upperT_start->value() + float(y)/imageband.height()*(param_upperT_end->value() - param_upperT_start->value());
                            }

                            if(temp_val<temp_lowerT || temp_val>temp_upperT )
                            {
                                res(x,y) = param_mark0->value();
                            }
                            else 
                            {
                                res(x,y) = param_mark1->value();
                            }
                        }
                    }
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(imageband.shape(), 1);
                    new_image->setBand(0, res);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_imageBand->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("floating thresholded ") + param_imageBand->toString());
                    
                    
                    QString descr("The following parameters were used for floating thresholding:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the floating thresholding of an image.
 *
 * \return A new instance of the FloatingImageThresholder.
 */
Algorithm* createFloatingImageThresholder()
{
	return new FloatingImageThresholder;
}




/**
 * This function extracts the "Thin lines" from an mask image. Thin lines are herein defined
 * as positions, where the distance transform yields low (width) values. The final decision is 
 * based on statistics of a union-find algorithm.
 */
class ThinLineExtractor
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ThinLineExtractor()
        {
            
            QStringList statistical_modes;
                statistical_modes.append("mean");
                statistical_modes.append("minimal");
                statistical_modes.append("maximal");

            m_parameters->addParameter("mask",        new ImageBandParameter<float>("Mask Image",	NULL));
            m_parameters->addParameter("linewidth",   new FloatParameter("maximal width of lines", 0,999999, 0));
            m_parameters->addParameter("regions",     new EnumParameter("use region statistic for comparison", statistical_modes,0));
            m_parameters->addParameter("no",          new FloatParameter("No Value Mark", 0,999999, 0));
            m_parameters->addParameter("yes",         new FloatParameter("Value Mark", 0,999999, 1));
            m_parameters->addParameter("save",        new BoolParameter("Save region statistics image", true));
            
        }
        QString typeName() const
        {
            return "ThinLineExtractor";
        }
    
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    emit statusMessage(0.0, QString("started"));
                    
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*>((*m_parameters)["mask"]);
                    
                    FloatParameter	* param_maxWidth		= static_cast<FloatParameter*>((*m_parameters)["linewidth"]);
                    
                    EnumParameter * param_stat_mode			= static_cast<EnumParameter*>((*m_parameters)["regions"]);
                    
                    FloatParameter	* param_mark0			= static_cast<FloatParameter*>((*m_parameters)["no"]);
                    FloatParameter	* param_mark1			= static_cast<FloatParameter*>((*m_parameters)["yes"]);
                    
                    BoolParameter	* param_saveRegionDists		= static_cast<BoolParameter*>((*m_parameters)["save"]);
                    
                    vigra::MultiArrayView<2,float> imageband =  param_imageBand->value();
                    
                    vigra::MultiArray<2, float> res(imageband.shape());
                    vigra::MultiArray<2, float> res_stats_val(imageband.shape());
                    vigra::MultiArray<2, float> res_stats_var(imageband.shape());
                    
                    using namespace vigra::functor;
                    vigra::distanceTransform(imageband, res,1 ,2);
                    
                    
                    //label image
                    vigra::MultiArray<2,unsigned int> labels(imageband.shape());
                    unsigned int max_label = vigra::labelImageWithBackground(imageband, labels, false, 0);
                    
                    // init functor as an array of 'max_label' FindMinMax-Functors
                    vigra::ArrayOfRegionStatistics<vigra::FindMinMax<float> > minmax(max_label);
                    vigra::inspectTwoImages(res, labels, minmax);
                    
                    // init functor as an array of 'max_label' FindAverage-Functors
                    vigra::ArrayOfRegionStatistics<vigra::FindAverageAndVariance<float> > average_variance(max_label);
                    vigra::inspectTwoImages(res, labels, average_variance);
                    
                    for (unsigned int y=0; y < (unsigned int)labels.height(); ++y)
                    {
                        for (unsigned int x=0; x < (unsigned int)labels.width(); ++x)
                        {
                            if( imageband(x,y)  == 0)
                                continue;
                            
                            float val=0;
                            
                            switch (param_stat_mode->value())
                            {
                                case 1:
                                    val = minmax[labels(x,y)].min;
                                    break;
                                case 2:
                                    val = minmax[labels(x,y)].max;
                                    break;
                                default:
                                case 0:
                                    val = average_variance[labels(x,y)].average();
                                    break;
                            }
                            
                            res_stats_val(x,y) = val;
                            res_stats_var(x,y) = average_variance[labels(x,y)].variance();
                            
                            if(val < param_maxWidth->value()/2.0)
                            {
                                res(x,y) = param_mark1->value();
                            }
                            else
                            {
                                res(x,y) = param_mark0->value();
                            }
                            
                        }
                    }
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(imageband.shape(), 1);
                    new_image->setBand(0, res);
                    
                    Image<float>* new_stat_image = new Image<float>(imageband.shape(), 2);
                    new_stat_image->setBand(0, res_stats_val);
                    new_stat_image->setBand(1, res_stats_val);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_imageBand->image()->copyMetadata(*new_image);
                    param_imageBand->image()->copyMetadata(*new_stat_image);
                    
                    new_image->setName(QString("filtered ") + param_imageBand->toString());
                    new_stat_image->setName(QString("region stats. of filtered ") + param_imageBand->toString());
                    
                    
                    QString descr("The following parameters were used for finding thin lines:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_image->setDescription(descr);
                    
                                
                    m_results.push_back(new_image);
                    
                    if( param_saveRegionDists->value() )
                    {
                        new_stat_image->setDescription(descr + "Band 0: distance value\nBand 1: variance of distance in region\n");
                        m_results.push_back(new_stat_image);
                    }
                    else
                    {
                        delete new_stat_image;
                    }

                    
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the extraction of thin (line) regions of an image.
 *
 * \return A new instance of the ThinLineExtractor.
 */
Algorithm* createThinLineExtractor()
{
	return new ThinLineExtractor;
}




/**
 * This algorithm transforms an image mask into a distance transfrom image, where each pixel value
 * corresponds to the distance to the mask's boundary.
 */
class DistanceTransformator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        DistanceTransformator()
        {
            m_parameters->addParameter("mask", new ImageBandParameter<float>("Mask image band",	NULL));
            
        }
    
        QString typeName() const
        {
            return "DistanceTransformator";
        }
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            if(!parametersValid())
            {
                //Parameters set incorrectly
                emit errorMessage(QString("Some parameters are not available"));
            }
            else
            {
                lockModels();
                try 
                {
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*>((*m_parameters)["mask"]);			
                    
                    vigra::MultiArrayView<2,float> imageband =  param_imageBand->value();
                    
                    //create new image and do the transform
                    Image<float>* new_image = new Image<float>(imageband.shape(), 1);
                    
                    //Copy all metadata from current image (will be overwritten later)
                    param_imageBand->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("distance transform of ") + param_imageBand->toString());
                    
                    using namespace vigra::functor;
                    vigra::distanceTransform(imageband, new_image->band(0), 1 ,2);
                    
                    QString descr("No parameters needed for distance transform!");
                    new_image->setDescription(descr);
                    
                    m_results.push_back(new_image);
                    emit statusMessage(100.0, QString("finished computation"));
                    emit finished();
                    
                }
                catch(std::exception& e)
                {
                    emit errorMessage(QString("Explainable error occured: ") + QString::fromStdString(e.what()));
                }
                catch(...)
                {
                    emit errorMessage(QString("Non-explainable error occured"));		
                }
                unlockModels();
            }
        }
};

/**
 * Creates a new algorithm for the distance transform of an image.
 *
 * \return A new instance of the DistanceTransformator.
 */
Algorithm* createDistanceTransformator()
{
	return new DistanceTransformator;
}





/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class ImageProcessingModule
:   public Module
{
	public:
        /**
         * Default constructor for the ImageProcessingModule.
         */
		ImageProcessingModule()
		{
        }
	
        /**
         * Returns the ModelFactory of the ImageProcessingModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the ImageProcessingModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the ImageProcessingModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the ImageProcessingModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the ImageProcessingModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
						
							  
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Basic image processing";
			
			//compute gaussian gradient
			alg_item.algorithm_name = "Compute gaussian gradient";	
			alg_item.algorithm_fptr =  &createGaussianGradientCalculator;
			alg_factory.push_back(alg_item);
			
			//1. Recursive smoothing filter
			alg_item.algorithm_name = "Recursive smoothing";	
			alg_item.algorithm_fptr = &createRecursiveSmoothingFilter;
			alg_factory.push_back(alg_item);
			
			//2. Gaussian smoothing filter
			alg_item.algorithm_name = "Gaussian smoothing";	
			alg_item.algorithm_fptr = &createGaussianSmoothingFilter;
			alg_factory.push_back(alg_item);
			
			//3. Normalized Gaussian smoothing filter
			alg_item.algorithm_name = "Normalized Gaussian smoothing";	
			alg_item.algorithm_fptr = &createNormalizedGaussianSmoothingFilter;
			alg_factory.push_back(alg_item);
						
			//4. Image Cropper
			alg_item.algorithm_name = "Crop image";
			alg_item.algorithm_fptr = &createImageCropper;
			alg_factory.push_back(alg_item);
			
			//5. Image Resizer
			alg_item.algorithm_name = "Resize image";
			alg_item.algorithm_fptr = &createImageResizer;
			alg_factory.push_back(alg_item);
			
			//6. Image Inverter
			alg_item.algorithm_name = "Invert image";
			alg_item.algorithm_fptr = &createImageInverter;
			alg_factory.push_back(alg_item);
			
			//7. Add imagesImage Inverter
			alg_item.algorithm_name = "Add/Sum images";
			alg_item.algorithm_fptr = &createAddImages;
			alg_factory.push_back(alg_item);
			
			
			alg_item.topic_name = "Mask processing";
			
			//1. Image Thresholder
			alg_item.algorithm_name = "Create mask from image band by thresholding";
			alg_item.algorithm_fptr = &createImageThresholder;
			alg_factory.push_back(alg_item);
			
			//2. Floating Image Thresholder
			alg_item.algorithm_name = "Create mask from image Band by floating thresholding";
			alg_item.algorithm_fptr = &createFloatingImageThresholder;
			alg_factory.push_back(alg_item);
			
			//3. Apply mask to Image
			alg_item.algorithm_name = "Apply mask to image";	
			alg_item.algorithm_fptr = &createApplyMaskToImage;
			alg_factory.push_back(alg_item);
			
			//4. Mask erosion
			alg_item.algorithm_name = "Mask erosion";	
			alg_item.algorithm_fptr = &createMaskErosion;
			alg_factory.push_back(alg_item);
			
			//5. Mask dilation
			alg_item.algorithm_name = "Mask dilation";	
			alg_item.algorithm_fptr = &createMaskDilation;
			alg_factory.push_back(alg_item);
			
			//6. Mask Union
			alg_item.algorithm_name = "Union of two masks";	
			alg_item.algorithm_fptr = &createMaskUnion;
			alg_factory.push_back(alg_item);
			
			//7. Mask Intersection
			alg_item.algorithm_name = "Intersection of two masks";	
			alg_item.algorithm_fptr = &createMaskIntersection;
			alg_factory.push_back(alg_item);
			
			//8. Mask Difference
			alg_item.algorithm_name = "Difference of two masks";	
			alg_item.algorithm_fptr = &createMaskDifference;
			alg_factory.push_back(alg_item);
			
			//9. Thin Line Extractor
			alg_item.algorithm_name = "Distance transform from mask image band";
			alg_item.algorithm_fptr = &createDistanceTransformator;
			alg_factory.push_back(alg_item);
			
			//10. Thin Line Extractor
			alg_item.algorithm_name = "Extract thin line mask from mask image band";
			alg_item.algorithm_fptr = &createThinLineExtractor;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
				
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "ImageProcessingModule"
         */
		QString name() const
        {
            return "ImageProcessingModule";
        }
};
    
} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  ImageProcessingModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::ImageProcessingModule;
	}
}




