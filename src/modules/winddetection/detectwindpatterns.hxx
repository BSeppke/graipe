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

#ifndef GRAIPE_WINDDETECTION_DETECTWINDPATTERNS_HXX
#define GRAIPE_WINDDETECTION_DETECTWINDPATTERNS_HXX

//vigra components needed
#include <vigra/multi_array.hxx>
#include <vigra/multi_fft.hxx>
#include <vigra/multi_math.hxx>
#include <vigra/multi_convolution.hxx>
#include <vigra/tinyvector.hxx>

#include <vigra/linear_algebra.hxx>
#include <vigra/affinegeometry.hxx>

//GRAIPE components needed
#include "vectorfields/vectorfields.h"

namespace graipe {

/**
 * This class implements a functor for the Wind Detection from SAR images
 * using the Fourier spectrum analysis.
 */
class FourierSpectrumWindDetectionFunctor
{
    public:
        /**
         * Constructor for the Fourier analysis based SAR wind estimation.
         *
         * \param smoothing Smoothing sigma which will be applied to the fourier space.
         * \param threshold Spectral magnitude intensity threshold.
         * \param radius The radius of the high-passed filter, to eliminate low-frequency responses.
         */
        FourierSpectrumWindDetectionFunctor(float smoothing, float threshold, float radius)
        :	m_smoothing(smoothing), 
            m_threshold(threshold),
            m_radius(radius)
        {
        }
        
        /**
         * Functor call: Computes one direction for one image patch.
         *
         * \param src The image patch, should be a part/window of a SAR image.
         * \param angle The angle of the derived direction.
         *              The angle is returned in degrees, with respect to the clock:
         *                 (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * \param quality The quality of the angle measurement: The (moment-based)
         *                ratio of both eigenvalues: 1 + la1/la2.
         */
        template <class T>
        void operator()(const vigra::MultiArrayView<2,T> & src, float & angle, float & quality)
        {
            // compute Fourier transform
            vigra::MultiArray<2, vigra::FFTWComplex<T> > fourier(src.shape());
            
            vigra::fourierTransform(src, fourier);
            vigra::moveDCToCenter(fourier);
            
            vigra::MultiArray<2, float> temp = vigra::multi_math::norm(fourier);
            
            if(m_smoothing > 0.5)
            {
                vigra::gaussianSmoothMultiArray(temp, temp, m_smoothing);
            }
            if(m_radius > 0.0)
            {
                deleteCenter(temp);
            }
            
            // init functor
            vigra::FindMinMax<float> minmax;
            
            vigra::inspectImage(temp, minmax);
            
            qDebug() << "Min: " << minmax.min << " Max: " << minmax.max;
            
            for(auto & t: temp)
            {
                if (t > minmax.max*m_threshold)
                    t = 255;
                else
                    t = 0;
            }
            
            angle = angleFromImage(temp, quality);
        }		
        
    private:
        /**
         * This function performes an ideal high-pass filter by removin
         * all low frequancies from a given fourier space.
         * The radius of the low-frequencies is given by m_radius. The center (DC)
         * frequency is assumed to be in the middle of the fourier image.
         *
         * \param src The image will will be high-passed filtered and thus manipulated, too.
         */
        template <class T>
        void deleteCenter(vigra::MultiArrayView<2,T> src) const
        {
            vigra::Shape2 image_shape = src.shape();
            
            const unsigned int 	i_w=(unsigned int)image_shape[0],
                                i_h=(unsigned int)image_shape[1],
                                i_cw = (i_w+0.5)/2,
                                i_ch = (i_h+0.5)/2,
                                rad = m_radius*std::min(i_h/2,i_w/2);
            
            if(i_cw>rad && i_ch > rad)
            {
                for(unsigned int y=i_ch-rad; y<i_ch+rad;y++)
                {
                    for(unsigned int x=i_cw-rad; x<i_cw+rad; x++)
                    {
                        if((x-i_cw)*(x-i_cw) + (y-i_ch)*(y-i_ch) <= rad*rad)
                            src(x,y) = 0;
                    }
                }
            }
        }
    
