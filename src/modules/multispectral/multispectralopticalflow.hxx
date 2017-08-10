/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2017 by Benjamin Seppke                 */
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

#ifndef GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOW_HXX
#define GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOW_HXX

#include "vigra/linear_algebra.hxx"

#include "opticalflow/opticalflowgradients.hxx"

namespace graipe {

/**
 * The classical (unweighted) Lucas & Kanade algorithm as described by them in 1982,
 * pimped by better gradient computations using vigra's gaussian convolution kernels
 * and extended to two consecutive image bands for each image.
 */
class OpticalFlow2BandsFunctor
{
    public:
        //Typedefs for the resulting flow field
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the classical Lucas and Kanade approach extended
         * to two bands.
         *
         * \param sigma The sigma is currently ignored.
         * \param threshold The gradient threshold.
         * \param iterations The count of iterations.
         */
        OpticalFlow2BandsFunctor(double sigma, double threshold, int iterations)
        :	m_iterations(iterations),
            m_sigma(sigma),
            m_threshold(threshold),
            m_level(0.0)
        {
        }

        /**
         * When applied on a pyramidal processing model, this function is called on 
         * every layer/octave change.
         *
         * \param level The level of the current octave.
         */
        void setLevel(int level)
        {
            m_level=level;
            //we assume the same m_sigma for all levels, thus nothing is done here!
        }
		
		/**
         * Returns the full name of the functor.
         *
         * \return Always "2Bands multispectral OFCE".
         */
        static QString name()
        {
            return "2Bands multispectral OFCE";
        }
		
		/**
         * Returns the short name of the functor.
         *
         * \return Always "2Bands MS OFCE".
         */
        static QString shortName()
        {
            return "2Bands MS OFCE";
        }
        
