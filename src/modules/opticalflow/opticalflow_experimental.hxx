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

/************************************************************************/
/************************************************************************/
/*****                                                              *****/
/*****   BEGIN OF EXPERIMENTAL (CURRENTLY NON-WORKING) ALGORITHMS   *****/
/*****                                                              *****/
/************************************************************************/
/************************************************************************/

#ifndef GRAIPE_OPTICALFLOW_OPTICALFLOW_EXPERIMENTAL_HXX
#define GRAIPE_OPTICALFLOW_OPTICALFLOW_EXPERIMENTAL_HXX

//debug output
#include <QtDebug>


//linear solving and eigenvector analysis
#include "vigra/linear_solve.hxx"
#include "vigra/eigensystem.hxx"

//OFCE Spatiotemporal Gradients
#include "opticalflowgradients.hxx"

//image representation
#include "vigra/stdimage.hxx"

//separable filters
#include "vigra/convolution.hxx"
#include "vigra/stdconvolution.hxx"

//functor processing
#include "vigra/combineimages.hxx"
#include <functional>					// for plus, muliplies & minus functors
#include "vigra/functorexpression.hxx" //for lambda-like functors

//tensor stuff
#include "vigra/boundarytensor.hxx"
#include "vigra/tensorutilities.hxx"
#include "vigra/orientedtensorfilters.hxx" 
#include "vigra/gradient_energy_tensor.hxx" 



namespace graipe {

/**
 * 
 * The new structure tensor approach which implement weighted sums using the gradient energy
 * tensor facilities of the vigra.
 */
class OpticalFlowGETFunctor
{
    public:
    
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
        OpticalFlowGETFunctor(double sigma=2.0, double outer_sigma=1.0, double threshold=1.0)
        :	m_outer_sigma(outer_sigma),
            m_sigma(sigma),
            m_threshold(threshold),
            m_level(0.0)
        { }
        
        void setLevel(int level)
        {
            m_level=level;
            //we assume the same m_sigma for all levels, thus nothing is done here!
        };
        
        static std::string name()
        {
            return "Gradient Energy Tensor filtered Structure Tensor OFCE";
        };
        static std::string shortName()
        {
            return "GET ST OFCE";
        };
        
        //The optical flow calculation according to the gradient energy filtered structure tensor
        template <class T1, class T2>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {	
            qInfo("Attention: GET ST OFCE is experimental and does not any mask if given.");
            
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
		
            vigra::Kernel1D<double> grad, smooth;
            grad.initGaussianDerivative(m_sigma, 1);
            smooth.initGaussian(m_outer_sigma);
		
            vigra::MultiArray<2, ValueType> gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()),
                                     temp(src1.shape());
		
		   
            using namespace vigra::multi_math;
            temp = src1 + src2;
            temp *= 0.5;
            
            vigra::MultiArray<2, vigra::TinyVector<float,3> > get(src1.shape());
            
            // calculate Structure Tensor at inner scale = sigma and outer scale = outer_sigma
            gradientEnergyTensor(temp, get, grad, smooth);
		
            //Spatiotemporal Gradients of first order: I_x, I_y and I_t
            spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
            
            //Save I_x =>  b_x = smooth<I_x * I_t>
            //     I_y =>  b_y = smooth<I_y * I_t>
            gradX = gradT * gradX;
            gradY = gradT * gradY;
            vigra::gaussianSmoothing(gradX, gradX, m_outer_sigma);
            vigra::gaussianSmoothing(gradY, gradY, m_outer_sigma);
            
            //create computation matrices
            vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
		
            for (int j=0; j<src1.height(); ++j)
            {
                for (int i=0; i<src1.width(); ++i)
                {
                    //Fill matrix a and vector b
                    A(0,0) = get(i,j)[0]; A(0,1)  = get(i,j)[1];
                    A(1,0) = get(i,j)[1]; A(1,1)  = get(i,j)[2];
                    
                    b(0,0)  = -gradX(i,j); 
                    b(1,0)  = -gradY(i,j); 
                    
                    //solve the linear system of equations
                    if(vigra::linearSolve( 	A, b, res) )
                    {
                        //threshold vectors using the smallest of both eigenvalue
                        vigra::symmetricEigensystem(A,ew,ev);
                        if(ew(1,0)> m_threshold)
                        {
                            flow(i,j)[0] = res(0,0);
                            flow(i,j)[1] = res(1,0);
                        }
                        else
                        {
                            flow(i,j)[0] = 0.0;
                            flow(i,j)[1] = 0.0;
                        }
                    }
                    else 
                    {
                        flow(i,j)[0] = 0.0;
                        flow(i,j)[1] = 0.0;
                    }
                }
            }
        }
    
        template <	class T1, class T2, class T3>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            qCritical("Error: Masked approach is not implemented for GET filtered Structure Tensor OFCE!");
        }
    
    private:
        int		m_outer_sigma;
        double	m_sigma;
        double	m_threshold;
        int		m_level;
};


