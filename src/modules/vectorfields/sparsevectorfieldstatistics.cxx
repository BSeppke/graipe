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

#include "vectorfields/sparsevectorfieldstatistics.hxx"

namespace graipe {
    
/**
 * Default constructor. Constructs an empty sparse vectorfield statistic with
 * a NULL pointer to the vectorfield.
 */
SparseVectorfield2DStatistics::SparseVectorfield2DStatistics()
: m_vf(NULL)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_origin.min    = PointType(max_val, max_val);
    m_origin.max    = PointType(min_val, min_val);
    m_origin.mean    = m_origin.stddev    = PointType(zero_val, zero_val);
    
    m_direction.min    = PointType(max_val, max_val);
    m_direction.max    = PointType(min_val, min_val);
    m_direction.mean    = m_direction.stddev    = PointType(zero_val, zero_val);
    
    m_length.min    = max_val;
    m_length.max    = min_val;
    m_length.mean    = m_length.stddev    = zero_val;
}

/**
 * A more useful constructor. Collects the statistics of a given 
 * sparse vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseVectorfield2DStatistics::SparseVectorfield2DStatistics(const SparseVectorfield2D* vf)
: m_vf(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_origin.min    = PointType(max_val, max_val);
    m_origin.max    = PointType(min_val, min_val);
    m_origin.mean    = m_origin.stddev    = PointType(zero_val, zero_val);
    
    m_direction.min    = PointType(max_val, max_val);
    m_direction.max    = PointType(min_val, min_val);
    m_direction.mean    = m_direction.stddev    = PointType(zero_val, zero_val);
    
    m_length.min    = max_val;
    m_length.max    = min_val;
    m_length.mean    = m_length.stddev    = zero_val;
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        const PointType& o = vf->origin(i);
        m_origin.min = std::min(m_origin.min, o);
        m_origin.max = std::max(m_origin.max, o);
        m_origin.mean +=  o;
        
        const PointType& d = vf->direction(i);
        m_direction.min = std::min(m_direction.min, d);
        m_direction.max = std::max(m_direction.max, d);
        m_direction.mean +=  d;
        
        double len=vf->length(i);
        m_length.min = std::min(len, m_length.min);
        m_length.max = std::max(len, m_length.max);
        m_length.mean +=  len;
    }
    
    m_origin.mean /= vf->size();
    m_direction.mean /= vf->size();
    m_length.mean /= vf->size();
    
    for (unsigned int i=0; i< vf->size(); ++i)
    {
        PointType o=(m_origin.mean - vf->origin(i));
        m_origin.stddev += PointType(o.x()*o.x(), o.y()*o.y());
        
        PointType d=(m_direction.mean - vf->direction(i));
        m_direction.stddev += PointType(d.x()*d.x(), d.y()*d.y());
        
        float len=vf->length(i);
        m_length.stddev += vigra::pow(m_length.mean - len, 2.0);
    }
    
    m_origin.stddev = m_origin.stddev/vf->size();
    m_origin.stddev.setX(sqrt(m_origin.stddev.x()));
    m_origin.stddev.setY(sqrt(m_origin.stddev.y()));
    
    m_direction.stddev = m_direction.stddev/vf->size();
    m_direction.stddev.setX(sqrt(m_direction.stddev.x()));
    m_direction.stddev.setY(sqrt(m_direction.stddev.y()));
    
    m_length.stddev = sqrt(m_length.stddev/vf->size());
}

/**
 * Returns statistics of the origins of this vectorfield.
 *
 * \return Statistics of the origins of this vectorfield.
 */
const BasicStatistics<Vectorfield2D::PointType>& SparseVectorfield2DStatistics::originStats() const
{
	return m_origin;
}

/**
 * Returns statistics of the directions of this vectorfield.
 *
 * \return Statistics of the directions of this vectorfield.
 */
const BasicStatistics<Vectorfield2D::PointType>& SparseVectorfield2DStatistics::directionStats() const
{
	return m_direction;
}

/**
 * Returns statistics of the lengths of this vectorfield.
 *
 * \return Statistics of the lengths of this vectorfield.
 */