        /**
         * This function uses first and second order moments to derive the angle 
         * of the wind direction vector in fourier space.
         *
         * \param src The input fourier image, for which we compute the angle.
         * \param quality Here we store the quality of the results by means of the 
         *                ratio of both eigenvalues: 1 + la1/la2.
         * \return The angle of the derived direction.
         *         The angle is returned in degrees, with respect to the clock:
         *                 (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         */
        template <class T>
        float angleFromImage(const vigra::MultiArrayView<2,T> & src, float & quality)
        {
            vigra::Shape2 image_shape = src.shape();
            
            const unsigned int 	i_w=(unsigned int)image_shape[0],
                                i_h=(unsigned int)image_shape[1],
                                i_cw = (i_w+0.5)/2,
                                i_ch = (i_h+0.5)/2;
            
            double my00 = 0, /*my01=0, my10=0,*/ my11=0, my02=0, my20=0;
            
            for(unsigned int y=0; y<i_h;y++)
            {
                for(unsigned int x=0; x<i_w;x++)
                {
                    if(src(x,y) != 0)
                    {
                        my00++;
                        //my10 += x-cw;
                        //my01 += y-ch;
                        my11 += (x-i_cw)*(y-i_ch);
                        my20 += (x-i_cw)*(x-i_cw);
                        my02 += (y-i_ch)*(y-i_ch);
                    }
                }
            }
            
            //normalize mys
            my11= my11/my00;
            my02= my02/my00;
            my20= my20/my00;
            
            float	la1 = 0.5*(my20+my02) + sqrt( (4.0*my11*my11 + (my20-my02)*(my20-my02))*0.5),
                    la2 = 0.5*(my20+my02) - sqrt( (4.0*my11*my11 + (my20-my02)*(my20-my02))*0.5);
            
            //quality by means of quotient of both axis lengths
            quality = 1+ la2/la1;
            
            float angle  = fmod(2*M_PI + atan2(2.0*(my11), (my20-my02)), 2*M_PI);
    
            return angle/M_PI*180;
        }
        
        float m_smoothing;
        float m_threshold;
        float m_radius;
};




/**
 * This class implements a functor for the Wind Detection from SAR images
 * using the Gradient Histogram analysis.
 */
class GradientHistogramWindDetectionFunctor
{
    public:
        /**
         * Constructor for the Gradient Histogram based SAR wind estimation.
         *
         * \param scale The scale of the gaussian gradient, which will be used. Defaults to 1.
         * \param threshold Gradient length threshold to be used for histogram creation. Defaults to 0.
         * \param smoothing Smoothing sigma for the histogram's values. Defaults to 12.
         * \param bins Count of bins of the histogram. Defaults to 360.
         */
        GradientHistogramWindDetectionFunctor(float scale=1.0, float threshold=0.0, float smoothing=12.0, int bins = 360)
        :	m_scale(scale), 
            m_threshold(threshold), 
            m_smoothing(smoothing), 
            m_bins(bins)
        {
        }
    
        /**
         * Functor call: Computes one direction for one image patch.
         *
         * \param src The image patch, should be a part/window of a SAR image.
         * \param angle The angle of the derived direction.
         *              The angle is returned in degrees, with respect to the clock:
         *                 (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * \param quality The quality of the angle measurement: (max_value-min_value)/min_value
         */
        template <class T>
        void operator()(const vigra::MultiArrayView<2,T> & src, float & angle, float & quality )
        {
            qDebug("GradientHistogramWindDetectionFunctor: Creating Gradient Histogram.");
            vigra::MultiArray<1,float> hist(m_bins);
            hist = 0;
            createGradientHistogram(src, hist);
            
            qDebug("GradientHistogramWindDetectionFunctor: Eventually smoothing Gradient Histogram.");
            
            if (m_smoothing>0.5)
            {
              gaussianSmoothMultiArray(hist, hist, m_smoothing);
            }
            unsigned int min_idx, max_idx;
            float min_value,max_value; 
            
            qDebug("GradientHistogramWindDetectionFunctor: Finding min and max (and idxs) of  Gradient Histogram.");
            vector2minmax(hist, min_value, min_idx, max_value, max_idx);
            
            angle  = 180.0*max_idx/m_bins;
            quality = (max_value-min_value)/min_value;
            qDebug("GradientHistogramWindDetectionFunctor:  Min value: %f @idx: %d", min_value, min_idx);
            qDebug("GradientHistogramWindDetectionFunctor:  Max value: %f @idx: %d", max_value, max_idx);
            qDebug("GradientHistogramWindDetectionFunctor:  ---> %f degrees, quality: %f", 180.0*max_idx/m_bins, (max_value-min_value)/min_value);
        }
        
