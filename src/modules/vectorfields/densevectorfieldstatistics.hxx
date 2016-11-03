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

#ifndef GRAIPE_VECTORFIELDS_DENSEVECTORFIELDSTATISTICS_HXX
#define GRAIPE_VECTORFIELDS_DENSEVECTORFIELDSTATISTICS_HXX

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
class GRAIPE_VECTORFIELDS_EXPORT DenseVectorfield2DStatistics
{
    public:
        //The used point type
        typedef DenseVectorfield2D::PointType PointType;
    
        /**
         * Default constructor. Constructs an empty sparse vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        DenseVectorfield2DStatistics();
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         * \param restrict_length_to_vf If true, only vectors with a length smaller
         *        than the diagonal of the vectorfields bounding rect are taken into
         *        account for the statistics. Default value: true.
         */
        DenseVectorfield2DStatistics(const DenseVectorfield2D* vf);
        
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
        const DenseVectorfield2D* m_vf;
        //Statistics storages
        BasicStatistics<PointType> m_direction;
        BasicStatistics<double> m_length;
};
    
class GRAIPE_VECTORFIELDS_EXPORT DenseWeightedVectorfield2DStatistics
:   public DenseVectorfield2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty sparse weighted vectorfield statistic with
         * a NULL pointer to the vectorfield.
         */
        DenseWeightedVectorfield2DStatistics();
    
        /**
         * A more useful constructor. Collects the statistics of a given 
         * sparse wehgited vectorfield and stores the pointer, too.
         *
         * \param vf The vectorfield, for which we want the statistics.
         * \param restrict_length_to_vf If true, only vectors with a length smaller
         *        than the diagonal of the vectorfields bounding rect are taken into
         *        account for the statistics. Default value: true.
         */
        DenseWeightedVectorfield2DStatistics(const DenseWeightedVectorfield2D* vf);
            
        /**
         * Returns statistics of the weights of this vectorfield.
         *
         * \return Statistics of the weights of this vectorfield.
         */
        const BasicStatistics<double>& weightStats() const;
    
    protected:
        //Pointer to the vectorfield
        const DenseWeightedVectorfield2D* m_vf;
        //Statistics storage
        BasicStatistics<double> m_weights;
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_DENSEVECTORFIELDSTATISTICS_HXX
