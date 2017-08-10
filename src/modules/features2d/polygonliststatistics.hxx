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

#ifndef GRAIPE_FEATURES2D_POLYGONLISTSTATISTICS_HXX
#define GRAIPE_FEATURES2D_POLYGONLISTSTATISTICS_HXX

#include "core/basicstatistics.hxx"

#include "features2d/polygonlist.hxx"
#include "features2d/config.hxx"

/**
 * @file
 * @brief Header file for statistics of 2d polygons
 *
 * @addtogroup features2d
 * @{
 */

namespace graipe {
 
/**
 * Empty statistics mother class for polygon lists.
 * Note, that the bounding box of each polygon is kept directly by
 * means of each polygon - so we need not statistics in this case.
 */
class GRAIPE_FEATURES2D_EXPORT PolygonList2DStatistics
{
    public:
        /**
         * Default constructor. Initializes the member with a NULL pointer.
         */
        PolygonList2DStatistics();
    
        /**
         * A more useful constructor.
         * 
         * \param pl The polygon list, for which we want to generate the statistics.
         */
        PolygonList2DStatistics(const PolygonList2D* pl);
    
    protected:
        //The polygon list
        const PolygonList2D* m_polygons;
};




/**
 * This class extends the empty mother class for weighted polygon lists.
 * It represents the weight statistics of all polygons inside the list.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPolygonList2DStatistics
	: public PolygonList2DStatistics
{
    public:
        /**
         * Default constructor. Initializes the member with a NULL pointer.
         */
        WeightedPolygonList2DStatistics();
        
        /**
         * A more useful constructor.
         * 
         * \param pl The weighted polygon list, for which we want to generate the statistics.
         */
        WeightedPolygonList2DStatistics(const WeightedPolygonList2D* pl);
	
         /**
         * Returns basic statistics of the weights of all polygons inside the list.
         *
         * \return Basic statistics of the weights of all polygons inside the list.
         */
         const BasicStatistics<float>& weightStats() const;
	
    protected:
        //The weighted polygon list
        const WeightedPolygonList2D* m_polygons;
    
        //Weight statistics
        BasicStatistics<float> m_weights;
};

} //end of namespace graipe
    
/**
 * @}
 */

#endif //GRAIPE_FEATURES2D_POLYGONLISTSTATISTICS_HXX
