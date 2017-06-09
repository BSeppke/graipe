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

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOWALGORITHMS_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOWALGORITHMS_HXX

#include "images/images.h"
#include "vectorfields/vectorfields.h"
#include "registration/registration.h"

#include "opticalflow_local.hxx"
#include "opticalflow_global.hxx"
#include "opticalflow_hybrid.hxx"

namespace graipe {

/**
 * The different hierarchical traversal strategies on the scale space. 
 * This is used inside the general Optical Flow framework to define the step
 * list for scale space.
 *
 * \return A QStringList containing V, W and Full W mode.
 */
QStringList hierarchical_modes()
{ 
	QStringList hierarchical_modes;
	hierarchical_modes.append("V \tBottom-Up and Top-Down");
	hierarchical_modes.append("W \tExtended V-mode");
	hierarchical_modes.append("Full W \tExtended W-mode");

	return hierarchical_modes;
}
/**
 * When hierarchical traversal strategies on the scale space are used, we need to
 * define the flow-progration strategy from one layer/octave to the next.
 *
 * \return A QStringList containing the available modes.
 */
QStringList propagation_modes()
{
	QStringList propagation_modes;
	propagation_modes.append("Initialize with previous result");
	propagation_modes.append("Warp according to previous result using subsampling to 5x5");
    propagation_modes.append("Warp according to previous result using subsampling of 10x10");
    propagation_modes.append("Warp according to previous result using subsampling of 15x15");
    propagation_modes.append("Warp according to previous result using subsampling of 20x20");
    propagation_modes.append("Warp according to previous result using subsampling of 25x25");
    propagation_modes.append("Warp according to previous result using subsampling of 30x30");
    propagation_modes.append("Warp according to previous result using subsampling of 35x35");
	propagation_modes.append("Warp according to previous result using subsampling of 40x40");
    propagation_modes.append("Warp according to previous result using subsampling of 45x45");
    propagation_modes.append("Warp according to previous result using subsampling of 50x50");

	return propagation_modes;
}

/**
 * This class defines the most general common part of all Optical Flow estimation 
 * algorithms by means of a specialization of graipe::Algorithm
 */
class OpticalFlowAlgorithm
: public Algorithm
{	
    public:
        /**
         * The default constructor. Does not introduce the commonly used parameters, since
         * we want to have control over ther orderung of them. See the following two member 
         * functions for further details.
         */
        OpticalFlowAlgorithm()
        {
        }
    
        QString typeName() const
        {
            return "OpticalFlowAlgorithm";
        }
        
    protected:
        /**
         * This function adds the image and mask parameters to an algorithm's instance.
         */
        virtual void addImageAndMaskParameters()
        {
            m_param_imageBand1		= new ImageBandParameter<float>("Reference Image", NULL);
            m_param_imageBand2		= new ImageBandParameter<float>("Second Image",	NULL);
            
            m_param_useMask			= new BoolParameter("use image band for masking flow");
            m_param_mask			= new ImageBandParameter<float>("Mask Image", m_param_useMask);
            
            m_parameters->addParameter("band1", m_param_imageBand1 );
            m_parameters->addParameter("band2", m_param_imageBand2 );
            
            
            m_parameters->addParameter("use_mask?", m_param_useMask );
            m_parameters->addParameter("mask", m_param_mask );
        }
        
        /**
         * This function adds the Optical Flow Framework parameters to an algorithm's instance.
         */
        void addFrameworkProcessingParameters()
        {
            
            m_param_useGME			= new BoolParameter("use global motion estimation");
            m_param_useHierarchy	= new BoolParameter("use hierarchical method");
            m_param_lowestLevel		= new IntParameter("lowest pyramid level", 0, 10, 0, m_param_useHierarchy);
            m_param_highestLevel	= new IntParameter("highest pyramid level", 0, 10, 3, m_param_useHierarchy);
            m_param_hmode			= new EnumParameter("scale processing", hierarchical_modes(), 0, m_param_useHierarchy);
            m_param_pmode			= new EnumParameter("propagation strategy", propagation_modes(), 0, m_param_useHierarchy);
            m_param_warp_sigma  	= new FloatParameter("apply gaussian smoothing before each warping using sigma: (0.0 = none)", 0.0, 50.0, 1.0, m_param_useHierarchy);
            
            
            m_param_saveIntermediateImages	= new BoolParameter("save intermediate (warped) images", false, m_param_useHierarchy);
            m_param_saveIntermediateFlow	= new BoolParameter("save intermediate flow fields", false, m_param_useHierarchy);
            
            m_parameters->addParameter("use_gme?", m_param_useGME);
            m_parameters->addParameter("use_hierarchy?", m_param_useHierarchy );
            m_parameters->addParameter("lowL", m_param_lowestLevel );
            m_parameters->addParameter("hiL", m_param_highestLevel );
            m_parameters->addParameter("hmode", m_param_hmode );
            m_parameters->addParameter("pmode", m_param_pmode );
            m_parameters->addParameter("warp_sigma", m_param_warp_sigma );
            m_parameters->addParameter("save-intermI", m_param_saveIntermediateImages );
            m_parameters->addParameter("save-intermVF", m_param_saveIntermediateFlow );
        }	
        
        /**
         * This templated (by the flow functor) function defines the prototype for all
         * flow processor calls according to the chosen parameters.
         *
         * \param func The Optical Flow Functor, which will carry out each step's 
         *             flow estimation.
         */
        template<class OpticalFlowFunctor>
        void computeFlow(OpticalFlowFunctor func)
        {
            typedef typename OpticalFlowFunctor::FlowValueType FlowValueType;
            
            vigra_assert(FlowValueType().size() > 1, "flow functor needs to return a vectorfield of at least (u,v) components");
            
            
            vigra::MultiArrayView<2,float> imageband1 = m_param_imageBand1->value();
            vigra::MultiArrayView<2,float> imageband2 = m_param_imageBand2->value();
            
            //Mask
            vigra::MultiArrayView<2,float> mask = m_param_mask->value();
            if(m_param_useMask->value())
                vigra_assert( mask.size() == imageband1.size(), "mask and image sizes differ!");
            
            std::vector<vigra::MultiArray<2,float> > img_list;
            std::vector<vigra::MultiArray<2,FlowValueType> > flow_list;
            std::vector<vigra::Matrix<double> > mat_list;
            std::vector<double> rotation_correlation_list;
            std::vector<double> translation_correlation_list;
            
            flow_list.push_back(vigra::MultiArray<2,FlowValueType>(imageband1.shape()));
            mat_list.push_back(vigra::Matrix<double>(3,3));
            rotation_correlation_list.push_back(0);
            translation_correlation_list.push_back(0);
            
            if ( !m_param_useHierarchy->value())
            {
                if (m_param_useMask->value()) 
                {
                    calculateOFCE(imageband1,
                                  imageband2,
                                  mask,
                                  flow_list[0],
                                  func,
                                  m_param_useGME->value(),
                                  mat_list[0],
                                  rotation_correlation_list[0],
                                  translation_correlation_list[0]);
                }
                else
                {
                    calculateOFCE(imageband1,
                                  imageband2,
                                  flow_list[0],
                                  func,
                                  m_param_useGME->value(),
                                  mat_list[0],
                                  rotation_correlation_list[0],
                                  translation_correlation_list[0]);
                }
            }
            
            else if(m_param_pmode->value() == 0)
            {
                
                if (m_param_useMask->value()) 
                {
                    calculateOFCEHierarchicallyInitialiser(imageband1,
                                                           imageband2,
                                                           mask,
                                                           flow_list,
                                                           func,
                                                           m_param_useGME->value(),
                                                           mat_list,
                                                           rotation_correlation_list,
                                                           translation_correlation_list,
                                                           m_param_highestLevel->value(), m_param_lowestLevel->value(),
                                                           m_param_hmode->value());
                }
                else
                {
                    calculateOFCEHierarchicallyInitialiser(imageband1,
                                                           imageband2,
                                                           flow_list,
                                                           func,
                                                           m_param_useGME->value(),
                                                           mat_list,
                                                           rotation_correlation_list,
                                                           translation_correlation_list,
                                                           m_param_highestLevel->value(), m_param_lowestLevel->value(),
                                                           m_param_hmode->value());
                    
                }
                
            }
            else
            {
                WarpTPSFunctor warp_func;
                if (m_param_useMask->value()) 
                {
                    calculateOFCEHierarchicallyWarping(imageband1,
                                                       imageband2,
                                                       mask,
                                                       img_list,
                                                       flow_list,
                                                       func,
                                                       m_param_useGME->value(),
                                                       mat_list,
                                                       rotation_correlation_list,
                                                       translation_correlation_list,
                                                       m_param_highestLevel->value(), m_param_lowestLevel->value(), m_param_hmode->value(),
                                                       warp_func, 5*m_param_pmode->value(), m_param_warp_sigma->value());
                }
                else 
                {
                    calculateOFCEHierarchicallyWarping(imageband1,
                                                       imageband2,
                                                       img_list,
                                                       flow_list,
                                                       func,
                                                       m_param_useGME->value(),
                                                       mat_list,
                                                       rotation_correlation_list,
                                                       translation_correlation_list,
                                                       m_param_highestLevel->value(), m_param_lowestLevel->value(), m_param_hmode->value(),
                                                       warp_func, 5*m_param_pmode->value(), m_param_warp_sigma->value());
                }
                
            }	
            
            for (unsigned int i=0; i< flow_list.size(); ++i)
            {
                //Save pyramid of vectorfields on demand
                if(i==0 || m_param_saveIntermediateFlow->value())
                {
                    DenseVectorfield2D* new_vectorfield = NULL;
                    
                    if(FlowValueType().size() == 2)
                    {
                       new_vectorfield =  new DenseVectorfield2D(flow_list[i].bindElementChannel(0),flow_list[i].bindElementChannel(1));
                    }
                    //Has to be larger
                    else
                    {
                        new_vectorfield =  new DenseWeightedVectorfield2D(flow_list[i].bindElementChannel(0),flow_list[i].bindElementChannel(1),flow_list[i].bindElementChannel(2));
                    }
                    
                    QString functor_name = QString::fromStdString(OpticalFlowFunctor::name());
                    QString functor_sname = QString::fromStdString(OpticalFlowFunctor::shortName());
                    
                    if( i != 0)
                    {
                        new_vectorfield->setName(QString("%1 (L%2) of %3 and %4").arg(functor_sname).arg(i).arg(m_param_imageBand1->toString()).arg(m_param_imageBand2->toString()));
                    }
                    else
                    {
                        new_vectorfield->setName(QString("%1 of %2 and %3").arg(functor_sname).arg(m_param_imageBand1->toString()).arg(m_param_imageBand2->toString()));
                    }
                    new_vectorfield->setGlobalMotion(QTransform(mat_list[i](0,0), mat_list[i](1,0), mat_list[i](2,0),
                                                                mat_list[i](0,1), mat_list[i](1,1), mat_list[i](2,1),
                                                                mat_list[i](0,2), mat_list[i](1,2), mat_list[i](2,2)));
                                                                
                    qDebug() << "Assigning GME for VF:" << new_vectorfield->globalMotion();
                    qDebug() << "Inverted GME for VF:" << new_vectorfield->globalMotion().inverted();
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)m_param_imageBand1->image()->timestamp().secsTo(m_param_imageBand2->image()->timestamp());
                    
                    if(seconds != 0)
                    {
                        new_vectorfield->setScale(m_param_imageBand1->image()->scale()*100.0/seconds * (m_param_imageBand1->image()->width()/flow_list[0].width()));
                    }
                    
                    QString descr = QString("The following parameters were used to calculate the %1\n").arg(functor_name);
                    
                    if( i != 0)
                    {
                        descr += QString("Level %1 of %2\n").arg(i).arg(flow_list.size());
                    }
                    descr += m_parameters->valueText("ImageBandParameter<float>");
                    new_vectorfield->setDescription(descr);
                    m_results.push_back(new_vectorfield);
                }
                //Also save warped images on demand
                if(m_param_pmode->value() !=0 && i!=0 && m_param_saveIntermediateImages->value()) 
                {
                    Image<float>* new_image = new Image<float>(img_list[i].shape(), 1);
                    new_image->setBand(0,img_list[i]);
                    
                    m_param_imageBand1->image()->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Warped Image (L%1) of %2").arg(i).arg(m_param_imageBand1->toString()));
                    new_image->setDescription(QString(  "The following parameters were used to calculate the warping:\n"
                                                        "TPS Functor\n"
                                                        "Subsampled each %1 pixel").arg(5*m_param_pmode->value()));
                    m_results.push_back(new_image);
                }
            }
        }
            
