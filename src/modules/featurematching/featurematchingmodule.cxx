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

#include "featurematching/featurematching.h"

#include <QElapsedTimer>

namespace graipe {
    
/**
 * This templated matching class represents the hull for blockwise points to image
 * matching approach. It does select a set of rect-aligned x,y-samples from the first 
 * image and searches (using the functor) for the best N matches of each point in 
 * the second image.
 */
template<class MATCHING_FUNCTOR>
class BlockWiseImageMatcher
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        BlockWiseImageMatcher()
        {
            m_parameters->addParameter("image1",    new ImageBandParameter<float>("Reference Image",	NULL));
            m_parameters->addParameter("image2",    new ImageBandParameter<float>("Second Image",	NULL));
            m_parameters->addParameter("x-samples", new IntParameter("x-Samples", 1, 999999));
            m_parameters->addParameter("y-samples", new IntParameter("y-Samples", 1, 999999));
            m_parameters->addParameter("mask_w",    new IntParameter("Mask width", 3, 999));
            m_parameters->addParameter("mask_h",    new IntParameter("Mask height", 3, 999));
            m_parameters->addParameter("max_d",     new IntParameter("Max Distance", 1, 999));
            m_parameters->addParameter("best_n",    new IntParameter("Find N best candidates", 1, 50));
            m_parameters->addParameter("gme?",      new BoolParameter("use global motion estimation"));
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
                    using namespace ::std;
                    using namespace ::vigra;
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand1 = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image1"]);
                    ImageBandParameter<float>	* param_imageBand2 = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image2"]);
                    IntParameter		* param_xSamples       = static_cast<IntParameter*> ((*m_parameters)["x-samples"]),
                                        * param_ySamples       = static_cast<IntParameter*> ((*m_parameters)["y-samples"]),
                                        * param_maskWidth      = static_cast<IntParameter*> ((*m_parameters)["mask_w"]),
                                        * param_maskHeight     = static_cast<IntParameter*> ((*m_parameters)["mask_h"]),
                                        * param_searchDistance = static_cast<IntParameter*> ((*m_parameters)["max_d"]),
                                        * param_nCandidates    = static_cast<IntParameter*> ((*m_parameters)["best_n"]);
                    BoolParameter		* param_useGME         = static_cast<BoolParameter*> ((*m_parameters)["gme?"]);
                    
                
                    vigra::MultiArrayView<2,float> imageband1 = param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 = param_imageBand2->value();
                        
                    PointFeatureList2D features_of_image1;
                        
                    unsigned int y_step = param_imageBand1->image()->height()/param_ySamples->value();
                    unsigned int x_step = param_imageBand1->image()->width()/param_xSamples->value();
                        
                    for(unsigned int y=y_step/2; y < param_imageBand1->image()->height(); y+=y_step)
                    {
                        for(unsigned int x=x_step/2; x < param_imageBand1->image()->width(); x+=x_step)
                        {
                            features_of_image1.addFeature(PointFeatureList2D::PointType(x, y));
                        }
                    }
                                                                                               
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    vigra::Matrix<double> mat = vigra::identityMatrix<double>(3);
                    double rotation_correlation=0, translation_correlation=0;
                    unsigned int used_distance = param_searchDistance->value();
                                                                                               
                    MATCHING_FUNCTOR func;
                    
                    QElapsedTimer timer;
                    timer.start();
                    
                    SparseWeightedMultiVectorfield2D* new_block_matching_vectorfield
                        = matchFeaturesToImage(imageband1,
                                               imageband2,
                                               features_of_image1, 
                                               func, 
                                               param_maskWidth->value(), param_maskHeight->value(), 
                                               param_searchDistance->value(), 
                                               param_nCandidates->value(), 
                                               param_useGME->value(),
                                               mat,
                                               rotation_correlation, translation_correlation,
                                               used_distance);
                    
                    qint64 processing_time = timer.elapsed();
                    
                    new_block_matching_vectorfield->setName(QString("I->I block matching with ") + param_imageBand1->toString() + QString(" and ") + param_imageBand2->toString());
                    
                    QString descr("The following parameters were used to calculate the Block Matching (Image->Image):\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));
                    
                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    descr += QString(   "Computed global motion matrix (I1 -> I2): %1\n"
                                        "rotation accuracy: %2\n"
                                        "translation accuracy: %3\n"
                                        "used maximum distance: %4\n"
                                        "processing time: %5 seconds").arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(used_distance).arg(processing_time/1000.0);
                    
                    new_block_matching_vectorfield->setDescription(descr);
                    
                    ((Model*)param_imageBand1->image())->copyGeometry(*new_block_matching_vectorfield);
                    new_block_matching_vectorfield->setGlobalMotion(transform);
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)param_imageBand1->image()->timestamp().secsTo(param_imageBand2->image()->timestamp());
                    
                    if(seconds != 0){
                        new_block_matching_vectorfield->setScale(param_imageBand1->image()->scale()*100.0/seconds);
                    }		
                    
                    m_results.push_back(new_block_matching_vectorfield);
                    
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
 * Creates a new block-wise image to image matcher, which uses
 * the fast unnormalized cross-correlation for similarity estimation
 *
 * \return A new instance of the BlockWiseImageMatcher<FastCCFunctor>.
 */
Algorithm* createBWMatcherFastCC()
{
	return new BlockWiseImageMatcher<FastCCFunctor>;
}

/**
 * Creates a new block-wise image to image matcher, which uses
 * the fast normalized cross-correlation for similarity estimation
 *
 * \return  A new instance of the BlockWiseImageMatcher<FastNCCFunctor>.
 */
Algorithm* createBWMatcherFastNCC()
{
	return new BlockWiseImageMatcher<FastNCCFunctor>;
}




/**
 * This templated matching class represents the hull for feature to feature matching strategoies.
 * It uses two feature lists and two (corresponding) images to search (using the functor)
 * for the best N matches of each point of the first list to each point of the second list.
 */
template<class MATCHING_FUNCTOR>
class FeatureToFeatureMatcher
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        FeatureToFeatureMatcher()
        {
            m_parameters->addParameter("image1",    new ImageBandParameter<float>("Reference image",	NULL));
            m_parameters->addParameter("features1", new ModelParameter("Features (of reference image)",	NULL,  "PointFeatureList2D | WeightedPointFeatureList2D | EdgelFeatureList2D"));
            m_parameters->addParameter("image2",    new ImageBandParameter<float>("Second Image",	NULL));
            m_parameters->addParameter("features2", new ModelParameter("Features (of second image)",		NULL,  "PointFeatureList2D | WeightedPointFeatureList2D | EdgelFeatureList2D"));
            m_parameters->addParameter("mask_w",    new IntParameter("Mask width", 3, 999));
            m_parameters->addParameter("mask_h",    new IntParameter("Mask height", 3, 999));
            m_parameters->addParameter("max_d",     new IntParameter("Max Distance", 1, 999));
            m_parameters->addParameter("best_n",    new IntParameter("Find N best candidates", 1, 50));
            m_parameters->addParameter("gme?",      new BoolParameter("use global motion estimation"));
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
                    using namespace ::std;
                    
                    emit statusMessage(0.0, QString("started"));
                        
                    ImageBandParameter<float>	* param_imageBand1         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image1"]);
                    ModelParameter	* param_features1		= static_cast<ModelParameter*> ( (*m_parameters)["features1"]);
                    ImageBandParameter<float>	* param_imageBand2         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image2"]);
                    ModelParameter	* param_features2		= static_cast<ModelParameter*> ( (*m_parameters)["features2"]);
                    
                    IntParameter        * param_maskWidth      = static_cast<IntParameter*> ((*m_parameters)["mask_w"]),
                                        * param_maskHeight     = static_cast<IntParameter*> ((*m_parameters)["mask_h"]),
                                        * param_searchDistance = static_cast<IntParameter*> ((*m_parameters)["max_d"]),
                                        * param_nCandidates    = static_cast<IntParameter*> ((*m_parameters)["best_n"]);
                    BoolParameter		* param_useGME         = static_cast<BoolParameter*> ((*m_parameters)["gme?"]);
                    
                    
                    vigra::MultiArrayView<2,float> imageband1 = param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 = param_imageBand2->value();
                    
                    vigra_assert( imageband1.shape() == imageband2.shape(), "image sizes differ!");
                    
                    PointFeatureList2D* features_of_image1 = static_cast<PointFeatureList2D*>( param_features1->value() );
                    PointFeatureList2D* features_of_image2 = static_cast<PointFeatureList2D*>( param_features2->value() );
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    vigra::Matrix<double> mat = vigra::identityMatrix<double>(3);
                    double rotation_correlation=0, translation_correlation=0;
                    unsigned int used_distance = param_searchDistance->value();
                    
                    MATCHING_FUNCTOR func;
                    
                    QElapsedTimer timer;
                    timer.start();
                                
                    SparseWeightedMultiVectorfield2D* new_vectorfield
                        = matchFeaturesToFeatures(imageband1,
                                                  imageband2,
                                                  *features_of_image1,
                                                  *features_of_image2,
                                                  func, 
                                                  param_maskWidth->value(), param_maskHeight->value(), 
                                                  param_searchDistance->value(), 
                                                  param_nCandidates->value(), param_useGME->value(),
                                                  mat,
                                                  rotation_correlation, translation_correlation,
                                                  used_distance);
                    
                    qint64 processing_time = timer.elapsed();
                    
                    new_vectorfield->setName(QString("F->F matching with ") + param_imageBand1->toString() + QString(" and ") + param_imageBand2->toString());
                    
                    QString descr("The following parameters were used to calculate the NCC (Features->Features):\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));

                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    descr += QString(   "Computed global motion matrix (I1 -> I2): %1\n"
                                        "rotation accuracy: %2\n"
                                        "translation accuracy: %3\n"
                                        "used maximum distance: %4\n"
                                        "processing time: %5 seconds").arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(used_distance).arg(processing_time/1000.0);
                    
                    new_vectorfield->setDescription(descr);

                    ((Model*)param_imageBand1->image())->copyGeometry(*new_vectorfield);
                    new_vectorfield->setGlobalMotion(transform);
                    
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)param_imageBand1->image()->timestamp().secsTo(param_imageBand2->image()->timestamp());
                    
                    if(seconds != 0){
                        new_vectorfield->setScale(param_imageBand1->image()->scale()*100.0/seconds);
                    }
                    m_results.push_back(new_vectorfield);
                    
                    
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
 * Creates one instance of the feature to feature matching
 * using the un-normalized correlation coefficient for feature/subimage comparison.
 *
 * \return A new instance of the FeatureToFeatureMatcher<CorrelationFunctor>.
 */
Algorithm* createFFMatcherC()
{
	return new FeatureToFeatureMatcher<CorrelationFunctor>;
}

/** 
 * Creates one instance of the feature to feature matching
 * using the correlation coefficient for feature/subimage comparison.
 *
 * \return A new instance of the FeatureToFeatureMatcher<NormalizedCorrelationFunctor>.
 */
Algorithm* createFFMatcherNC()
{
	return new FeatureToFeatureMatcher<NormalizedCorrelationFunctor>;
}




/**
 * This templated matching class represents the hull for feature to image matching strategoies.
 * It uses a feature list and two (corresponding) images to search (using the functor)
 * for the best N matches of each point of the first list to every position of the second image.
 */
template<class MATCHING_FUNCTOR>
class FeatureToImageMatcher
:   public Algorithm
{
	
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        FeatureToImageMatcher()
        {
            m_parameters->addParameter("image1",    new ImageBandParameter<float>("Reference image",	NULL));
            m_parameters->addParameter("features1", new ModelParameter("Features (of reference image)",	NULL,  "PointFeatureList2D | WeightedPointFeatureList2D | EdgelFeatureList2D"));
            m_parameters->addParameter("image2",    new ImageBandParameter<float>("Second Image",	NULL));
            m_parameters->addParameter("mask_w",    new IntParameter("Mask width", 3, 999));
            m_parameters->addParameter("mask_h",    new IntParameter("Mask height", 3, 999));
            m_parameters->addParameter("max_d",     new IntParameter("Max Distance", 1, 999));
            m_parameters->addParameter("best_n",    new IntParameter("Find N best candidates", 1, 50));
            m_parameters->addParameter("gme?",      new BoolParameter("use global motion estimation"));
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
                    using namespace ::std;
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand1  = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image1"]);
                    ModelParameter              * param_features1   = static_cast<ModelParameter*> ( (*m_parameters)["features1"]);
                    ImageBandParameter<float>	* param_imageBand2  = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image2"]);
                    
                    IntParameter        * param_maskWidth      = static_cast<IntParameter*> ((*m_parameters)["mask_w"]),
                                        * param_maskHeight     = static_cast<IntParameter*> ((*m_parameters)["mask_h"]),
                                        * param_searchDistance = static_cast<IntParameter*> ((*m_parameters)["max_d"]),
                                        * param_nCandidates    = static_cast<IntParameter*> ((*m_parameters)["best_n"]);
                    BoolParameter		* param_useGME         = static_cast<BoolParameter*> ((*m_parameters)["gme?"]);
                    
                    
                    vigra::MultiArrayView<2,float> imageband1 = param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 = param_imageBand2->value();
                    
                    vigra_assert( imageband1.size() == imageband2.size(), "image sizes differ!");
                    
                    PointFeatureList2D* features_of_image1 = static_cast<PointFeatureList2D*>( param_features1->value() );
                    
                    emit statusMessage(1.0, QString("starting computation"));
                
                    vigra::Matrix<double> mat = vigra::identityMatrix<double>(3);
                    double rotation_correlation=0, translation_correlation=0;
                    unsigned int used_distance = param_searchDistance->value();
                    
                    MATCHING_FUNCTOR func;
                    
                    QElapsedTimer timer;
                    timer.start();
                    
                    
                    SparseWeightedMultiVectorfield2D* new_vectorfield
                        = matchFeaturesToImage(imageband1,
                                               imageband2,
                                               *features_of_image1,
                                               func, 
                                               param_maskWidth->value(), param_maskHeight->value(), 
                                               param_searchDistance->value(),
                                               param_nCandidates->value(), param_useGME->value(),
                                               mat,
                                               rotation_correlation, translation_correlation,
                                               used_distance);
                    
                    qint64 processing_time = timer.elapsed();
                    
                    new_vectorfield->setName(QString("F->I Matching with ") + param_imageBand1->toString() + QString(" and ") + param_imageBand2->toString());
                    
                    QString descr("The following parameters were used to calculate the matching (Features->Image):\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));

                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    descr += QString(   "Computed global motion matrix (I1 -> I2): %1\n"
                                        "rotation accuracy: %2\n"
                                        "translation accuracy: %3\n"
                                        "used maximum distance: %4\n"
                                        "processing time: %5 seconds").arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(used_distance).arg(processing_time/1000.0);
                    
                    
                    new_vectorfield->setDescription(descr);
                    
                    ((Model*)param_imageBand1->image())->copyGeometry(*new_vectorfield);
                    new_vectorfield->setGlobalMotion(transform);
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)param_imageBand1->image()->timestamp().secsTo(param_imageBand2->image()->timestamp());
                
                    if(seconds != 0){
                        new_vectorfield->setScale(param_imageBand1->image()->scale()*100.0/seconds);
                    }		
                    
                    m_results.push_back(new_vectorfield);
                    
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
 * Creates one instance of the feature to image matching
 * using the fast unnormalized cross-correlation.
 *
 * \return A new instance of the FeatureToImageMatcher<FastCCFunctor>.
 */
Algorithm* createFIMatcherFastCC()
{
	return new FeatureToImageMatcher<FastCCFunctor>;
}


/** 
 * Creates one instance of the feature to image matching
 * using the fast normalized cross-correlation.
 *
 * \return A new instance of the FeatureToImageMatcher<FastNCCFunctor>.
 */
Algorithm* createFIMatcherFastNCC()
{
	return new FeatureToImageMatcher<FastNCCFunctor>;
}



/**
 * Special Feature matcher, which uses two featurelist and two
 * corresponding images, but does not use the features themselves for matching.
 * Instead, it creates "Shape Context"s of their distribution and uses these
 * signatures for matching
 */
class ShapeContextMatcher
:   public Algorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        ShapeContextMatcher()
        {
            m_parameters->addParameter("image1",    new ImageBandParameter<float>("Reference image",	NULL));
            m_parameters->addParameter("features1", new ModelParameter("Features (of reference image)",	NULL,  "PointFeatureList2D | WeightedPointFeatureList2D | EdgelFeatureList2D"));
            m_parameters->addParameter("image2",    new ImageBandParameter<float>("Second Image",	NULL));
            m_parameters->addParameter("features2", new ModelParameter("Features (of second image)",		NULL,  "PointFeatureList2D | WeightedPointFeatureList2D | EdgelFeatureList2D"));
            m_parameters->addParameter("mask_w",    new IntParameter("Mask width", 3, 999));
            m_parameters->addParameter("mask_h",    new IntParameter("Mask height", 3, 999));
            m_parameters->addParameter("max_d",     new IntParameter("Max Distance", 1, 999));
            m_parameters->addParameter("best_n",    new IntParameter("Find N best candidates", 1, 50));
            m_parameters->addParameter("gme?",      new BoolParameter("use global motion estimation"));
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
                    using namespace ::std;
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand1         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image1"]);
                    ModelParameter	* param_features1		= static_cast<ModelParameter*> ( (*m_parameters)["features1"]);
                    ImageBandParameter<float>	* param_imageBand2         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image2"]);
                    ModelParameter	* param_features2		= static_cast<ModelParameter*> ( (*m_parameters)["features2"]);
                    
                    IntParameter        * param_maskWidth      = static_cast<IntParameter*> ((*m_parameters)["mask_w"]),
                    * param_maskHeight     = static_cast<IntParameter*> ((*m_parameters)["mask_h"]),
                    * param_searchDistance = static_cast<IntParameter*> ((*m_parameters)["max_d"]),
                    * param_nCandidates    = static_cast<IntParameter*> ((*m_parameters)["best_n"]);
                    BoolParameter		* param_useGME         = static_cast<BoolParameter*> ((*m_parameters)["gme?"]);
                    
                    
                    vigra::MultiArrayView<2,float> imageband1 = param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 = param_imageBand2->value();
                    
                    vigra_assert( imageband1.size() == imageband2.size(), "image sizes differ!");
                    
                    PointFeatureList2D* features_of_image1 = static_cast<PointFeatureList2D*>( param_features1->value() );
                    PointFeatureList2D* features_of_image2 = static_cast<PointFeatureList2D*>( param_features2->value() );
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    vigra::Matrix<double> mat = vigra::identityMatrix<double>(3);
                    double rotation_correlation=0, translation_correlation=0;
                    unsigned int used_distance = param_searchDistance->value();
                    
                    QElapsedTimer timer;
                    timer.start();
                                
                    SparseWeightedMultiVectorfield2D* new_sc_vectorfield
                        = matchFeaturesToFeaturesUsingShapeContext(imageband1,
                                                                   imageband2,
                                                                   *features_of_image1,  *features_of_image2,
                                                                   param_maskWidth->value(), param_maskHeight->value(), 
                                                                   param_searchDistance->value(),
                                                                   param_nCandidates->value(), param_useGME->value(),
                                                                   mat,
                                                                   rotation_correlation, translation_correlation,
                                                                   used_distance);
                    
                    qint64 processing_time = timer.elapsed();
                    
                    new_sc_vectorfield->setName(QString("Shape Context matching (F->F) with ") + features_of_image1->name() + QString("and ") + features_of_image2->name());
                    QString descr("The following parameters were used to calculate the Shape Context matching (Features->Features):\n");
                    descr += m_parameters->valueText("ModelParameter");
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));

                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    descr += QString(   "Computed global motion matrix (I1 -> I2): %1\n"
                                        "rotation accuracy: %2\n"
                                        "translation accuracy: %3\n"
                                        "used maximum distance: %4\n"
                                        "processing time: %5 seconds").arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(used_distance).arg(processing_time/1000.0);
                    
                    new_sc_vectorfield->setDescription(descr);
                    
                    ((Model*)param_imageBand1->image())->copyGeometry(*new_sc_vectorfield);
                    new_sc_vectorfield->setGlobalMotion(transform);
                    
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)param_imageBand1->image()->timestamp().secsTo(param_imageBand2->image()->timestamp());
                    
                    if(seconds != 0){
                        new_sc_vectorfield->setScale(param_imageBand1->image()->scale()*100.0/seconds);
                    }		
                    
                    
                    m_results.push_back(new_sc_vectorfield);
                    
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
 * Creates one instance of the Shape Context Matcher
 * algorithm defined above.
 *
 * \return A new instance of the ShapeContextMatcher.
 */
Algorithm* createShapeContextMatcher()
{
	return new ShapeContextMatcher;
}





/**
 * Specialized matching procedure for SIFT features. 
 */
class SIFTMatcher
:   public Algorithm
{
    
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        SIFTMatcher()
        {
            m_parameters->addParameter("image1", new ImageBandParameter<float>("Reference image",	NULL));
            m_parameters->addParameter("sift1", new ModelParameter("SIFT Features (of reference image)",	NULL,  "SIFTFeatureList2D"));
            m_parameters->addParameter("image2", new ImageBandParameter<float>("Second Image",	NULL));
            m_parameters->addParameter("sift2", new ModelParameter("SIFT Features (of second image)",		NULL,  "SIFTFeatureList2D"));
            m_parameters->addParameter("max_sift_d", new FloatParameter("Max. distance of point descriptors", 1, 1000000,1000));
            m_parameters->addParameter("max_d", new FloatParameter("Max. geometrical distance of points", 1, 100000,100));
            m_parameters->addParameter("best_n", new IntParameter("Find N best candidates", 1, 50,10));
            m_parameters->addParameter("gme?", new BoolParameter("use global motion estimation"));
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
                    
                    using namespace ::std;
                    using namespace ::vigra;
                    
                    emit statusMessage(0.0, QString("started"));
                    
                    ImageBandParameter<float>	* param_imageBand1         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image1"]);
                    ModelParameter	* param_features1		= static_cast<ModelParameter*> ( (*m_parameters)["sift1"]);
                    ImageBandParameter<float>	* param_imageBand2         = static_cast<ImageBandParameter<float>*> ((*m_parameters)["image2"]);
                    ModelParameter	* param_features2		= static_cast<ModelParameter*> ( (*m_parameters)["sift2"]);
                
                    FloatParameter	*	param_maxDistance = static_cast<FloatParameter*> ( (*m_parameters)["max_sift_d"]),
                                    *	param_maxGeoDistance = static_cast<FloatParameter*> ( (*m_parameters)["max_geo_d"]);
                    IntParameter	*	param_nCandidates = static_cast<IntParameter*> ( (*m_parameters)["best_n"]);
                
                    BoolParameter	*	param_useGME = static_cast<BoolParameter*> ( (*m_parameters)["gme?"]);
                    
                    
                    vigra::MultiArrayView<2,float> imageband1 = param_imageBand1->value();
                    vigra::MultiArrayView<2,float> imageband2 = param_imageBand2->value();
                    
                    vigra_assert( imageband1.size() == imageband2.size(), "image sizes differ!");
                    
                    
                    SIFTFeatureList2D* features_of_image1 = static_cast<SIFTFeatureList2D*>( param_features1->value() );
                    SIFTFeatureList2D* features_of_image2 = static_cast<SIFTFeatureList2D*>( param_features2->value() );
                    
                    
                    emit statusMessage(1.0, QString("starting computation"));
                    
                    vigra::Matrix<double> mat = vigra::identityMatrix<double>(3);
                    double rotation_correlation=0, translation_correlation=0;
                    unsigned int used_distance = param_maxGeoDistance->value();
                    
                    QElapsedTimer timer;
                    timer.start();
                                
                    SparseWeightedMultiVectorfield2D* new_sift_vectorfield
                        = matchSIFTFeaturesUsingDistance(imageband1,
                                                         imageband2,
                                                         *features_of_image1,
                                                         *features_of_image2,
                                                         param_maxDistance->value(),
                                                         param_maxGeoDistance->value(),
                                                         param_nCandidates->value(),
                                                         param_useGME->value(),
                                                         mat,
                                                         rotation_correlation, translation_correlation,
                                                         used_distance);
                    
                    qint64 processing_time = timer.elapsed();
                    
                    new_sift_vectorfield->setName(QString("SIFT (F->F) with ") + features_of_image1->name() + QString("and ") + features_of_image2->name());
                    
                    QString descr("The following parameters were used to calculate the SIFT matching (Features->Features):\n");
                    descr += m_parameters->valueText("ModelParameter");
                    
                    QTransform transform(mat(0,0), mat(1,0), mat(2,0),
                                         mat(0,1), mat(1,1), mat(2,1),
                                         mat(0,2), mat(1,2), mat(2,2));

                    QString mat_str = "";//TODO:TransformParameter::valueText(transform);
                    
                    descr += QString(   "Computed global motion matrix (I1 -> I2): %1\n"
                                        "rotation accuracy: %2\n"
                                        "translation accuracy: %3\n"
                                        "used maximum distance: %4\n"
                                        "processing time: %5 seconds").arg(mat_str).arg(rotation_correlation).arg(translation_correlation).arg(used_distance).arg(processing_time/1000.0);
                                        
                    new_sift_vectorfield->setDescription(descr);
                    
                    ((Model*)param_imageBand1->image())->copyGeometry(*new_sift_vectorfield);
                    new_sift_vectorfield->setGlobalMotion(transform);
                    
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)param_imageBand1->image()->timestamp().secsTo(param_imageBand2->image()->timestamp());
                    
                    if(seconds != 0){
                        new_sift_vectorfield->setScale(param_imageBand1->image()->scale()*100.0/seconds);
                    }		
                    
                    
                    m_results.push_back(new_sift_vectorfield);
                    
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
 * Creates one instance of the SIFT Matcher
 * algorithm defined above.
 *
 * \return A new instance of the SIFTMatcher.
 */
Algorithm* createSIFTMatcher()
{
	return new SIFTMatcher;
}




/**
 * This class encapsulates all the functionality of this module in a 
 * way that it can be used within graipe. To achieve this, it extends
 * graipe's Module class by introducing new algorithms.
 */
class FeatureMatchingModule
:   public Module
{
	public:
        /**
         * Default constructor for the FeatureMatchingModule.
         */
		FeatureMatchingModule()
		{
        }
    
        /**
         * Returns the ModelFactory of the FeatureMatchingModule.
         * Since this module does not introduce any new Models, it will
         * return an empty ModelFactory.
         *
         * \return An empty ModelFactory of the FeatureMatchingModule.
         */
		ModelFactory modelFactory() const
		{
			return ModelFactory();
		}
    
        /**
         * Returns the ViewControllerFactory of the FeatureMatchingModule.
         * Since this module does not introduce any new Models,  no views are
         * needed. Thus, it will return an empty ViewControllerFactory.
         *
         * \return An empty ViewControllerFactory of the FeatureMatchingModule.
         */
		ViewControllerFactory viewControllerFactory() const
		{
			return ViewControllerFactory();
		}
		
        /**
         * Returns the AlgorithmFactory of the FeatureMatchingModule.
         * Here, we provide the tree algorithms defined above.
         *
         * \return An AlgorithmFactory containing the analysis algorithms defined herein.
         */
		AlgorithmFactory algorithmFactory() const
		{
			AlgorithmFactory alg_factory;
			
			//Match features to features
			AlgorithmFactoryItem alg_item;
			alg_item.topic_name = "Feature matching";

			//1. Using (unnormalized) cross-correlation	
			alg_item.algorithm_name = "Cross-correlation (F->F)";
			alg_item.algorithm_fptr = &createFFMatcherC;
			alg_factory.push_back(alg_item);
			
			//2. Using (fast) normalized cross-correlation
			alg_item.algorithm_name = "Normalized cross-correlation (F->F)";	
			alg_item.algorithm_fptr = &createFFMatcherNC;
			alg_factory.push_back(alg_item);
			
			//3. Using Shape Contexts and matching
			alg_item.algorithm_name = "Shape Context matching (F->F)";	
			alg_item.algorithm_fptr = &createShapeContextMatcher;
			alg_factory.push_back(alg_item);
			
			//4. Using SIFT features
			alg_item.algorithm_name = "SIFT matching (F->F)";	
			alg_item.algorithm_fptr = &createSIFTMatcher;
			alg_factory.push_back(alg_item);
			
			
			//Match features to image (no second featureset needed for this class of algorithms)
			
			//1. Fast Cross-Correlation of features->image
			alg_item.algorithm_name = "Fast cross-correlation (F->I)";		
			alg_item.algorithm_fptr = &createFIMatcherFastCC;
			alg_factory.push_back(alg_item);
			
			//2. Fast Normalized Cross-Correlation of features->image
			alg_item.algorithm_name = "Fast normalized cross-correlation (F->I)";
			alg_item.algorithm_fptr = &createFIMatcherFastNCC;
			alg_factory.push_back(alg_item);
			
			
			
			//Match image to image (blockwise)			
			
			//1. blockwise using Fast cross-correlation
			alg_item.algorithm_name = "Fast cross-correlation block-matching (I->I)";	
			alg_item.algorithm_fptr = &createBWMatcherFastCC;
			alg_factory.push_back(alg_item);
			
			//2. blockwise using fast Normalized Cross-Correlation
			alg_item.algorithm_name = "Fast normalized cross-correlation block-matching (I->I)";
			alg_item.algorithm_fptr = &createBWMatcherFastNCC;
			alg_factory.push_back(alg_item);
			
			return alg_factory;
		}
		
        /**
         * Returns the name of this Module.
         * 
         * \return Always: "FeatureMatchingModule"
         */
		QString name() const
        {
            return "FeatureMatchingModule";
        }
};
    
} //end of namespace graipe

/**
 *Interface to the ModuleHandler
 */

/**
 *  The initialization procedure returns a pointer to the
 *  FeatureMatchingModule (which inherits from Module) acutal
 *  implementation of the class above
 *
 *	\return The pointer to a new instance of this module.
 */
#include <QtCore/QtGlobal>
extern "C"{
    Q_DECL_EXPORT graipe::Module* initialize()
	{
        return new graipe::FeatureMatchingModule;
	}
}
