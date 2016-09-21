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

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOW_LOCAL_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOW_LOCAL_HXX

//debug output
#include <QtDebug>

//OFCE Spatiotemporal Gradients
#include "opticalflowgradients.hxx"

//Image interpolation using splines
#include <vigra/splineimageview.hxx>


namespace graipe {

/**
 * Optical Flow computation according to the Lucas & Kanade approach
 * These algorithms try to solve the aperture problem by postulating
 * the same flow inside a local neighborhood of the image which leads
 * to a least suare fitting using sums:
 *
 *  SUM [ nabla(I_x)^2           nabla(I_x)*nabla(I_y)  ] * [ u ]   = SUM [nabla(I_x)] * nabla(I_t)
 *      [ nabla(I_x)*nabla(I_y)  nabla(I_y)^2           ]   [ v ]         [nabla(I_y)] 
 * 
 * This has some kind of similarity with the well-known structure tensor!
 *
 * 1. The classical (unweighted) Lucas & Kanade algorithm as described by them in 1982
 *
 * 2. A new structure tensor approach which implement weighted sums not using the structure
 *    tensor facilities of the vigra, but emulating them by explicit computations to enable
 *    an iterative scheme.
 */

/**
 * The classical (unweighted) Lucas & Kanade algorithm as described by them in 1982,
 * pimped by better gradient computations using vigra's gaussian convolution kernels
 */
class OpticalFlowLKFunctor
{
	public:
        //Typedefs for the resulting flow field
       typedef float ValueType;
       typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the "Gaussian-pimped" Lucas and Kanade Optical Flow approach.
         *
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time. Defaults to 1.0
         * \param mask_size The size of the equvalent motion neightborhod in rows&cols. Defaults to 11.
         * \param threshold The minimal gradient magnitude to count a value inside the neigborhoof. Defaults to 0.0.
         * \param iterations The count of iterations. Defaults to 100.
         */
		OpticalFlowLKFunctor(double sigma=1.0, unsigned int  mask_size=11, double threshold=0.0, unsigned int iterations =100)
		:	m_sigma(sigma),
			m_mask_size(mask_size),
			m_threshold(threshold),
			m_iterations(iterations),
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
         * \return Always "Lucas Kanade OFCE".
         */
		static std::string name()
		{
			return "Lucas Kanade OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "LK OFCE".
         */
		static std::string shortName()
		{
			return "LK OFCE";
		}
		
		/**
         * The optical flow calculation according to Lucas & Kanade 1982
		 * computation of optical flow.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <	class T1, class T2>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
						const vigra::MultiArrayView<2, T2> & src2,
						vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2, ValueType>	gradX1(src1.shape()), gradY1(src1.shape()),
                                            gradX2(src1.shape()), gradY2(src1.shape()),
                                            gradT1(src1.shape()), gradT2(src1.shape());
			
			
			vigra::gaussianGradient(src1, gradX1, gradY1, m_sigma);
			vigra::gaussianGradient(src2, gradX2, gradY2, m_sigma);
			
			vigra::gaussianSmoothing(src1, gradT1, m_sigma);
			vigra::gaussianSmoothing(src2, gradT2, m_sigma);
			
			vigra::SplineImageView<1,ValueType> gradX2_s(gradX2), gradY2_s(gradY2), gradT2_s(gradT2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v, gx, gy, gt, sum_w;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				for(unsigned int j=m_mask_size/2; j<src1.height()-m_mask_size/2; ++j)
				{
					for(unsigned int i=m_mask_size/2; i<src1.width()-m_mask_size/2; ++i)
					{
						last_u = flow(i,j)[0];
						last_v = flow(i,j)[1];
						
						if(		i+last_u+m_mask_size/2 >= src1.width()
						   ||	i+last_u-m_mask_size/2 < 0
						   ||	j+last_v+m_mask_size/2 >= src1.height()
						   ||	j+last_v-m_mask_size/2 < 0 ) 
							continue;
						
						//reset Matrix and result vector 	
						A(0,0)=0; A(0,1) = 0;
						A(1,0)=0; A(1,1) = 0;
						
						b(0,0) = 0; 
						b(1,0) = 0;
						
						/** Create Sums of gradients to calculate optical flow:
						 *
						 *  SUM [ nabla(I_x)^2           nabla(I_x)*nabla(I_y)  ] * [ u ]   = SUM [nabla(I_x)] * nabla(I_t)
						 *      [ nabla(I_x)*nabla(I_y)  nabla(I_y)^2           ]   [ v ]         [nabla(I_y)] 
						 */
						for(unsigned int mj = j-m_mask_size/2; mj<j+m_mask_size/2;	++mj)
						{
							for(unsigned int mi = i-m_mask_size/2; mi<i+m_mask_size/2;	++mi)
							{
								gx = (gradX1(mi,mj) + gradX2_s(mi+last_u,mj+last_v))/2.0;
								gy = (gradY1(mi,mj) + gradY2_s(mi+last_u,mj+last_v))/2.0;
								gt = (gradT2_s(mi+last_u,mj+last_v)-gradT1(mi,mj));
								
								A(0,0) += gx*gx; // (nabla I_x)^2
								A(0,1) += gx*gy; // (nabla I_x)*(nabla I_y)
								A(1,1) += gy*gy;  // (nabla I_y)^2
								
								b(0,0) += gx*gt;
								b(1,0) += gy*gt;
							}
						}
						
						sum_w = (m_mask_size*m_mask_size);
						
						//Normalize acc. t sum_w
						A(0,0)/= sum_w;		A(0,1)/= sum_w;
						A(0,1)/= sum_w;		A(1,1)/= sum_w;
						b(0,0)/= sum_w;
						b(1,0)/= sum_w;
						
						A(1,0) = A(0,1);
						b	   = -b;
						
						//solve the linear system of equations
						if(vigra::linearSolve( 	A, b, res))
						{
							//threshold vectors using the smallest of both eigenvalue scaled to one pixel 
							vigra::symmetricEigensystem(A,ew,ev);
							if(ew(1,0)>= m_threshold)
							{
								flow(i,j)[0] = last_u+res(0,0);
								flow(i,j)[1] = last_v+res(1,0);
								flow(i,j)[2] = ew(1,0);
							}
						}
					}
				}
			}
		}
				        
        /**
         * Masked Optical flow computation for Lucas & Kanade 1982 method.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
						const vigra::MultiArrayView<2, T2> & src2,
						const vigra::MultiArrayView<2, T3> & mask,
						vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2, ValueType>	gradX1(src1.shape()), gradY1(src1.shape()),
                                            gradX2(src1.shape()), gradY2(src1.shape()),
                                            gradT1(src1.shape()), gradT2(src1.shape());
			
			
			gaussianGradientWithMask(src1, mask, gradX1, gradY1, m_sigma);
			gaussianGradientWithMask(src2, mask, gradX2, gradY2, m_sigma);
			
			gaussianSmoothingWithMask(src1, mask, gradT1, m_sigma);
			gaussianSmoothingWithMask(src2, mask, gradT2, m_sigma);
			
			vigra::SplineImageView<1,ValueType> gradX2_s(gradX2), gradY2_s(gradY2), gradT2_s(gradT2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v, gx, gy, gt, sum_we;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				for(unsigned int j=m_mask_size/2; j<src1.height()-m_mask_size/2; ++j)
				{
					for(unsigned int i=m_mask_size/2; i<src1.width()-m_mask_size/2; ++i)
					{
						if(mask(i,j) != 0)
                        {
                            last_u = flow(i,j)[0];
                            last_v = flow(i,j)[1];
                            
                            if(		i+last_u+m_mask_size/2 >= src1.width()
                               ||	i+last_u-m_mask_size/2 < 0
                               ||	j+last_v+m_mask_size/2 >= src1.height()
                               ||	j+last_v-m_mask_size/2 < 0 ) 
                                continue;
                            
                            //reset Matrix and result vector 	
                            A(0,0)=0; A(0,1) = 0;
                            A(1,0)=0; A(1,1) = 0;
                            
                            b(0,0) = 0; 
                            b(1,0) = 0;
                            
                            /** Create Sums of gradients to calculate optical flow:
                             *
                             *  SUM [ nabla(I_x)^2           nabla(I_x)*nabla(I_y)  ] * [ u ]   = SUM [nabla(I_x)] * nabla(I_t)
                             *      [ nabla(I_x)*nabla(I_y)  nabla(I_y)^2           ]   [ v ]         [nabla(I_y)] 
                             */
                            for(unsigned int mj = j-m_mask_size/2; mj<j+m_mask_size/2;	++mj)
                            {
                                for(unsigned int mi = i-m_mask_size/2; mi<i+m_mask_size/2;	++mi)
                                {
                                    gx = (gradX1(mi,mj) + gradX2_s(mi+last_u,mj+last_v))/2.0;
                                    gy = (gradY1(mi,mj) + gradY2_s(mi+last_u,mj+last_v))/2.0;
                                    gt = (gradT2_s(mi+last_u,mj+last_v)-gradT1(mi,mj));
                                    
                                    A(0,0) += gx*gx; // (nabla I_x)^2
                                    A(0,1) += gx*gy; // (nabla I_x)*(nabla I_y)
                                    A(1,1) += gy*gy;  // (nabla I_y)^2
                                    
                                    b(0,0) += gx*gt;
                                    b(1,0) += gy*gt;
                                }
                            }
                            
                            sum_we = (m_mask_size*m_mask_size);
                            
                            //Normalize acc. t sum_w
                            A(0,0)/= sum_we;		A(0,1)/= sum_we;
                            A(0,1)/= sum_we;		A(1,1)/= sum_we;
                            b(0,0)/= sum_we;
                            b(1,0)/= sum_we;
                            
                            A(1,0) = A(0,1);
                            b	   = -b;
                            
                            //solve the linear system of equations
                            if(vigra::linearSolve( 	A, b, res))
                            {
                                //threshold vectors using the smallest of both eigenvalue scaled to one pixel 
                                vigra::symmetricEigensystem(A,ew,ev);
                                if(ew(1,0)>= m_threshold)
                                {
                                    flow(i,j)[0] = last_u+res(0,0);
                                    flow(i,j)[1] = last_v+res(1,0);
                                    flow(i,j)[2] = ew(1,0);
                                }
                            }
                        }
                    }
				}
			}
		}
	
	private:
		double			m_sigma;
		unsigned int	m_mask_size;
		double			m_threshold;
		unsigned int	m_iterations;
		
		unsigned int	m_level;
};