const BasicStatistics<double>& SparseVectorfield2DStatistics::lengthStats() const
{	
	return m_length;
}




/**
 * Default constructor. Constructs an empty sparse weighted vectorfield statistic with
 * a NULL pointer to the vectorfield.
 */
SparseWeightedVectorfield2DStatistics::SparseWeightedVectorfield2DStatistics()
: SparseVectorfield2DStatistics()
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weight.min    = max_val;
    m_weight.max    = min_val;
    m_weight.mean   = m_weight.stddev    =  zero_val;
}
    
/**
 * A more useful constructor. Collects the statistics of a given 
 * sparse weighted vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseWeightedVectorfield2DStatistics::SparseWeightedVectorfield2DStatistics(const SparseWeightedVectorfield2D* vf)
: SparseVectorfield2DStatistics(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weight.min    = max_val;
    m_weight.max    = min_val;
    m_weight.mean   = m_weight.stddev    =  zero_val;
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        double w = vf->weight(i);
        
        m_weight.min = std::min(w, m_weight.min);
        m_weight.max = std::max(w, m_weight.max);
        
        m_weight.mean += w;
    }
    
    m_weight.mean /= vf->size();
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        m_weight.stddev += pow(m_weight.mean - vf->weight(i),2.0f);
    }
    m_weight.stddev = sqrt(m_weight.stddev/vf->size());
}

/**
 * Returns statistics of the weights of this vectorfield.
 *
 * \return Statistics of the weights of this vectorfield.
 */
const BasicStatistics<double>& SparseWeightedVectorfield2DStatistics::weightStats() const
{
	return m_weight;
}



/**
 * Default constructor. Constructs an empty sparse multi vectorfield statistic with
 * a NULL pointer to the vectorfield.
 */
SparseMultiVectorfield2DStatistics::SparseMultiVectorfield2DStatistics()
: SparseVectorfield2DStatistics()
{
}

