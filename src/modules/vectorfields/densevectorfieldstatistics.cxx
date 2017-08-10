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

#include "vectorfields/densevectorfieldstatistics.hxx"

#include <vigra/separableconvolution.hxx>

namespace graipe {


DenseVectorfield2DStatistics::DenseVectorfield2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_direction.min    = PointType(max_val, max_val);
    m_direction.max    = PointType(min_val, min_val);
    m_direction.mean   = m_direction.stddev    = PointType(zero_val, zero_val);
    
    m_length.min    = max_val;
    m_length.max    = min_val;
    m_length.mean   = m_length.stddev    = zero_val;
}

DenseVectorfield2DStatistics::DenseVectorfield2DStatistics(const DenseVectorfield2D* vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    //---------------------------------------------------------------------------
    //Direction and length statistics
    
    m_direction.min    = PointType(max_val, max_val);
    m_direction.max    = PointType(min_val, min_val);
    m_direction.mean   = m_direction.stddev    = PointType(zero_val, zero_val);
    
    m_length.min    = max_val;
    m_length.max    = min_val;
    m_length.mean   = m_length.stddev    = zero_val;
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        const PointType& d = vf->direction(i);
        m_direction.min = std::min(m_direction.min, d);
        m_direction.max = std::max(m_direction.max, d);
        m_direction.mean +=  d;
        
        float len=vf->length(i);
        m_length.min = std::min((double)len, m_length.min);
        m_length.max = std::max((double)len, m_length.max);
        m_length.mean +=  len;
    }
    
    m_direction.mean /= vf->size();
    m_length.mean /= vf->size();
    
    for (unsigned int i=0; i< vf->size(); ++i)
    {
        PointType d=(m_direction.mean - vf->direction(i));
        m_direction.stddev += PointType(d.x()*d.x(), d.y()*d.y());
        
        float len=vf->length(i);
        m_length.stddev += vigra::pow(m_length.mean - len, 2.0);
    }
    
    m_direction.stddev = m_direction.stddev/vf->size();
    m_direction.stddev.setX(sqrt(m_direction.stddev.x()));
    m_direction.stddev.setY(sqrt(m_direction.stddev.y()));
    
    m_length.stddev = sqrt(m_length.stddev/vf->size());
}


const BasicStatistics<Vectorfield2D::PointType>& DenseVectorfield2DStatistics::directionStats() const
{
	return m_direction;
}

const BasicStatistics<double>& DenseVectorfield2DStatistics::lengthStats() const
{	
	return m_length;
}

DenseWeightedVectorfield2DStatistics::DenseWeightedVectorfield2DStatistics()
: DenseVectorfield2DStatistics()
{
    typedef float value_type;
    
    value_type min_val  = vigra::NumericTraits<value_type>::min();
    value_type max_val  = vigra::NumericTraits<value_type>::max();
    value_type zero_val = vigra::NumericTraits<value_type>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
}

DenseWeightedVectorfield2DStatistics::DenseWeightedVectorfield2DStatistics(const DenseWeightedVectorfield2D* vf)
: DenseVectorfield2DStatistics(vf)
{
    typedef float value_type;
    
    value_type min_val  = vigra::NumericTraits<value_type>::min();
    value_type max_val  = vigra::NumericTraits<value_type>::max();
    value_type zero_val = vigra::NumericTraits<value_type>::zero();
    
    m_weights.min    = max_val;
    m_weights.max    = min_val;
    m_weights.mean   = m_weights.stddev    =  zero_val;
    
    for (unsigned int i=0; i< vf->size(); ++i)
    {
        m_weights.min = std::min( (double)vf->weight(i),m_weights.min);
        m_weights.max = std::max( (double)vf->weight(i),m_weights.max);
        
        m_weights.mean += vf->weight(i);
    }
    
    m_weights.mean /= vf->size();
    
    for (unsigned int i=0; i< vf->size(); ++i)
    {
        m_weights.stddev += pow(m_weights.mean - vf->weight(i),2.0f);
    }
    m_weights.stddev = sqrt(m_weights.stddev/vf->size());
}
    
const BasicStatistics<double>& DenseWeightedVectorfield2DStatistics::weightStats() const
{
	return m_weights;
}
    

}//end of namespace graipe
