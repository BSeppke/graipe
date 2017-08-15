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

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOW_HYBRID_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOW_HYBRID_HXX

//debug output
#include <QtDebug>

//OFCE Spatiotemporal Gradients
#include "opticalflowgradients.hxx"


namespace graipe {

/**
 * @addtogroup graipe_opticalflow
 * @{
 *
 * @file
 * @brief Header file for the modern hybrid Optical Flow algorithms.
 */
    
/**
 * The linear combined local global approach as described by Bruhn, Weickert, Schnörr et. al 2004
 */
class OpticalFlowCLGFunctor
{
	public:
        /** The single value type of a flow field **/
        typedef float ValueType;
        /** The flow vector type. 2 elements: u,v **/
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
        /**
         * Constructor for linear combined local global approach as described by Bruhn, Weickert, Schnörr et. al 2004.
         *
         * \param alpha The alpha weight between gradient and smoothness.
         * \param sigma The sigma of the Gaussian used to compute the spatio-temporal gradients.
         * \param outer_sigma The sigma of the Gaussian used to apply the smoothing step for the Structure Tensor.
         * \param omega Linear penalizer weight. Defaults to 1.0.
         * \param iterations The count of iterations.
         */
		OpticalFlowCLGFunctor(double alpha=1.0, double sigma=1.0, double outer_sigma=3.0, double omega=1.0, int iterations=100)
		:	m_outer_sigma(outer_sigma),
			m_sigma(sigma),
			m_alpha(alpha),
			m_omega(omega),
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
         * \return Always "Combined local global OFCE".
         */
		static std::string name()
		{
			return "Combined local global OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "CLG OFCE".
         */
        static std::string shortName()
		{
			return "CLG OFCE";
		}
		
		/**
         * Optical Flow calculation according to Bruhn et. al 2004.
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
            using namespace ::vigra::multi_math;
			
			vigra::MultiArray<2, ValueType> stxx(src1.shape()), stxy(src1.shape()), styy(src1.shape()),
                                            gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()), temp(src1.shape());
            
            vigra::MultiArray<2, FlowValueType> last_flow(src1.shape());
			
            temp = (src1+src2)/2;
            
			// calculate Structure Tensor at inner scale = sigma and outer scale = sigma2
			vigra::structureTensor(temp, stxx, stxy, styy, m_sigma, m_outer_sigma);
			
			
			//Calculate spatio temporal gradients for the vector "b"
			
			//1. step: calculate spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
			
			//2. step:	set I_x =>  b_x = smooth<I_x * I_t>
			//			    I_y =>  b_y = smooth<I_y * I_t>
			gradX = gradT*gradX;
			gradY = gradT*gradY;
			vigra::gaussianSmoothing(gradY, gradX,	m_outer_sigma);
			vigra::gaussianSmoothing(gradY, gradY,	m_outer_sigma);
			
			
			int max_iter = m_iterations;
			double	omega = m_omega,
					iter_change=0,
					mean_change=0,
					max_change=0;
			
			for(int iteration=1; iteration<=max_iter; iteration++)
			{
				//save last results
				last_flow = flow;
				
				mean_change=0;
				max_change=0;
				
				for (int j=1;j<src1.height()-1; ++j)
				{
					for (int i=1;i<src1.width()-1; ++i)
					{
						 //With SOR (successive over-relaxation)
						 flow(i,j)[0] = (1.0-omega)*last_flow(i,j)[0]
										+	omega*(		(flow(i-1,j)[0] + flow(i,j-1)[0])
													+	(last_flow(i+1,j)[0]+last_flow(i,j+1)[0])
													-	(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[1] + gradX(i,j)))
										/	(4.0 + (1.0/m_alpha)*stxx(i,j));
						
						 flow(i,j)[1] = (1.0-omega)*last_flow(i,j)[1]
									   +	omega*(		(flow(i-1,j)[1] + flow(i,j-1)[1])
													+	(last_flow(i+1,j)[1]+last_flow(i,j+1)[1])
													-	(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[0] + gradY(i,j)))
									   /	(4.0 + (1.0/m_alpha)*styy(i,j));
						
						iter_change =  vigra::norm(last_flow(i,j) - flow(i,j));
						mean_change += iter_change;
										
                        max_change = std::max(max_change, iter_change);
					}
				}
				mean_change = mean_change / src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
	
		/**
         * Masked Optical Flow calculation according to Bruhn et. al 2004.
		 *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The mask, where to estimate the flow.
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
            using namespace ::vigra::multi_math;
            
			vigra::MultiArray<2, ValueType> stxx(src1.shape()), stxy(src1.shape()), styy(src1.shape()),
                                            gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()), temp(src1.shape());
           
            vigra::MultiArray<2,FlowValueType> last_flow(src1.shape());
            
            temp = (src1+src2)/2;
            
			// calculate Structure Tensor at inner scale = sigma and outer scale = sigma2
			structureTensorWithMask(temp, mask, stxx, stxy, styy, m_sigma, m_outer_sigma);
			
			
			//Calculate spatio temporal gradients for the vector "b"
			
			//1. step: calculate spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradientWithMask(src1, src2, mask, gradX , gradY, gradT, m_sigma);
			
			//2. step:	set I_x =>  b_x = smooth<I_x * I_t>
			//			    I_y =>  b_y = smooth<I_y * I_t>
			gradX = gradT*gradX;
			gradY = gradT*gradY;
			gaussianSmoothingWithMask(gradY, mask, gradX,	m_outer_sigma);
			gaussianSmoothingWithMask(gradY, mask, gradY,	m_outer_sigma);
			
			
			int max_iter = m_iterations;
			double	omega = m_omega,
			iter_change=0,
			mean_change=0,
			max_change=0;
			
			for(int iteration=1; iteration<=max_iter; iteration++)
			{
				//save last results
				last_flow = flow;
				
				mean_change=0;
				max_change=0;
				
				for (int j=1;j<src1.height()-1; ++j)
				{
					for (int i=1;i<src1.width()-1; ++i)
					{
						if(	mask(i,j) !=0 )
						{
						 //With SOR (successive over-relaxation)
						 flow(i,j)[0] = (1.0-omega)*last_flow(i,j)[0]
										+	omega*(		(flow(i-1,j)[0] + flow(i,j-1)[0])
													+	(last_flow(i+1,j)[0]+last_flow(i,j+1)[0])
													-	(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[1] + gradX(i,j)))
										/	(4.0 + (1.0/m_alpha)*stxx(i,j));
						
						 flow(i,j)[1] = (1.0-omega)*last_flow(i,j)[1]
									   +	omega*(		(flow(i-1,j)[1] + flow(i,j-1)[1])
													+	(last_flow(i+1,j)[1]+last_flow(i,j+1)[1])
													-	(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[0] + gradY(i,j)))
									   /	(4.0 + (1.0/m_alpha)*styy(i,j));
						
						iter_change =  vigra::norm(last_flow(i,j) - flow(i,j));
						mean_change += iter_change;
										
                        max_change = std::max(max_change, iter_change);
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
		double	m_outer_sigma;
		double	m_sigma;
		double	m_alpha;
		double  m_omega;
		int		m_iterations;
		int		m_level;
};


/**
 * The nonlinear combined local global approach as described by Bruhn, Weickert, Schnörr et. al 2004
 */
class OpticalFlowCLGNonlinearFunctor
{
	public:
        /** The single value type of a flow field **/
        typedef float ValueType;
        /** The flow vector type. 2 elements: u,v **/
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
    
        /**
         * Constructor for linear combined local global approach as described by Bruhn, Weickert, Schnörr et. al 2004.
         *
         * \param alpha The alpha weight between gradient and smoothness.
         * \param sigma The sigma of the Gaussian used to compute the spatio-temporal gradients.
         * \param outer_sigma The sigma of the Gaussian used to apply the smoothing step for the Structure Tensor.
         * \param omega Linear penalizer weight. Defaults to 1.0.
         * \param iterations The count of iterations.
         */
		OpticalFlowCLGNonlinearFunctor(double alpha=1.0, double sigma=1.0, double outer_sigma=3.0, double omega=1.0, int iterations=100)
			:	m_outer_sigma(outer_sigma),
				m_sigma(sigma),
				m_alpha(alpha),
				m_omega(omega),
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
         * \return Always "Nonlinear Combined local global OFCE".
         */
		static std::string name()
		{
			return "Nonlinear Combined local global OFCE";
		}
	
        /**
         * Returns the short (abbrv.) name of the functor.
         *
         * \return Always "NL CLG OFCE".
         */
		static std::string shortName()
		{
			return "NL CLG OFCE";
		}
		
		/**
         * Optical Flow calculation according to Bruhn, Weickert, Schnörr et. al 2004.
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
            using namespace ::vigra::multi_math;
			
			vigra::MultiArray<2, ValueType> stxx(src1.shape()), stxy(src1.shape()), styy(src1.shape()),
                                    gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()), temp(src1.shape());
            
            vigra::MultiArray<2,FlowValueType> last_flow(src1.shape());
			
            temp = (src1+src2)/2;
            
			// calculate Structure Tensor at inner scale = sigma and outer scale = sigma2
			vigra::structureTensor(temp, stxx, stxy, styy, m_sigma, m_outer_sigma);
			
			
			//Calculate spatio temporal gradients for the vector "b"
			
			//1. step: calculate spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
			
			//2. step:	set I_x =>  b_x = smooth<I_x * I_t>
			//			    I_y =>  b_y = smooth<I_y * I_t>
			gradX = gradT*gradX;
			gradY = gradT*gradY;
			vigra::gaussianSmoothing(gradY, gradX,	m_outer_sigma);
			vigra::gaussianSmoothing(gradY, gradY,	m_outer_sigma);
			
			int max_iter = m_iterations;
			double	omega = m_omega,
					iter_change=0,
					mean_change=0,
					max_change=0;
			
			for(int iteration=0; iteration<max_iter; iteration++)
			{
				//save last results
				last_flow = flow;
				
				mean_change=0;
				max_change=0;
				
				for (int j=1;j<src1.height()-1; ++j)
				{
					for (int i=1;i<src1.width()-1; ++i)
					{
						//Some abbrev. for convenience
						double	p2i_minus_u = (pen(2,     flow(i-1,j)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
								p2j_minus_u = (pen(2,     flow(i,j-1)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
								p2i_plus_u  = (pen(2,last_flow(i+1,j)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
								p2j_plus_u  = (pen(2,last_flow(i,j+1)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
								p1_u = pen(1,last_flow(i,j)[0]),
							
								p2i_minus_v = (pen(2,     flow(i-1,j)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
								p2j_minus_v = (pen(2,     flow(i,j-1)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
								p2i_plus_v  = (pen(2,last_flow(i+1,j)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
								p2j_plus_v  = (pen(2,last_flow(i,j+1)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
								p1_v = pen(1,last_flow(i,j)[1]);
						
						//With SOR (successive over-relaxation)
						flow(i,j)[0] = (1.0-omega)*last_flow(i,j)[0]
									  +	omega*(	/*** SUM over N- an penalize ***/
													(p2i_minus_u * flow(i-1,j)[0])
											   + 	(p2j_minus_u * flow(i,j-1)[0])
											   /*** SUM over N+ an penalize ***/
											   +
													(p2i_plus_u * last_flow(i+1,j)[0])
											   +	(p2j_plus_u * last_flow(i,j+1)[0])
			
											   /*** Structure tensor term ***/
											   -	p1_u*(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[1] + gradX(i,j)))
									  /	
									  /*** SUM over all penalizer values for normalisation **/
									  (		p2i_minus_u + p2j_minus_u 
									   +	p2i_plus_u  + p2j_plus_u
									   +	(	p1_u
											 *	(1.0/m_alpha)*stxx(i,j)));
						
						flow(i,j)[1] = (1.0-omega)*last_flow(i,j)[1]
									  +	omega*(	/*** SUM over N- an penalize ***/
													(p2i_minus_v * flow(i-1,j)[1])
											   + 	(p2j_minus_v * flow(i,j-1)[1])
											   /*** SUM over N+ an penalize ***/
											   +
													(p2i_plus_v * last_flow(i+1,j)[1])
											   +	(p2j_plus_v * last_flow(i,j+1)[1])
			
											   /*** Structure tensor term ***/
											   -	p1_v*(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[0] + gradY(i,j)))
									  /	
									  /*** SUM over all penalizer values for normalisation **/
									  (		p2i_minus_v + p2j_minus_v 
									   +	p2i_plus_v + p2j_plus_v
									   +	(	p1_v
											 *	(1.0/m_alpha)*styy(i,j)));
						
						iter_change = vigra::norm(last_flow(i,j) - flow(i,j));
						mean_change+= iter_change;
						max_change  = max(max_change, iter_change);
					}
				}
				mean_change = mean_change /src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}
	
		/**
         * Optical Flow calculation according to Bruhn, Weickert, Schnörr et. al 2004.
		 *
         * \param[in] src1 First image of the series.
         * \param[in] src2 Second image of the series.
         * \param[in] mask The mask, where to estimate the flow.
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
            using namespace ::vigra::multi_math;
			
			vigra::MultiArray<2, ValueType> stxx(src1.shape()), stxy(src1.shape()), styy(src1.shape()),
                                    gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()), temp(src1.shape());
            
			vigra::MultiArray<2, FlowValueType> last_flow(src1.shape());
            
            temp = (src1+src2)/2;
            
			// calculate Structure Tensor at inner scale = sigma and outer scale = sigma2
			structureTensorWithMask(temp, mask, stxx, stxy, styy, m_sigma, m_outer_sigma);
			
			
			//Calculate spatio temporal gradients for the vector "b"
			
			//1. step: calculate spatiotemporal Gradients of first order: I_x, I_y and I_t
			spatioTemporalGradientWithMask(src1, src2, mask, gradX, gradY, gradT, m_sigma);
			
			//2. step:	set I_x =>  b_x = smooth<I_x * I_t>
			//			    I_y =>  b_y = smooth<I_y * I_t>
			gradX = gradT*gradX;
			gradY = gradT*gradY;
			gaussianSmoothingWithMask(gradY, mask, gradX,	m_outer_sigma);
			gaussianSmoothingWithMask(gradY, mask, gradY,	m_outer_sigma);
			
			int max_iter = m_iterations;
			double	omega = m_omega,
					iter_change=0,
					mean_change=0,
					max_change=0;
			
			for(int iteration=0; iteration<max_iter; iteration++)
			{
				//save last results
				last_flow = flow;
				
				mean_change=0;
				max_change=0;
				
				for (int j=1;j<src1.height()-1; ++j)
				{
					for (int i=1;i<src1.width()-1; ++i)
					{
                        if(mask(i,j) != 0)
                        {
                            //Some abbrev. for convenience
                            double	p2i_minus_u = (pen(2,     flow(i-1,j)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
                                    p2j_minus_u = (pen(2,     flow(i,j-1)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
                                    p2i_plus_u  = (pen(2,last_flow(i+1,j)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
                                    p2j_plus_u  = (pen(2,last_flow(i,j+1)[0]) + pen(2,last_flow(i,j)[0]))/2.0,
                                    p1_u = pen(1,last_flow(i,j)[0]),
                                
                                    p2i_minus_v = (pen(2,     flow(i-1,j)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
                                    p2j_minus_v = (pen(2,     flow(i,j-1)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
                                    p2i_plus_v  = (pen(2,last_flow(i+1,j)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
                                    p2j_plus_v  = (pen(2,last_flow(i,j+1)[1]) + pen(2,last_flow(i,j)[1]))/2.0,
                                    p1_v = pen(1,last_flow(i,j)[1]);
                            
                            //With SOR (successive over-relaxation)
                            flow(i,j)[0] = (1.0-omega)*last_flow(i,j)[0]
                                          +	omega*(	/*** SUM over N- an penalize ***/
                                                        (p2i_minus_u * flow(i-1,j)[0])
                                                   + 	(p2j_minus_u * flow(i,j-1)[0])
                                                   /*** SUM over N+ an penalize ***/
                                                   +
                                                        (p2i_plus_u * last_flow(i+1,j)[0])
                                                   +	(p2j_plus_u * last_flow(i,j+1)[0])
                
                                                   /*** Structure tensor term ***/
                                                   -	p1_u*(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[1] + gradX(i,j)))
                                          /	
                                          /*** SUM over all penalizer values for normalisation **/
                                          (		p2i_minus_u + p2j_minus_u 
                                           +	p2i_plus_u  + p2j_plus_u
                                           +	(	p1_u
                                                 *	(1.0/m_alpha)*stxx(i,j)));
                            
                            flow(i,j)[1] = (1.0-omega)*last_flow(i,j)[1]
                                          +	omega*(	/*** SUM over N- an penalize ***/
                                                        (p2i_minus_v * flow(i-1,j)[1])
                                                   + 	(p2j_minus_v * flow(i,j-1)[1])
                                                   /*** SUM over N+ an penalize ***/
                                                   +
                                                        (p2i_plus_v * last_flow(i+1,j)[1])
                                                   +	(p2j_plus_v * last_flow(i,j+1)[1])
                
                                                   /*** Structure tensor term ***/
                                                   -	p1_v*(1.0/m_alpha)*(stxy(i,j)*last_flow(i,j)[0] + gradY(i,j)))
                                          /	
                                          /*** SUM over all penalizer values for normalisation **/
                                          (		p2i_minus_v + p2j_minus_v 
                                           +	p2i_plus_v + p2j_plus_v
                                           +	(	p1_v
                                                 *	(1.0/m_alpha)*styy(i,j)));
                            
                            iter_change = vigra::norm(last_flow(i,j) - flow(i,j));
                            mean_change+= iter_change;
                            max_change  = max(max_change, iter_change);
                        }
                    }
				}
				mean_change = mean_change /src1.size();
				//qDebug() << iteration << ":\t mean change this iteration: " << mean_change << "\n";
				//qDebug() << iteration << ":\t max change this iteration: " << max_change << "\n\n";
				//if(mean_change < 0.001/(m_alpha*m_alpha*m_alpha) || max_change < 0.001/(m_alpha*m_alpha)) break;
			}
		}

	protected:
        /**
         * Non-linear penalizer function as described by Bruhn, Weickert, Schnörr et. al 2004.
         *
         * \param i Setting of the beta-function (either 0 or 1).
         * \param s Value to be penalized.
         * \return The penalized value.
         */
		inline double pen(const int i, const double s) const
		{
			double beta[] ={1,1};
            return 1.0/ sqrt(1+std::abs(s)/(beta[i-1]*beta[i-1]));
		}
		

	private:
		double	m_outer_sigma;
		double	m_sigma;
		double	m_alpha;
		double  m_omega;
		int		m_iterations;
		int		m_level;
};

/**
 * @}
 */
    
} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOW_HYBRID_HXX
