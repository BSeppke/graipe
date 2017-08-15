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

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOW_GLOBAL_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOW_GLOBAL_HXX

//debug output
#include <QtDebug>

//linear solving and eigenvector analysis
#include <vigra/multi_math.hxx>
#include <vigra/convolution.hxx>
#include <vigra/stdconvolution.hxx>
#include <vigra/linear_solve.hxx>
#include <vigra/eigensystem.hxx>

//OFCE Spatiotemporal Gradients
#include "opticalflowgradients.hxx"




namespace graipe {

/**
 * @addtogroup graipe_opticalflow
 * @{
 *
 * @file
 * @brief Header file for the classical global Optical Flow algorithms.
 */
 
/** 
 * Beginning of Horn & Schuck's famous optical flow algorithm (iterative)
 * The iterations be can be cut by a given number of max. iterations. 
 * Otherwise the algorithm will finish if the mean change rate of the 
 * current step is below 0.001
 *
 * 1. The original version as described in [Horn1981] with poor
 *    approximations of the spatial gradients. 
 *    However, these approx. lead to a very high performance of the algorithm.
 *    Btw., you can prefilter the images in space with a gaussian smoothing
 *    filter of given sigma.
 *
 * 2. Modified Version of the above algorithm. This version uses gaussian gradient
 *    and smoothness filters of given sigma to calculate the means and gradients.
 */
 
/**
 * The original Horn & Schuck OFCE algorithm as proposed in Horn & Schunck 1981
 */
class OpticalFlowHSOriginalFunctor
{
	public:
        /** The single value type of a flow field **/
        typedef float ValueType;
        /** The flow vector type. 2 elements: u,v **/
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
        
        /**
         * Constructor for the classical Horn&Schunck approach.
         *
         * \param alpha The alpha weight between gradient and smoothness.
         * \param iterations The count of iterations.
         * \param sigma The sigma is currently ignored.
         */
		OpticalFlowHSOriginalFunctor(double alpha=50, int iterations=1, double sigma=1.0)
		:	m_alpha(alpha),
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
		}
		
        /**
         * Returns the full name of the functor.
         *
         * \return Always "Horn Schunck OFCE".
         */
		static std::string name()
		{
			return "Horn Schunck OFCE";
		}
		
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "HS OFCE".
         */
		static std::string shortName()
		{
			return "HS OFCE";
		}
	
