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

#ifndef GRAIPE_FEATURES_CUBICSPLINELISTSTATISTICS_HXX
#define GRAIPE_FEATURES_CUBICSPLINELISTSTATISTICS_HXX

#include "core/basicstatistics.hxx"

#include "features2d/cubicsplinelist.hxx"
#include "features2d/config.hxx"

namespace graipe {

/**
 * Empty statistics mother class for 2D Cubic spline list.
 * Note, that the bounding box of each spline is kept directly by
 * means of each spline - so we need not statistics in this case.
 */
class GRAIPE_FEATURES_EXPORT CubicSplineList2DStatistics
{
    public:
        /**
         * Default constructor. Initializes the member with a NULL pointer.
         */
        CubicSplineList2DStatistics();
    
        /**
         * A more useful constructor.
         * 
         * \param spl The spline list, for which we want to generate the statistics.
         */
        CubicSplineList2DStatistics(const CubicSplineList2D* spl);
    
    protected:
        //The 2D cubic spline list
        const CubicSplineList2D* m_cubicsplines;
};




/**
 * This class extends the empty mother class for weighted 2D cubic spline lists.
 * It represents the weight statistics of all splines inside the list.
 */
class GRAIPE_FEATURES_EXPORT WeightedCubicSplineList2DStatistics
:   public CubicSplineList2DStatistics
{
    public:
        /**
         * Default constructor. Initializes the member with a NULL pointer and the 
         * weight statistics with default values.
         */
        WeightedCubicSplineList2DStatistics();
        
        /**
         * A more useful constructor.
         * 
         * \param spl The weighted spline list, for which we want to generate the statistics.
         */
        WeightedCubicSplineList2DStatistics(const WeightedCubicSplineList2D* spl);
	
        /**
         * Returns basic statistics of the weights of all 2D cubic splines inside the list.
         *
         * \return Basic statistics of the weights of all 2D cubic splines inside the list.
         */
        BasicStatistics<float> weightStats() const;
	
    protected:
        //The weighted 2D cubic spline list
        const WeightedCubicSplineList2D* m_cubicsplines;
    
        //Weight statistics
        BasicStatistics<float> m_weights;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES_CUBICSPLINELISTSTATISTICS_HXX
