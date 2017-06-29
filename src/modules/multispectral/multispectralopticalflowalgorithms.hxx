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

#ifndef GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOWALGORITHMS_HXX
#define GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOWALGORITHMS_HXX

#include "opticalflow/opticalflowalgorithms.hxx"
#include "multispectral/multispectralopticalflowframework.hxx"


namespace graipe {
    
/**
 * This class implements the base for all 2band / multispectral Optical
 * Flow estimation algorithms. Common parameters are already introduced.
 * This class inherits from graipe::Algorithm.
 */
class OpticalFlowAlgorithm2Bands
:   public Algorithm
{	
    public:
        /**
         * The default constructor. Does not introduce the commonly used parameters, since
         * we want to have control over ther orderung of them. See the following two member 
         * functions for further details.
         */
        OpticalFlowAlgorithm2Bands(Workspace* wsp)
        : Algorithm(wsp)
        {
        }

    
        QString typeName() const
        {
            return "OpticalFlowAlgorithm2Bands";
        }
    protected:
        /**
         * This function adds the image and mask parameters to an algorithm's instance.
         */
        void addImageAndMaskParameters()
        {
            m_param_image1		= new ModelParameter("Reference Image",  "Image", NULL, false, m_environment);
            m_param_image1Band1	= new IntParameter("Reference Image band 1", 0, 200,0);
            m_param_image1Band2	= new IntParameter("Reference Image band 2", 0, 200,0);
            m_param_image2		= new ModelParameter("Second Image", "Image", NULL, false, m_environment);
            m_param_image2Band1	= new IntParameter("Second Image band 1", 0, 200,0);
            m_param_image2Band2	= new IntParameter("Second Image band 2", 0, 200,0);
            
            m_param_useMask			= new BoolParameter("use image band for masking flow");
            m_param_mask			= new ImageBandParameter<float>("Mask Image band", m_param_useMask, false, m_environment);
            
            m_parameters->addParameter("image1", m_param_image1 );
            m_parameters->addParameter("i1-band1", m_param_image1Band1 );
            m_parameters->addParameter("i1-band1", m_param_image1Band2 );
            m_parameters->addParameter("image2",  m_param_image2 );
            m_parameters->addParameter("i2-band1", m_param_image2Band1 );
            m_parameters->addParameter("i2-band2", m_param_image2Band2 );
            
            
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
            
            Image<float>* image1 = static_cast<Image<float>*>(m_param_image1->value());
            Image<float>* image2 = static_cast<Image<float>*>(m_param_image2->value());
            
            //Check for bands and image sizes:
            vigra_assert(	(unsigned int)m_param_image1Band1->value() < image1->numBands() 
                         &&	(unsigned int)m_param_image1Band2->value() < image1->numBands()
                         
                         &&	(unsigned int)m_param_image2Band1->value() < image2->numBands()
                         &&	(unsigned int)m_param_image2Band2->value() < image2->numBands(), "Invalid band numbers!");
            
            vigra_assert(m_param_image1Band1->value() != m_param_image1Band2->value(), "Need to use different bands for first image");
            vigra_assert(m_param_image2Band1->value() != m_param_image2Band2->value(), "Need to use different bands for second image");
            
            //Mask
            vigra::MultiArrayView<2,float> mask = m_param_mask->value();
            vigra_assert( mask.shape() == image1->size(), "mask and image sizes differ!");
            
            std::vector<vigra::MultiArray<2,float>> img11_list, img12_list;	
            
            std::vector<vigra::MultiArray<2,FlowValueType> > flow_list;
            std::vector<vigra::Matrix<double> > mat_list;
            std::vector<double> rotation_correlation_list;
            std::vector<double> translation_correlation_list;
            
            flow_list.push_back(vigra::MultiArray<2,FlowValueType> (image1->size()));
            mat_list.push_back(vigra::Matrix<double>(3,3));
            rotation_correlation_list.push_back(0);
            translation_correlation_list.push_back(0);
            
            if ( !m_param_useHierarchy->value())
            {
                if (m_param_useMask->value()) 
                {
                    calculateOFCE2Bands(image1->band(m_param_image1Band1->value()),
                                        image1->band(m_param_image1Band2->value()),
                                        image2->band(m_param_image2Band1->value()),
                                        image2->band(m_param_image2Band2->value()),
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
                    calculateOFCE2Bands(image1->band(m_param_image1Band1->value()),
                                        image1->band(m_param_image1Band2->value()),
                                        image2->band(m_param_image2Band1->value()),
                                        image2->band(m_param_image2Band2->value()),
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
                    calculateOFCEHierarchicallyInitialiser2Bands(image1->band(m_param_image1Band1->value()),
                                                                 image1->band(m_param_image1Band2->value()),
                                                                 image2->band(m_param_image2Band1->value()),
                                                                 image2->band(m_param_image2Band2->value()),
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
                    calculateOFCEHierarchicallyInitialiser2Bands(image1->band(m_param_image1Band1->value()),
                                                                 image1->band(m_param_image1Band2->value()),
                                                                 image2->band(m_param_image2Band1->value()),
                                                                 image2->band(m_param_image2Band2->value()),
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
                    calculateOFCEHierarchicallyWarping2Bands(image1->band(m_param_image1Band1->value()),
                                                             image1->band(m_param_image1Band2->value()),
                                                             image2->band(m_param_image2Band1->value()),
                                                             image2->band(m_param_image2Band2->value()),
                                                             mask,
                                                             img11_list, img12_list,
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
                    calculateOFCEHierarchicallyWarping2Bands(image1->band(m_param_image1Band1->value()),
                                                             image1->band(m_param_image1Band2->value()),
                                                             image2->band(m_param_image2Band1->value()),
                                                             image2->band(m_param_image2Band2->value()),
                                                             img11_list, img12_list,
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
                       new_vectorfield =  new DenseVectorfield2D(flow_list[i].bindElementChannel(0),flow_list[i].bindElementChannel(1), m_environment);
                    }
                    //Has to be larger
                    else
                    {
                        new_vectorfield =  new DenseWeightedVectorfield2D(flow_list[i].bindElementChannel(0),flow_list[i].bindElementChannel(1),flow_list[i].bindElementChannel(2), m_environment);
                    }
                    
                    QString functor_name =  OpticalFlowFunctor::name();
                    QString functor_sname = OpticalFlowFunctor::shortName();
                    
                    if( i != 0)
                    {
                        new_vectorfield->setName(QString("%1 (L%2) of %3 and %4").arg(functor_sname).arg(i).arg(image1->name()).arg(image2->name()));
                    }
                    else
                    {
                        new_vectorfield->setName(QString("%1 of %2 and %3").arg(functor_sname).arg(image1->name()).arg(image2->name()));
                    }
                    new_vectorfield->setGlobalMotion(QTransform(mat_list[i](0,0), mat_list[i](1,0), mat_list[i](2,0),
                                                                mat_list[i](0,1), mat_list[i](1,1), mat_list[i](2,1),
                                                                mat_list[i](0,2), mat_list[i](1,2), mat_list[i](2,2)));
                    
                    qDebug() << "Assigning GME for VF:" << new_vectorfield->globalMotion();
                    qDebug() << "Inverted GME for VF:" << new_vectorfield->globalMotion().inverted();
                    
                    //Get time diff
                    unsigned int seconds = (unsigned int)image1->timestamp().secsTo(image2->timestamp());
                    
                    if(seconds != 0)
                    {
                        new_vectorfield->setScale(image1->scale()*100.0/seconds * (image1->width()/flow_list[0].width()));
                    }
                    
                    QString descr = QString("The following parameters were used to calculate the %1\n").arg(functor_name);
                    
                    if( i != 0)
                    {
                        descr += QString("Level %1 of %2\n").arg(i).arg(flow_list.size());
                    }
                    
                    descr += m_parameters->valueText("ModelParameter");
                    new_vectorfield->setDescription(descr);
                    m_results.push_back(new_vectorfield);
                }
                //Also save warped images on demand
                if(m_param_pmode->value() !=0 && i!=0 && m_param_saveIntermediateImages->value()) 
                {
                    Image<float>* new_image = new Image<float>(img11_list[i].shape(), 2, m_environment);
                    new_image->setBand(0, img11_list[i]);
                    new_image->setBand(1, img12_list[i]);
                    
                    image1->copyMetadata(*new_image);
                    
                    new_image->setName(QString("Warped Image (L%1) of %2").arg(i).arg(image1->name()));
                    new_image->setDescription(QString(  "The following parameters were used to calculate the warping:\n"
                                                        "TPS Functor\n"
                                                        "Subsampled each %1 pixel").arg(5*m_param_pmode->value()));
                    m_results.push_back(new_image);
                }
            }
        }

    protected:
        //Additional parameters
        ModelParameter * m_param_image1;
        IntParameter *   m_param_image1Band1;
        IntParameter *   m_param_image1Band2;
        ModelParameter * m_param_image2;
        IntParameter *   m_param_image2Band1;
        IntParameter *   m_param_image2Band2;

        BoolParameter *  m_param_useMask;
        ImageBandParameter<float> * m_param_mask;

        BoolParameter	*  m_param_useGME;
        BoolParameter	*  m_param_useHierarchy;

        IntParameter *   m_param_lowestLevel;
        IntParameter *   m_param_highestLevel;

        EnumParameter * m_param_hmode;
        EnumParameter * m_param_pmode;

        FloatParameter *   m_param_warp_sigma;

        BoolParameter	* m_param_saveIntermediateImages;
        BoolParameter	* m_param_saveIntermediateFlow;
};
  
} //end of namespace graipe

#endif //GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOWALGORITHMS_HXX
