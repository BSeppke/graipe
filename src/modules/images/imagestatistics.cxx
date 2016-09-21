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

#include "images/imagestatistics.hxx"
#include <vigra/accumulator.hxx>

namespace graipe {
    
/**
 * Default constructor. Initializes the member with a NULL pointer.
 */
template< class T>
ImageStatistics<T>::ImageStatistics()
{
}

/**
 * A more useful constructor.
 * 
 * \param img The image, for which we want to generate the statistics.
 */
template< class T>
ImageStatistics<T>::ImageStatistics(const Image<T>* img)
 : m_image(img)
{
    using namespace vigra::acc;
    
    AccumulatorChain<T,Select<Variance, Mean, StdDev, Minimum, Maximum> > a;
    
    BasicStatistics<double> stats;
    for( unsigned int c=0; c<img->numBands(); ++c)
    {
        extractFeatures(img->band(c).begin(), img->band(c).end(), a);
        stats.min = get<Minimum>(a);
        stats.max = get<Maximum>(a);
        stats.mean = get<Mean>(a);
        stats.stddev = get<StdDev>(a);

        m_intensityStats.push_back(stats);
    }
}

/**
 * Returns basic statistics of the intensity of all bands inside the image.
 *
 * \return Basic statistics of the intensity of all bands inside the image..
 */
template< class T>
std::vector<BasicStatistics<double> > ImageStatistics<T>::intensityStats() const
{
	return m_intensityStats;
}

//Promoted class instantiations for all promoted image classes
template class ImageStatistics<float>;
template class ImageStatistics<int>;
template class ImageStatistics<unsigned char>;

} //End of namespace graipe