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

#ifndef GRAIPE_MULTISPECTRAL_MULTISPECTRALGRADIENTS_HXX
#define GRAIPE_MULTISPECTRAL_MULTISPECTRALGRADIENTS_HXX

//GRAIPE Feature and Image Types
#include "features/features.h"
#include "images/images.h"

//vigra components needed
#include "vigra/edgedetection.hxx"

//Compute Multispectral Gradients

namespace graipe {
    
/**
 * A helper function to compute the jacobian matrix of an image.
 * The Jacobi matrix holds the first order partial derivatives for each
 * channel and may further be used for the multispectral gradient methods.
 *
 * \param[in] img The input image.
 * \param[out] jacobian The jacobian matrix given as a vector of partial derivative images.
 * \param[in] scale The gaussian scale for first derivative estimation.
 */
template <class T1, class T2>
void imageToJacobian(const Image<T1>* img, std::vector<vigra::MultiArray<2,vigra::TinyVector<T2, 2> > > & jacobian, float scale)
{
	jacobian.clear();
	jacobian.resize(img->numBands(), vigra::MultiArray<2,vigra::TinyVector<float, 2> >(img->size()));
    
	for (unsigned int c = 0; c < img->numBands(); ++c) 
	{
		vigra::gaussianGradient(img->band(c), jacobian[c], scale);
	}
}


/**
 * This class is implementing a functor for the 
 * computation of the Mean Multispectral Gradient
 * given a jacobian matrix of the partial derivatives of each bands.
 */
class MSMeanGradientFunctor
{
	public:
        /**
         * Call of the MSMeanGradientFunctor. Computes the multispectral
         * mean gradient from a jacobian matrix.
         *
         * \param[in] jacobian The jacobian matrix as estimated e.g. by imageToJacobian.
         * \param[out] result The resulting single gradient.
         */
		void operator()(const std::vector<vigra::MultiArray<2,vigra::TinyVector<float, 2> > > & jacobian, vigra::MultiArray<2,vigra::TinyVector<float, 2> >  & result)
		{
            vigra_precondition(jacobian.size() != 0, "jacobian needs to have at least one channel");
			
			result = jacobian[0];
			
			for (unsigned int c = 1; c < jacobian.size(); ++c) 
			{
				vigra_precondition(jacobian[c].width() == result.width() && jacobian[c].height() == result.height(), "jacobian needs to have same dimensions at every channel");
				
				result += jacobian[c];
			}
            
            result.bindElementChannel(0) /= jacobian.size();
            result.bindElementChannel(1) /= jacobian.size();
		}
    
		/**
         * Returns the full name of the functor.
         *
         * \return Always "Mean multispectral gradient".
         */
		static QString name()
		{
			return "Mean multispectral gradient";
		}
    
		/**
         * Returns the short name of the functor.
         *
         * \return Always "MS mean gradient".
         */
		static QString shortName()
		{
			return "MS mean gradient";
		}
};


/**
 * This class is implementing a functor for the 
 * computation of the Maximum Multispectral Gradient 
 * given a jacobian matrix of the partial derivatives of each bands.
 */
class MSMaxGradientFunctor
{
	public:
        /**
         * Call of the MSMaxGradientFunctor. Computes the multispectral
         * max. gradient from a jacobian matrix.
         *
         * \param[in] jacobian The jacobian matrix as estimated e.g. by imageToJacobian.
         * \param[out] result The resulting single gradient.
         */
		void operator()(const std::vector<vigra::MultiArray<2,vigra::TinyVector<float, 2> > > & jacobian, vigra::MultiArray<2,vigra::TinyVector<float, 2> >  & result)
		{
			vigra_precondition(jacobian.size() != 0, "jacobian needs to have at least one channel");
			
			result = jacobian[0];
			
			unsigned int	w = (unsigned int)result.width(),
							h = (unsigned int)result.height();
			
			for (unsigned int c = 1; c < jacobian.size(); ++c) 
			{
				vigra_precondition(jacobian[c].width() == result.width() && jacobian[c].height() == result.height(), "jacobian needs to have same dimensions at every channel");
				
				for (unsigned int y = 0; y < h; ++y) 
				{
					for (unsigned int x = 0; x < w; ++x) 
					{
						if( norm(jacobian[c](x,y)) > norm(result(x,y)))
							result(x,y) = jacobian[c](x,y);
					}
				}
			}
		}

		/**
         * Returns the full name of the functor.
         *
         * \return Always "Max. multispectral gradient".
         */
		static QString name()
		{
			return "Max. multispectral gradient";
		}
		
