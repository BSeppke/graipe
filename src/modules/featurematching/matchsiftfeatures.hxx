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

#ifndef GRAIPE_FEATUREMATCHING_MATCHSIFTFEATURES_HXX
#define GRAIPE_FEATUREMATCHING_MATCHSIFTFEATURES_HXX

//vigra components needed
#include <vigra/stdimage.hxx>
#include <vigra/linear_algebra.hxx>
#include <vigra/affinegeometry.hxx>

//GRAIPE components needed
#include "features2d/features2d.h"
#include "vectorfields/vectorfields.h"
#include "registration/registration.h"

namespace graipe {

/**
 * @addtogroup graipe_featurematching
 * @{
 *
 * @file
 * @brief Header file for the matching of  SIFT features.
 */

/** 
 * Feature matching using sift features of the first image and sift features of the second image to search for
 * the N most likely features of the second image.
 * The algorithm is brute force: it finds the closest
 * and second-closest matches, and registers the the correspondence if the distance ratio is above a certain threshold.
 * This function returns a (probability-)weighted 2-dimensional multi vectorfield holding the results.
 *
 * \param src1 The first image.
 * \param src2 The second image.
 * \param points1 The features of the first image.
 * \param points2 The features of the second image.
 * \param max_descr_dist The maximal distance between the sift feature descriptors.
 * \param max_geo_dist The maximal geometric distance between the sift features.
 * \param n_candidates The number of candidate matches, which shall be collected.
 * \param use_global Use the global estimation method before the matching to perform "focussed search"?
 * \param mat If use_global is true, this keeps the global motion matrix.
 * \param rotation_correlation If use_global is true, this keeps rotation correlation coefficient.
 * \param translation_correlation If use_global is true, this keeps translation correlation coefficient.
 * \param used_max_distance If use_global is true, this contains the used search distance after the gme.
 * \return A Sparse weighted multi vectorfield containing all found matches.
 */
template <class T1, class T2>
SparseWeightedMultiVectorfield2D* matchSIFTFeaturesUsingDistance(const vigra::MultiArrayView<2,T1> & src1,
                                                                 const vigra::MultiArrayView<2,T2> & src2,
                                                                 SIFTFeatureList2D& points1,
                                                                 SIFTFeatureList2D& points2,
                                                                 float max_descr_dist,
                                                                 float max_geo_dist,
                                                                 unsigned int n_candidates,
                                                                 bool use_global,
                                                                 vigra::Matrix<double> & mat,
                                                                 double & rotation_correlation, double & translation_correlation,
                                                                 unsigned int & used_max_distance)
{
    using namespace ::std;
    using namespace ::vigra;
    
    mat = vigra::identityMatrix<double>(3);
    
    if(use_global)
    {
        estimateGlobalRotationTranslation(src1,
                                          src2,
                                          mat,
                                          rotation_correlation,
                                          translation_correlation);
        
        //Mat now contains transform for I2->I1
    }
    
    used_max_distance = max(1, int(0.5 + max_geo_dist - sqrt(mat(0,2)*mat(0,2) + mat(1,2)*mat(1,2))));
    
    //Create resulting vectorfield
    SparseWeightedMultiVectorfield2D* result_vf = new SparseWeightedMultiVectorfield2D(points1.workspace());
    
    for(unsigned int i=0; i<points1.size(); i++)
    {
        QVector<float> di = points1.descriptor(i);
        list<WeightedTarget2D>		candidates_list;
        
        double min_distance = max_descr_dist;
        
        for(unsigned int j=0; j<points2.size(); j++)
        {
            QVector<float> dm = points2.descriptor(j);
            
            double distance = 0;
            
            int s2_x = vigra::round(points2.position(j).x()),
                s2_y = vigra::round(points2.position(j).y());
            
            float	s2t_x = s2_x*mat(0,0) + s2_y*mat(0,1) + mat(0,2),
                    s2t_y = s2_x*mat(1,0) + s2_y*mat(1,1) + mat(1,2);
            
            if(		(points1.position(i).x()-s2t_x)*(points1.position(i).x()-s2t_x)
               +	(points1.position(i).y()-s2t_y)*(points1.position(i).y()-s2t_y)
               >	used_max_distance*used_max_distance)
                continue;
            
            for(unsigned int k=0; k<(unsigned int)di.size() && k<(unsigned int)dm.size(); k++)
            {
                distance += (di[k]-dm[k]) * (di[k]-dm[k]);
            }
            distance = sqrt(distance);
            
            if(distance < max_descr_dist)     // smallest distance
            {
                WeightedTarget2D target;
                target.x=s2t_x;
                target.y=s2t_y;
                target.weight=distance;
                candidates_list.push_back(target);
                min_distance = std::min(distance,min_distance);
            }
            
        }
        //qDebug() << "min1 = " << min1 << ", min2 = " << min2
        if(candidates_list.size()>0){
            candidates_list.sort();
            candidates_list.reverse();
            
            typedef Vectorfield2D::PointType PointType;
            vector<PointType>	dirs(n_candidates);
            vector<float>		weights(n_candidates);
            
            
            //initialize vectors
            for(unsigned int c=0; c<n_candidates; ++c)
            {
                dirs[c] = PointType(0,0);
                weights[c] = 0.0;
            }
            
            list<WeightedTarget2D>::iterator iter = candidates_list.begin();
            for(unsigned int c=0; c<n_candidates && iter!= candidates_list.end(); ++c, ++iter)
            {
                dirs[c] = PointType(iter->x-points1.position(i).x(), iter->y-points1.position(i).y());
                weights[c] = 1 - iter->weight;
            }
            
            result_vf->addVector(points1.position(i), dirs, weights);
        }
    }
    
    
    //affineMat contains I2 -> I1 get I2->I1
    vigra::Matrix<double> imat = vigra::identityMatrix<double>(3);
    imat(0,0) = mat(0,0); imat(0,1) = mat(1,0);
    imat(1,0) = mat(0,1); imat(1,1) = mat(1,1);
    imat(0,2) = - (imat(0,0)*mat(0,2) + imat(0,1)*mat(1,2));
    imat(1,2) = - (imat(1,0)*mat(0,2) + imat(1,1)*mat(1,2));
    
    //Always store I1 -> I2 transforms
    mat = imat;
    
    return result_vf;
}

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_FEATUREMATCHING_MATCHSIFTFEATURES_HXX
