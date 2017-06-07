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

#ifndef GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDSTATISTICS_HXX
#define GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDSTATISTICS_HXX

#include "core/basicstatistics.hxx"

#include "vectorfields/sparsevectorfield.hxx"
#include "vectorfields/densevectorfield.hxx"
#include "vectorfields/config.hxx"

namespace graipe {

/**
 * This class holds basic statistics for any class,
 * which fulfilles the Vectorfield2D interface
 * Statistics are kept for the origins, the directions ans the lengths
 * of all vectors of the vectorfield.
 */
template <class T>
class GRAIPE_VECTORFIELDS_EXPORT SparseVectorfield2DStatisticsBase
{
    public:
        //The used point type
        typedef typename T::PointType PointType;
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseVectorfield2DStatisticsBase(const SparseVectorfield2DBase<T>* vf)
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
            
            for(const Vector2D& v : *vf)
            {
                m_origin.min = std::min(m_origin.min, v.origin);
                m_origin.max = std::max(m_origin.max, v.origin);
                m_origin.mean +=  v.origin;
                
                m_direction.min = std::min(m_direction.min, v.direction);
                m_direction.max = std::max(m_direction.max, v.direction);
                m_direction.mean +=  v.direction;
                
                m_length.min = std::min((double)v.direction.length(), m_length.min);
                m_length.max = std::max((double)v.direction.length(), m_length.max);
                m_length.mean +=  v.direction.length();
            }
            
            m_origin.mean /= vf->size();
            m_direction.mean /= vf->size();
            m_length.mean /= vf->size();
            
