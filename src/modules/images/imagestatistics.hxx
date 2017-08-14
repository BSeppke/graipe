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

#ifndef GRAIPE_IMAGES_IMAGESTATISTICS_HXX
#define GRAIPE_IMAGES_IMAGESTATISTICS_HXX

#include "core/basicstatistics.hxx"

#include "images/image.hxx"
#include "images/config.hxx"

namespace graipe {

/**
 * @addtogroup graipe_images
 * @{
 *
 * @file
 * @brief Header file for the statstics of images
 */

/**
 * This class defines a basic statistics class for Images.
 * It represents the intensity statistics of all bands inside the image.
 */
template <class T>
class GRAIPE_IMAGES_EXPORT ImageStatistics
{
    public:
        /**
         * Default constructor. Initializes the member with a NULL pointer.
         */
        ImageStatistics();
    
        /**
         * A more useful constructor.
         * 
         * \param img The image, for which we want to generate the statistics.
         */
        ImageStatistics(const Image<T>* img);
	
        /**
         * Returns basic statistics of the intensity of all bands inside the image.
         *
         * \return Basic statistics of the intensity of all bands inside the image..
         */
        std::vector<BasicStatistics<double> > intensityStats() const;
    
    protected:
        /** The image **/
        const Image<T>* m_image;
    
        /** The intensity statistics (band-wise) **/
        std::vector<BasicStatistics<double> > m_intensityStats;
};

/**
 * @}
 */
    
} //end of namespace graipe

#endif //GRAIPE_IMAGES_IMAGESTATISTICS_HXX
