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
class GRAIPE_VECTORFIELDS_EXPORT SparseVectorfield2DStatistics
{
    public:
        //The used point type
        typedef SparseVectorfield2D::PointType PointType;
    
        /**
         * Default constructor. Constructs an empty sparse vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        SparseVectorfield2DStatistics();
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseVectorfield2DStatistics(const SparseVectorfield2D* vf);

        /**
         * Returns statistics of the origins of this vectorfield.
         *
         * \return Statistics of the origins of this vectorfield.
         */
        const BasicStatistics<PointType>& originStats() const;
        
        /**
         * Returns statistics of the directions of this vectorfield.
         *
         * \return Statistics of the directions of this vectorfield.
         */
        const BasicStatistics<PointType>& directionStats() const;
        
        /**
         * Returns statistics of the lengths of this vectorfield.
         *
         * \return Statistics of the lengths of this vectorfield.
         */
        const BasicStatistics<double>& lengthStats() const;
    
    protected:
        //Pointer to the vectorfield
        const SparseVectorfield2D* m_vf;
    
        //Statistics storages
        BasicStatistics<PointType> m_origin, m_direction;
        BasicStatistics<double> m_length;
};
    
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedVectorfield2DStatistics
:   public SparseVectorfield2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty sparse weighted vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        SparseWeightedVectorfield2DStatistics();
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse weighted vectorfield and stores the pointer, too.
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
        //Pointer to the vectorfield
        const SparseWeightedVectorfield2D* m_vf;
    
        //Statistics storage
        BasicStatistics<double> m_weight;
};

    
class GRAIPE_VECTORFIELDS_EXPORT SparseMultiVectorfield2DStatistics
:   public SparseVectorfield2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty sparse multi vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        SparseMultiVectorfield2DStatistics();
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse multi vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         */
        SparseMultiVectorfield2DStatistics(const SparseMultiVectorfield2D* vf);
    
        /**
         * Returns statistics of each alternative direction of this vectorfield.
         *
         * \return Statistics of each alternative direction of this vectorfield.
         */
        const std::vector<BasicStatistics<PointType> >& altDirectionStats() const;
        
        /**
         * Returns statistics of each alternative length of this vectorfield.
         *
         * \return Statistics of each alternative length of this vectorfield.
         */
        const std::vector<BasicStatistics<double> >& altLengthStats() const;
    
        /**
         * Returns a combined statistic of the directions and alternative
         * directions of this vectorfield.
         *
         * \return Combined statistics of the directions of this vectorfield.
         */
        const BasicStatistics<PointType>& combinedDirectionStats() const;
        
        /**
         * Returns a combined statistic of the directions and alternative
         * directions of this vectorfield.
         *
         * \return Combined statistics of the directions of this vectorfield.
         */
        const BasicStatistics<double>& combinedLengthStats() const;
    
    protected:
        //Pointer to the vectorfield
        const SparseMultiVectorfield2D* m_vf;
    
        //Statistics storage
        std::vector<BasicStatistics<PointType> > m_alt_directions;
        std::vector<BasicStatistics<double> > m_alt_lengths;
        BasicStatistics<PointType> m_combined_direction;
        BasicStatistics<double> m_combined_length;
};

    
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedMultiVectorfield2DStatistics
:   public SparseMultiVectorfield2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty sparse weighted multi vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        SparseWeightedMultiVectorfield2DStatistics();
    
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
        //Pointer to the vectorfield
        const SparseWeightedMultiVectorfield2D* m_vf;
    
        //Statistics storage
        BasicStatistics<double> m_weight;
        std::vector<BasicStatistics<double> > m_alt_weights;
        BasicStatistics<double> m_combined_weight;
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDSTATISTICS_HXX