    private:
        /**
         * Find the extrema and the position of the extrema in a 1D MultiArrayView.
         *
         * \param vec The 1D MultiArray, which we want to analyze.
         * \param min_value The minimal value.
         * \param min_idx   Index of the minimal value.
         * \param max_value The maximal value.
         * \param max_idx   Index of the maximal value.
         */
        template <class T>
        void vector2minmax(const vigra::MultiArrayView<1,T> & vec, float & min_value, unsigned int & min_idx, float & max_value, unsigned int & max_idx) const
        {
            unsigned int bins = (unsigned int)vec.shape()[0];
            
            max_value=vec(0);
            max_idx=0;
            min_value=vec(0);
            min_idx=0;
            
            for(unsigned int i=0; i<bins; ++i)
            {
                T val = vec(i);
                
                if(max_value<val)
                {
                    max_value= val;
                    max_idx = i;
                }
                else if(min_value>val)
                {
                    min_value= val;
                    min_idx = i;
                }
            }
            
        }
    
        /**
         * Creates a gradient histogram for a given MultiArrayView and stores the result
         * in a 2D MultiArrayView. The gradient will be estimated at gaussian scale m_scale.
         * The angle is sampled into m_bins bins. Only gradients with a length above m_length
         * will be counted.
         *
         * \param src The input image, for which we compute the gradient histogram.
         * \param hist The angular binned gradient histogram.
         */
        template <class T1, class T2>
        void createGradientHistogram(const vigra::MultiArrayView<2,T1> & src, vigra::MultiArrayView<1,T2> hist) const
        {
            
            unsigned int w   = (unsigned int)src.width(),
                         h   = (unsigned int)src.height(),
                        bins = (unsigned int)hist.width();
            
            vigra::MultiArray<2, vigra::TinyVector<float, 2> > grad(w,h);
            
            // calculate gradient vector at given scale
            vigra::gaussianGradientMultiArray(src, grad, m_scale);
            
            for(const vigra::TinyVector<float, 2>& g : grad)
            {
               float length = norm(g);
                
                if (length > m_threshold)
                {
                    //Compute angle
                    float angle  = fmod(2*M_PI + atan2(g[1], g[0]), 2*M_PI);
                    
                    //Compute index for histogram:
                    if(angle == 2*M_PI)
                        angle = 0;
                
                    unsigned int idx = angle/(2*M_PI) * bins;
                
                    hist(idx) += length;
                }
            }
        }
        
