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

#include "features2d/featureliststatistics.hxx"

#include <vigra/numerictraits.hxx>

namespace graipe {

/**
 * @addtogroup graipe_features2d
 * @{
 *     @file
 *     @brief Implementation file for statistics of 2d feature lists
 * @}
 */

PointFeatureList2DStatistics::PointFeatureList2DStatistics()
: m_features(NULL)
{
    typedef PointFeatureList2D::PointType PointType;
    
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_points.min    = PointType(max_val, max_val);
    m_points.max    = PointType(min_val, min_val);
    m_points.mean    = m_points.stddev    = PointType(zero_val, zero_val);
}

PointFeatureList2DStatistics::PointFeatureList2DStatistics(const PointFeatureList2D* features)
        : m_features(features)
{
    typedef PointFeatureList2D::PointType PointType;
    
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_points.min    = PointType(max_val, max_val);
    m_points.max    = PointType(min_val, min_val);
    m_points.mean   = m_points.stddev
                    = PointType(zero_val, zero_val);
    
    for (unsigned int i=0; i<features->size(); ++i)
    {
        const PointType& p=features->position(i);
        
        m_points.min = std::min(p, m_points.min);
        m_points.max = std::max(p, m_points.max);
        
        m_points.mean +=  p;
    }
    
    m_points.mean /= features->size();
    
    for (unsigned int i=0; i< features->size(); ++i)
    {
        PointType p = (m_points.mean - features->position(i));
        
        m_points.stddev += PointType(p.x()*p.x(), p.y()*p.y());
    }
    
    m_points.stddev = m_points.stddev/features->size();
    m_points.stddev.setX(sqrt(m_points.stddev.x()));
    m_points.stddev.setY(sqrt(m_points.stddev.y()));
}

BasicStatistics<PointFeatureList2D::PointType> PointFeatureList2DStatistics::pointStats() const
{
	return m_points;
}











WeightedPointFeatureList2DStatistics::WeightedPointFeatureList2DStatistics()
: PointFeatureList2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
}

WeightedPointFeatureList2DStatistics::WeightedPointFeatureList2DStatistics(const WeightedPointFeatureList2D* features)
: PointFeatureList2DStatistics(features)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_weights.min = std::min(features->weight(i),m_weights.min);
		m_weights.max = std::max(features->weight(i),m_weights.max);
		
		m_weights.mean += features->weight(i);
	}
	
	m_weights.mean /= features->size();
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_weights.stddev += pow(m_weights.mean - features->weight(i),2.0f);
	}	
	m_weights.stddev = sqrt(m_weights.stddev/features->size());
}

const BasicStatistics<float>& WeightedPointFeatureList2DStatistics::weightStats() const
{
	return m_weights;
}











EdgelFeatureList2DStatistics::EdgelFeatureList2DStatistics()
: WeightedPointFeatureList2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_orientations.min    = max_val;
    m_orientations.max    = min_val;
    m_orientations.mean   = m_orientations.stddev    =  zero_val;
}

EdgelFeatureList2DStatistics::EdgelFeatureList2DStatistics(const EdgelFeatureList2D* features)
: WeightedPointFeatureList2DStatistics(features)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_orientations.min    = max_val;
    m_orientations.max    = min_val;
    m_orientations.mean   = m_orientations.stddev    =  zero_val;
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_orientations.min = std::min( features->orientation(i),m_weights.min);
		m_orientations.max = std::max( features->orientation(i),m_weights.max);
		
		m_orientations.mean += features->orientation(i);
	}
	
	m_orientations.mean /= features->size();
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_orientations.stddev += pow(m_orientations.mean - features->orientation(i),2.0f);
	}	
	m_orientations.stddev = sqrt(m_orientations.stddev/features->size());
}

const BasicStatistics<float>& EdgelFeatureList2DStatistics::orientationStats() const
{
	return m_orientations;
}










SIFTFeatureList2DStatistics::SIFTFeatureList2DStatistics()
: EdgelFeatureList2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
}

SIFTFeatureList2DStatistics::SIFTFeatureList2DStatistics(const SIFTFeatureList2D* features)
: EdgelFeatureList2DStatistics(features)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_scales.min    = max_val;
    m_scales.max    = min_val;
    m_scales.mean   = m_scales.stddev    =  zero_val;
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_scales.min = std::min(features->scale(i),m_scales.min);
		m_scales.max = std::max(features->scale(i),m_scales.max);
		
		m_scales.mean += features->weight(i);
	}
	
	m_scales.mean /= features->size();
	
	for (unsigned int i=0; i< features->size(); ++i)
	{
		m_scales.stddev += pow(m_scales.mean - features->scale(i),2.0f);
	}
	m_scales.stddev = sqrt(m_scales.stddev/features->size());
}

const BasicStatistics<float>& SIFTFeatureList2DStatistics::scaleStats() const
{
	return m_scales;
}
 
} //End of namespace graipe