/**
 * 2. A new structure tensor approach which implement weighted sums not using the structure
 *    tensor facilities of the vigra, but emulating them by explicit computations to enable
 *    an iterative scheme.
 */
class OpticalFlowSTFunctor
{
	public:		
        //Typedefs for the resulting flow field
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the Structure Tensor Optical Flow approach.
         *
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time. Defaults to 1.0
         * \param outer_sigma The sigma of the Gaussian used to smooth the partial derivatives
         *              in space and time. Defaults to 3.0
         * \param mask_size The size of the equvalent motion neightborhod in rows&cols. Defaults to 11.
         * \param threshold The minimal gradient magnitude to count a value inside the neigborhoof. Defaults to 0.0.
         * \param iterations The count of iterations. Defaults to 100.
         */
		OpticalFlowSTFunctor(double sigma=1.0, float outer_sigma=3.0, unsigned int mask_size=11, double threshold=0.0, unsigned int  iterations =100)
		:	m_sigma(sigma),
			m_outer_sigma(outer_sigma),
			m_mask_size(mask_size),
			m_threshold(threshold),
			m_iterations(iterations),
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
         * \return Always "Structure Tensor OFCE".
         */
		static std::string name()
		{
			return "Structure Tensor OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "ST OFCE".
         */
		static std::string shortName()
		{
			return "ST OFCE";
		}
		
		/**
         * The optical flow calculation according to Structure Tensor approach.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2, ValueType>	gradX1(src1.shape()), gradY1(src1.shape()),
                                            gradX2(src1.shape()), gradY2(src1.shape()),
                                            gradT1(src1.shape()), gradT2(src1.shape());
			
			
			vigra::gaussianGradient(src1, gradX1, gradY1, m_sigma);
			vigra::gaussianGradient(src2, gradX2, gradY2, m_sigma);
			
			vigra::gaussianSmoothing(src1, gradT1, m_sigma);
			vigra::gaussianSmoothing(src2, gradT2, m_sigma);
			
			vigra::SplineImageView<1,ValueType> gradX2_s(gradX2), gradY2_s(gradY2), gradT2_s(gradT2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Gaussian<double> gauss(m_outer_sigma);
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v, gx, gy, gt, we, sum_we;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				
				for(unsigned int j=m_mask_size/2; j<src1.height()-m_mask_size/2; ++j)
				{
					for(unsigned int i=m_mask_size/2; i<src1.width()-m_mask_size/2; ++i)
					{
						last_u = flow(i,j)[0];
						last_v = flow(i,j)[1];
						
						if(		i+last_u+m_mask_size/2 >= src1.width()
						   ||	i+last_u-m_mask_size/2 < 0
						   ||	j+last_v+m_mask_size/2 >= src1.height()
						   ||	j+last_v-m_mask_size/2 < 0 ) 
							continue;
						
						//reset Matrix and result vector 	
						A(0,0)=0; A(0,1) = 0;
						A(1,0)=0; A(1,1) = 0;
						
						b(0,0) = 0; 
						b(1,0) = 0;
						
						sum_we = 0;
						
						/** Create Sums of gradients to calculate optical flow:
						 *
						 *  SUM [ nabla(I_x)^2           nabla(I_x)*nabla(I_y)  ] * [ u ]   = SUM [nabla(I_x)] * nabla(I_t)
						 *      [ nabla(I_x)*nabla(I_y)  nabla(I_y)^2           ]   [ v ]         [nabla(I_y)] 
						 */
						for(unsigned int mj = j-m_mask_size/2; mj<j+m_mask_size/2;	++mj)
						{
							for(unsigned int mi = i-m_mask_size/2; mi<i+m_mask_size/2;	++mi)
							{
								gx = (gradX1(mi,mj) + gradX2_s(mi+last_u,mj+last_v))/2.0;
								gy = (gradY1(mi,mj) + gradY2_s(mi+last_u,mj+last_v))/2.0;
								gt = (gradT2_s(mi+last_u,mj+last_v)-gradT1(mi,mj));
								
								we =  gauss(sqrt((mi-i)*(mi-i)+(mj-j)*(mj-j)));
								sum_we += we;
								
								A(0,0) += gx*gx * we; // (nabla I_x)^2
								A(0,1) += gx*gy * we; // (nabla I_x)*(nabla I_y)
								A(1,1) += gy*gy * we;  // (nabla I_y)^2
								
								b(0,0) += gx*gt * we;
								b(1,0) += gy*gt * we;
								
							}
						}
						//Normalize acc. t sum_we
						A(0,0)/= sum_we;		A(0,1)/= sum_we;
						A(0,1)/= sum_we;		A(1,1)/= sum_we;
						b(0,0)/= sum_we;
						b(1,0)/= sum_we;
						
						A(1,0) = A(0,1);
						b	   = -b;
						
						//solve the linear system of equations
						if(vigra::linearSolve( 	A, b, res))
						{
							//threshold vectors using the smallest of both eigenvalue scaled to one pixel 
							vigra::symmetricEigensystem(A,ew,ev);
							if(ew(1,0)>= m_threshold)
							{
								flow(i,j)[0] = last_u+res(0,0);
								flow(i,j)[1] = last_v+res(1,0);
								flow(i,j)[2] = ew(1,0);
							}
						}
					}
				}
			}
		}
		        
        /**
         * Masked Optical flow computation for Structure Tensor method.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2, ValueType>	gradX1(src1.shape()), gradY1(src1.shape()),
                                            gradX2(src1.shape()), gradY2(src1.shape()),
                                            gradT1(src1.shape()), gradT2(src1.shape());
			
			
			gaussianGradientWithMask(src1, mask, gradX1, gradY1, m_sigma);
			gaussianGradientWithMask(src2, mask, gradX2, gradY2, m_sigma);
			
			gaussianSmoothingWithMask(src1, mask, gradT1, m_sigma);
			gaussianSmoothingWithMask(src2, mask, gradT2, m_sigma);
			
			vigra::SplineImageView<1,ValueType> gradX2_s(gradX2), gradY2_s(gradY2), gradT2_s(gradT2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Gaussian<double> gauss(m_outer_sigma);
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v, gx, gy, gt, we, sum_we;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				
				for(unsigned int j=m_mask_size/2; j<src1.height()-m_mask_size/2; ++j)
				{
					for(unsigned int i=m_mask_size/2; i<src1.width()-m_mask_size/2; ++i)
					{
                        if(mask(i,j) != 0)
                        {
                            last_u = flow(i,j)[0];
                            last_v = flow(i,j)[1];
                            
                            if(		i+last_u+m_mask_size/2 >= src1.width()
                               ||	i+last_u-m_mask_size/2 < 0
                               ||	j+last_v+m_mask_size/2 >= src1.height()
                               ||	j+last_v-m_mask_size/2 < 0 ) 
                                continue;
                            
                            //reset Matrix and result vector 	
                            A(0,0)=0; A(0,1) = 0;
                            A(1,0)=0; A(1,1) = 0;
                            
                            b(0,0) = 0; 
                            b(1,0) = 0;
                            
                            sum_we = 0;
                            
                            /** Create Sums of gradients to calculate optical flow:
                             *
                             *  SUM [ nabla(I_x)^2           nabla(I_x)*nabla(I_y)  ] * [ u ]   = SUM [nabla(I_x)] * nabla(I_t)
                             *      [ nabla(I_x)*nabla(I_y)  nabla(I_y)^2           ]   [ v ]         [nabla(I_y)] 
                             */
                            for(unsigned int mj = j-m_mask_size/2; mj<j+m_mask_size/2;	++mj)
                            {
                                for(unsigned int mi = i-m_mask_size/2; mi<i+m_mask_size/2;	++mi)
                                {
                                    gx = (gradX1(mi,mj) + gradX2_s(mi+last_u,mj+last_v))/2.0;
                                    gy = (gradY1(mi,mj) + gradY2_s(mi+last_u,mj+last_v))/2.0;
                                    gt = (gradT2_s(mi+last_u,mj+last_v)-gradT1(mi,mj));
                                    
                                    we =  gauss(sqrt((mi-i)*(mi-i)+(mj-j)*(mj-j)));
                                    sum_we += we;
                                    
                                    A(0,0) += gx*gx * we; // (nabla I_x)^2
                                    A(0,1) += gx*gy * we; // (nabla I_x)*(nabla I_y)
                                    A(1,1) += gy*gy * we;  // (nabla I_y)^2
                                    
                                    b(0,0) += gx*gt * we;
                                    b(1,0) += gy*gt * we;
                                    
                                }
                            }
                            //Normalize acc. t sum_w
                            A(0,0)/= sum_we;		A(0,1)/= sum_we;
                            A(0,1)/= sum_we;		A(1,1)/= sum_we;
                            b(0,0)/= sum_we;
                            b(1,0)/= sum_we;
                            
                            A(1,0) = A(0,1);
                            b	   = -b;
                            
                            //solve the linear system of equations
                            if(vigra::linearSolve( 	A, b, res))
                            {
                                //threshold vectors using the smallest of both eigenvalue scaled to one pixel 
                                vigra::symmetricEigensystem(A,ew,ev);
                                if(ew(1,0)>= m_threshold)
                                {
                                    flow(i,j)[0] = last_u+res(0,0);
                                    flow(i,j)[1] = last_v+res(1,0);
                                    flow(i,j)[2] = ew(1,0);
                                }
                            }
                        }
					}
				}
			}
		}
	
		
	private:
		double			m_sigma;
		double			m_outer_sigma;
		unsigned int	m_mask_size;
		double			m_threshold;
		unsigned int	m_iterations;
		
		unsigned int	m_level;
};




