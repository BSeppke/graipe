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

#include "images/images.h"
#include "core/core.h"
#include "winddetection/detectwindpatterns.hxx"

#include <vigra/tinyvector.hxx>

namespace graipe {

/**
 * We are dealing with direction (according to the compass) here, so we have to define their
 * prototypical directions first.
 * Note that these are just steps to the corresonding pixels and not normalized vectors. All directions
 * are in image coordinates - with a flipped y-axis.
 *
 * \return A vector of eight direction TinyVectors<int> starting at East, going clockwise.
 */
static std::vector<vigra::TinyVector<int,2> > direction_vectors()
{
	std::vector<vigra::TinyVector<int,2> > direction_vectors(8);
	direction_vectors[0] = vigra::TinyVector<int,2>( 1,  0); direction_vectors[1] = vigra::TinyVector<int,2>( 1,  1);
	direction_vectors[2] = vigra::TinyVector<int,2>( 0,  1); direction_vectors[3] = vigra::TinyVector<int,2>(-1,  1);
	direction_vectors[4] = vigra::TinyVector<int,2>(-1,  0); direction_vectors[5] = vigra::TinyVector<int,2>(-1, -1);
	direction_vectors[6] = vigra::TinyVector<int,2>( 0, -1); direction_vectors[7] = vigra::TinyVector<int,2>( 1, -1);
	
	return direction_vectors;
}

/**
 * We are dealing with direction (according to the compass) here, so we define names for the
 * prototypical directions, too.
 *
 * \return A vector of eight direction names starting at "East", going clockwise.
 */
static QStringList direction_names()
{
	QStringList direction_names;
    direction_names.append("East"); direction_names.append("SouthEast");
	direction_names.append("South"); direction_names.append("SouthWest");
    direction_names.append("West"); direction_names.append("NorthWest");
	direction_names.append("North"); direction_names.append("NorthEast");

	return direction_names;
}




/**
 * This class is the base class for many wind detection algorithms, which just use a functor
 * on the SAR image to detect the wind pattern. The result of this class can be aquired by calling the 
 * detect_wind_prototype function with the corresponding functor.
 */
class WindDetector
:   public Algorithm
{
    public:
        /**
         * Default constructor. Introduces additional parameters.
         */
        WindDetector()
        {
            m_parameters->addParameter("band", new ImageBandParameter<float>("Image band",	NULL));
            m_parameters->addParameter("dir", new EnumParameter("(known) wind direction", direction_names()));
            m_parameters->addParameter("x-samples", new IntParameter("x-samples", 1, 9999, 10));
            m_parameters->addParameter("y-samples", new IntParameter("y-samples", 1, 9999, 10));
            m_parameters->addParameter("mask_w", new IntParameter("Mask width", 3, 999));
            m_parameters->addParameter("mask_h", new IntParameter("Mask height", 3, 999));
        }

    protected:
        /**
         * Prototypical functor call for the estimation of the wind patterns. Use this function with an 
         * appropriate instance of a functor toe derive the wind patterns from SAR images. For examples, see
         * FourierSpectrumWindDetector and GradientHistogramWindDetector.
         * classes.
         *
         * \param func The functor, which will be used for the derivation of wind patterns.
         */
        template <class WINDESTIMATION_FUNCTOR>
        void detect_wind_prototype(WINDESTIMATION_FUNCTOR func)
        {
            
            std::vector<Model *> result;
            
            ImageBandParameter<float>  * param_imageBand = static_cast<ImageBandParameter<float>*> ((*m_parameters)["band"]);
            EnumParameter		* param_wind_knowledge = static_cast<EnumParameter*>((*m_parameters)["dir"]);
            IntParameter		* param_xSamples = static_cast<IntParameter*>((*m_parameters)["x-samples"]),
                                * param_ySamples = static_cast<IntParameter*>((*m_parameters)["y-samples"]),
                                * param_maskWidth = static_cast<IntParameter*>((*m_parameters)["mask_w"]),
                                * param_maskHeight = static_cast<IntParameter*>((*m_parameters)["mask_h"]);
        
            vigra::MultiArrayView<2,float> imageband =  param_imageBand->value();
            
            SparseWeightedVectorfield2D* new_wind_vectorfield
                = estimateWindDirectionFromSARImage(imageband,
                                                    func, 
                                                    param_xSamples->value(), param_ySamples->value(), 
                                                    param_maskWidth->value(), param_maskHeight->value(),
                                                    direction_vectors()[param_wind_knowledge->value()]);
            
            ((Model*)param_imageBand->image())->copyGeometry(*new_wind_vectorfield);
            
            m_results.push_back(new_wind_vectorfield);
        }
};




/**
 * This class implements the wind detection from SAR images using the Fourier space analysis
 * as specialization of the WindDetector base class
 */
class FourierSpectrumWindDetector
:   public WindDetector
{
    public:
        /**
         * Default constructor. Introduces additional parameters.
         */
        FourierSpectrumWindDetector()
        {	
            m_parameters->addParameter("sigma", new FloatParameter("energy smoothing", 0.0, 10.0, 1));
            m_parameters->addParameter("T", new FloatParameter("energy threshold [%]", 0.0, 1, 0.8f));
            m_parameters->addParameter("hp", new FloatParameter("high-pass cut-off [%]", 0.0, 1, 0.2f));
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            lockModels();
            try 
            {
                    
                emit statusMessage(0.0, QString("started"));
                
                ImageBandParameter<float>	* param_imageBand = static_cast<ImageBandParameter<float>*> ((*m_parameters)["band"]);
                
                FloatParameter	* param_smoothing = static_cast<FloatParameter*>((*m_parameters)["sigma"]),
                                    * param_threshold = static_cast<FloatParameter*>((*m_parameters)["T"]),
                                    * param_radius = static_cast<FloatParameter*>((*m_parameters)["hp"]);
                
                emit statusMessage(1.0, QString("starting computation"));
                
                WindDetector::detect_wind_prototype(FourierSpectrumWindDetectionFunctor(param_smoothing->value(), 
                                                                                        param_threshold->value(), 
                                                                                        param_radius->value()));
                
                m_results.back()->setName(QString("FFT Wind estimation using ") + param_imageBand->valueText());
                
                QString descr("The following parameters were used to calculate the Fourier Wind field:\n");
                descr += m_parameters->valueText("ModelParameter");
                m_results.back()->setDescription(descr);
                
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
};

/** 
 * Creates one instance of the fourier analysis SAR wind detection
 * algorithm defined above.
 *
 * \return A new instance of the FourierSpectrumWindDetector.
 */
Algorithm* createFourierSpectrumWindDetector()
{
	return new FourierSpectrumWindDetector;
}




/**
 * This class implements the wind detection from SAR images using the Gradient Histogram
 * as specialization of the WindDetector base class
 */
class GradientHistogramWindDetector
:   public WindDetector
{
    public:
        /**
         * Default constructor. Introduces additional parameters.
         */
        GradientHistogramWindDetector()
        {	
            m_parameters->addParameter("sigma", new FloatParameter("gadient scale", 0.0, 10.0, 1.0));
            m_parameters->addParameter("T",     new FloatParameter("gradient-threshold", 0.0, 10.0, 1.0));
        }
        
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {	
            lockModels();
            try 
            {
                emit statusMessage(0.0, QString("started"));
                
                ModelParameter	* param_image = static_cast<ModelParameter*> ((*m_parameters)["band"]);
                
                FloatParameter	* param_scale = static_cast<FloatParameter*>((*m_parameters)["sigma"]),
                                    * param_threshold = static_cast<FloatParameter*>((*m_parameters)["T"]);
                
                Image<float>* image = static_cast<Image<float>*>(  param_image->value() );	
                        
                emit statusMessage(1.0, QString("starting computation"));
                
                WindDetector::detect_wind_prototype(GradientHistogramWindDetectionFunctor(param_scale->value(), 
                                                                                          param_threshold->value()));
                
                m_results.back()->setName(QString("GH Wind estimation using ") + image->name());
                
                QString descr("The following parameters were used to calculate the Gradient Histogram Wind field:\n");
                descr += m_parameters->valueText("ModelParameter");
                m_results.back()->setDescription(descr);
                
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
};

/** 
 * Creates one instance of the gradient histogram SAR wind detection
 * algorithm defined above.
 *
 * \return A new instance of the GradientHistogramWindDetector.
 */
Algorithm* createGradientHistogramWindDetector()
{
	return new GradientHistogramWindDetector;
}




/**
 * This class implements the wind detection from SAR images using the Structure Tensor
 * as specialization of the graipe::Algorithm class
 */
class StructureTensorWindDetector
:   public Algorithm
{
    public:
        /**
         * Default constructor. Introduces additional parameters.
         */
        StructureTensorWindDetector()
        {
            m_parameters->addParameter("band",   new ImageBandParameter<float>("Image band",	NULL));
            m_parameters->addParameter("dir",    new EnumParameter("(known) wind direction", direction_names()));
            m_parameters->addParameter("sigma1", new FloatParameter("innner scale", 0.0, 10.0, 1.0));
            m_parameters->addParameter("sigma2", new FloatParameter("outer scale", 0.0, 10.0, 1.0));
        }
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run()
        {
            lockModels();
            try 
            {
                emit statusMessage(0.0, QString("started"));
            
                ImageBandParameter<float>	* param_imageBand = static_cast<ImageBandParameter<float>*> ((*m_parameters)["band"]);
                EnumParameter		* param_wind_knowledge = static_cast<EnumParameter*>((*m_parameters)["dir"]);
                FloatParameter      * param_innerScale = static_cast<FloatParameter*>((*m_parameters)["sigma1"]),
                                    * param_outerScale = static_cast<FloatParameter*>((*m_parameters)["sigma2"]);
                
                vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                
                emit statusMessage(1.0, QString("starting computation"));
                
                DenseVectorfield2D* new_wind_vectorfield
                    = estimateWindDirectionFromSARImageUsingStructureTensor(imageband,
                                                                            param_innerScale->value(), param_outerScale->value(), 
                                                                            direction_vectors()[param_wind_knowledge->value()]);
            
                new_wind_vectorfield->setName(QString("ST Wind estimation using ") + param_imageBand->valueText());
                
                QString descr("The following parameters were used to compute ST winds:\n");
                descr += m_parameters->valueText("ModelParameter");
                new_wind_vectorfield->setDescription(descr);		
                
                ((Model*)param_imageBand->image())->copyGeometry(*new_wind_vectorfield);
                
                m_results.push_back(new_wind_vectorfield);
                
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
};

/** 
 * Creates one instance of the Structure Tensor SAR wind detection
 * algorithm defined above.
 *
 * \return A new instance of the StructureTensorWindDetector.
 */
Algorithm* createStructureTensorWindDetector()
{
	return new StructureTensorWindDetector;
}




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class WindDetectionModule: public Module
{
	public:
        /**
         * Default constructor for the WindDetectionModule.
         */
        WindDetectionModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the WindDetectionModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the WindDetectionModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the WindDetectionModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the WindDetectionModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the WindDetectionModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Wind detection";				
			
			//1. Wind detection using FFT analysis
			alg_item.algorithm_name = "Fourier spectra method";		
			alg_item.algorithm_fptr = &createFourierSpectrumWindDetector;
			alg_factory.push_back(alg_item);
			
			//2. Wind detection using Gradient Histogram analysis
			alg_item.algorithm_name = "Gradient histogram analysis method";	
			alg_item.algorithm_fptr = &createGradientHistogramWindDetector;
			alg_factory.push_back(alg_item);
			
			//3. Wind detection using Structure Tensor analysis
			alg_item.algorithm_name = "Structure tensor analysis method";
			alg_item.algorithm_fptr = &createStructureTensorWindDetector;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "WindDetectionModule"
         */
		QString name() const
        {
            return "WindDetectionModule";
        }
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  FeatureDetectionModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */

#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::WindDetectionModule;
	}
}




