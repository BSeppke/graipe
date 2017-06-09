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
#include "features2d/features2d.h"
#include "featuredetection/featuredetection.h"

#include "vigra/convolution.hxx"

namespace graipe {

/**
 * This algorithm computes the features as monotony class members.
 */
class MonotonyFeatureDetector
:   public Algorithm
{
    public:
        /**
         * Default constructor. Initializes additional parameters of the algorithm.
         */
        MonotonyFeatureDetector()
        {
            m_parameters->addParameter("image",  new ImageBandParameter<float>("Image", NULL));
            m_parameters->addParameter("mask?",  new BoolParameter("Use image band as mask", true));
            m_parameters->addParameter("mask",   new ImageBandParameter<float>("Mask Image band", (*m_parameters)["mask?"]));
            m_parameters->addParameter("lowM",   new IntParameter("Lowest Monotony class", 0,8, 7));
            m_parameters->addParameter("hiM",    new IntParameter("Highest Monotony class", 0,8, 8));
        }
	
    
        QString typeName() const
        {
            return "MonotonyFeatureDetector";
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
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["image"]);
                    
                    BoolParameter			* param_useMask   = static_cast<BoolParameter*>( (*m_parameters)["mask?"]);
                    ImageBandParameter<float>	* param_mask_image	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["mask"]);
                    
                    IntParameter			* param_lowestMonotonyClass  = static_cast<IntParameter*>( (*m_parameters)["lowM"]),
                                            * param_highestMonotonyClass = static_cast<IntParameter*>( (*m_parameters)["hiM"]);
                            
                    vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                    
                    