		/**
		 * Here comes the implementatation, better known as the computation of optical flow
		 * according to Norn & Schunck 81.
		 *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
			vigra::MultiArray<2, FlowValueType> last_flow;
			
			double iter_change=0, mean_change=0, max_change=0;
			
			for (int iteration=1;iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
				last_flow = flow;
				
				for (int j=1; j<src1.height()-1; ++j)
				{
					for (int i=1; i<src1.width()-1; ++i)
					{
					
						double	E_x = 0.25*(		src1(i+1,j  ) - src1(i,  j  )
												+	src1(i+1,j+1) - src1(i,  j+1)
												+	src2(i+1,j  ) - src2(i,  j  )
												+	src2(i+1,j+1) - src2(i,  j+1)),
								
								E_y = 0.25*(		src1(i,  j+1) - src1(i,  j  )
												+	src1(i+1,j+1) - src1(i+1,j  )
												+	src2(i,  j+1) - src2(i,  j  )
												+	src2(i+1,j+1) - src2(i+1,j  )),
								
								E_t = 0.25*(		src2(i,  j  ) - src1(i,  j  )
												+	src2(i+1,j  ) - src1(i+1,j  )
												+	src2(i,  j+1) - src1(i,  j+1)
												+	src2(i+1,j+1) - src1(i+1,j+1)),
											
								u_mean =	(last_flow(i-1,j)[0]   + last_flow(i,j+1)[0]   +  last_flow(i+1,j)[0]   +  last_flow(i,j-1)[0]) /6.0
										 +	(last_flow(i-1,j-1)[0] + last_flow(i-1,j+1)[0] +  last_flow(i+1,j+1)[0] +  last_flow(i-1,j-1)[0])/12.0,
											
								v_mean =	(last_flow(i-1,j)[1]   + last_flow(i,j+1)[1]   +  last_flow(i+1,j)[1]   +  last_flow(i,j-1)[1]) /6.0
										 +	(last_flow(i-1,j-1)[1] + last_flow(i-1,j+1)[1] +  last_flow(i+1,j+1)[1] +  last_flow(i-1,j-1)[1])/12.0,

								fix_part = (E_x*u_mean + E_y*v_mean + E_t)	/	(m_alpha*m_alpha + E_x*E_x + E_y*E_y);
						
						
						flow(i,j)[0] = u_mean - fix_part*E_x;
						flow(i,j)[1] = v_mean - fix_part*E_y;
						
						iter_change = vigra::norm(last_flow(i,j) - flow(i,j));
						mean_change += iter_change;
						max_change = std::max(max_change, iter_change);
					}
				}
				mean_change= mean_change / (src1.size());
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
    
        /**
		 * Here comes the implementatation, better known as the computation of optical flow
		 * according to Norn & Schunck 81.
		 *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask THe mask, where pixel values are assumed to be valid.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        const vigra::MultiArrayView<2,T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {            
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            vigra::MultiArray<2, FlowValueType> last_flow;
			
			double iter_change=0, mean_change=0, max_change=0;
			
			for (int iteration=1;iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
                last_flow = flow;
				
				for (int j=1; j<src1.height()-1; ++j)
				{
					for (int i=1; i<src1.width()-1; ++i)
					{
					
						if(	  mask(i,  j  ) !=0  && mask(i,  j+1) !=0
						   && mask(i+1,j  ) !=0  && mask(i+1,j+1) !=0)
                        {
                            double	E_x = 0.25*(		src1(i+1,j  ) - src1(i,  j  )
                                                    +	src1(i+1,j+1) - src1(i,  j+1)
                                                    +	src2(i+1,j  ) - src2(i,  j  )
                                                    +	src2(i+1,j+1) - src2(i,  j+1)),
                                    
                                    E_y = 0.25*(		src1(i,  j+1) - src1(i,  j  )
                                                    +	src1(i+1,j+1) - src1(i+1,j  )
                                                    +	src2(i,  j+1) - src2(i,  j  )
                                                    +	src2(i+1,j+1) - src2(i+1,j  )),
                                    
                                    E_t = 0.25*(		src2(i,  j  ) - src1(i,  j  )
                                                    +	src2(i+1,j  ) - src1(i+1,j  )
                                                    +	src2(i,  j+1) - src1(i,  j+1)
                                                    +	src2(i+1,j+1) - src1(i+1,j+1)),
                                                
                                    u_mean =	(last_flow(i-1,j)[0]   + last_flow(i,j+1)[0]   +  last_flow(i+1,j)[0]   +  last_flow(i,j-1)[0]) /6.0
                                             +	(last_flow(i-1,j-1)[0] + last_flow(i-1,j+1)[0] +  last_flow(i+1,j+1)[0] +  last_flow(i-1,j-1)[0])/12.0,
                                                
                                    v_mean =	(last_flow(i-1,j)[1]   + last_flow(i,j+1)[1]   +  last_flow(i+1,j)[1]   +  last_flow(i,j-1)[1]) /6.0
                                             +	(last_flow(i-1,j-1)[1] + last_flow(i-1,j+1)[1] +  last_flow(i+1,j+1)[1] +  last_flow(i-1,j-1)[1])/12.0,

                                    fix_part = (E_x*u_mean + E_y*v_mean + E_t)	/	(m_alpha*m_alpha + E_x*E_x + E_y*E_y);
                            
                            
                            flow(i,j)[0] = u_mean - fix_part*E_x;
                            flow(i,j)[1] = v_mean - fix_part*E_y;
                            
                            iter_change = vigra::norm(last_flow(i,j) - flow(i,j));
                            mean_change += iter_change;
                            max_change = std::max(max_change, iter_change);
                        }
                    }
                }
				mean_change= mean_change / (src1.size());
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}

    private:
		double	m_alpha;
		int		m_iterations;
    //  double  m_sigma;
		int		m_level;
};




/**
 * The modified (or better: extended) version of the Horn & Schunck algorithm
 * which uses the vigra's functionality with an arbitrary sigma for the
 * smoothing and gradient convolution filter kernels.
 */ 
class OpticalFlowHSFunctor
{
	public:
        /** The single value type of a flow field **/
        typedef float ValueType;
        /** The flow vector type. 2 elements: u,v **/
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
        /**
         * Constructor for the Gaussian Horn&Schunck approach.
         *
         * \param alpha The alpha weight between gradient and smoothness.
         * \param iterations The count of iterations.
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time.
         */
		OpticalFlowHSFunctor(double alpha=50, int iterations=1, double sigma=1.0)
		:	m_alpha(alpha),
			m_iterations(iterations),
			m_sigma(sigma),
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
         * \return Always "Gaussian Horn Schunck OFCE".
         */
		static std::string name()
		{
			return "Gaussian Horn Schunck OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "GA HS OFCE".
         */
		static std::string shortName()
		{
			return "GA HS OFCE";
		}
		
		/**
		 * Here comes the implementatation, better known as the computation of optical flow
		 * according to Norn & Schunck but with better gradient estimation.
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2,ValueType> gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape());
            
            vigra::MultiArray<2, FlowValueType>  mean_flow(src1.shape());
						
			//spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
			
			double iter_change=0, mean_change=0, max_change=0;
			
			for (int iteration=1;iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
				vigra::gaussianSmoothing(flow, mean_flow,	m_sigma);
				
				for (int j=0; j<src1.height(); ++j)
				{
					for (int i=0; i<src1.width(); ++i)
					{
						double	fix_part	= (gradX(i,j)*mean_flow(i,j)[0]+ gradY(i,j)*mean_flow(i,j)[1]+ gradT(i,j))	/	(m_alpha*m_alpha + gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j)),
								new_u		= mean_flow(i,j)[0] - fix_part*gradX(i,j),
								new_v		= mean_flow(i,j)[1] - fix_part*gradY(i,j);
						
						iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
										   +	pow(flow(i,j)[1] - new_v,2));
						
						mean_change	+= iter_change;
						max_change = max(max_change, iter_change);
                        
						flow(i,j)[0] = new_u;
                        flow(i,j)[1] = new_v;
					}
				}
				mean_change = mean_change / src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
		
		/**
		 * Here comes the masked implementatation, better known as the computation of optical flow
		 * according to Norn & Schunck but with better gradient estimation
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask THe mask, where pixel values are assumed to be valid.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        const vigra::MultiArrayView<2,T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {            
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            
			vigra::MultiArray<2,ValueType> gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape());
            
            vigra::MultiArray<2, FlowValueType>  mean_flow(src1.shape());
						
			//spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradientWithMask(src1, src2, mask, gradX, gradY, gradT, m_sigma);
			
			double iter_change=0, mean_change=0, max_change=0;
			
			for (int iteration=1;iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
				gaussianSmoothingWithMask(flow, mask, mean_flow,	m_sigma);
				
				for (int j=0; j<src1.height(); ++j)
				{
					for (int i=0; i<src1.width(); ++i)
					{
						if(mask(i,j) !=0)
                        {
                            double	fix_part	= (gradX(i,j)*mean_flow(i,j)[0]+ gradY(i,j)*mean_flow(i,j)[1]+ gradT(i,j))	/	(m_alpha*m_alpha + gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j)),
                                    new_u		= mean_flow(i,j)[0] -fix_part*gradX(i,j),
                                    new_v		= mean_flow(i,j)[1] -fix_part*gradY(i,j);
						
                            iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
										   +	pow(flow(i,j)[1] - new_v,2));
						
                            mean_change	+= iter_change;
                            max_change = max(max_change, iter_change);
						
                            flow(i,j)[0] = new_u;
                            flow(i,j)[1] = new_v;
						}
					}
				}
				mean_change = mean_change / src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
    
	private:
		double	m_alpha;
		int		m_iterations;
		double  m_sigma;
		int		m_level;
};




/**
 * The optical flow as defined by Nagel and Enkelmann - takes the second derivative
 * of the vectorfields into account. Slightly slower thand Horn & Schunk, because
 * this derivative has to be re-calculated for each iteration.
 *
 * Implementation by means of: Barron, Fleet and Beauchemin IJCV Paper 1992
 *
 */ 
class OpticalFlowNEFunctor
{
	public:		
        /** The single value type of a flow field **/
        typedef float ValueType;
        /** The flow vector type. 2 elements: u,v **/
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
        /**
         * Constructor for the Nagel & Enkelmann approach.
         *
         * \param alpha The alpha weight between gradient and smoothness.
         * \param iterations The count of iterations.
         * \param sigma The sigma of the Gaussian used to estimate the partial derivatives
         *              in space and time.
         */
		OpticalFlowNEFunctor(double alpha=50, int iterations=1, double sigma=1.0)
		:	m_alpha(alpha),
			m_iterations(iterations),
			m_sigma(sigma),
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
         * \return Always "Nagel Enkelmann OFCE".
         */
		static std::string name()
		{
			return "Nagel Enkelmann OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "NE OFCE".
         */
		static std::string shortName()
		{
			return "NE OFCE";
		}
		
		/**
         * Standard Optical flow calculation according to Nagel & Enkelmann
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            using namespace ::vigra::linalg;
            
			vigra::MultiArray<2,ValueType>  gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()),
                                     gradXX(src1.shape()), gradXY(src1.shape()), gradYY(src1.shape()),
                                     q_x(src1.shape()), q_y(src1.shape()),
                                     u_x(src1.shape()),	v_x(src1.shape()),
                                     u_y(src1.shape()),	v_y(src1.shape()),
                                     u_xy(src1.shape()), v_xy(src1.shape()),
                                     temp(src1.shape());
            
            vigra::MultiArray<2,FlowValueType> mean_flow(src1.shape());
            
			double	delta  = 1; 						
            
            temp = src1;
            temp += src2;
            temp /= 2;
            
			// Second order derivatives from hessian (matrix) of gaussian
			vigra::hessianMatrixOfGaussian(temp, gradXX, gradXY, gradYY, m_sigma);
			
			//spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
					
			//preparing q_x and q_y (is constant for all iterations)
			for (int j=0; j<src1.height(); ++j)
			{
				for (int i=0; i<src1.width(); ++i)
				{
					vigra::Matrix<double>	W(2,2),	hessOfGauss(2,2), invHessOfGauss(2,2), nablaI_t(1,2), q_mat(1,2);
					
					//Calculate weight matrix W
					W(0,0) = gradY(i,j)*gradY(i,j) + delta;
					W(1,0) = -gradX(i,j)*gradY(i,j);
					W(0,1) = -gradX(i,j)*gradY(i,j);
					W(1,1) = gradX(i,j)*gradX(i,j) + delta;
					
					W = 1.0/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2.0*delta) * W;
					
					//Calculate q
					invHessOfGauss(0,0) = gradYY(i,j);			
					invHessOfGauss(1,0) = -gradXY(i,j);
					invHessOfGauss(0,1) = -gradXY(i,j);
					invHessOfGauss(1,1) = gradXX(i,j);
					
					hessOfGauss(0,0) = gradXX(i,j);	
					hessOfGauss(1,0) = gradXY(i,j);	
					hessOfGauss(0,1) = gradXY(i,j);
					hessOfGauss(1,1) = gradYY(i,j);
					
					nablaI_t(0,0) = gradX(i,j);
					nablaI_t(0,1) = gradY(i,j);
					
					q_mat	=	1.0/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2.0*delta)
								*	(nablaI_t * (invHessOfGauss + 2.0*hessOfGauss*W));
					
					q_x(i,j) = q_mat(0,0);
					q_y(i,j) = q_mat(0,1);
				}
			}
			
			double iter_change=0, mean_change=0,max_change=0;
			
			double	Xi_u, Xi_v, fix_part, new_u, new_v;
			
			//prepare gaussian kernel for smoothing of vectorfields
			vigra::Kernel1D<float> k; 
			k.initGaussianDerivative(m_sigma, 1);
			
			//do iterations
			for (int iteration=1; iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
				//u,v-mean
				vigra::gaussianSmoothing(flow, mean_flow, m_sigma);
				
				// u_x and u_y
				vigra::gaussianGradient(flow.bindElementChannel(0), u_x, u_y, m_sigma);
				
				// u_xy
				vigra::separableConvolveX(flow.bindElementChannel(0), temp, k);
				vigra::separableConvolveY(temp, u_xy, k);
								
				// v_x and v_y
				vigra::gaussianGradient(flow.bindElementChannel(1), v_x, v_y, m_sigma);
				
				// v_xy
				vigra::separableConvolveX(flow.bindElementChannel(1), temp, k);
				vigra::separableConvolveY(temp, v_xy, k);
				
                for (int j=0; j<src1.height(); ++j)
                {
                    for (int i=0; i<src1.width(); ++i)
                    {
						//Calculate Xi's
						Xi_u =		mean_flow(i,j)[0] 
								-	2.0*gradX(i,j)*gradY(i,j)*u_xy(i,j)/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2*delta)
								-	(q_x(i,j)*u_x(i,j)+q_y(i,j)*u_y(i,j));
						
						Xi_v =		mean_flow(i,j)[1]
								-	2.0*gradX(i,j)*gradY(i,j)*v_xy(i,j)/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2*delta)
								-	(q_x(i,j)*v_x(i,j)+q_y(i,j)*v_y(i,j));
						
						//Assign new values
						fix_part =  (gradX(i,j)*Xi_u +gradY(i,j)*Xi_v + gradT(i,j)) / (gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + m_alpha*m_alpha);
						
						new_u = Xi_u - gradX(i,j)*fix_part;
						new_v = Xi_v - gradY(i,j)*fix_part;
						
						//qDebug() << "q: (" << q_x(i,j) << ", "<< q_y(i,j) << ")\n";
						//qDebug() << "Xi: (" << Xi_u << ", "<< Xi_v << ")\n";
						//qDebug() << "fix_part: (" << fix_part << ")\n";
						//qDebug() << "new: (" << new_u << ", "<< new_v << ")\n";
						
						iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
											+	pow(flow(i,j)[1] - new_v,2));
						
						mean_change += iter_change;
						max_change = max(max_change, iter_change);
						
						flow(i,j)[0] = new_u;
						flow(i,j)[1] = new_v;
					}
				}
				
				mean_change = mean_change / src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
	
		/**
         * Masked Optical flow calculation according to Nagel & Enkelmann
         *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask THe mask, where pixel values are assumed to be valid.
         * \param[out] flow The resulting Optical Flow field.
         */
		template <class T1, class T2, class T3>
		void operator()(const vigra::MultiArrayView<2,T1> & src1,
                        const vigra::MultiArrayView<2,T2> & src2,
                        const vigra::MultiArrayView<2,T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
		{
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == mask.shape(), "image and mask sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
            
            using namespace ::vigra;
            using namespace ::vigra::linalg;
            
			vigra::MultiArray<2,ValueType>  gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()),
                                     gradXX(src1.shape()), gradXY(src1.shape()), gradYY(src1.shape()),
                                     mean_u(src1.shape()), mean_v(src1.shape()),
                                     q_x(src1.shape()), q_y(src1.shape()),
                                     u_x(src1.shape()),	v_x(src1.shape()),
                                     u_y(src1.shape()),	v_y(src1.shape()),
                                     u_xy(src1.shape()), v_xy(src1.shape()),
                                     temp(src1.shape());
            
            vigra::MultiArray<2,FlowValueType> mean_flow(src1.shape());
            
			double	delta  = 1;
            
            temp = src1;
            temp += src2;
            temp /= 2;
			
			// Second order derivatives from hessian (matrix) of gaussian
			hessianMatrixOfGaussianWithMask(temp,  mask, gradXX, gradXY, gradYY, m_sigma);
			
			//spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradientWithMask(src1, src2, mask, gradX, gradY, gradT, m_sigma);
			
			//preparing q_x and q_y (is constant for all iterations)
            for (int j=0; j<src1.height(); ++j)
            {
                for (int i=0; i<src1.width(); ++i)
                {
                    
					vigra::Matrix<double>	W(2,2),	hessOfGauss(2,2), invHessOfGauss(2,2), nablaI_t(1,2), q_mat(1,2);
					
					//Calculate weight matrix W
					W(0,0) = gradY(i,j)*gradY(i,j) + delta;
					W(1,0) = -gradX(i,j)*gradY(i,j);
					W(0,1) = -gradX(i,j)*gradY(i,j);
					W(1,1) = gradX(i,j)*gradX(i,j) + delta;
					
					W = 1.0/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2.0*delta) * W;
					
					//Calculate q
					invHessOfGauss(0,0) = gradYY(i,j);			
					invHessOfGauss(1,0) = -gradXY(i,j);
					invHessOfGauss(0,1) = -gradXY(i,j);
					invHessOfGauss(1,1) = gradXX(i,j);
					
					hessOfGauss(0,0) = gradXX(i,j);	
					hessOfGauss(1,0) = gradXY(i,j);	
					hessOfGauss(0,1) = gradXY(i,j);
					hessOfGauss(1,1) = gradYY(i,j);
					
					nablaI_t(0,0) = gradX(i,j);
					nablaI_t(0,1) = gradY(i,j);
					
					q_mat	=	1.0/	(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2.0*delta)
									*	(nablaI_t * (invHessOfGauss + 2.0*hessOfGauss*W));
					
					q_x(i,j) = q_mat(0,0);
					q_y(i,j) = q_mat(0,1);
				}
			}
			
			double iter_change=0, mean_change=0,max_change=0;
			
			double	Xi_u, Xi_v, fix_part, new_u, new_v;
			
			//prepare gaussian kernel for smoothing of vectorfields
			vigra::Kernel1D<float> k; 
			k.initGaussianDerivative(m_sigma, 1);
			
			//do iterations
			for (int iteration=1; iteration<=m_iterations; ++iteration)
			{
				mean_change=0;
				max_change=0;
				
				//u,v-mean
				gaussianSmoothingWithMask(flow, mask, mean_flow, m_sigma);
				
				// u_x and u_y
				gaussianGradientWithMask(flow.bindElementChannel(0), mask, u_x, u_y,	m_sigma);
				
				// u_xy
				gaussianGradient2ndMixedTermWithMask(flow.bindElementChannel(0), mask, u_xy, m_sigma);
								
				// v_x and v_y
				gaussianGradientWithMask(flow.bindElementChannel(1), mask, v_x, v_y, m_sigma);
				
				// v_xy
				gaussianGradient2ndMixedTermWithMask(flow.bindElementChannel(1), mask, v_xy, m_sigma);
				
                for (int j=0; j<src1.height(); ++j)
                {
                    for (int i=0; i<src1.width(); ++i)
                    {
						if(mask(i,j) !=0)
						{
                            //Calculate Xi's
                            Xi_u =		mean_flow(i,j)[0] 
                                    -	2.0*gradX(i,j)*gradY(i,j)*u_xy(i,j)/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2*delta)
                                    -	(q_x(i,j)*u_x(i,j)+q_y(i,j)*u_y(i,j));
                            
                            Xi_v =		mean_flow(i,j)[1]
                                    -	2.0*gradX(i,j)*gradY(i,j)*v_xy(i,j)/(gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + 2*delta)
                                    -	(q_x(i,j)*v_x(i,j)+q_y(i,j)*v_y(i,j));
                            
                            //Assign new values
                            fix_part =  (gradX(i,j)*Xi_u +gradY(i,j)*Xi_v + gradT(i,j)) / (gradX(i,j)*gradX(i,j) + gradY(i,j)*gradY(i,j) + m_alpha*m_alpha);
                            
                            new_u = Xi_u - gradX(i,j)*fix_part;
                            new_v = Xi_v - gradY(i,j)*fix_part;
                            
                            //qDebug() << "q: (" << q_x(i,j) << ", "<< q_y(i,j) << ")\n";
                            //qDebug() << "Xi: (" << Xi_u << ", "<< Xi_v << ")\n";
                            //qDebug() << "fix_part: (" << fix_part << ")\n";
                            //qDebug() << "new: (" << new_u << ", "<< new_v << ")\n";
                            
                            iter_change = sqrt(		pow(flow(i,j)[0] - new_u,2)
                                               +	pow(flow(i,j)[1] - new_v,2));
                            
                            mean_change += iter_change;
                            max_change = std::max(max_change, iter_change);
                            
                            flow(i,j)[0] = new_u;
                            flow(i,j)[1] = new_v;
                        }
					}
				}
				
				mean_change = mean_change /src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
			
		}

   private:
		double	m_alpha;
		int		m_iterations;
		double  m_sigma;
		int		m_level;
};

/**
 * @}
 */
    
} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOW_GLOBAL_HXX