        float m_scale;
        float m_threshold;
        float m_smoothing;
        int m_bins;
};




/**
 * Templated wrapper for the Fourier and Gradient analysis SAR Wind detection
 * algorithms, which both result in sparse weighted vectorfields.
 *
 * \param src The SAR image, for which we want to detect the wind.
 * \param func The wind detection functor. 
 * \param x_res Point sampling in x-direction.
 * \param y_res Point sampling in y-direction.
 * \param mask_width Width of the local analysis window.
 * \param mask_height Height of the local analysis window.
 * \param wind_knowledge Prior knowledge of the half space of the wind.
 * \return A sparse weighted vectorfield containing the wind directions, without speed.
 */
template <class T, class WindDetectionFunctor>
SparseWeightedVectorfield2D* estimateWindDirectionFromSARImage(const vigra::MultiArrayView<2,T> &  src,
                                                               WindDetectionFunctor &  func,
                                                               int x_res, int y_res,  int mask_width, int mask_height,
                                                               const vigra::TinyVector<int, 2> & wind_knowledge)
{
	typedef typename Vectorfield2D::PointType  PointType;
	
	unsigned int image_width   = (unsigned int)src.width(),
                 image_height  = (unsigned int)src.height();
	
	//Create resulting vectorfield
	SparseWeightedVectorfield2D* result_vf = new SparseWeightedVectorfield2D;	
	
	unsigned int y_step = image_height/y_res,
                 x_step = image_width/x_res;
	
	qDebug() << "x_step: " << x_step << "\n";
	qDebug() << "y_step: " << y_step << "\n";
	
	for(unsigned int y=y_step/2; y < image_height-y_step/2; y+=y_step)
	{
		for(unsigned int x=x_step/2; x < image_width-x_step/2; x+=x_step)
		{
			
			unsigned int ul_x = std::max((unsigned int)0, (unsigned int)(x-mask_width/2)),
                         ul_y = std::max((unsigned int)0, (unsigned int)(y-mask_height/2)),
                         lr_x = std::min(image_width-1,  x+mask_width/2),
                         lr_y = std::min(image_height-1, y+mask_height/2);
			
			if(lr_x - ul_x ==  mask_width && lr_y - ul_y == mask_height )
			{
				//qDebug() << "calling functor for region: (" << ul_x << ", " << ul_y <<") -> (" << lr_x << ", " << lr_y << ")\n";
				float angle, quality;
				
				func( src.subarray(vigra::Shape2(ul_x, ul_y), vigra::Shape2(lr_x, lr_y)), angle, quality);
				//qDebug() << "... resulting angle:" << angle <<" @quality: " << quality << "\n";
				
				float dir_x = cos(angle/180.0*M_PI), dir_y = sin(angle/180.0*M_PI);
				//Flip vectors if they do not point into the right direction
				if(wind_knowledge[0]*dir_x + wind_knowledge[1]*dir_y < 0)
				{
					dir_x=-dir_x; 
					dir_y=-dir_y; 
				}
				result_vf->addVector(PointType(x,y),PointType(dir_x,dir_y),std::abs(quality));
			}
		}
	}
	return result_vf;
}




/**
 * Structure Tensor SAR Wind detection algorithm, which results in a dense vectorfield.
 *
 * \param src The SAR image, for which we want to detect the wind.
 * \param inner_scale The inner scale of the Structure Tensor.
 * \param outer_scale The outer scale of the Structure Tensor.
 * \param wind_knowledge Prior knowledge of the half space of the wind.
 * \return A dense vectorfield containing the wind directions, without speed.
 */
template <class T>
DenseVectorfield2D* estimateWindDirectionFromSARImageUsingStructureTensor(const vigra::MultiArrayView<2,T> & src,
																		  float inner_scale, float outer_scale,
																		  const vigra::TinyVector<int, 2> & wind_knowledge)
{
	//Create resulting vectorfield
	DenseVectorfield2D* result_vf = new DenseVectorfield2D(src.shape());
	
	vigra::MultiArray<2, vigra::TinyVector<float, 3> > st(src.shape());
	
	// calculate Structure Tensor at inner scale and outer scale
	vigra::structureTensorMultiArray(src, st, inner_scale, outer_scale);
		
	double u = 0, v=0;
	
	for(int y=0; y < src.height(); y++)
    {
		for(int x=0; x < src.width(); x++)
        {
			
			//float	la1 = 0.5*(stxx(x,y)+styy(x,y)) + sqrt( (4.0*stxy(x,y)*stxy(x,y) + (stxx(x,y)-styy(x,y))*(stxx(x,y)-styy(x,y)))*0.5),
			//		la2 = 0.5*(stxx(x,y)+styy(x,y)) - sqrt( (4.0*stxy(x,y)*stxy(x,y) + (stxx(x,y)-styy(x,y))*(stxx(x,y)-styy(x,y)))*0.5);
			//quality by means of quotient of both axis lengths
			//quality = 1+ la2/la1;
			
			u = (st(x,y)[0]-st(x,y)[2]);
			v = 2.0*(st(x,y)[1]);
			
			//Flip vectors if they do not point into the right direction
			if(wind_knowledge[0]*u + wind_knowledge[1]*v < 0){
				u=-u; 
				v=-v; 
			}
            result_vf->setDirection(x,y, DenseVectorfield2D::PointType(u, v));
			
		}
	}
	return result_vf;
}

} //end of namespace graipe

#endif //GRAIPE_WINDDETECTION_DETECTWINDPATTERNS_HXX
