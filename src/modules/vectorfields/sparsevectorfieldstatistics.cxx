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

#include "vectorfields/sparsevectorfieldstatistics.hxx"

namespace graipe {

/**
 * A more useful constructor. Collects the statistics of a given 
 * sparse weighted vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseWeightedVectorfield2DStatistics::SparseWeightedVectorfield2DStatistics(const SparseWeightedVectorfield2D* vf)
: SparseVectorfield2DStatisticsBase<graipe::WeightedVector2D>(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weight.min    = max_val;
    m_weight.max    = min_val;
    m_weight.mean   = m_weight.stddev    =  zero_val;
    
    for(const WeightedVector2D& v : *vf)
    {
        m_weight.min = std::min((double)v.weight, m_weight.min);
        m_weight.max = std::max((double)v.weight, m_weight.max);
        m_weight.mean +=  v.weight;
        
    }
    m_weight.mean    /= vf->size();
    
    for(const WeightedVector2D& v : *vf)
    {
        m_weight.stddev += vigra::pow(m_weight.mean - v.weight, 2.0);
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
 * A more useful constructor. Collects the statistics of a given 
 * sparse multi vectorfield and stores the pointer, too.
 *
 * \param vf The vectorfield, for which we want the statistics.
 */
SparseWeightedMultiVectorfield2DStatistics::SparseWeightedMultiVectorfield2DStatistics(const SparseWeightedMultiVectorfield2D* vf)
: SparseMultiVectorfield2DStatisticsBase<WeightedMultiVector2D>(vf)
{
    float min_val  = vigra::NumericTraits<float>::min();
    float max_val  = vigra::NumericTraits<float>::max();
    float zero_val = vigra::NumericTraits<float>::zero();
    
    m_weight.min  = m_combined_weight.min  = max_val;
    m_weight.max  = m_combined_weight.max  = min_val;
    m_weight.mean = m_weight.stddev = m_combined_weight.mean = m_combined_weight.stddev = zero_val;
    
    int alternatives = vf->alternatives();
    
    m_alt_weights.resize(alternatives);
    
    for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
    {
        m_alt_weights[alt_i].min    = max_val;
        m_alt_weights[alt_i].max    = min_val;
        m_alt_weights[alt_i].mean   = m_alt_weights[alt_i].stddev    = zero_val;
    }
    
    for (const WeightedMultiVector2D& v : *vf)
    {
        double w=v.weight;
        m_weight.min  =  std::min(m_weight.min, w);
        m_weight.max  =  std::max(m_weight.max, w);
        m_weight.mean += w;
    
        m_combined_weight.min  =  std::min(m_combined_weight.min, w);
        m_combined_weight.max  =  std::max(m_combined_weight.max, w);
        m_combined_weight.mean += w;
    
        for(unsigned int alt_i=0; alt_i<v.altDirections.size(); ++alt_i)
        {
            double alt_w = v.altWeights[alt_i];
            m_alt_weights[alt_i].min = std::min(m_alt_weights[alt_i].min, alt_w);
            m_alt_weights[alt_i].max = std::max(m_alt_weights[alt_i].max, alt_w);
            m_alt_weights[alt_i].mean +=  alt_w;
            
            m_combined_weight.min  =  std::min(m_combined_weight.min, alt_w);
            m_combined_weight.max  =  std::max(m_combined_weight.max, alt_w);
            m_combined_weight.mean += alt_w;
        }
    }

    m_weight.mean /= vf->size();
    m_combined_weight.mean /= (vf->size()*(alternatives+1));
    
    for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
    {
        m_alt_weights[alt_i].mean /= vf->size();
    }
    
    for (const WeightedMultiVector2D& v :*vf)
    {
        double w=v.weight;
        m_weight.stddev += pow(m_weight.mean - w,2.0f);
        m_combined_weight.stddev += pow(m_combined_weight.mean - w,2.0f);
        
        for(unsigned int alt_i=0; alt_i<v.altDirections.size(); ++alt_i)
        {
            double alt_w = v.altWeights[alt_i];
            m_alt_weights[alt_i].stddev += pow(m_alt_weights[alt_i].mean - alt_w, 2.0f);
            m_combined_weight.stddev += pow(m_combined_weight.mean - alt_w, 2.0f);
        }
    }
    
    m_weight.stddev = sqrt(m_weight.stddev/vf->size());
    m_combined_weight.stddev = sqrt(m_combined_weight.stddev/(vf->size()*(alternatives+1)));
    
    for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
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