    protected:
        ImageBandParameter<float> * m_param_imageBand1;
        ImageBandParameter<float> * m_param_imageBand2;
        
        BoolParameter *  m_param_useMask;
        ImageBandParameter<float> * m_param_mask;
        
        BoolParameter	*  m_param_useGME;
        BoolParameter	*  m_param_useHierarchy;
        
        IntParameter *   m_param_lowestLevel;
        IntParameter *   m_param_highestLevel;
        
        EnumParameter		* m_param_hmode;
        EnumParameter		* m_param_pmode;
        
        FloatParameter *   m_param_warp_sigma;
        
        BoolParameter	* m_param_saveIntermediateImages;
        BoolParameter	* m_param_saveIntermediateFlow;
};



    
/**
 * This templated class provides the base for all Horn-Schunk-like Optical Flow algorithms. 
 * The Flow computation itself is given as the template argument (a functor) and will be 
 * called using the specialized graipe::OpticalFlowAlgorithm class.
 */
template<class OPTICALFLOW_FUNCTOR>
class OpticalFlowHSEstimator
:   public OpticalFlowAlgorithm
{	
	public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
		OpticalFlowHSEstimator()
		{
			addImageAndMaskParameters();
			
			m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 30, 1);
			m_param_alpha = new FloatParameter("Weight alpha", 0, 99999, 1);
			m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 100);
			
			m_parameters->addParameter("sigma", m_param_sigma );
			m_parameters->addParameter("alpha", m_param_alpha );
			m_parameters->addParameter("iterations", m_param_iterations );
		
			addFrameworkProcessingParameters();
		}
    
        QString typeName() const
        {
            return "OpticalFlowHSEstimator";
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
                    
                    OPTICALFLOW_FUNCTOR func(m_param_alpha->value(),
                                             m_param_iterations->value(),
                                             m_param_sigma->value());
                    
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
        FloatParameter * m_param_sigma;
        FloatParameter * m_param_alpha;
        IntParameter* m_param_iterations;
};