/**
 * A more useful constructor. Collects the statistics of a given 
 * sparse multi vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseMultiVectorfield2DStatistics::SparseMultiVectorfield2DStatistics(const SparseMultiVectorfield2D* vf)
: SparseVectorfield2DStatistics(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    
    m_combined_direction.min    = PointType(max_val, max_val);
    m_combined_direction.max    = PointType(min_val, min_val);
    m_combined_direction.mean   = m_combined_direction.stddev    = PointType(zero_val, zero_val);
    
    m_combined_length.min    = max_val;
    m_combined_length.max    = min_val;
    m_combined_length.mean   = m_combined_length.stddev    = zero_val;
    
    m_alt_directions.resize(vf->alternatives());
    m_alt_lengths.resize(vf->alternatives());
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_directions[alt_i].min    = PointType(max_val, max_val);
        m_alt_directions[alt_i].max    = PointType(min_val, min_val);
        m_alt_directions[alt_i].mean   = m_alt_directions[alt_i].stddev    = PointType(zero_val, zero_val);
        
        m_alt_lengths[alt_i].min    = max_val;
        m_alt_lengths[alt_i].max    = min_val;
        m_alt_lengths[alt_i].mean   = m_alt_lengths[alt_i].stddev    = zero_val;
    }
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        const PointType& d = vf->direction(i);
        m_combined_direction.min = std::min(m_combined_direction.min, d);
        m_combined_direction.max = std::max(m_combined_direction.max, d);
        m_combined_direction.mean +=  d;
        
        double len=vf->length(i);
        m_combined_length.min = std::min(len, m_combined_length.min);
        m_combined_length.max = std::max(len, m_combined_length.max);
        m_combined_length.mean +=  len;
        
        for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
        {
            const PointType& alt_d = vf->altDirection(i,alt_i);
            m_alt_directions[alt_i].min = std::min(m_alt_directions[alt_i].min, alt_d);
            m_alt_directions[alt_i].max = std::max(m_alt_directions[alt_i].max, alt_d);
            m_alt_directions[alt_i].mean +=  alt_d;
            
            m_combined_direction.min = std::min(m_combined_direction.min, alt_d);
            m_combined_direction.max = std::max(m_combined_direction.max, alt_d);
            m_combined_direction.mean += alt_d;
            
            double alt_len=vf->altLength(i, alt_i);
            m_alt_lengths[alt_i].min = std::min(m_alt_lengths[alt_i].min, alt_len);
            m_alt_lengths[alt_i].max = std::max(m_alt_lengths[alt_i].max, alt_len);
            m_alt_lengths[alt_i].mean +=  alt_len;
            
            m_combined_length.min = std::min((double)alt_len, m_combined_length.min);
            m_combined_length.max = std::max((double)alt_len, m_combined_length.max);
            m_combined_length.mean +=  alt_len;
        }
    }
    
    m_combined_direction.mean /= (vf->size()*(vf->alternatives()+1));
    m_combined_length.mean /= (vf->size()*(vf->alternatives()+1));
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_directions[alt_i].mean /= vf->size();
        m_alt_lengths[alt_i].mean /= vf->size();
    }
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        PointType d=(m_combined_direction.mean - vf->direction(i));
        m_combined_direction.stddev += PointType(d.x()*d.x(), d.y()*d.y());
        m_combined_length.stddev +=  pow(m_combined_length.mean - vf->length(i), 2.0f);
        
        for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
        {
            PointType d=(m_alt_directions[alt_i].mean - vf->altDirection(i,alt_i));
            m_alt_directions[alt_i].stddev += PointType(d.x()*d.x(), d.y()*d.y());
            m_alt_lengths[alt_i].stddev += pow(m_alt_lengths[alt_i].mean - vf->altLength(i, alt_i), 2.0f);
            
            PointType c_d=(m_combined_direction.mean - vf->altDirection(i, alt_i));
            m_combined_direction.stddev += PointType(c_d.x()*c_d.x(), c_d.y()*c_d.y());
            m_combined_length.stddev += pow(m_combined_length.mean - vf->altLength(i, alt_i), 2.0f);
        }
    }
    
        m_combined_direction.stddev = m_combined_direction.stddev/(vf->size()*(vf->alternatives()+1));
        m_combined_direction.stddev.setX(sqrt(m_combined_direction.stddev.x()));
        m_combined_direction.stddev.setY(sqrt(m_combined_direction.stddev.y()));
        
        m_combined_length.stddev = sqrt(m_combined_length.stddev/(vf->size()*(vf->alternatives()+1)));
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_directions[alt_i].stddev = m_alt_directions[alt_i].stddev/vf->size();
        m_alt_directions[alt_i].stddev.setX(sqrt(m_alt_directions[alt_i].stddev.x()));
        m_alt_directions[alt_i].stddev.setY(sqrt(m_alt_directions[alt_i].stddev.y()));
        
        m_alt_lengths[alt_i].stddev = sqrt(m_alt_lengths[alt_i].stddev/vf->size());
    }
}

/**
 * Returns statistics of each alternative direction of this vectorfield.
 *
 * \return Statistics of each alternative direction of this vectorfield.
 */
const std::vector<BasicStatistics<SparseMultiVectorfield2DStatistics::PointType> >& SparseMultiVectorfield2DStatistics::altDirectionStats() const
{
	return m_alt_directions;
}

/**
 * Returns statistics of each alternative length of this vectorfield.
 *
 * \return Statistics of each alternative length of this vectorfield.
 */
const std::vector<BasicStatistics<double> >& SparseMultiVectorfield2DStatistics::altLengthStats() const
{	
	return m_alt_lengths;
}

/**
 * Returns a combined statistic of the directions and alternative
 * directions of this vectorfield.
 *
 * \return Combined statistics of the directions of this vectorfield.
 */
const BasicStatistics<Vectorfield2D::PointType>& SparseMultiVectorfield2DStatistics::combinedDirectionStats() const
{
	return m_combined_direction;
}

/**
 * Returns a combined statistic of the directions and alternative
 * directions of this vectorfield.
 *
 * \return Combined statistics of the directions of this vectorfield.
 */
const BasicStatistics<double>& SparseMultiVectorfield2DStatistics::combinedLengthStats() const
{	
	return m_combined_length;
}



/**
 * Default constructor. Constructs an empty sparse weighted multi vectorfield statistic with
 * a NULL pointer to the vectorfield.
 */