/**
 * The hourglass optical flow calculation. 
 * This could be a nonlinar anisotropic optical flow filter...
 */
class OpticalFlowHGFunctor
{
    public:
    
        typedef float ValueType;
        typedef vigra::TinyVector<ValueType,2> FlowValueType;
    
        OpticalFlowHGFunctor(double sigma=2.0, double outer_sigma=1.0, double threshold=1.0)
        :	m_outer_sigma(outer_sigma),
            m_sigma(sigma),
            m_threshold(threshold),
            m_level(0.0)
        {
        }
        
        void setLevel(int level)
        {
            m_level=level;
            //we assume the same m_sigma for all levels, thus nothing is done here!
        }
        
        static std::string name()
        {
            return "Hourglass filtered Structure Tensor OFCE";
        }
        
        static std::string shortName()
        {
            return "HG ST OFCE";
        }
        
        //The optical flow calculation according to the hourglass filtered structure tensor
        template <	class T1, class T2>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {	
            qInfo("Attention: HG ST OFCE is experimental and does not any mask if given.");
            
            vigra_precondition(src1.shape() == src2.shape(), "image sizes differ!");
            vigra_precondition(src1.shape() == flow.shape(), "flow array sizes differ from image sizes!");
		
            vigra::Kernel1D<double> grad, smooth;
            grad.initGaussianDerivative(m_sigma, 1);
            smooth.initGaussian(m_outer_sigma);
		
            vigra::MultiArray<2, ValueType> gradX(src1.shape()), gradY(src1.shape()), gradT(src1.shape()),
                                            temp(src1.shape());
		
		   
            using namespace vigra::multi_math;
            temp = src1 + src2;
            temp *= 0.5;
            
            vigra::MultiArray<2, vigra::TinyVector<float, 2> >  gradient(src1.shape());
            vigra::MultiArray<2, vigra::TinyVector<float, 3> >  tensor(src1.shape()), smoothedTensor(src1.shape());
            
            gaussianGradient(temp, gradient, m_sigma);
            vectorToTensor(gradient, tensor);
            hourGlassFilter(tensor, smoothedTensor, m_outer_sigma, 0.4);

            //Spatiotemporal Gradients of first order: I_x, I_y and I_t
            spatioTemporalGradient(src1, src2, gradX, gradY, gradT, m_sigma);
            
            //Save I_x =>  b_x = smooth<I_x * I_t>
            //     I_y =>  b_y = smooth<I_y * I_t>
            gradX = gradT * gradX;
            gradY = gradT * gradY;
            vigra::gaussianSmoothing(gradX, gradX, m_outer_sigma);
            vigra::gaussianSmoothing(gradY, gradY, m_outer_sigma);
            
            //create computation matrices
            vigra::Matrix<double> A(2,2), b(2,1), res(2,1), ev(2,2), ew(2,1);
		
            for (int j=0; j<src1.height(); ++j)
            {
                for (int i=0; i<src1.width(); ++i)
                {
                    //Fill matrix a and vector b
                    A(0,0) = smoothedTensor(i,j)[0]; A(0,1)  = smoothedTensor(i,j)[1];
                    A(1,0) = smoothedTensor(i,j)[1]; A(1,1)  = smoothedTensor(i,j)[2];
                    
                    b(0,0)  = -gradX(i,j); 
                    b(1,0)  = -gradY(i,j); 
                    
                    //solve the linear system of equations
                    if(vigra::linearSolve( 	A, b, res) )
                    {
                        //threshold vectors using the smallest of both eigenvalue
                        vigra::symmetricEigensystem(A,ew,ev);
                        if(ew(1,0)> m_threshold)
                        {
                            flow(i,j)[0] = res(0,0);
                            flow(i,j)[1] = res(1,0);
                        }
                        else
                        {
                            flow(i,j)[0] = 0.0;
                            flow(i,j)[1] = 0.0;
                        }
                    }
                    else 
                    {
                        flow(i,j)[0] = 0.0;
                        flow(i,j)[1] = 0.0;
                    }
                }
            }
        }
    
        //No masked version available:
        template <	class T1, class T2, class T3>
        void operator()(const vigra::MultiArrayView<2, T1> & src1,
                        const vigra::MultiArrayView<2, T2> & src2,
                        const vigra::MultiArrayView<2, T3> & mask,
                        vigra::MultiArrayView<2, FlowValueType> flow)
        {
            qCritical("Error: Masked approach is not implemented for Hourglass filtered Structure Tensor OFCE!");
        }
    
    private:
        double	m_outer_sigma;
        double	m_sigma;
        double	m_threshold;
        int		m_level;
};

/************************************************************************/
/************************************************************************/
/*****                                                              *****/
/*****     END OF EXPERIMENTAL (CURRENTLY NON-WORKING) ALGORITHMS   *****/
/*****                                                              *****/
/************************************************************************/
/************************************************************************/
    
} //end of namespace graipe

#endif //GRAIPE_OPTICALFLOW_OPTICALFLOW_EXPERIMENTAL_HXX