/**
 * This could be close to the approach proposed by Y. Sun  in "Ice Motion retrieval by the optical flow method"
 * The assumption here is a constant contrast of moving objects!
 */
class OpticalFlowCCFunctor
{
	public:
        //Typedefs for the resulting flow field
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the Constant Contrast Optical Flow approach.
         *
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time. Defaults to 1.0
         * \param threshold The minimal gradient magnitude to count a value inside the neigborhoof. Defaults to 0.0.
         * \param iterations The count of iterations. Defaults to 10.
         */
		OpticalFlowCCFunctor(double sigma=1.0, double threshold=0.0, unsigned int iterations = 10)
		:	m_sigma(sigma),
			m_threshold(threshold),
			m_iterations(iterations),
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
         * \return Always "Constant Contrast OFCE".
         */
		static std::string name()
		{
			return "Constant Contrast OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "CC OFCE".
         */
		static std::string shortName()
		{
			return "CC OFCE";
		}
		
		/**
         * Optical flow calculation according to the constant contrast assumption.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            
			vigra::MultiArray<2, ValueType>	gradXX1(src1.shape()), gradXX2(src1.shape()),
                                            gradXY1(src1.shape()), gradXY2(src1.shape()),
                                            gradYY1(src1.shape()), gradYY2(src1.shape()),
                                            gradX1(src1.shape()),  gradX2(src1.shape()),
                                            gradY1(src1.shape()),  gradY2(src1.shape());
			
			
			// First image: First order derivatives
			vigra::gaussianGradient(src1, gradX1, gradY1, m_sigma);
			// First image: Second order derivatives from hessian (matrix) of gaussian
			vigra::hessianMatrixOfGaussian(src1, gradXX1, gradXY1, gradYY1, m_sigma);
			
			// Second image: First order derivatives
			vigra::gaussianGradient(src2, gradX2, gradY2, m_sigma);
            
			// Second image: Second order derivatives from hessian (matrix) of gaussian
			vigra::hessianMatrixOfGaussian(src2, gradXX2, gradXY2, gradYY2, m_sigma);
			
			vigra::SplineImageView<1,ValueType> gradX2_s(gradX2), gradY2_s(gradY2),
                                                gradXX2_s(gradXX2), gradXY2_s(gradXY2), gradYY2_s(gradYY2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				for(unsigned int j=0;j<src1.height(); ++j)
				{
					for(unsigned int i=0;i<src1.width(); ++i)
					{
						last_u = flow(i,j)[0];
						last_v = flow(i,j)[1];
						
						if(		i+last_u >= 0 && i+last_u < src1.width()
							&&	j+last_v >= 0 && j+last_v < src1.height())
						{
							A(0,0) = (gradXX1(i,j) + gradXX2_s(i+last_u,j+last_v))/2.0;
							A(1,0) = A(0,1) = (gradXY1(i,j) + gradXY2_s(i+last_u,j+last_v))/2.0;
							A(1,1) = (gradYY1(i,j) + gradYY2_s(i+last_u,j+last_v))/2.0;
							
							b(0,0) = - (gradX2_s(i+last_u,j+last_v) - gradX1(i, j));
							b(1,0) = - (gradY2_s(i+last_u,j+last_v) - gradY1(i, j));
							
							//solve the linear system of equations
							if(vigra::linearSolve( 	A, b, res) )
							{
								double detA = determinant(A);
								//threshold vectors using the determinant
								if(detA> m_threshold)
								{
									flow(i,j)[0] = last_u+res(0,0);
									flow(i,j)[1] = last_v+res(1,0);
									flow(i,j)[2] = detA;
								}
							}
						}
					}
				}
			}
		}
		        
        /**
         * Masked Optical flow computation for the Constant Contrast method.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2, T1> & src1,
						const vigra::MultiArrayView<2, T2> & src2,
						const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            
			vigra::MultiArray<2, T3>	gradXX1(src1.shape()), gradXX2(src1.shape()),
										gradXY1(src1.shape()), gradXY2(src1.shape()),
										gradYY1(src1.shape()), gradYY2(src1.shape()),
										gradX1(src1.shape()),  gradX2(src1.shape()),
										gradY1(src1.shape()),  gradY2(src1.shape());
			
			
			// First image: First order derivatives
			gaussianGradientWithMask(src1, mask, gradX1, gradY1, m_sigma);
			// First image: Second order derivatives from hessian (matrix) of gaussian
			hessianMatrixOfGaussianWithMask(src1, mask, gradXX1, gradXY1, gradYY1, m_sigma);
			
			// Second image: First order derivatives
			gaussianGradientWithMask(src2, mask, gradX2, gradY2, m_sigma);
            
			// Second image: Second order derivatives from hessian (matrix) of gaussian
			hessianMatrixOfGaussianWithMask(src2, mask, gradXX2, gradXY2, gradYY2, m_sigma);
			
			vigra::SplineImageView<1,T3> gradX2_s(gradX2), gradY2_s(gradY2),
                                         gradXX2_s(gradXX2), gradXY2_s(gradXY2), gradYY2_s(gradYY2);
			
			//Matrix A, vecor b and eigenvectors resp. eigenvalues
			vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
			double last_u, last_v;
			
			for(unsigned int it=1; it<=m_iterations; ++it)
			{
				for(unsigned int j=0;j<src1.height(); ++j)
				{
					for(unsigned int i=0;i<src1.width(); ++i)
					{
						if(mask(i,j) != 0)
                        {
                            last_u = flow(i,j)[0];
                            last_v = flow(i,j)[1];
                            
                            if(		i+last_u >= 0 && i+last_u < src1.width()
                                &&	j+last_v >= 0 && j+last_v < src1.height())
                            {	
                                A(0,0) = (gradXX1(i,j) + gradXX2_s(i+last_u,j+last_v))/2.0;
                                A(1,0) = A(0,1) = (gradXY1(i,j) + gradXY2_s(i+last_u,j+last_v))/2.0;
                                A(1,1) = (gradYY1(i,j) + gradYY2_s(i+last_u,j+last_v))/2.0;
                                
                                b(0,0) = - (gradX2_s(i+last_u,j+last_v) - gradX1(i, j));
                                b(1,0) = - (gradY2_s(i+last_u,j+last_v) - gradY1(i, j));
                                
                                //solve the linear system of equations
                                if(vigra::linearSolve( 	A, b, res) )
                                {
                                    double detA = determinant(A);
                                    //threshold vectors using the determinant
                                    if(detA> m_threshold)
                                    {
                                        flow(i,j)[0] = last_u+res(0,0);
                                        flow(i,j)[1] = last_v+res(1,0);
                                        flow(i,j)[2] = detA;
                                    }
                                }
                            }
                        }
					}
				}
			}
		}
	
	private:
		double			m_sigma;
		double			m_threshold;
		unsigned int	m_iterations;
	
		unsigned int	m_level;
};


/**
 * Optical Flow according to the Farnebaeck algorithm
 */
class OpticalFlowFBFunctor
{
    public:
        //Typedefs for the resulting flow field
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,3> FlowValueType;
    
        /**
         * Constructor for the Structure Tensor Optical Flow approach.
         *
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time. Defaults to 1.0
         * \param mask_size The size of the equvalent motion neightborhod in rows&cols. Defaults to 11.
         * \param threshold The minimal gradient magnitude to count a value inside the neigborhoof. Defaults to 0.0.
         * \param iterations The count of iterations. Defaults to 100.
         */
        OpticalFlowFBFunctor(double sigma=1.0, unsigned int mask_size=11, float threshold=0.0, unsigned int iterations = 100)
        :	m_sigma(sigma),
            m_mask_size(mask_size),	
            m_threshold(threshold),
            m_iterations(iterations),
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
         * \return Always "Farnebaeck OFCE".
         */
        static std::string name()
        {
            return "Farnebaek OFCE";
        }
        
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "FB OFCE".
         */
        static std::string shortName()
        {
            return "FB OFCE";
        }
    
        /**
         * Optical flow computation for Farnebaek's method using polynomial expansion.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <class T1, class T2>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            //1. Create Images for polynomial Expansion R0 and R1 and the flow matrix:
            vigra::MultiArray<2, vigra::TinyVector<float,5> > R0(src1.shape()), R1(src1.shape()), M(src1.shape());
            
            //2. Do polynomial expansion for both images
            polynomialExpansion(src1, R0, /*poly_n*/ m_mask_size/2, /*poly_sigma*/ m_sigma);
            polynomialExpansion(src2, R1, /*poly_n*/ m_mask_size/2, /*poly_sigma*/ m_sigma);
            
            //Create an iterpolated view on the second polynomial exp.
            vigra::SplineImageView<1, vigra::TinyVector<float,5> > R1_s(R1);
            
            for(unsigned int i=1; i<=m_iterations; ++i)
            {
                //A: Compute the current flow matrix M - according to both poly exps and the current flow
                for(unsigned int y = 0; y < src1.height(); y++ )
                {
                    for(unsigned int x = 0; x < src1.width(); x++ )
                    {
                        float dx = flow(x,y)[0], dy = flow(x,y)[1];
                        float fx = x + dx, fy = y + dy;
                        
                        float r2, r3, r4, r5, r6;
                        
                        if( fx >=0 && fx < src1.width() && fy >=0 && fy < src1.height())
                        {
                            r2 = R1_s(fx,fy)[0];
                            r3 = R1_s(fx,fy)[1];
                            r4 = (R0(x,y)[2] + R1_s(fx,fy)[2])*0.5f;
                            r5 = (R0(x,y)[3] + R1_s(fx,fy)[3])*0.5f;
                            r6 = (R0(x,y)[4] + R1_s(fx,fy)[4])*0.25f;
                        }
                        else
                        {
                            r2 = r3 = 0.f;
                            r4 = R0(x,y)[2];
                            r5 = R0(x,y)[3];
                            r6 = R0(x,y)[4]*0.5f;
                        }
                        
                        r2 = (r2 - R0(x,y)[0])*0.5f;
                        r3 = (r3 - R0(x,y)[1])*0.5f;
                        
                        r2 += r4*dy + r6*dx;
                        r3 += r6*dy + r5*dx;
                        
                        M(x,y)[0] = r4*r4 + r6*r6; // G(1,1)
                        M(x,y)[1] = (r4 + r5) *r6; // G(1,2)=G(2,1)
                        M(x,y)[2] = r5*r5 + r6*r6; // G(2,2)
                        M(x,y)[3] = r4*r2 + r6*r3; // h(1)
                        M(x,y)[4] = r6*r2 + r5*r3; // h(2)
                    }
                }
                
                //B: Update the flow Matrix by smoothing and compute the flow
                //gaussian Smooth Matrix:
                vigra::gaussianSmoothing(M, M, m_sigma);
                
                for(unsigned int y = 0; y < src1.height(); y++ )
                {
                    for(unsigned int x = 0; x < src1.width(); x++ )
                    {
                        double g11_ = M(x,y)[0];
                        double g12_ = M(x,y)[1];
                        double g22_ = M(x,y)[2];
                        double h1_  = M(x,y)[3];
                        double h2_  = M(x,y)[4];
                        
                        double det = (g11_*g22_ - g12_*g12_);
                        
                        if(det != 0 && det >= m_threshold)
                        {
                            flow(x,y)[0] = float(g11_*h2_-g12_*h1_)/det;
                            flow(x,y)[1] = float(g22_*h1_-g12_*h2_)/det;
                            flow(x,y)[2] = det;
                        }
                    }
                }
            }		
        }
    
