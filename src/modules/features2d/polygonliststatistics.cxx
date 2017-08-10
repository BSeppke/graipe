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

#include "features2d/polygonliststatistics.hxx"

#include <vigra/numerictraits.hxx>

namespace graipe {
    
/**
 * @file
 * @brief Implementation file for statistics of 2d polygons
 *
 * @addtogroup features2d
 * @{
 */
 
/**
 * Default constructor. Initializes the member with a NULL pointer.
 */
PolygonList2DStatistics::PolygonList2DStatistics()
: m_polygons(NULL)
{	
}

/**
 * A more useful constructor.
 * 
 * \param pl The polygon list, for which we want to generate the statistics.
 */
PolygonList2DStatistics::PolygonList2DStatistics(const PolygonList2D* pl)
			: m_polygons(pl)
{
}




/**
 * Default constructor. Initializes the member with a NULL pointer.
 */
WeightedPolygonList2DStatistics::WeightedPolygonList2DStatistics()
: PolygonList2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
}

/**
 * A more useful constructor.
 * 
 * \param pl The polygon list, for which we want to generate the statistics.
 */
WeightedPolygonList2DStatistics::WeightedPolygonList2DStatistics(const WeightedPolygonList2D* pl)
: PolygonList2DStatistics(pl)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
	
	for (unsigned int i=0; i<pl->size(); ++i)
	{
		m_weights.min = std::min( pl->weight(i),m_weights.min);
		m_weights.max = std::max( pl->weight(i),m_weights.max);
		
		
		m_weights.mean += pl->weight(i);
	}
	
	m_weights.mean /= pl->size();
	
	for (unsigned int i=0; i<pl->size(); ++i)
	{
		m_weights.stddev += pow(m_weights.mean - pl->weight(i),2.0f);
	}	
	m_weights.stddev = sqrt(m_weights.stddev/pl->size());
}

/**
 * Returns basic statistics of the weights of all polygons inside the list.
 *
 * \return Basic statistics of the weights of all polygons inside the list.
 */
const BasicStatistics<float>& WeightedPolygonList2DStatistics::weightStats() const
{
	return m_weights;
}

} //End of namespace graipe

/**
 * @}
 */