		/**
         * Returns the short name of the functor.
         *
         * \return Always "MS max gradient".
         */
		static QString shortName()
		{
			return "MS max gradient";
		}
};


/**
 * This class is implementing a functor for the 
 * computation of the Multi-vector Multispectral Gradient
 * given a jacobian matrix of the partial derivatives of each bands.
 */
class MSMVGradientFunctor
{
	public:
        /**
         * Call of the MSMVGradientFunctor. Computes the multispectral
         * multi-vector gradient from a jacobian matrix.
         *
         * \param[in] jacobian The jacobian matrix as estimated e.g. by imageToJacobian.
         * \param[out] result The resulting single gradient.
         */
		void operator()(const std::vector<vigra::MultiArray<2,vigra::TinyVector<float, 2> > > & jacobian, vigra::MultiArray<2,vigra::TinyVector<float, 2> >  & result)
		{
			vigra_precondition(jacobian.size() != 0, "jacobian needs to have at least one channel");
	
			unsigned int	w = (unsigned int)jacobian[0].width(),
							h = (unsigned int)jacobian[0].height();
			
			vigra::MultiArray<2,float>a11(w,h), a12(w,h), a22(w,h), temp(w,h);
			
			using namespace vigra::multi_math;
            
			result = jacobian[0];
			
			for (unsigned int c = 1; c < jacobian.size(); ++c) 
			{
				vigra_precondition(jacobian[c].width() == result.width() && jacobian[c].height() == result.height(), "jacobian needs to have same dimensions at every channel");
								
				for (unsigned int y = 0; y < h; ++y) 
				{
					for (unsigned int x = 0; x < w; ++x) 
					{
                        //a11 = a11 + I_x_c*I_x_c
                        a11(x,y) += jacobian[c](x,y)[0] * jacobian[c](x,y)[0];
                
                        //a12 = a12 + I_x_c*I_y_c
                        a12(x,y) += jacobian[c](x,y)[0] * jacobian[c](x,y)[1];
				
                        //a22 = a22 + I_y_c*I_y_c
                        a22(x,y) += jacobian[c](x,y)[1] * jacobian[c](x,y)[1];
                    }
                }
			}
            
            for (unsigned int y = 0; y < h; ++y) 
			{
            	for (unsigned int x = 0; x < w; ++x)
                {
                    double lambda_max = 0.5*(a11(x,y)+a22(x,y) + sqrt((a11(x,y)-a22(x,y))*(a11(x,y)-a22(x,y))  +4*a12(x,y)*a12(x,y))),
                           phi_max   = atan2(lambda_max - a11(x,y), a12(x,y));
                        
                    result(x,y)[0] =   cos(phi_max) * sqrt(lambda_max/jacobian.size());
                    result(x,y)[1] =   sin(phi_max) * sqrt(lambda_max/jacobian.size());
                }
            }
            
			//voting (counted in temp)
			temp=0;
			for (unsigned int c = 0; c < jacobian.size(); ++c) 
			{
				for (unsigned int y = 0; y < h; ++y) 
				{
					for (unsigned int x = 0; x < w; ++x) 
					{
						float prod = vigra::dot(result(x,y), jacobian[c](x,y));
					
						temp(x,y) += (prod < 0)? -1 : 1;
					}
				}
			}
            
            //set result_x = signum(voting)*vec
            for (unsigned int y = 0; y < h; ++y)
            {
                for (unsigned int x = 0; x < w; ++x)
                {
                    if(temp(x,y) < 0)
                    {
                        result(x,y) *= -1;
                    }
                }
            }
		}
	
		/**
         * Returns the full name of the functor.
         *
         * \return Always "Mult-vector multispectral gradient".
         */
		static QString  name()
		{
			return "Mult-vector multispectral gradient";
		}
		
		/**
         * Returns the short name of the functor.
         *
         * \return Always "MS mv gradient".
         */
		static QString  shortName()
		{
			return "MS mv gradient";
		}
};

/**
 * This fucntion computes a feature list according to the Canny
 * Edge Detector on multispectral images.
 * The template argumenet describes the functor, wihch is then used
 * to compute the gradient before the Canny estimation.
 *
 * \param img The multispectral input image.
 * \param scale The scale for the jacobian estimation.
 * \param threshold The threshold for the edge strengths of the edgels.
 * \return The Canny Edgel list.
 */
template <class MS_GRADIENT_FUNCTOR>
EdgelFeatureList2D* msCannyFeatures(const Image<float>* img, float scale, float threshold)
{
	std::vector<vigra::MultiArray<2,vigra::TinyVector<float, 2> > > jacobian;
	vigra::MultiArray<2,vigra::TinyVector<float, 2> >  gradient;
	
	imageToJacobian(img, jacobian, scale);
	
	MS_GRADIENT_FUNCTOR func;
	func(jacobian, gradient);
	
	// empty edgel list
    std::vector<vigra::Edgel> v_edgels;
	// find edgels at scale
    vigra::cannyEdgelListThreshold(gradient, v_edgels, threshold);
	
    EdgelFeatureList2D* edgels = new EdgelFeatureList2D;
	
	for(unsigned int i=0; i< v_edgels.size(); ++i)
	{
		edgels->addFeature(EdgelFeatureList2D::PointType(v_edgels[i].x,v_edgels[i].y), v_edgels[i].strength, v_edgels[i].orientation);
	}
	return  edgels;
}
    
} //end of namespace graipe

#endif //GRAIPE_MULTISPECTRAL_MULTISPECTRALGRADIENTS_HXX