        /**
         * Masked Optical flow computation for Farnebaek's method using polynomial expansion.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The masked area under the series.
         * \param[out] flow The resulting Optical Flow field.
         */
        template <class T1, class T2, class T3>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");            
            
            //1. Create Images for polynomial Expansion R0 and R1 and the flow matrix:
            vigra::MultiArray<2, vigra::TinyVector<float,5> > R0(src1.shape()), R1(src1.shape()), M(src1.shape());
            
            //2. Do polynomial expansion for both images
            polynomialExpansionWithMask(src1, mask, R0, /*poly_n*/ m_mask_size/2, /*poly_sigma*/ m_sigma);
            polynomialExpansionWithMask(src2, mask, R1, /*poly_n*/ m_mask_size/2, /*poly_sigma*/ m_sigma);
            
            //Create an iterpolated view on the second polynomial exp.
            vigra::SplineImageView<1, vigra::TinyVector<float,5> > R1_s(R1);
            
            for(unsigned int i=1; i<=m_iterations; ++i)
            {
                //A: Compute the current flow matrix M - according to both poly exps and the current flow
                for(unsigned int y = 0; y < src1.height(); y++ )
                {
                    for(unsigned int x = 0; x < src1.width(); x++ )
                    {
                        if (mask(x,y) != 0)
                        {
                            float dx = flow(x,y)[0], dy = flow(x,y)[1];
                            float fx = x + dx, fy = y + dy;
                            
                            float r2, r3, r4, r5, r6;
                            
                            if( fx >=0 && fx < src1.width() && fy >=0 && fy < src1.height())
                            {
                                r2 = R1_s(fx,fy)[0];
                                r3 = R1_s(fx,fy)[1];
                                r4 = (R0(x,y)[2] + R1_s(fx,fy)[2])*0.5f;
                                r5 = (R0(x,y)[3] + R1_s(fx,fy)[3])*0.5f;
                                r6 = (R0(x,y)[4] + R1_s(fx,fy)[4])*0.25f;
                            }
                            else
                            {
                                r2 = r3 = 0.f;
                                r4 = R0(x,y)[2];
                                r5 = R0(x,y)[3];
                                r6 = R0(x,y)[4]*0.5f;
                            }
                            
                            r2 = (r2 - R0(x,y)[0])*0.5f;
                            r3 = (r3 - R0(x,y)[1])*0.5f;
                            
                            r2 += r4*dy + r6*dx;
                            r3 += r6*dy + r5*dx;
                            
                            M(x,y)[0] = r4*r4 + r6*r6; // G(1,1)
                            M(x,y)[1] = (r4 + r5) *r6; // G(1,2)=G(2,1)
                            M(x,y)[2] = r5*r5 + r6*r6; // G(2,2)
                            M(x,y)[3] = r4*r2 + r6*r3; // h(1)
                            M(x,y)[4] = r6*r2 + r5*r3; // h(2)
                        }
                    }
                }
                
                //B: Update the flow Matrix by smoothing and compute the flow
                //gaussian Smooth Matrix:
                gaussianSmoothingWithMask(M, mask, M, m_sigma);
                
                for(unsigned int y = 0; y < src1.height(); y++ )
                {
                    for(unsigned int x = 0; x < src1.width(); x++ )
                    {
                        if(mask(x,y) != 0)
                        {
                            double g11_ = M(x,y)[0];
                            double g12_ = M(x,y)[1];
                            double g22_ = M(x,y)[2];
                            double h1_  = M(x,y)[3];
                            double h2_  = M(x,y)[4];
                        
                            double det = (g11_*g22_ - g12_*g12_);
                            if(det != 0 && det >= m_threshold)
                            {
                                flow(x,y)[0] = float(g11_*h2_-g12_*h1_)/det;
                                flow(x,y)[1] = float(g22_*h1_-g12_*h2_)/det;
                                flow(x,y)[2] = det;
                            }
                        }
                    }
                }
            }		
        }
        
    protected:
        /**
         * Computes the Polynomial Expansion according to Farnebaeck.
         * Takes a single band image as input and computes a dest image, where each pixel is assigned to 
         * a 5D-Vector, containing the polynomial expansions of the image. For more infos
         * look at the original paper of Farnebaek.
         *
         * \param[in] src The input image (one band).
         * \param[out] dest The polynomial expansion of src given as an image of type vigra::TinyVector<T2,5>.
         * \param[in] n Radius of the used kernels
         * \param[in] sigma scale for the derivation of the polynomial expansion.
         */
        template <class T1, class T2>
        void polynomialExpansion(const vigra::MultiArrayView<2, T1> & src,
                                 vigra::MultiArrayView<2, vigra::TinyVector<T2,5> > dest,
                                 int n, double sigma)
        {
            vigra_precondition(src.shape() == dest.shape(), "src and dest sizes differ!");
            
            using namespace ::vigra;
            
            vigra::Kernel1D<double> gKern, xgKern, xxgKern;
            
            gKern.initExplicitly(-n, n) = 1.0;
            xgKern.initExplicitly(-n, n) = 1.0;
            xxgKern.initExplicitly(-n, n) = 1.0;
            
            for( int i = -n; i <= n; i++ )
            {
                gKern[i] = (float)std::exp(-i*i/(2*sigma*sigma));
            }
            gKern.normalize();
            
            for(int i = -n; i <= n; i++ )
            {
                xgKern[i]  = i* gKern[i];
                xxgKern[i] = i*xgKern[i];
            }
            
            
            vigra::Matrix<double>	matG(6, 6);
            matG.init(0.0);
            
            for(int j = -n; j <= n; j++ )
            {
                for(int i = -n; i <= n; i++ )
                {
                    matG(0,0) += gKern[j]*gKern[i];
                    matG(1,1) += gKern[j]*gKern[i]*i*i;
                    matG(3,3) += gKern[j]*gKern[i]*i*i*i*i;
                    matG(5,5) += gKern[j]*gKern[i]*i*i*j*j;
                }
            }
            
            matG(0,0) = 1.;
            matG(2,2) = matG(0,3) = matG(0,4) = matG(3,0) = matG(4,0) = matG(1,1);
            matG(4,4) = matG(3,3);
            matG(3,4) = matG(4,3) = matG(5,5);
            
            // invG:
            // [ x        e  e    ]
            // [    y             ]
            // [       y          ]
            // [ e        z       ]
            // [ e           z    ]
            // [                u ]   
            
            
            vigra::Matrix<double> invG = inverse(matG);
            double	ig11 = invG(1,1),
                    ig03 = invG(0,3),
                    ig33 = invG(3,3),
                    ig55 = invG(5,5);
            
            vigra::MultiArray<2,float> 	temp(src.shape()), ig(src.shape());
            
            
            vigra::MultiArrayView<2, T2> d0(dest.bindElementChannel(0)), d1(dest.bindElementChannel(1)),
                                         d2(dest.bindElementChannel(2)), d3(dest.bindElementChannel(3)),  d4(dest.bindElementChannel(4));
            
            separableConvolveY(src,	 temp,  gKern);
            separableConvolveX(temp, ig,    gKern);     // ig:  I * G
            separableConvolveX(temp, d0,    xgKern);	// d0:  I * G * x
            separableConvolveX(temp, d2,    xxgKern);	// d2:  I * G * xx
            
            separableConvolveY(src,  temp,  xgKern);
            separableConvolveX(temp, d1,    gKern);     // d1:  I * G * y
            separableConvolveX(temp, d3,    xgKern);	// d3:  I * G * xy
            
            separableConvolveY(src,  temp,  xxgKern);
            separableConvolveX(temp, d4,    gKern);     // d4:  I * G * yy
            
            using namespace vigra::multi_math;
            
            d0 = d1 * ig11;
            d1 = d0 * ig11;
            temp = d3 * ig55; //store it. will become d2
            d3 = ig * ig03 + d2 * ig33;
            d4 = ig * ig03 + d4 * ig33;
            
            d2 = temp;
        }
        
        /**
         * Computes the Masked Polynomial Expansion according to Farnebaeck.
         * Takes a single band image as input and computes a dest image, where each pixel is assigned to 
         * a 5D-Vector, containing the polynomial expansions of the image. For more infos
         * look at the original paper of Farnebaek.
         *
         * \param[in] src The input image (one band).
         * \param[in] mask The input image's mask (one band).
         * \param[out] dest The polynomial expansion of src given as an image of type vigra::TinyVector<T2,5>.
         * \param[in] n Radius of the used kernels
         * \param[in] sigma scale for the derivation of the polynomial expansion.
         */
        template <class T1, class T2, class T3>
        void polynomialExpansionWithMask(const vigra::MultiArrayView<2, T1> & src,
                                         const vigra::MultiArrayView<2, T2> & mask,
                                         vigra::MultiArrayView<2, vigra::TinyVector<T3,5> > dest,
                                         int n, double sigma,
                                         vigra::BorderTreatmentMode btmode = vigra::BORDER_TREATMENT_AVOID)
        {
            vigra_precondition(src.shape() == mask.shape(), "src and mask sizes differ!");
            vigra_precondition(src.shape() == dest.shape(), "src and dest sizes differ!");
            
            using namespace ::vigra;
            
            vigra::Kernel1D<double> gKern, xgKern, xxgKern;
            
            gKern.initExplicitly(-n, n) = 1.0;
            xgKern.initExplicitly(-n, n) = 1.0;
            xxgKern.initExplicitly(-n, n) = 1.0;
            
            for( int i = -n; i <= n; i++ )
            {
                gKern[i] = (float)std::exp(-i*i/(2*sigma*sigma));
            }
            gKern.normalize();
            
            for(int i = -n; i <= n; i++ )
            {
                xgKern[i]  = i* gKern[i];
                xxgKern[i] = i*xgKern[i];
            }
            
            
            vigra::Matrix<double>	matG(6, 6);
            matG.init(0.0);
            
            for(int j = -n; j <= n; j++ )
            {
                for(int i = -n; i <= n; i++ )
                {
                    matG(0,0) += gKern[j]*gKern[i];
                    matG(1,1) += gKern[j]*gKern[i]*i*i;
                    matG(3,3) += gKern[j]*gKern[i]*i*i*i*i;
                    matG(5,5) += gKern[j]*gKern[i]*i*i*j*j;
                }
            }
            
            matG(0,0) = 1.;
            matG(2,2) = matG(0,3) = matG(0,4) = matG(3,0) = matG(4,0) = matG(1,1);
            matG(4,4) = matG(3,3);
            matG(3,4) = matG(4,3) = matG(5,5);
            
            // invG:
            // [ x        e  e    ]
            // [    y             ]
            // [       y          ]
            // [ e        z       ]
            // [ e           z    ]
            // [                u ]   
            
            
            vigra::Matrix<double> invG = inverse(matG);
            double	ig11 = invG(1,1),
                    ig03 = invG(0,3),
                    ig33 = invG(3,3),
                    ig55 = invG(5,5);
            
            vigra::MultiArray<2,float> 	temp(src.shape()), ig(src.shape());
            
            vigra::MultiArrayView<2, T2> d0(dest.bindElementChannel(0)), d1(dest.bindElementChannel(1)),
                                         d2(dest.bindElementChannel(2)), d3(dest.bindElementChannel(3)),  d4(dest.bindElementChannel(4));
            
            Kernel2D<double> gKern2, xgKern2, ygKern2, xxgKern2, xygKern2, yygKern2;
            
            gKern2.initSeparable(gKern,gKern);
            gKern2.setBorderTreatment(btmode);
            
            xgKern2.initSeparable(xgKern, gKern);
            xgKern2.setBorderTreatment(btmode);
            
            ygKern2.initSeparable(gKern, xgKern);
            ygKern2.setBorderTreatment(btmode);
            
            xxgKern2.initSeparable(xxgKern, gKern);
            xxgKern2.setBorderTreatment(btmode);
            
            xygKern2.initSeparable(xgKern, xgKern);
            xygKern2.setBorderTreatment(btmode);
            
            yygKern2.initSeparable(gKern, xxgKern);
            yygKern2.setBorderTreatment(btmode);
            
            vigra::normalizedConvolveImage(src, mask,	ig, gKern2);	// ig:  I * G
            vigra::normalizedConvolveImage(src, mask,	d0, xgKern2);	// d0:  I * G * x
            vigra::normalizedConvolveImage(src, mask,	d1, ygKern2);	// d1:  I * G * y
            vigra::normalizedConvolveImage(src, mask,	d2, xxgKern2);	// d2:  I * G * x * x
            vigra::normalizedConvolveImage(src, mask,	d3, xygKern2);	// d3:  I * G * x * y
            vigra::normalizedConvolveImage(src, mask,	d4, yygKern2);	// d4:  I * G * y * y
            
            using namespace vigra::multi_math;
            
            d0 = d1 * ig11;
            d1 = d0 * ig11;
            temp = d3 * ig55; //store it. will become d2
            d3 = ig * ig03 + d2 * ig33;
            d4 = ig * ig03 + d4 * ig33;
            
            d2 = temp;
        }
    
    private:
        double			m_sigma;
        unsigned int	m_mask_size;
        float			m_threshold;
        unsigned int	m_iterations;
        
        unsigned int	m_level;
};

} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOW_LOCAL_HXX