            for(const Vector2D& v : *vf)
            {
                PointType o=(m_origin.mean - v.origin);
                m_origin.stddev += PointType(o.x()*o.x(), o.y()*o.y());
                
                PointType d=(m_direction.mean - v.direction);
                m_direction.stddev += PointType(d.x()*d.x(), d.y()*d.y());
                
                m_length.stddev += vigra::pow(m_length.mean - v.direction.length(), 2.0);
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
        const BasicStatistics<PointType>& originStats() const
        {
            return m_origin;
        }
    
        /**
         * Returns statistics of the directions of this vectorfield.
         *
         * \return Statistics of the directions of this vectorfield.
         */
        const BasicStatistics<PointType>& directionStats() const
        {
            return m_direction;
        }
        
        /**
         * Returns statistics of the lengths of this vectorfield.
         *
         * \return Statistics of the lengths of this vectorfield.
         */
        const BasicStatistics<double>& lengthStats() const
        {
            return m_length;
        }
    
    protected:
        //Statistics storages
        BasicStatistics<PointType> m_origin, m_direction;
        BasicStatistics<double> m_length;
};

typedef SparseVectorfield2DStatisticsBase<Vector2D> SparseVectorfield2DStatistics;

class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedVectorfield2DStatistics
:   public SparseVectorfield2DStatisticsBase<WeightedVector2D>
{
    public:
        //The used point type
        typedef typename SparseVectorfield2DStatisticsBase<WeightedVector2D>::PointType PointType;
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseWeightedVectorfield2DStatistics(const SparseWeightedVectorfield2D* vf);
    
        /**
         * Returns statistics of the weights of this vectorfield.
         *
         * \return Statistics of the weights of this vectorfield.
         */
        const BasicStatistics<double>& weightStats() const;
    
    
    protected:
        //Additional statistics storage
        BasicStatistics<double> m_weight;
};

template <class T>
class GRAIPE_VECTORFIELDS_EXPORT SparseMultiVectorfield2DStatisticsBase
:   public SparseVectorfield2DStatisticsBase<T>
{
    public:
        typedef typename T::PointType PointType;
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse multi vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseMultiVectorfield2DStatisticsBase(const SparseMultiVectorfield2DBase<T> * vf)
        : SparseVectorfield2DStatisticsBase<T>(vf)
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
            
            int alternatives = 0;
            
            for(const MultiVector2D& v : *vf)
            {
                alternatives = std::max(alternatives, (int)v.altDirections.size());
            }
            
            m_alt_directions.resize(alternatives);
            m_alt_lengths.resize(alternatives);
            
            for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
            {
                m_alt_directions[alt_i].min    = PointType(max_val, max_val);
                m_alt_directions[alt_i].max    = PointType(min_val, min_val);
                m_alt_directions[alt_i].mean   = m_alt_directions[alt_i].stddev    = PointType(zero_val, zero_val);
                
                m_alt_lengths[alt_i].min    = max_val;
                m_alt_lengths[alt_i].max    = min_val;
                m_alt_lengths[alt_i].mean   = m_alt_lengths[alt_i].stddev    = zero_val;
            }
            
            for (const MultiVector2D& v : *vf)
            {
                const PointType& d = v.direction;
                m_combined_direction.min = std::min(m_combined_direction.min, d);
                m_combined_direction.max = std::max(m_combined_direction.max, d);
                m_combined_direction.mean +=  d;
                
                double len=d.length();
                m_combined_length.min = std::min(len, m_combined_length.min);
                m_combined_length.max = std::max(len, m_combined_length.max);
                m_combined_length.mean +=  len;
                
                for(unsigned int alt_i=0; alt_i<v.altDirections.size(); ++alt_i)
                {
                    const PointType& alt_d = v.altDirections[alt_i];
                    m_alt_directions[alt_i].min = std::min(m_alt_directions[alt_i].min, alt_d);
                    m_alt_directions[alt_i].max = std::max(m_alt_directions[alt_i].max, alt_d);
                    m_alt_directions[alt_i].mean +=  alt_d;
                    
                    m_combined_direction.min = std::min(m_combined_direction.min, alt_d);
                    m_combined_direction.max = std::max(m_combined_direction.max, alt_d);
                    m_combined_direction.mean += alt_d;
                    
                    double alt_len = alt_d.length();
                    m_alt_lengths[alt_i].min = std::min(m_alt_lengths[alt_i].min, alt_len);
                    m_alt_lengths[alt_i].max = std::max(m_alt_lengths[alt_i].max, alt_len);
                    m_alt_lengths[alt_i].mean +=  alt_len;
                    
                    m_combined_length.min = std::min((double)alt_len, m_combined_length.min);
                    m_combined_length.max = std::max((double)alt_len, m_combined_length.max);
                    m_combined_length.mean +=  alt_len;
                }
            }
            
            m_combined_direction.mean /= (vf->size()*(alternatives+1));
            m_combined_length.mean /= (vf->size()*(alternatives+1));
            
            for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
            {
                m_alt_directions[alt_i].mean /= vf->size();
                m_alt_lengths[alt_i].mean /= vf->size();
            }
            
            for (const MultiVector2D& v :*vf)
            {
                PointType d=(m_combined_direction.mean - v.direction);
                m_combined_direction.stddev += PointType(d.x()*d.x(), d.y()*d.y());
                m_combined_length.stddev +=  pow(m_combined_length.mean - d.length(), 2.0f);
                
                for(unsigned int alt_i=0; alt_i<v.altDirections.size(); ++alt_i)
                {
                    PointType d=(m_alt_directions[alt_i].mean - v.altDirections[alt_i]);
                    m_alt_directions[alt_i].stddev += PointType(d.x()*d.x(), d.y()*d.y());
                    m_alt_lengths[alt_i].stddev += pow(m_alt_lengths[alt_i].mean - d.length(), 2.0f);
                    
                    PointType c_d=(m_combined_direction.mean - v.altDirections[alt_i]);
                    m_combined_direction.stddev += PointType(c_d.x()*c_d.x(), c_d.y()*c_d.y());
                    m_combined_length.stddev += pow(m_combined_length.mean - d.length(), 2.0f);
                }
            }
            
            m_combined_direction.stddev = m_combined_direction.stddev/(vf->size()*(alternatives+1));
            m_combined_direction.stddev.setX(sqrt(m_combined_direction.stddev.x()));
            m_combined_direction.stddev.setY(sqrt(m_combined_direction.stddev.y()));
                
            m_combined_length.stddev = sqrt(m_combined_length.stddev/(vf->size()*(alternatives+1)));
            
            for(unsigned int alt_i=0; alt_i<alternatives; ++alt_i)
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
        const std::vector<BasicStatistics<PointType> >& altDirectionStats() const
        {
            return m_alt_directions;
        }
        
        /**
         * Returns statistics of each alternative length of this vectorfield.
         *
         * \return Statistics of each alternative length of this vectorfield.
         */
        const std::vector<BasicStatistics<double> >& altLengthStats() const
        {
            return m_alt_lengths;
        }
    
    
        /**
         * Returns a combined statistic of the directions and alternative
         * directions of this vectorfield.
         *
         * \return Combined statistics of the directions of this vectorfield.
         */
        const BasicStatistics<PointType>& combinedDirectionStats() const
        {
            return m_combined_direction;
        }
    
        
        /**
         * Returns a combined statistic of the directions and alternative
         * directions of this vectorfield.
         *
         * \return Combined statistics of the directions of this vectorfield.
         */
        const BasicStatistics<double>& combinedLengthStats() const
        {
            return m_combined_length;
        }
    
    
    protected:
        //Statistics storage
        std::vector<BasicStatistics<PointType> > m_alt_directions;
        std::vector<BasicStatistics<double> > m_alt_lengths;
        BasicStatistics<PointType> m_combined_direction;
        BasicStatistics<double> m_combined_length;
};


typedef SparseMultiVectorfield2DStatisticsBase<MultiVector2D> SparseMultiVectorfield2DStatistics;
    
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedMultiVectorfield2DStatistics
:   public SparseMultiVectorfield2DStatisticsBase<WeightedMultiVector2D>
{
    public:
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse multi vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseWeightedMultiVectorfield2DStatistics(const SparseWeightedMultiVectorfield2D* vf);
    
    
        /**
         * Returns statistics of the weights of this vectorfield.
         *
         * \return Statistics of the weights of this vectorfield.
         */
        const BasicStatistics<double>& weightStats() const;
        
        /**
         * Returns statistics of each alternative weight of this vectorfield.
         *
         * \return Statistics of each alternative weight of this vectorfield.
         */
        const std::vector<BasicStatistics<double> >& altWeightStats() const;
        
        /**
         * Returns a combined statistic of the weights and alternative
         * weights of this vectorfield.
         *
         * \return Combined statistics of the weights of this vectorfield.
         */
        const BasicStatistics<double>& combinedWeightStats() const;
    
    protected:
        //Statistics storage
        BasicStatistics<double> m_weight;
        std::vector<BasicStatistics<double> > m_alt_weights;
        BasicStatistics<double> m_combined_weight;
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDSTATISTICS_HXX