/**
 * This templated class provides the base for all Bruhns "combined local global"-like Optical Flow algorithms.
 * The Flow computation itself is given as the template argument (a functor) and will be 
 * called using the specialized graipe::OpticalFlowAlgorithm class.
 */
template<class OPTICALFLOW_FUNCTOR>
class OpticalFlowBruhnEstimator
:   public OpticalFlowAlgorithm
{	
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowBruhnEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_inner_sigma = new FloatParameter("inner sigma of gauss. gradient", 0, 30, 1);
            m_param_outer_sigma = new FloatParameter("outer sigma of gauss. gradient", 0, 30, 5);
            m_param_alpha = new FloatParameter("Weight alpha", 0, 100, 1);
            m_param_omega = new FloatParameter("Weight omega", 0, 100, 1);
            m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 100);
            
            
            m_parameters->addParameter("sigma1", m_param_inner_sigma );
            m_parameters->addParameter("sigma2", m_param_outer_sigma );
            m_parameters->addParameter("alpha", m_param_alpha );
            m_parameters->addParameter("omega", m_param_omega );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowBruhnEstimator";
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
                    
                    OPTICALFLOW_FUNCTOR	func(m_param_alpha->value(),
                                             m_param_inner_sigma->value(), 
                                             m_param_outer_sigma->value(),
                                             m_param_omega->value(),
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
        FloatParameter * m_param_inner_sigma;
        FloatParameter * m_param_outer_sigma;
        FloatParameter * m_param_alpha;
        FloatParameter * m_param_omega;	
        IntParameter* m_param_iterations;
};