		/**
         * The optical flow calculation according to Lucas & Kanade 1982
		 * computation of optical flow extended to 2 bands for each image.
         *
         * \param[in] src11 First band of the first image of the series.
         * \param[in] src12 Second band of the first image of the series.
         * \param[in] src21 First band of the secong image of the series.
         * \param[in] src22 Second band of the second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <class T1, class T2>
        void operator()(const vigra::MultiArrayView<2, T1> & src11,
                        const vigra::MultiArrayView<2, T1> & src12,
                        const vigra::MultiArrayView<2, T2> & src21,
                        const vigra::MultiArrayView<2, T2> & src22,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src11.shape() == src12.shape() ,"image sizes differ!");
            vigra_precondition(src12.shape() == src21.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == src22.shape() ,"image sizes differ!");
            vigra_precondition(src11.shape() == flow.shape() ,"image and flow array sizes differ!");
            
            vigra::Shape2 shape = src11.shape();
            
            vigra::MultiArray<2,ValueType>	gradX1c1(shape), gradX1c2(shape), gradY1c1(shape), gradY1c2(shape), gradT1c1(shape), gradT1c2(shape),
                                            gradX2c1(shape), gradX2c2(shape), gradY2c1(shape), gradY2c2(shape), gradT2c1(shape), gradT2c2(shape);
            
            vigra::gaussianGradient(src11,	gradX1c1, gradY1c1, m_sigma);
            vigra::gaussianGradient(src12,	gradX1c2, gradY1c2, m_sigma);
            
            vigra::gaussianGradient(src21,	gradX2c1, gradY2c1, m_sigma);
            vigra::gaussianGradient(src22,	gradX2c2, gradY2c2, m_sigma);
            
            vigra::gaussianSmoothing(src11,	gradT1c1, m_sigma);
            vigra::gaussianSmoothing(src12,	gradT1c2, m_sigma);
            
            vigra::gaussianSmoothing(src21,	gradT2c1, m_sigma);
            vigra::gaussianSmoothing(src21,	gradT2c2, m_sigma);
            
            //Matrix A, vecor b and eigenvectors resp. eigenvalues
            vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2);
            vigra::Matrix<std::complex<double> >  ew(2,1);
            double last_u, last_v;
            
            for(int it=1; it<=m_iterations; ++it)
            {
                for (int j=0; j<shape[1]; ++j)
                {
                    for (int i=0; i<shape[0]; ++i)
                    {
                        last_u = flow(i,j)[0];
                        last_v = flow(i,j)[1];
                        
                        if(		i+last_u >= shape[0] 
                           ||	i+last_u < 0
                           ||	j+last_v >= shape[1] 
                           ||	j+last_v < 0 ) 
                            continue;
                        
                        //reset Matrix and result vector 	
                        A(0,0)= (gradX1c1(i,j) + gradX2c1(i+last_u,j+last_v))/2.0;
                        A(1,0)= (gradX1c2(i,j) + gradX2c2(i+last_u,j+last_v))/2.0;
                        A(0,1)= (gradY1c1(i,j) + gradY2c1(i+last_u,j+last_v))/2.0;
                        A(1,1)= (gradY1c2(i,j) + gradY2c2(i+last_u,j+last_v))/2.0;
                        
                        b(0,0) = (gradT2c1(i+last_u,j+last_v)-gradT1c1(i,j));
                        b(1,0) = (gradT2c2(i+last_u,j+last_v)-gradT1c2(i,j));
                        
                        b = -b;
                        
                        //solve the linear system of equations
                        if(vigra::linearSolve( 	A, b, res))
                        {	
                            float a_det = vigra::abs(A(0,0)*A(1,1)-A(1,0)*A(0,1));
                            
                            //threshold vectors using the absolute determinant of A
                            vigra::nonsymmetricEigensystem(A,ew,ev);
                            if(a_det >= m_threshold )
                            {
                                flow(i,j)[0] = last_u+res(0,0);
                                flow(i,j)[1] = last_v+res(1,0);
                                flow(i,j)[2] = a_det;
                                
                            }
                            else
                            {
                                flow(i,j)[0] = last_u;
                                flow(i,j)[1] = last_v;
                            }
                        }
                        else 
                        {
                            flow(i,j)[0] = last_u;
                            flow(i,j)[1] = last_v;
                        }
                    }
                }
            }
        }
        
		/**
         * The masked optical flow calculation according to Lucas & Kanade 1982
		 * computation of optical flow extended to 2 bands for each image.
         *
         * \param[in] src11 First band of the first image of the series.
         * \param[in] src12 Second band of the first image of the series.
         * \param[in] src21 First band of the secong image of the series.
         * \param[in] src22 Second band of the second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <	class T1, class T2, class T3>
        void operator()(const vigra::MultiArrayView<2, T1> & src11,
                        const vigra::MultiArrayView<2, T1> & src12,
                        const vigra::MultiArrayView<2, T2> & src21,
                        const vigra::MultiArrayView<2, T2> & src22,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src11.shape() == src12.shape() ,"image sizes differ!");
            vigra_precondition(src12.shape() == src21.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == src22.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == mask.shape() ,"image and mask sizes differ!");
            vigra_precondition(src11.shape() == flow.shape() ,"image and flow array sizes differ!");
            
            vigra::Shape2 shape = src11.shape();
            
            vigra::MultiArray<2,ValueType>	gradX1c1(shape), gradX1c2(shape), gradY1c1(shape), gradY1c2(shape), gradT1c1(shape), gradT1c2(shape),
                                            gradX2c1(shape), gradX2c2(shape), gradY2c1(shape), gradY2c2(shape), gradT2c1(shape), gradT2c2(shape);
            
            
            gaussianGradientWithMask(src11,	mask,   gradX1c1, gradY1c1, m_sigma);
            gaussianGradientWithMask(src12, mask,	gradX1c2, gradY1c2, m_sigma);
            
            gaussianGradientWithMask(src21, mask,	gradX2c1, gradY2c1, m_sigma);
            gaussianGradientWithMask(src22, mask,	gradX2c2, gradY2c2, m_sigma);
            
            gaussianSmoothingWithMask(src11, mask,	gradT1c1, m_sigma);
            gaussianSmoothingWithMask(src12, mask,	gradT1c2, m_sigma);
            
            gaussianSmoothingWithMask(src21, mask,	gradT2c1, m_sigma);
            gaussianSmoothingWithMask(src21, mask,	gradT2c2, m_sigma);
            
            //Matrix A, vecor b and eigenvectors resp. eigenvalues
            vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2);
            vigra::Matrix<std::complex<double> >  ew(2,1);
            double last_u, last_v;
            
            for(int it=1; it<=m_iterations; ++it)
            {
                for (int j=0; j<shape[1]; ++j)
                {
                    for (int i=0; i<shape[0]; ++i)
                    {
                        last_u = flow(i,j)[0];
                        last_v = flow(i,j)[1];
                        
                        if(		mask(i,j) ==0
                           ||   i+last_u >= shape[0]
                           ||	i+last_u < 0
                           ||	j+last_v >= shape[1] 
                           ||	j+last_v < 0 ) 
                            continue;
                        
                        //reset Matrix and result vector 	
                        A(0,0)= (gradX1c1(i,j) + gradX2c1(i+last_u,j+last_v))/2.0;
                        A(1,0)= (gradX1c2(i,j) + gradX2c2(i+last_u,j+last_v))/2.0;
                        A(0,1)= (gradY1c1(i,j) + gradY2c1(i+last_u,j+last_v))/2.0;
                        A(1,1)= (gradY1c2(i,j) + gradY2c2(i+last_u,j+last_v))/2.0;
                        
                        b(0,0) = (gradT2c1(i+last_u,j+last_v)-gradT1c1(i,j));
                        b(1,0) = (gradT2c2(i+last_u,j+last_v)-gradT1c2(i,j));
                        
                        b = -b;
                        
                        //solve the linear system of equations
                        if(vigra::linearSolve( 	A, b, res))
                        {	
                            float a_det = vigra::abs(A(0,0)*A(1,1)-A(1,0)*A(0,1));
                            
                            //threshold vectors using the absolute determinant of A
                            vigra::nonsymmetricEigensystem(A,ew,ev);
                            if(a_det >= m_threshold )
                            {
                                flow(i,j)[0] = last_u+res(0,0);
                                flow(i,j)[1] = last_v+res(1,0);
                                flow(i,j)[2] = a_det;
                                
                            }
                            else
                            {
                                flow(i,j)[0] = last_u;
                                flow(i,j)[1] = last_v;
                            }
                        }
                        else 
                        {
                            flow(i,j)[0] = last_u;
                            flow(i,j)[1] = last_v;
                        }
                    }
                }
            }
        }

    private:
        int		m_iterations;
        double	m_sigma;
        double	m_threshold;
        int		m_level;
};

/**
 * The classical Horn & Schuck algorithm as described by them in 1981,
 * pimped by better gradient computations using vigra's gaussian convolution kernels
 * and extended to two consecutive image bands for each image.
 */
class OpticalFlowHS2BandsFunctor
{
    public:
        //Typedefs for the resulting flow field
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the classical Horn and Schunck approach extended
         * to two bands.
         *
         * \param sigma The sigma is currently ignored.
         * \param threshold The gradient threshold.
         * \param iterations The count of iterations.
         */
        OpticalFlowHS2BandsFunctor(double sigma, double alpha, int iterations)
        :	m_iterations(iterations),
            m_sigma(sigma),
            m_alpha(alpha),
            m_level(0.0)
        {
        }

