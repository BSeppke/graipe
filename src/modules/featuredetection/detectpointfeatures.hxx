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

#ifndef GRAIPE_FEATUREDETECTION_DETECTPOINTFEATURES_HXX
#define GRAIPE_FEATUREDETECTION_DETECTPOINTFEATURES_HXX

//vigra components needed
#include <vigra/edgedetection.hxx>
#include <vigra/convolution.hxx>
#include <vigra/cornerdetection.hxx>
#include <iostream>
#include <vigra/multi_localminmax.hxx>

//GRAIPE Feature Types
#include "features2d/features2d.h"

#include <cmath>
#include <math.h>
#include <algorithm>

namespace graipe {
    
/** 
 * Feature detection using Thresholding
 * These functions will extract a list of pointfeatures from an image.
 * A featurepoint is added each time the pixel value v=src(x,y): v>=lower and v<=upper
 *
 * \param src The source image.
 * \param lower The lower treshold. The pixel value has to be at least of this value.
 * \param upper The upper treshold. The pixel value has to be at max. of this value.
 * \return A weighted featurelist, where each passing pixels is one feature, weighted with
 *         The pixels value.
 */
template <class T>
WeightedPointFeatureList2D* detectFeaturesUsingThreshold(const vigra::MultiArrayView<2,T>& src, T lower, T upper)
{
	
	WeightedPointFeatureList2D* featureList = new WeightedPointFeatureList2D;
	
	for(int y=0; y<src.height(); ++y)
	{
		for(int x=0; x<src.width(); ++x)
		{
			T val = src(x,y);
            
			if(val>=lower && val<=upper)
			{
				featureList->addFeature(WeightedPointFeatureList2D::PointType(x,y),val);
			}
		}
	}
	return featureList;
}
    
/** 
 * Feature detection using Thresholding and a mask
 * These functions will extract a list of pointfeatures from an image.
 * A featurepoint is added each time if the pixel is not masked (masx(x,y)!=0),
 * and the pixel value v=src(x,y): v>=lower and v<=upper
 *
 * \param src The source image.
 * \param mask The mask image.
 * \param lower The lower treshold. The pixel value has to be at least of this value.
 * \param upper The upper treshold. The pixel value has to be at max. of this value.
 * \return A weighted featurelist, where each passing pixels is one feature, weighted with
 *         The pixels value.
 */
template <	class T, class M>
WeightedPointFeatureList2D* detectFeaturesUsingThresholdWithMask(const vigra::MultiArrayView<2,T> & src,
                                                                 const vigra::MultiArrayView<2,M> & mask,
                                                                 T lower, T upper)
{
    vigra_precondition( src.shape() == mask.shape(), "mask and source shapes differ!");
    
	WeightedPointFeatureList2D* featureList = new WeightedPointFeatureList2D;
	
	for(int y=0; y<src.height(); ++y)
	{
		for(int x=0; x<src.width(); ++x)
		{
			T val = src(x,y);
			if(val>=lower && val<=upper && mask(x,y) != 0)
			{
				featureList->addFeature(WeightedPointFeatureList2D::PointType(x,y),val);
			}
		}
	}
	return featureList;
}




/**
 * Function to transform each image pixel to its degree of monotony. This is the 
 * foundation transform for the Monotony Operator.
 *
 * \param src The source image.
 * \param dest the destination image.
 * \param offset The offset in x- and y-direction of the neighbors. Defaults to 1.
 */
template <class T>
void monotony_operator(const vigra::MultiArrayView<2,T> & src, vigra::MultiArrayView<2,unsigned char> dest, unsigned int offset=1)
{
	for(int y=offset; y<src.height()-offset; ++y)
	{
		for(int x=offset; x<src.width()-offset; ++x)
		{
			T val = src(x,y);
            
			dest(x,y) = val > src(x-offset,y-offset)? 1 : 0;
			dest(x,y)+= val > src(x       ,y-offset)? 1 : 0;
            dest(x,y)+= val > src(x+offset,y-offset)? 1 : 0;
            dest(x,y)+= val > src(x-offset,y       )? 1 : 0;
            dest(x,y)+= val > src(x+offset,y       )? 1 : 0;
            dest(x,y)+= val > src(x-offset,y+offset)? 1 : 0;
            dest(x,y)+= val > src(x       ,y+offset)? 1 : 0;
            dest(x,y)+= val > src(x+offset,y+offset)? 1 : 0;
        }
    }
}

/** 
 * Feature detection using the Monotony Operator
 * These function will extract a list of pointfeatures from an image.
 * As a first step, the image is transformed into another image that represents
 * the degree of monotony of the image given a certain offset.
 * Afterwards, a featurepoint is added each time the monotony n: n>=lowest_level and n<=highest_level-
 *
 * \param src The source image.
 * \param lowest_level the lowest allowed level of monotony.
 * \param highest_level the highest allowed level of monotony.
 * \param offset The offset in x- and y-direction of the neighbors. Defaults to 1.
 * \return Weighted Point feature list for every Monotony feature, which passes the 
 *         above thresholds. The weight is defined as the monotony class.
 */
template <class T>
WeightedPointFeatureList2D* detectFeaturesUsingMonotonyOperator(const vigra::MultiArrayView<2,T>& src, unsigned char lowest_level, unsigned char highest_level, unsigned int monotony_offset=1)
{
	vigra::MultiArray<2,unsigned char> monotony_img(src.shape());
	monotony_operator(src, monotony_img, monotony_offset);

	return detectFeaturesUsingThreshold(monotony_img, lowest_level, highest_level);
}

/** 
 * Feature detection using the Monotony Operator and a mask
 * These function will extract a list of pointfeatures from an image.
 * As a first step, the image (under the mask) is transformed into another image that represents
 * the degree of monotony of the image given a certain offset.
 * Afterwards, a featurepoint is added each time the monotony n: n>=lowest_level and n<=highest_level-
 *
 * \param src The source image.
 * \param lowest_level the lowest allowed level of monotony.
 * \param highest_level the highest allowed level of monotony.
 * \param offset The offset in x- and y-direction of the neighbors. Defaults to 1.
 * \return Weighted Point feature list for every Monotony feature, which passes the 
 *         above thresholds. The weight is defined as the monotony class.
 */
template <	class T, class M>
WeightedPointFeatureList2D* detectFeaturesUsingMonotonyOperatorWithMask(const vigra::MultiArrayView<2,T>& src, const vigra::MultiArrayView<2,M>& mask,
                                                                        unsigned char lowest_level, unsigned char highest_level,
																		unsigned int monotony_offset=1)
{
    vigra_precondition( src.shape() == mask.shape(), "mask and source shapes differ!");
    
	vigra::MultiArray<2,unsigned char> monotony_img(src.shape());
	monotony_operator(src, monotony_img, monotony_offset);
	
	return detectFeaturesUsingThresholdWithMask(monotony_img, mask,	lowest_level, highest_level);
}




/**  
 * Feature detection using the Harris corner detector
 * This function will extract a list of 2d-weighted-features from an image.
 * The weight is given by the response of the corresponding pixels.
 *
 * \param src The source image.
 * \param scale The (gaussian sigma) scale used for the Harris operator.
 * \param threhsold The minimal response, for which a feaure will be created. Defaults to 0.
 * \return Weighted point featurelist. The list of local maxima of the Harris operator
 *         with reponses above the given threshold.
 */
template <class T>
WeightedPointFeatureList2D* detectFeaturesUsingHarris(const vigra::MultiArrayView<2,T>& src, double scale, double threshold = 0)
{
    vigra::MultiArray<2,float>cornerResponse(src.shape());
	vigra::MultiArray<2,unsigned char> filteredResponse(src.shape());
	
	// find corner response at given scale
    vigra::cornerResponseFunction(src, cornerResponse, scale);
	
    // find local maxima of corner response
    vigra::localMaxima(cornerResponse, filteredResponse);
	
	WeightedPointFeatureList2D* new_featurelist = new WeightedPointFeatureList2D;
	
    // sample grid on img1 and try to add ctrl points
    for (int y=0; y < src.height(); y++ )
	{
        for (int x=0; x < src.width(); x++ )
		{
			if (filteredResponse(x,y) != 0)
			{
                double resp = cornerResponse(x,y);
            
                if (resp > threshold)
                {
                    // add to feature list
                    new_featurelist->addFeature(WeightedPointFeatureList2D::PointType(x,y), resp);
                }
            }
		}
    }
	return new_featurelist;
}

/**  
 * Feature detection using the Harris corner detector and a mask
 * This function will extract a list of 2d-weighted-features from an image part under the mask.
 * The weight is given by the response of the corresponding pixels.
 *
 * \param src The source image.
 * \param scale The (gaussian sigma) scale used for the Harris operator.
 * \param threshold The minimal response, for which a feaure will be created. Defaults to 0.
 * \return Weighted point featurelist. The list of local maxima of the Harris operator
 *         with reponses above the given threshold and under the mask.
 */
template <class T, class M>
WeightedPointFeatureList2D* detectFeaturesUsingHarrisWithMask(const vigra::MultiArrayView<2,T>& src, const vigra::MultiArrayView<2,M>& mask, double scale, double threshold = 0)
{
    vigra_precondition( src.shape() == mask.shape(), "mask and source shapes differ!");
    
    vigra::MultiArray<2,float>cornerResponse(src.shape());
	vigra::MultiArray<2,unsigned char> filteredResponse(src.shape());
	
	// find corner response at given scale
    vigra::cornerResponseFunction(src, cornerResponse, scale);
	
    // find local maxima of corner response
    vigra::localMaxima(cornerResponse, filteredResponse);
	
	WeightedPointFeatureList2D* new_featurelist = new WeightedPointFeatureList2D;
	
    // sample grid on img1 and try to add ctrl points
    for (int y=0; y < src.height(); y++ )
	{
        for (int x=0; x < src.width(); x++ )
		{
			if (filteredResponse(x,y) != 0 && mask(x,y) != 0)
			{
                double resp = cornerResponse(x,y);
            
                if (resp > threshold)
                {
                    // add to feature list
                    new_featurelist->addFeature(WeightedPointFeatureList2D::PointType(x,y), resp);
                }
            }
		}
    }
	return new_featurelist;
}





/** 
 * Feature detection using the Canny Edge operator.
 * This functions will extract a list of 2d-edgel-features from an image.
 *
 * \param src The source image.
 * \param scale The (gaussian sigma) scale used for the Canny operator.
 * \param threshold The minimal response, for which a feaure will be created. Defaults to 0.
 * \return Edgel point featurelist. The list of all found canny edgels at given scale over the threshold.
 */
template <class T>
EdgelFeatureList2D* detectFeaturesUsingCanny(const vigra::MultiArrayView<2,T>& src, double scale, double threshold=0.0)
{
	// empty edgel list
    std::vector<vigra::Edgel> v_edgels;
    
	// find edgels at scale
    vigra::cannyEdgelListThreshold(src, v_edgels, scale, threshold);
	
    EdgelFeatureList2D* edgels = new EdgelFeatureList2D;
	
	for(auto e : v_edgels)
	{
		edgels->addFeature(EdgelFeatureList2D::PointType(e.x,e.y), e.strength, fmod(2*M_PI + e.orientation, 2*M_PI));
	}
    
	return  edgels;
}




/** 
 * Feature detection using the Canny Edge operator and a mask.
 * This functions will extract a list of 2d-edgel-features from an image under the mask.
 *
 * \param src The source image.
 * \param scale The (gaussian sigma) scale used for the Canny operator.
 * \param threshold The minimal response, for which a feaure will be created. Defaults to 0.
 * \return Edgel point featurelist. The list of all found canny edgels at given scale over the threshold.
 */
template <class T, class M>
EdgelFeatureList2D* detectFeaturesUsingCannyWithMask(const vigra::MultiArrayView<2,T>& src,const vigra::MultiArrayView<2,M>& mask, double scale, double threshold=0.0)
{
	// empty edgel list
    std::vector<vigra::Edgel> v_edgels;
    
	// find edgels at scale
    vigra::cannyEdgelListThreshold(src, v_edgels, scale, threshold);
	
    EdgelFeatureList2D* edgels = new EdgelFeatureList2D;
	
	for(auto e : v_edgels)
	{
        if(mask(e.x+.5,  e.y+.5) != 0)
        {
            edgels->addFeature(EdgelFeatureList2D::PointType(e.x,e.y), e.strength, fmod(2*M_PI + e.orientation, 2*M_PI));
        }
    }
    
	return  edgels;
}

} //end of namespace graipe

#endif //GRAIPE_FEATUREDETECTION_DETECTPOINTFEATURES_HXX
