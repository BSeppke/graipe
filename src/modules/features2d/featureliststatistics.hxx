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

#ifndef GRAIPE_FEATURES2D_FEATURELISTSTATISTICS_HXX
#define GRAIPE_FEATURES2D_FEATURELISTSTATISTICS_HXX

#include "core/basicstatistics.hxx"

#include "features2d/featurelist.hxx"
#include "features2d/config.hxx"

namespace graipe {

/**
 * @addtogroup graipe_features2d
 * @{
 *
 * @file
 * @brief Header file for statistics of 2d feature lists
 */
 
/**
 * Statistics mother class for 2D feature lists.
 * It keeps the basic statistics over all features in the lists.
 */
class GRAIPE_FEATURES2D_EXPORT PointFeatureList2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty point statistic with
         * a NULL pointer to the features.
         */
        PointFeatureList2DStatistics();
        
        /**
         * A more useful constructor. Collects the statistics of a given 
         * point feature list and stores the pointer, too.
         *
         * \param features The point feature list, for which we want the statistics.
         */
        PointFeatureList2DStatistics(const PointFeatureList2D* features);
        
        /**
         * Returns the basic statistics over the points of the feature list.
         *
         * \return Basic statistics over the points of the feature list.
         */
        BasicStatistics<PointFeatureList2D::PointType> pointStats() const;
        
    protected:
        //Const pointer to the assigned features
        const PointFeatureList2D* m_features;
        //Statistics of the positions
        BasicStatistics<PointFeatureList2D::PointType> m_points;				
};

/**
 * This class extends the basic point feature statistics by
 * a statistics of the weights for weighted feature lists
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPointFeatureList2DStatistics
	: public PointFeatureList2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty weighted statistic with
         * a NULL pointer to the features.
         */
        WeightedPointFeatureList2DStatistics();
        
        /**
         * A more useful constructor. Collects the statistics of a given 
         * weighted feature list and stores the pointer, too.
         *
         * \param features The weighted feature list, for which we want the statistics.
         */
        WeightedPointFeatureList2DStatistics(const WeightedPointFeatureList2D* features);
        
        /**
         * Returns the basic statistics over the weights of the feature list.
         *
         * \return Basic statistics over the weights of the feature list.
         */
        const BasicStatistics<float>& weightStats() const;
        
    protected:
        //Const pointer to the assigned weighted features
        const WeightedPointFeatureList2D* m_features;
        //Statistics of the weights
        BasicStatistics<float> m_weights;			
};



/**
 * This class extends the weighted feature statistics by
 * a statistics of the orientation for edgel feature lists
 */
class GRAIPE_FEATURES2D_EXPORT EdgelFeatureList2DStatistics
	: public WeightedPointFeatureList2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty edgel statistic with
         * a NULL pointer to the features.
         */
        EdgelFeatureList2DStatistics();
        
        /**
         * A more useful constructor. Collects the statistics of a given 
         * edgel feature list and stores the pointer, too.
         *
         * \param features The edgel feature list, for which we want the statistics.
         */
        EdgelFeatureList2DStatistics(const EdgelFeatureList2D* features);
        
        /**
         * Returns the basic statistics over the orientations of the feature list.
         *
         * \return Basic statistics over the orientations of the feature list.
         */
        const BasicStatistics<float>& orientationStats() const;
        
    protected:
        //Const pointer to the assigned edgel features
        const EdgelFeatureList2D* m_features;
        //Statistics of the orientations
        BasicStatistics<float> m_orientations;
};




/**
 * This class extends the edgel feature statistics by
 * a statistics of the scales for SIFT feature lists
 */
class GRAIPE_FEATURES2D_EXPORT SIFTFeatureList2DStatistics
	: public EdgelFeatureList2DStatistics
{
    public:
        /**
         * Default constructor. Constructs an empty SIFT/scale statistic with
         * a NULL pointer to the features.
         */
        SIFTFeatureList2DStatistics();
        
        /**
         * A more useful constructor. Collects the statistics of a given 
         * SIFT/scale feature list and stores the pointer, too.
         *
         * \param features The SIFT feature list, for which we want the statistics
         */
        SIFTFeatureList2DStatistics(const SIFTFeatureList2D* features);
        
        /**
         * Returns the basic statistics over the scales of the feature list.
         *
         * \return Basic statistics over the scales of the feature list.
         */
        const BasicStatistics<float>& scaleStats() const;
        
    protected:
        //Const pointer to the assigned SIFT features
        const SIFTFeatureList2D* m_features;
        //Statistics of the scales
        BasicStatistics<float> m_scales;
};

/**
 * @}
 */
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_FEATURELISTSTATISTICS_HXX
