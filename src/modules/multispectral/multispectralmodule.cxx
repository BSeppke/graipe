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
#include "features2d/features2d.h"
#include "core/core.h"
#include "vectorfields/vectorfields.h"
#include "multispectral/multispectral.h"

namespace graipe {

/**
 * Static definition of all implemented multispectral gradient
 * modes.
 *
 * \return A List of strings, where the ordering is as follows:
 *         0: Mean Gradient, 1: Max. Gradient, 2: Eigenvalue method.
 */
static QStringList ms_gradient_modes()
{
	QStringList ms_gradient_modes;
	ms_gradient_modes.append("Mean multispectral Gradient");
	ms_gradient_modes.append("Max multispectral Gradient");
	ms_gradient_modes.append("Multispectral Gradient");

	return ms_gradient_modes;
}

/**
 * This extends the well-known Canny approach for Edge detection to use
 * the multispectral gradient definitions insetad of the single gradient
 * computation (for single band images). The implementation is made by
 * means of a specialization of the graipe::Algorithm class.
 */
class MSCannyFeatureDetector
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MSCannyFeatureDetector()
        {
            m_parameters->addParameter("image", new ModelParameter("Image","Image"));
            m_parameters->addParameter("sigma", new FloatParameter("Canny Scale", 0,9999999, 0));
            m_parameters->addParameter("T", new FloatParameter("Canny (gradient strength) threshold", 0,9999999, 0));
            m_parameters->addParameter("mode", new EnumParameter("MS gradient mode:", ms_gradient_modes(),0));
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
                    
                    ModelParameter	* param_image	= static_cast<ModelParameter*> ( (*m_parameters)["image"]);
                    
                    FloatParameter*	param_cannyScale = static_cast<FloatParameter*>( (*m_parameters)["sigma"]);
                    FloatParameter*	param_cannyThreshold = static_cast<FloatParameter*>( (*m_parameters)["T"]);
                    
                    EnumParameter* param_gradient = static_cast<EnumParameter*> ((*m_parameters)["mode"]);
                    
                    Image<float>* image = static_cast<Image<float>*> (param_image->value()); 
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                        
                    EdgelFeatureList2D* new_edgel_feature_list;
                    
                    QString alg_type;
                    
                    switch (param_gradient->value())
                    {
                        case 2:
                            new_edgel_feature_list = msCannyFeatures<MSMVGradientFunctor>(image, param_cannyScale->value(), param_cannyThreshold->value());
                            alg_type = "msMV";
                            break;
                        case 1:
                            new_edgel_feature_list = msCannyFeatures<MSMaxGradientFunctor>(image, param_cannyScale->value(), param_cannyThreshold->value());
                            alg_type = "msMax";
                            break;
                        case 0:				
                        default:
                            new_edgel_feature_list = msCannyFeatures<MSMeanGradientFunctor>(image, param_cannyScale->value(), param_cannyThreshold->value());
                            alg_type = "msMean";
                            break;
                    }
                    
                    new_edgel_feature_list->setName(alg_type + QString(" Canny-Edgel Features of ") + image->name());
                    QString descr("The following parameters were used to determine the Canny-Edgel Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_edgel_feature_list->setDescription(descr);
                    
                    ((Model*)image)->copyGeometry(*new_edgel_feature_list);
                    
                    m_results.push_back(new_edgel_feature_list);
                    
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
 * Creates one instance of the multspectral Canny Edge detection
 * algorithm defined above.
 *
 * \return A new instance of the MSCannyFeatureDetector.
 */
Algorithm* createMSCannyFeatureDetector()
{
	return new MSCannyFeatureDetector;
}




/**
 * This class implements the multispectral gradient computation by
 * means of a specialization of the graipe::Algorithm class.
 * The template for this class is the multispectral gradient functor.
 */
template<class MS_GRADIENT_FUNCTOR>
class MSGradientCalculator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        MSGradientCalculator()
        {
            m_parameters->addParameter("image", new ModelParameter("Image","Image"));
            m_parameters->addParameter("sigma", new FloatParameter("Gradient Scale", 0,9999999, 0));
        }
    
        QString typeName() const
        {
            return "MSGradientCalculator";
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
                    
                    ModelParameter	* param_image	= static_cast<ModelParameter*> ( (*m_parameters)["image"]);
                    
                    FloatParameter*	param_scale = static_cast<FloatParameter*>( (*m_parameters)["sigma"]);
                    
                    Image<float>* image = static_cast<Image<float>*> (param_image->value()); 
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    std::vector<vigra::MultiArray<2, vigra::TinyVector<float,2> > > jacobian;
                    vigra::MultiArray<2, vigra::TinyVector<float,2> >  gradient;
                    imageToJacobian(image, jacobian, param_scale->value());
                    
                    MS_GRADIENT_FUNCTOR func;
                    func(jacobian, gradient);
                    
                    DenseVectorfield2D* new_gradient_vf = new DenseVectorfield2D(gradient.bindElementChannel(0) , gradient.bindElementChannel(1));
                    
                    new_gradient_vf->setName(func.shortName() + QString(" gradient ") + image->name());
                    QString descr = QString("The following parameters were used to determine the %1 gradient\n").arg(func.name());
                    descr += m_parameters->valueText("ModelParameter");
                    new_gradient_vf->setDescription(descr);
                    
                    ((Model*)image)->copyGeometry(*new_gradient_vf);
                    
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


template<>
QString MSGradientCalculator<MSMeanGradientFunctor>::typeName() const
{
    return "MSGradientCalculator<MSMeanGradientFunctor>";
}
template<>
QString MSGradientCalculator<MSMaxGradientFunctor>::typeName() const
{
    return "MSGradientCalculator<MSMaxGradientFunctor>";
}
template<>
QString MSGradientCalculator<MSMVGradientFunctor>::typeName() const
{
    return "MSGradientCalculator<MSMVGradientFunctor>";
}

/** 
 * Creates one instance of the multspectral mean gradient
 * estimation algorithm defined above.
 *
 * \return A new instance of the MSGradientCalculator<MSMeanGradientFunctor>.
 */
Algorithm* createMSMeanGradientCalculator()
{
	return new MSGradientCalculator<MSMeanGradientFunctor>;
}

/** 
 * Creates one instance of the multspectral maximal gradient
 * estimation algorithm defined above.
 *
 * \return A new instance of the MSGradientCalculator<MSMaxGradientFunctor>.
 */
Algorithm* createMSMaxGradientCalculator()
{
	return new MSGradientCalculator<MSMaxGradientFunctor>;
}

/** 
 * Creates one instance of the multspectral eigenvector gradient
 * estimation algorithm defined above.
 *
 * \return A new instance of the MSGradientCalculator<MSMVGradientFunctor>.
 */
Algorithm* createMSMVGradientCalculator()
{
	return new MSGradientCalculator<MSMVGradientFunctor>;
}




/**
 * This class implements the multispectral Normalized Differenced Vegetation
 * Index (NDVI) by means of a specialization of the graipe::Algorithm class.
 */
class NDVIEstimator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        NDVIEstimator()
        {
            m_parameters->addParameter("image", new ModelParameter("Image","Image"));
            m_parameters->addParameter("red-id", new IntParameter("Red band-id", 0,9999999, 2));
            m_parameters->addParameter("nir-id", new IntParameter("NIR band-id", 0,9999999, 3));
        }
    
        QString typeName() const
        {
            return "NDVIEstimator";
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
                    
                    ModelParameter	* param_image	= static_cast<ModelParameter*> ( (*m_parameters)["image"]);
                    
                    IntParameter	* red_band_param = static_cast<IntParameter*> ( (*m_parameters)["red-id"]);
                    IntParameter	* nir_band_param = static_cast<IntParameter*> ( (*m_parameters)["nir-id"]);
                    
                    Image<float>* image = static_cast<Image<float>*>(param_image->value());
                    
                    //Check for bands and image sizes:
                    vigra_assert(	(unsigned int)nir_band_param->value() < image->numBands() 
                                 &&	(unsigned int)red_band_param->value() < image->numBands(), "Invalid band numbers!");
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    Image<float>* new_image = new Image<float>(image->size(), 1);
                    
                    using namespace vigra::functor;
                    
                    computeNDVI(image->band(nir_band_param->value()),
                                image->band(red_band_param->value()),
                                new_image->band(0),
                                this);
                    
                    image->copyMetadata(*new_image);
                    
                    new_image->setName("NDVI of " + image->name() );
                    
                    QString descr("The following parameters were used to determine the NDVI");
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
 * Creates one instance of the NDVI
 * algorithm defined above.
 *
 * \return A new instance of the NDVIEstimator.
 */
Algorithm* createNDVIEstimator()
{
	return new NDVIEstimator;
}




/**
 * This class implements the multispectral Enhanced Vegetation
 * Index (EVI) by means of a specialization of the graipe::Algorithm class.
 */
class EVIEstimator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        EVIEstimator()
        {
            m_parameters->addParameter("image", new ModelParameter("Image","Image"));
            m_parameters->addParameter("blue-id", new IntParameter("Blue band-id", 0,9999999, 0));
            m_parameters->addParameter("red-id", new IntParameter("Red band-id", 0,9999999, 2));
            m_parameters->addParameter("nir-id", new IntParameter("NIR band-id", 0,9999999, 3));
            
            m_parameters->addParameter("L", new FloatParameter("L", 0,9999999, 1.0));
            m_parameters->addParameter("C1", new FloatParameter("C1", 0,9999999, 6.0));
            m_parameters->addParameter("C2", new FloatParameter("C2", 0,9999999, 7.5));
            m_parameters->addParameter("Gain", new FloatParameter("Gain", 0,9999999, 2.5));
        }
    
        QString typeName() const
        {
            return "EVIEstimator";
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
                    
                    ModelParameter	* param_image	= static_cast<ModelParameter*> ( (*m_parameters)["image"]);
                    
                    IntParameter	* blue_band_param = static_cast<IntParameter*> ( (*m_parameters)["blue-id"]);
                    IntParameter	* red_band_param = static_cast<IntParameter*> ( (*m_parameters)["red-id"]);
                    IntParameter	* nir_band_param = static_cast<IntParameter*> ( (*m_parameters)["nir-id"]);
                    
                    FloatParameter	* param_L = static_cast<FloatParameter*> ( (*m_parameters)["L"]);
                    FloatParameter	* param_C1 = static_cast<FloatParameter*> ( (*m_parameters)["C1"]);
                    FloatParameter	* param_C2 = static_cast<FloatParameter*> ( (*m_parameters)["C2"]);
                    FloatParameter	* param_G = static_cast<FloatParameter*> ( (*m_parameters)["Gain"]);
                    
                    Image<float>* image = static_cast<Image<float>*>(param_image->value());
                    
                    //Check for bands and image sizes:
                    vigra_assert(	(unsigned int)nir_band_param->value() < image->numBands() 
                                 &&	(unsigned int)blue_band_param->value() < image->numBands()
                                 &&	(unsigned int)red_band_param->value() < image->numBands(), "Invalid band numbers!");
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    Image<float>* new_image = new Image<float>(image->size(), 1);
                    
                    computeEVI(image->band(nir_band_param->value()),
                               image->band(red_band_param->value()),
                               image->band(blue_band_param->value()),
                               new_image->band(0),
                               param_C1->value(), param_C2->value(), param_L->value(), param_G->value(),
                               this);
                    
                    image->copyMetadata(*new_image);
                    
                    new_image->setName("EVI of " + image->name() );
                    
                    QString descr("The following parameters were used to determine the EVI");
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
 * Creates one instance of the EVI
 * algorithm defined above.
 *
 * \return A new instance of the EVIEstimator.
 */
Algorithm* createEVIEstimator()
{
	return new EVIEstimator;
}





/**
 * This class implements the multispectral Enhanced Vegetation
 * Index (EVI) by means of a specialization of the graipe::Algorithm class.
 * Instead af read, green and blue band, this one uses just red and near-
 * infrared band.
 */
class EVI2BandsEstimator
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        EVI2BandsEstimator()
        {
            m_parameters->addParameter("image", new ModelParameter("Image","Image"));
            m_parameters->addParameter("red-id", new IntParameter("Red band-id", 0,9999999, 2));
            m_parameters->addParameter("nir-id", new IntParameter("NIR band-id", 0,9999999, 3));
            
            m_parameters->addParameter("L", new FloatParameter("L", 0,9999999, 1));
            m_parameters->addParameter("C", new FloatParameter("C", 0,9999999, 2.4f));
            m_parameters->addParameter("Gain", new FloatParameter("Gain", 0,9999999, 2.5f));
        }
    
        QString typeName() const
        {
            return "EVI2BandsEstimator";
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
                    
                    ModelParameter	* param_image	= static_cast<ModelParameter*> ( (*m_parameters)["image"]);
                    
                    IntParameter	* red_band_param = static_cast<IntParameter*> ( (*m_parameters)["red-id"]);
                    IntParameter	* nir_band_param = static_cast<IntParameter*> ( (*m_parameters)["nir_id"]);
                    
                    FloatParameter	* param_L = static_cast<FloatParameter*> ( (*m_parameters)["L"]);
                    FloatParameter	* param_C = static_cast<FloatParameter*> ( (*m_parameters)["C"]);
                    FloatParameter	* param_G = static_cast<FloatParameter*> ( (*m_parameters)["Gain"]);
                    
                    Image<float>* image = static_cast<Image<float>*>(param_image->value());
                    
                    //Check for bands and image sizes:
                    vigra_assert(	(unsigned int)nir_band_param->value() < image->numBands() 
                                 &&	(unsigned int)red_band_param->value() < image->numBands(), "Invalid band numbers!");
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    Image<float>* new_image = new Image<float>(image->size(), 1);
                    
                    computeEVI2(image->band(nir_band_param->value()),
                                image->band(red_band_param->value()),
                                new_image->band(0),
                                param_C->value(), param_L->value(), param_G->value(),
                                this);
                    
                    image->copyMetadata(*new_image);
                    
                    new_image->setName("EVI (2 bands) of " + image->name() );
                    
                    QString descr("The following parameters were used to determine the 2band-EVI");
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
 * Creates one instance of the EVI 2 Bands
 * algorithm defined above.
 *
 * \return A new instance of the EVI2BandsEstimator.
 */
Algorithm* createEVI2BandsEstimator()
{
	return new EVI2BandsEstimator;
}




/**
 * This class implements the naive 2bands OFCE approach. The second band is used
 * to eliminate the aperture problem. Please note, that this can only be successful,
 * if each two bands have a low correlation. The implementation is made by
 * means of a specialization of the graipe::Algorithm class.
 */
class OpticalFlow2BandsEstimator
:   public OpticalFlowAlgorithm2Bands
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlow2BandsEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 15);
            m_param_threshold = new FloatParameter("Threshold - det(A)>=t", 0, 100000);
            m_param_iterations =  new IntParameter("iterations", 0, 1000, 1);
            
            m_parameters->addParameter("sigma", m_param_sigma );
            m_parameters->addParameter("T", m_param_threshold );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
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
                    
                    OpticalFlow2BandsFunctor func(m_param_sigma->value(), 
                                                  m_param_threshold->value(),
                                                  m_param_iterations->value());
                    
                    emit statusMessage(1.0, QString("started computation"));
                    
                    computeFlow(func);
                    
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
        
    protected:
        //Additional Parameters
        FloatParameter* m_param_sigma;
        FloatParameter* m_param_threshold;
        IntParameter* m_param_iterations;
};
 
/** 
 * Creates one instance of the 2 bands Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlow2BandsEstimator.
 */
Algorithm* createOpticalFlow2BandsEstimator()
{
	return new OpticalFlow2BandsEstimator;
}




/**
 * This class implements a more sophisticated 2bands Horn and Schunck OFCE approach
 * The smoothness constraint of Horn and Schunck and the two bands are used 
 * to eliminate the aperture problem. Please note, that this can only be successful,
 * if each two bands have a low correlation. The implementation is made by
 * means of a specialization of the graipe::Algorithm class.
 */
class OpticalFlowHS2BandsEstimator
:   public OpticalFlowAlgorithm2Bands
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowHS2BandsEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 15);
            m_param_alpha = new FloatParameter("Weight alpha", 0, 99999);
            m_param_iterations = new IntParameter("No. of iterations", 1, 999);
            
            m_parameters->addParameter("sigma", m_param_sigma );
            m_parameters->addParameter("alpha", m_param_alpha );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
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
                    
                    OpticalFlowHS2BandsFunctor func(m_param_sigma->value(), 
                                                    m_param_alpha->value(),
                                                    m_param_iterations->value());
                    
                    emit statusMessage(1.0, QString("started computation"));
                    
                    computeFlow(func);
                    
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
        
    protected:
        //Additional Parameters
        FloatParameter* m_param_sigma;
        FloatParameter* m_param_alpha;
        IntParameter* m_param_iterations;
};

/** 
 * Creates one instance of the 2 bands Horn & Schunck Optical Flow
 * algorithm defined above.
 *
 * \return A new instance of the OpticalFlowHS2BandsEstimator.
 */
Algorithm* createOpticalFlowHS2BandsEstimator()
{
	return new OpticalFlowHS2BandsEstimator;
}

    
    
    
/**
 * This class encapsulates all the functionality of this module in a
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class MultispectralModule
:   public Module
{
    public:
        /**
         * Default constructor for the MultispectralModule.
         */
        MultispectralModule()
        {
        }
        
        /**
         * Returns the ModelFactory of the MultispectralModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the MultispectralModule.
         */
        ModelFactory modelFactory() const
        {
            return ModelFactory();
        }
        
        /**
         * Returns the ViewControllerFactory of the MultispectralModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the MultispectralModule.
         */
        ViewControllerFactory viewControllerFactory() const
        {
            return ViewControllerFactory();
        }
        
        /**
         * Returns the AlgorithmFactory of the MultispectralModule.
         * Here, we provide the algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Multispectral approaches";
			
			//estimate ndvi
			alg_item.algorithm_name = "compute NDVI (from 2 bands)";		
			alg_item.algorithm_fptr = &createNDVIEstimator;
			alg_factory.push_back(alg_item);
			
			//estimate evi
			alg_item.algorithm_name = "compute EVI (from 2 bands)";		
			alg_item.algorithm_fptr = &createEVI2BandsEstimator;
			alg_factory.push_back(alg_item);
			
			//estimate evi
			alg_item.algorithm_name = "compute EVI (from 3 bands)";		
			alg_item.algorithm_fptr = &createEVIEstimator;
			alg_factory.push_back(alg_item);
			
			//estimate gradient using mean approach 
			alg_item.algorithm_name = "MS mean gradient computation";		
			alg_item.algorithm_fptr = &createMSMeanGradientCalculator;
			alg_factory.push_back(alg_item);
			
			//estimate gradient using max approach 
			alg_item.algorithm_name = "MS max gradient computation";		
			alg_item.algorithm_fptr = &createMSMaxGradientCalculator;
			alg_factory.push_back(alg_item);
			
			//estimate gradient using mv approach 
			alg_item.algorithm_name = "MS MV gradient computation";		
			alg_item.algorithm_fptr = &createMSMVGradientCalculator;
			alg_factory.push_back(alg_item);
			
			//find ms features canny
			alg_item.algorithm_name = "MS Canny feature detection";		
			alg_item.algorithm_fptr = &createMSCannyFeatureDetector;
			alg_factory.push_back(alg_item);
			
			//estimate ms optical flow using two bands
			alg_item.algorithm_name = "MS 2-band Optical Flow estimation";		
			alg_item.algorithm_fptr = &createOpticalFlow2BandsEstimator;
			alg_factory.push_back(alg_item);
			
			//estimate horn&schunck ms optical flow using two bands
			alg_item.algorithm_name = "Horn and Schunk MS 2-band Optical Flow estimation";		
			alg_item.algorithm_fptr = &createOpticalFlowHS2BandsEstimator;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
        }
    
        /**
         * Returns the name of this Module.
         *
         * \return Always: "MultispectralModule"
         */
        QString name() const
        {
            return "MultispectralModule";
        }
};

} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  MultispectralModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::MultispectralModule;
	}
}




