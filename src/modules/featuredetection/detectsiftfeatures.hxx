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

#ifndef GRAIPE_FEATUREDETECTION_DETECTSIFTFEATURES_HXX
#define GRAIPE_FEATUREDETECTION_DETECTSIFTFEATURES_HXX

//vigra components needed
#include "vigra/stdimage.hxx"
#include "vigra/edgedetection.hxx"
#include "vigra/convolution.hxx"

//GRAIPE Feature Types
#include "features2d/features2d.h"

#include "featuredetection/sift.hxx"

#include <math.h>

#include <QVector>

namespace graipe {
/**
 * @addtogroup graipe_featuredetection
 * @{
 *
 * @file
 * @brief Header file for the SIFT calling fucntion
 */
    
/**
 * This is just a simple wrapper to keep the STL & VIGRA part of the
 * SIFT implementation away from the Qt and other stuff.
 * The STL containers will be used and converted into GRAIPE's data structures.
 *
 * \param src                 The image, for which we want to compute SIFT features.
 * \param sigma               The sigma for one octave (sigma -> 2*sigma). Defaults to 1.
 * \param octaves             The count of octaves. If zero, the number will be estimated
 *                            using the image size. Defaults to 0.
 * \param levels              The count of intra-octave levels. Defaults to 3.
 * \param contrast_threshold  The minimal realtive contrast (0..1) of a feature point.
 *                            Defaults to 0.03.
 * \param curvature_threshold The minimal curvature by means of and inner circle's 
 *                            radius in pixels. Defaults to 10.
 * \param double_image_size   If true, the lowest octave will start are 2*width, 2*height of the image.
 * \param normalize_image     If true, the image will be normalized to 0..1 for the further computations.
 * \param wsp                 The worskpace of the SIFT detection.
 * \return A list of all detected SIFT features.
 */

template <class T>
SIFTFeatureList2D* detectFeaturesUsingSIFT(const vigra::MultiArrayView<2,T>& src,
                                          float sigma, unsigned int octaves, unsigned int levels,
									      float contrast_threshold, float curvature_threshold, bool double_image_size, bool normalize_image,
                                          Workspace * wsp)
{
	SIFTFeatureList2D * result = new SIFTFeatureList2D(wsp);
    
    std::vector<SIFTFeature> std_result = computeSIFTDescriptors(src, sigma, octaves, levels, contrast_threshold, curvature_threshold, double_image_size, normalize_image);
        
    for(const SIFTFeature& sift : std_result)
    {
        result->addFeature(SIFTFeatureList2D::PointType(sift.position[0], sift.position[1]),
                           sift.contrast,
                           sift.orientation,
                           sift.scale,
                           QVector<float>(sift.descriptor.begin(), sift.descriptor.end()));
    }
    
    return result;
}
    

/**
 * @}
 */
 
} //end of namespace graipe

#endif