        /**
         * When applied on a pyramidal processing model, this function is called on 
         * every layer/octave change.
         *
         * \param level The level of the current octave.
         */
        void setLevel(int level)
        {
            m_level=level;
            //we assume the same m_sigma for all levels, thus nothing is done here!
        }
		
		/**
         * Returns the full name of the functor.
         *
         * \return Always "HS multispectral (2bands) OFCE".
         */
        static QString name()
        {
            return "HS multispectral (2bands) OFCE";
        }
		
		/**
         * Returns the short name of the functor.
         *
         * \return Always "HS 2bands OFCE".
         */
        static QString shortName()
        {
            return "HS 2bands OFCE";
        }
    
        /**
         * The optical flow calculation according to Horn & Schuck 1981
		 * computation of optical flow extended to 2 bands for each image.
         *
         * \param[in] src11 First band of the first image of the series.
         * \param[in] src12 Second band of the first image of the series.
         * \param[in] src21 First band of the secong image of the series.
         * \param[in] src22 Second band of the second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <	class T1, class T2>
        void operator()(const vigra::MultiArrayView<2, T1> & src11,
                        const vigra::MultiArrayView<2, T1> & src12,
                        const vigra::MultiArrayView<2, T2> & src21,
                        const vigra::MultiArrayView<2, T2> & src22,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src11.shape() == src12.shape() ,"image sizes differ!");
            vigra_precondition(src12.shape() == src21.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == src22.shape() ,"image sizes differ!");
            vigra_precondition(src11.shape() == flow.shape() ,"image and flow array sizes differ!");
            
            vigra::Shape2 shape = src11.shape();
            
            vigra::MultiArray<2,ValueType>	gradXc1(shape), gradXc2(shape), gradYc1(shape), gradYc2(shape), gradTc1(shape), gradTc2(shape);
        
            //to hold the new mean result vectors of the last result
            vigra::MultiArray<2,FlowValueType> mean_flow(shape);
            
            //spatiotemporal Gradients of first order: I_1x, I_1y and I_1t
            spatioTemporalGradient(src11,
                                   src21,
                                   gradXc1,
                                   gradYc1,
                                   gradTc1,
                                   m_sigma);
            
            //spatiotemporal Gradients of first order: I_2x, I_2y and I_2t
            spatioTemporalGradient(src12,
                                   src22,
                                   gradXc2,
                                   gradYc2,
                                   gradTc2,
                                   m_sigma);
            
            double iter_change=0, mean_change=0, max_change=0;
            
            double alpha2 = m_alpha*m_alpha;
            
            for (int iteration=1;iteration<=m_iterations; ++iteration)
            {
                mean_change=0;
                max_change=0;
                
                //smooth last vector-images
                vigra::gaussianSmoothing(flow, mean_flow,	 m_sigma);
                
                
                for (int j=0; j<shape[1]; ++j)
                {
                    for (int i=0; i<shape[0]; ++i)
                    {
                        vigra::Matrix<double> A(2,2), b(2,1);
                        
                        A(0,0) =(alpha2+ gradXc1(i,j)*gradXc1(i,j)) + (alpha2+ gradXc2(i,j)*gradXc2(i,j));
                        A(1,0) = A(0,1) = (gradXc1(i,j)*gradYc1(i,j)) + (gradXc2(i,j)*gradYc2(i,j));
                        A(1,1) = (alpha2+ gradYc1(i,j)*gradYc1(i,j)) + (alpha2+ gradYc2(i,j)*gradYc2(i,j));
                        
                        b(0,0) = (gradXc1(i,j)*gradTc1(i,j)) + (gradXc2(i,j)*gradTc2(i,j));
                        b(1,0) = (gradYc1(i,j)*gradTc1(i,j)) + (gradYc2(i,j)*gradTc2(i,j));
                        
                        double	detA	= determinant(A),
                        new_u	=  (alpha2*2.0*A(0,0)*mean_flow(i,j)[0] - A(0,1)*mean_flow(i,j)[1] - b(0,0))/detA,
                        new_v	=  (alpha2*2.0*A(1,1)*mean_flow(i,j)[1] - A(1,0)*mean_flow(i,j)[0] - b(1,0))/detA;
                        
                        iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
                                           +	pow(flow(i,j)[1] - new_v,2));
                        
                        mean_change	+= iter_change;
                        max_change = std::max(max_change, iter_change);
                        
                        flow(i,j)[0] = new_u;
                        flow(i,j)[1] = new_v;
                        flow(i,j)[2] = detA;
                        
                    }
                }
                mean_change = mean_change / (shape[0]*shape[1]);
                //qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
                //qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
                //if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
            }
        }
    
        /**
         * The masked optical flow calculation according to Horn & Schuck 1981
		 * computation of optical flow extended to 2 bands for each image.
         *
         * \param[in] src11 First band of the first image of the series.
         * \param[in] src12 Second band of the first image of the series.
         * \param[in] src21 First band of the secong image of the series.
         * \param[in] src22 Second band of the second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <	class T1, class T2, class T3>
        void operator()(const vigra::MultiArrayView<2, T1> & src11,
                        const vigra::MultiArrayView<2, T1> & src12,
                        const vigra::MultiArrayView<2, T2> & src21,
                        const vigra::MultiArrayView<2, T2> & src22,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src11.shape() == src12.shape() ,"image sizes differ!");
            vigra_precondition(src12.shape() == src21.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == src22.shape() ,"image sizes differ!");
            vigra_precondition(src21.shape() == mask.shape() ,"image and mask sizes differ!");
            vigra_precondition(src11.shape() == flow.shape() ,"image and flow array sizes differ!");
            
            vigra::Shape2 shape = src11.shape();
            
            vigra::MultiArray<2,ValueType>	gradXc1(shape), gradXc2(shape), gradYc1(shape), gradYc2(shape), gradTc1(shape), gradTc2(shape);
            
            //to hold the new mean result vectors of the last result
            vigra::MultiArray<2,FlowValueType> mean_flow(shape);
            
            //spatiotemporal Gradients of first order: I_1x, I_1y and I_1t
            spatioTemporalGradientWithMask(src11,
                                           src21,
                                           mask,
                                           gradXc1,
                                           gradYc1,
                                           gradTc1,
                                           m_sigma);
            
            //spatiotemporal Gradients of first order: I_2x, I_2y and I_2t
            spatioTemporalGradientWithMask(src12,
                                           src22,
                                           mask,
                                           gradXc2,
                                           gradYc2,
                                           gradTc2,
                                           m_sigma);
            
            double iter_change=0, mean_change=0, max_change=0;
            
            double alpha2 = m_alpha*m_alpha;
            
            for (int iteration=1;iteration<=m_iterations; ++iteration)
            {
                mean_change=0;
                max_change=0;
                
                //smooth last vector-images
                gaussianSmoothingWithMask(flow.bindElementChannel(0), mask, mean_flow.bindElementChannel(0), m_sigma);
                gaussianSmoothingWithMask(flow.bindElementChannel(1), mask, mean_flow.bindElementChannel(2), m_sigma);
                
                for (int j=0; j<shape[1]; ++j)
                {
                    for (int i=0; i<shape[0]; ++i)
                    {
                        
                        if(	mask(i,j)==0 )
                            continue;
                            
                        vigra::Matrix<double> A(2,2), b(2,1);
                        
                        A(0,0) =(alpha2+ gradXc1(i,j)*gradXc1(i,j)) + (alpha2+ gradXc2(i,j)*gradXc2(i,j));
                        A(1,0) = A(0,1) = (gradXc1(i,j)*gradYc1(i,j)) + (gradXc2(i,j)*gradYc2(i,j));
                        A(1,1) = (alpha2+ gradYc1(i,j)*gradYc1(i,j)) + (alpha2+ gradYc2(i,j)*gradYc2(i,j));
                        
                        b(0,0) = (gradXc1(i,j)*gradTc1(i,j)) + (gradXc2(i,j)*gradTc2(i,j));
                        b(1,0) = (gradYc1(i,j)*gradTc1(i,j)) + (gradYc2(i,j)*gradTc2(i,j));
                        
                        double	detA	= determinant(A),
                        new_u	=  (alpha2*2.0*A(0,0)*mean_flow(i,j)[0] - A(0,1)*mean_flow(i,j)[1] - b(0,0))/detA,
                        new_v	=  (alpha2*2.0*A(1,1)*mean_flow(i,j)[1] - A(1,0)*mean_flow(i,j)[0] - b(1,0))/detA;
                        
                        iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
                                           +	pow(flow(i,j)[1] - new_v,2));
                        
                        mean_change	+= iter_change;
                        max_change = std::max(max_change, iter_change);
                        
                        flow(i,j)[0] = new_u;
                        flow(i,j)[1] = new_v;
                        flow(i,j)[2] = detA;
                        
                    }
                }
                mean_change = mean_change / (shape[0]*shape[1]);
                //qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
                //qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
                //if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
            }
        }

    private:
        int		m_iterations;
        double	m_sigma;
        double	m_alpha;
        int		m_level;
};
    
} //end of namespace graipe

#endif //GRAIPE_MULTISPECTRAL_MULTISPECTRALOPTICALFLOW_HXX