/**
 * This class provides the good-old Lucas-Kanade Optical Flow algorithms by means
 * of a specialized graipe::OpticalFlowAlgorithm class.
 */
class OpticalFlowLKEstimator
:   public OpticalFlowAlgorithm
{	
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowLKEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 30, 1);
            m_param_mask_size = new IntParameter("Mask size", 3, 1000, 31);
            m_param_threshold = new FloatParameter("Threshold (lower ew)", 0, 100000, 0);
            m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 1);
            
            m_parameters->addParameter("sigma", m_param_sigma );
            m_parameters->addParameter("mask_size", m_param_mask_size );
            m_parameters->addParameter("T", m_param_threshold );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowLKEstimator";
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
                    
                    OpticalFlowLKFunctor func(m_param_sigma->value(), 
                                              m_param_mask_size->value(),
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
        FloatParameter * m_param_sigma;
        IntParameter * m_param_mask_size;
        FloatParameter * m_param_threshold;	
        IntParameter* m_param_iterations;
};




/**
 * This class provides the approach of Farnebaeck using polynomial expansion to
 * derive the Optical Flow by means of a specialized graipe::OpticalFlowAlgorithm class.
 */
class OpticalFlowFBEstimator
:   public OpticalFlowAlgorithm
{
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowFBEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 30, 1);
            m_param_mask_size = new IntParameter("Mask size", 3, 1000, 31);
            m_param_threshold = new FloatParameter("threshold (det >= t)", 0, 1000000000, 0);
            m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 1);
            
            m_parameters->addParameter("sigma", m_param_sigma );
            m_parameters->addParameter("mask_size", m_param_mask_size );
            m_parameters->addParameter("T", m_param_threshold );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowFBEstimator";
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
                    
                    OpticalFlowFBFunctor func(m_param_sigma->value(),
                                              m_param_mask_size->value(),
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
        FloatParameter * m_param_sigma;
        IntParameter * m_param_mask_size;
        FloatParameter * m_param_threshold;
        IntParameter* m_param_iterations;	
};