                    WeightedPointFeatureList2D* new_feature_list;
                        
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    if(param_useMask->value())
                    {
                        vigra::MultiArrayView<2,float> mask = param_mask_image->value();
                        
                        new_feature_list = detectFeaturesUsingMonotonyOperatorWithMask(imageband,
                                                                                       mask,
                                                                                       param_lowestMonotonyClass->value(), param_highestMonotonyClass->value());
                    }
                    else
                    {
                        new_feature_list = detectFeaturesUsingMonotonyOperator(imageband,
                                                                               param_lowestMonotonyClass->value(), param_highestMonotonyClass->value());
                    }
                    new_feature_list->setName(QString("Monotony Features of ") + param_imageBand->toString());
                    QString descr("The following parameters were used to determine the Monotony Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    new_feature_list->setDescription(descr);
                    
                    ((Model*)param_imageBand->image())->copyGeometry(*new_feature_list);
                    
                    m_results.push_back(new_feature_list);
                    
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
 * Creates one instance of the Monotony Feature Detector
 * algorithm defined above.
 *
 * \return A new instance of the MonotonyFeatureDetector.
 */
Algorithm* createMonotonyFeatureDetector()
{
	return new MonotonyFeatureDetector;
}




/**
 * Implementation of a Harris Corner Detector by means of an Algorithm.
 */
class HarrisCornerDetector
:   public Algorithm
{
    public:
	
        /**
         * Default constructor. Initializes additional parameters of the algorithm.
         */
        HarrisCornerDetector()
        {
            m_parameters->addParameter("image", new ImageBandParameter<float>("Image", NULL));
            m_parameters->addParameter("mask?", new BoolParameter("Use image band as mask", true));
            m_parameters->addParameter("mask",  new ImageBandParameter<float>("Mask Image band", (*m_parameters)["mask?"]));
            m_parameters->addParameter("sigma", new FloatParameter("sigma for calculation of gauss. gradient", 0,99, 0.6f));
            m_parameters->addParameter("T",     new FloatParameter("Corner response threshold", 0,999999, 0));
        }
	
    
        QString typeName() const
        {
            return "HarrisCornerDetector";
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
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["image"]);
                    
                    BoolParameter       * param_useMask   = static_cast<BoolParameter*>( (*m_parameters)["mask?"]);
                    ImageBandParameter<float>	* param_mask	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["mask"]);
                    
                    FloatParameter	* param_gradientSigma = static_cast<FloatParameter*>( (*m_parameters)["sigma"]),
                                    * param_responseThreshold = static_cast<FloatParameter*>( (*m_parameters)["T"]);
                    
                    vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                    
                    WeightedPointFeatureList2D* new_feature_list;
                        
                    emit statusMessage(1.0, QString("starting computation"));
                        
                    if(param_useMask->value())
                    {
                        vigra::MultiArrayView<2,float> mask =  param_mask->value();
                        
                        new_feature_list = detectFeaturesUsingHarrisWithMask(imageband,
                                                                             mask,
                                                                             param_gradientSigma->value(), param_responseThreshold->value());
                        
                    }
                    else
                    {
                        new_feature_list = detectFeaturesUsingHarris(imageband,
                                                                     param_gradientSigma->value(), param_responseThreshold->value());
                    }
                        
                    new_feature_list->setName(QString("Harris Features of ") + param_imageBand->toString());
                    QString descr("The following parameters were used to determine the Harris Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_feature_list->setDescription(descr);
                    
                    ((Model*)param_imageBand->image())->copyGeometry(*new_feature_list);
                    
                    m_results.push_back(new_feature_list);
                    
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
 * Creates one instance of the Harris Corner Detector
 * algorithm defined above.
 *
 * \return A new instance of the HarrisCornerDetector.
 */
Algorithm* createHarrisCornerDetector()
{
	return new HarrisCornerDetector;
}




/**
 * Implementation of a Canny Edge Detector by means of an Algorithm.
 */
class CannyFeatureDetector
:   public Algorithm
{
    public:
	
        /**
         * Default constructor. Initializes additional parameters of the algorithm.
         */
        CannyFeatureDetector()
        {
            m_parameters->addParameter("image",  new ImageBandParameter<float>("Image", NULL));
            m_parameters->addParameter("mask?",  new BoolParameter("Use image band as mask", true));
            m_parameters->addParameter("mask",   new ImageBandParameter<float>("Mask Image band", (*m_parameters)["mask?"]));
            m_parameters->addParameter("sigma",  new FloatParameter("Canny Scale", 0,9999999, 0));
            m_parameters->addParameter("sigmaT", new FloatParameter("Canny (gradient strength) threshold", 0,9999999, 0));
        }
        
    
        QString typeName() const
        {
            return "CannyFeatureDetector";
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
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["image"]);
                    
                    BoolParameter		* param_useMask   = static_cast<BoolParameter*>( (*m_parameters)["mask?"]);
                    ImageBandParameter<float>	* param_mask	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["mask"]);
                    
                    FloatParameter*	param_cannyScale = static_cast<FloatParameter*>( (*m_parameters)["sigma"]);
                    FloatParameter*	param_cannyThreshold = static_cast<FloatParameter*>( (*m_parameters)["sigmaT"]);
                    
                    vigra::MultiArrayView<2,float> imageband = param_imageBand->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                        
                    EdgelFeatureList2D* new_edgel_feature_list;
                    
                    if(param_useMask->value())
                    {
                        vigra::MultiArrayView<2,float> mask = param_mask->value();
                        
                        new_edgel_feature_list = detectFeaturesUsingCannyWithMask(imageband,
                                                                                  mask,
                                                                                  param_cannyScale->value(), param_cannyThreshold->value());
                    }
                    else
                    {
                        new_edgel_feature_list = detectFeaturesUsingCanny(imageband,
                                                                          param_cannyScale->value(), param_cannyThreshold->value());
                    }
                    
                    new_edgel_feature_list->setName(QString("Canny-Edgel Features of ") + param_imageBand->toString());
                    QString descr("The following parameters were used to determine the Canny-Edgel Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    new_edgel_feature_list->setDescription(descr);
                    
                    ((Model*)param_imageBand->image())->copyGeometry(*new_edgel_feature_list);
                    
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
 * Creates one instance of the Canny Feature Detector
 * algorithm defined above.
 *
 * \return A new instance of the CannyFeatureDetector.
 */
Algorithm* createCannyFeatureDetector()
{
	return new CannyFeatureDetector;
}




/**
 * Implementation of a Canny Edge length filter by means of an Algorithm.
 */
class CannyFeatureLengthFilter
:   public Algorithm
{
    public:
        /**
         * Default constructor. Initializes additional parameters of the algorithm.
         */
        CannyFeatureLengthFilter()
        {
            m_parameters->addParameter("edgels",     new ModelParameter("Edgel Featurelist (2D)", "EdgelFeatureList2D"));
            m_parameters->addParameter("min-length", new FloatParameter("Minimal Edgel length", 0,9999999, 0));
            m_parameters->addParameter("radius",     new FloatParameter("Search radius for Edgel-unions", 0,9999999, 1.5));
        }
        
    
        QString typeName() const
        {
            return "CannyFeatureLengthFilter";
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
                    
                    ModelParameter	* param_features	= static_cast<ModelParameter*> ( (*m_parameters)["edgels"]);
                    
                    FloatParameter*	param_minimalLength = static_cast<FloatParameter*>( (*m_parameters)["min-length"]);
                    FloatParameter*	param_searchRadius  = static_cast<FloatParameter*>( (*m_parameters)["radius"]);
                    
                    EdgelFeatureList2D* features = static_cast<EdgelFeatureList2D*> (param_features->value()); 
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    EdgelFeatureList2D* new_edgel_feature_list = removeShortEdgesFromEdgelList(features, param_minimalLength->value(),param_searchRadius->value());
                    
                    new_edgel_feature_list->setName(QString("Filtered ") + features->name());
                    QString descr("The following parameters were used to filter the Canny-Edgel Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    new_edgel_feature_list->setDescription(descr);
                    
                    features->copyGeometry(*new_edgel_feature_list);
                    
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
    
    private:
        /**
         * Privately implemented removal of short edgel chains.
         *
         * \param features A list of Canny Edgel features.
         * \param min_lenngth The min. length of a consecutive edgel chain.
         * \param search_radius The radius where to search for continuations.
         * \return A new edgel list, where short edgel chaines have been eliminated.
         */
        EdgelFeatureList2D* removeShortEdgesFromEdgelList(EdgelFeatureList2D* features, double min_length, float search_radius)
        {
            EdgelFeatureList2D* new_featurelist = new EdgelFeatureList2D;
            
            std::vector<bool> marked(features->size(), false);
            std::vector<unsigned int> trace, boundary;
            
            for (unsigned int i=0; i<features->size(); ++i)
            {
                if( !marked[i] )
                {
                    //start running
                    marked[i] = true;
                    float min_x, max_x, min_y, max_y;
                    
                    trace.clear();
                    trace.push_back(i);
                    boundary.clear();
                    boundary.push_back(i);
                    
                    while (boundary.size() > 0) 
                    {
                        std::vector<unsigned int> new_boundary;
                        
                        for(unsigned int b=0; b<boundary.size(); ++b)
                        {
                            const EdgelFeatureList2D::PointType& pos_b = features->position(boundary[b]);
                            
                            min_x = pos_b.x() - search_radius;	max_x = pos_b.x() + search_radius;
                            min_y = pos_b.y()- search_radius;	max_y = pos_b.y() + search_radius;
                            
                            
                            for (unsigned int j=0; j<features->size(); ++j)
                            {
                                const EdgelFeatureList2D::PointType& pos_j = features->position(j);
                                
                                if( !marked[j]
                                   && pos_j.x() >= min_x && pos_j.x() <= max_x
                                   && pos_j.y() >= min_y && pos_j.y() <= max_y)
                                {
                                    marked[j] = true;
                                    trace.push_back(j);
                                    new_boundary.push_back(j);
                                }
                            }
                        }
                        boundary = new_boundary;
                    }
                    if(trace.size() >= min_length)
                    {
                        for (unsigned int t=0; t < trace.size(); t++)
                        {
                            const EdgelFeatureList2D::PointType& pos_t = features->position(trace[t]);
                            
                            new_featurelist->addFeature(pos_t, features->weight(trace[t]),features->orientation(trace[t]));
                        }
                    }
                }
            }
            return new_featurelist;
        }
};

/** 
 * Creates one instance of the Canny Feature Length Filter
 * algorithm defined above.
 *
 * \return A new instance of the CannyFeatureLengthFilter.
 */
Algorithm* createCannyFeatureLengthFilter()
{
	return new CannyFeatureLengthFilter;
}




/**
 * Implementation of a SIFT Keypoint Detector by means of an Algorithm.
 */
class SIFTFeatureDetector
:   public Algorithm
{
    public:
	
        /**
         * Default constructor. Initializes additional parameters of the algorithm.
         */
        SIFTFeatureDetector()
        {
            m_parameters->addParameter("image",     new ImageBandParameter<float>("Image", NULL));
            m_parameters->addParameter("sigma",     new FloatParameter("sigma of first octave", 0, 100,  1));
            m_parameters->addParameter("octaves",   new IntParameter("octaves", 0,100, 4));
            m_parameters->addParameter("levels",    new IntParameter("intra-octave levels", 0, 100, 3));
            m_parameters->addParameter("contrast",  new FloatParameter("contrast threshold",  0,   1,  0.03f));
            m_parameters->addParameter("curvature", new FloatParameter("curvature threshold", 0, 100, 10));
            m_parameters->addParameter("double",    new BoolParameter("double image resolution orientation", true));
            m_parameters->addParameter("norm",      new BoolParameter("normalize image to 0..1", true));
        }
	
    
        QString typeName() const
        {
            return "SIFTFeatureDetector";
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
                    
                    ImageBandParameter<float>	* param_imageBand	= static_cast<ImageBandParameter<float>*> ( (*m_parameters)["image"]);
                    
                    FloatParameter	*	param_sigma = static_cast<FloatParameter*>( (*m_parameters)["sigma"]);
                    
                    IntParameter	*	param_octaves = static_cast<IntParameter*>( (*m_parameters)["octaves"]),
                                    *	param_levels  = static_cast<IntParameter*>( (*m_parameters)["levels"]);
                        
                    FloatParameter	*	param_contrast_threshold = static_cast<FloatParameter*>( (*m_parameters)["contrast"]),
                                    *	param_curvature_threshold  = static_cast<FloatParameter*>( (*m_parameters)["curvature"]);
                    
                    BoolParameter	*	param_double_size  = static_cast<BoolParameter*>( (*m_parameters)["double"]),
                                    *	param_normalize = static_cast<BoolParameter*>( (*m_parameters)["norm"]);
                    
                    
                    vigra::MultiArrayView<2,float>	imageband = param_imageBand->value();
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    SIFTFeatureList2D* new_feature_list =  detectFeaturesUsingSIFT(imageband,
                                                                                   param_sigma->value(), param_octaves->value(), param_levels->value(),
                                                                                   param_contrast_threshold->value(), param_curvature_threshold->value(), param_double_size->value(), param_normalize->value());
                    
                    new_feature_list->setName(QString("SIFT Features of ") + param_imageBand->toString());
                    QString descr("The following parameters were used to determine the SIFT Features:\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    new_feature_list->setDescription(descr);
                                
                    ((Model*)param_imageBand->image())->copyGeometry(*new_feature_list);
            
                    m_results.push_back(new_feature_list);
                    
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
 * Creates one instance of the SIFT Feature Detector
 * algorithm defined above.
 *
 * \return A new instance of the SIFTFeatureDetector.
 */
Algorithm* createSIFTFeatureDetector()
{
	return new SIFTFeatureDetector;
}




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class FeatureDetectionModule
:   public Module
{
	public:
        /**
         * Default constructor of the FeatureDetectionModule.
         */
		FeatureDetectionModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the FeatureDetectionModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the FeatureDetectionModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
				
        /**
         * Returns the ViewControllerFactory of the FeatureDetectionModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the FeatureDetectionModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the FeatureDetectionModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Feature detection";
			
			//find features monotony
			alg_item.algorithm_name = "Monotony operator";	
			alg_item.algorithm_type = "MonotonyFeatureDetector";
			alg_item.algorithm_fptr =  &createMonotonyFeatureDetector;
			alg_factory.push_back(alg_item);
			
			//find features canny
			alg_item.algorithm_name = "Harris corner detector";	
			alg_item.algorithm_type = "HarrisCornerDetector";
			alg_item.algorithm_fptr = &createHarrisCornerDetector;
			alg_factory.push_back(alg_item);
			
			//find features canny
			alg_item.algorithm_name = "Canny operator";		
			alg_item.algorithm_type = "CannyFeatureDetector";
			alg_item.algorithm_fptr = &createCannyFeatureDetector;
			alg_factory.push_back(alg_item);
			
			//find features canny
			alg_item.algorithm_name = "SIFT technique";	
			alg_item.algorithm_type = "SIFTFeatureDetector";
			alg_item.algorithm_fptr = &createSIFTFeatureDetector;
			alg_factory.push_back(alg_item);
			
			//filter canny features by length
			alg_item.algorithm_name = "Filter Canny edgels by length";
			alg_item.algorithm_type = "CannyFeatureLengthFilter";
			alg_item.algorithm_fptr = &createCannyFeatureLengthFilter;
			alg_factory.push_back(alg_item);
			
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "FeatureDetectionModule"
         */
		QString name() const
        {
            return "FeatureDetectionModule";
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
        return new graipe::FeatureDetectionModule;
	}
}