SparseWeightedMultiVectorfield2DStatistics::SparseWeightedMultiVectorfield2DStatistics()
: SparseMultiVectorfield2DStatistics()
{
}

/**
 * A more useful constructor. Collects the statistics of a given 
 * sparse multi vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseWeightedMultiVectorfield2DStatistics::SparseWeightedMultiVectorfield2DStatistics(const SparseWeightedMultiVectorfield2D* vf)
: SparseMultiVectorfield2DStatistics(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weight.min  = m_combined_weight.min  = max_val;
    m_weight.max  = m_combined_weight.max  = min_val;
    m_weight.mean = m_weight.stddev = m_combined_weight.mean = m_combined_weight.stddev = zero_val;
    
    m_alt_weights.resize(vf->alternatives());
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_weights[alt_i].min    = max_val;
        m_alt_weights[alt_i].max    = min_val;
        m_alt_weights[alt_i].mean   = m_alt_weights[alt_i].stddev    = zero_val;
    }
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        double w = vf->weight(i);
    
        m_weight.min  =  std::min(m_weight.min, w);
        m_weight.max  =  std::max(m_weight.max, w);
        m_weight.mean += w;
    
        m_combined_weight.min  =  std::min(m_combined_weight.min, w);
        m_combined_weight.max  =  std::max(m_combined_weight.max, w);
        m_combined_weight.mean += w;
    
    
        for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
        {
            double alt_w = vf->altWeight(i,alt_i);
            m_alt_weights[alt_i].min = std::min(m_alt_weights[alt_i].min, alt_w);
            m_alt_weights[alt_i].max = std::max(m_alt_weights[alt_i].max, alt_w);
            m_alt_weights[alt_i].mean +=  alt_w;
            
            m_combined_weight.min  =  std::min(m_combined_weight.min, alt_w);
            m_combined_weight.max  =  std::max(m_combined_weight.max, alt_w);
            m_combined_weight.mean += alt_w;
        }
    }
    
    m_weight.mean /= vf->size();
    m_combined_weight.mean /= (vf->size()*(vf->alternatives()+1));
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_weights[alt_i].mean /= vf->size();
    }
    
    for (unsigned int i=0; i<vf->size(); ++i)
    {
        m_weight.stddev += pow(m_weight.mean - vf->weight(i),2.0f);
        m_combined_weight.stddev += pow(m_combined_weight.mean - vf->weight(i),2.0f);

        for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
        {
            double alt_w = vf->altWeight(i,alt_i);
            m_alt_weights[alt_i].stddev += pow(m_alt_weights[alt_i].mean - alt_w, 2.0f);
            m_combined_weight.stddev += pow(m_combined_weight.mean - alt_w, 2.0f);
        }
    }
    
    m_weight.stddev = sqrt(m_weight.stddev/vf->size());
    m_combined_weight.stddev = sqrt(m_combined_weight.stddev/(vf->size()*(vf->alternatives()+1)));
    
    for(unsigned int alt_i=0; alt_i<vf->alternatives(); ++alt_i)
    {
        m_alt_weights[alt_i].stddev = sqrt(m_alt_weights[alt_i].stddev/vf->size());
    }
}
/**
 * Returns statistics of the weights of this vectorfield.
 *
 * \return Statistics of the weights of this vectorfield.
 */
const BasicStatistics<double>& SparseWeightedMultiVectorfield2DStatistics::weightStats() const
{
    return m_weight;
}

/**
 * Returns statistics of each alternative weight of this vectorfield.
 *
 * \return Statistics of each alternative weight of this vectorfield.
 */
const std::vector<BasicStatistics<double> >&  SparseWeightedMultiVectorfield2DStatistics::altWeightStats() const
{
	return m_alt_weights;
}
 
/**
 * Returns a combined statistic of the weights and alternative
 * weights of this vectorfield.
 *
 * \return Combined statistics of the weights of this vectorfield.
 */
const BasicStatistics<double>& SparseWeightedMultiVectorfield2DStatistics::combinedWeightStats() const
{
    return m_combined_weight;
}

}//end of namespace graipe