/**
 * This templated class provides the base for all "Structure Tensor"-based Optical Flow algorithms.
 * The Flow computation itself is given as the template argument (a functor) and will be 
 * called using the specialized graipe::OpticalFlowAlgorithm class.
 */
template<class OPTICALFLOW_FUNCTOR>
class OpticalFlowTensorEstimator
:   public OpticalFlowAlgorithm
{	
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowTensorEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_inner_sigma = new FloatParameter("inner sigma of gauss. gradient", 0, 30, 1);
            m_param_outer_sigma = new FloatParameter("outer sigma of gauss. smoothing", 0, 100, 5);
            m_param_mask_size = new IntParameter("mask size", 1, 1000, 31);
            m_param_threshold = new FloatParameter("Threshold (lower ew)", 0, 1000000000, 0);
            m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 1);
            
            
            m_parameters->addParameter("sigma1", m_param_inner_sigma );
            m_parameters->addParameter("sigma2", m_param_outer_sigma );
            m_parameters->addParameter("mask_size", m_param_mask_size );
            m_parameters->addParameter("T", m_param_threshold );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowTensorEstimator";
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
                    
                    OPTICALFLOW_FUNCTOR func(m_param_inner_sigma->value(), 
                                             m_param_outer_sigma->value(),
                                             m_param_mask_size->value(),
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
        FloatParameter * m_param_inner_sigma;
        FloatParameter * m_param_outer_sigma;
        IntParameter* m_param_mask_size;
        FloatParameter * m_param_threshold;	
        IntParameter* m_param_iterations;
};




/**
 * This class provides the approach of Verri using constant contrast assumption to
 * derive the Optical Flow by means of a specialized graipe::OpticalFlowAlgorithm class.
 */
class OpticalFlowCCEstimator
:   public OpticalFlowAlgorithm
{	
    public:
        /**
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         */
        OpticalFlowCCEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_sigma = new FloatParameter("sigma of gauss. gradient", 0, 30, 1);
            m_param_threshold = new FloatParameter("Threshold (lower ew)", 0, 100000, 0);
            m_param_iterations = new IntParameter("No. of iterations", 1, 1000, 1);
            
            m_parameters->addParameter("sigma", m_param_sigma );
            m_parameters->addParameter("T", m_param_threshold );
            m_parameters->addParameter("iterations", m_param_iterations );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowCCEstimator";
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
                    
                    OpticalFlowCCFunctor func(m_param_sigma->value(),
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
        FloatParameter * m_param_sigma;
        FloatParameter * m_param_threshold;	
        IntParameter* m_param_iterations;
};




/**
 * Experimental algorithms are not working right now.
 * 
 * TODO: If possible, fix them. If not, discard them.
 */
/*
template<class OPTICALFLOW_FUNCTOR>
class OpticalFlowExperimentalEstimator
:   public OpticalFlowAlgorithm
{	
    public:
         **
         * Default constructor. Adds all neccessary parameters for this algorithm to run.
         *
        OpticalFlowExperimentalEstimator()
        {
            addImageAndMaskParameters();
            
            m_param_inner_sigma = new FloatParameter("inner sigma of gauss. gradient", 0, 30, 1);
            m_param_outer_sigma = new FloatParameter("outer sigma of gauss. smoothing", 0, 100, 5);
            m_param_threshold = new FloatParameter("Threshold (lower ew)", 0, 1000000000, 0);
            
            m_parameters->addParameter("sigma1", m_param_inner_sigma );
            m_parameters->addParameter("sigma2", m_param_outer_sigma );
            m_parameters->addParameter("T", m_param_threshold );
            
            addFrameworkProcessingParameters();
        }
    
        QString typeName() const
        {
            return "OpticalFlowExperimentalEstimator";
        }
 

         **
         * Specialization of the running phase of this algorithm.
         *
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
                    
                    OPTICALFLOW_FUNCTOR func(m_param_inner_sigma->value(), 
                                             m_param_outer_sigma->value(),
                                             m_param_threshold->value());
                    
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
        FloatParameter * m_param_inner_sigma;
        FloatParameter * m_param_outer_sigma;
        FloatParameter * m_param_threshold;
};
*/


} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOWALGORITHMS_HXX
