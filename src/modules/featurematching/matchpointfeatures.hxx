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

#ifndef GRAIPE_FEATUREMATCHING_MATCHPOINTFEATURES_HXX
#define GRAIPE_FEATUREMATCHING_MATCHPOINTFEATURES_HXX

//vigra components needed
#include <vigra/stdimage.hxx>
#include <vigra/linear_algebra.hxx>
#include <vigra/affinegeometry.hxx>
#include <vigra/affine_registration_fft.hxx>
#include <vigra/correlation.hxx>

//GRAIPE components needed
#include "features2d/features2d.h"
#include "vectorfields/vectorfields.h"
#include "registration/registration.h"

namespace graipe {

/**
 * A very basic matching functor for (sub)images.
 * Uses the Fast Normalized Cross-correlation
 */
class FastNCCFunctor
{
public:
    FastNCCFunctor()
    {
    }
    
    template <class SrcArray, class MaskArray, class DestArray>
    void operator()(SrcArray src, MaskArray mask, DestArray dest)
    {
        vigra::fastNormalizedCrossCorrelation(src, mask, dest);
    }
};

/**
 * A very basic matching functor for (sub)images.
 * Uses the Fast Cross-correlation
 */
class FastCCFunctor
{
public:
    FastCCFunctor()
    {
    }
    
    template <class SrcArray, class MaskArray, class DestArray>
    void operator()(SrcArray src, MaskArray mask, DestArray dest)
    {
        vigra::fastCrossCorrelation(src, mask, dest);
    }
};

/** 
 * Feature matching using features of the first image and an area at the second image to search for
 * the N most likely positions of the second image.
 * This function returns a (probability-)weighted 2-dimensional multi vectorfield holding the results.
 *
 * \param src1 The first image.
 * \param src2 The second image.
 * \param features The features of the first image.
 * \param func The matching functor, which shall be used for comparison.
 * \param mask_width The width of the subimage, which will be compared for each feature.
 * \param mask_height The height of the subimage, which will be compared for each feature.
 * \param max_distance The maximal distance between the features of image 1 and image 2.
 * \param n_candidates The number of candidate matches, which shall be collected.
 * \param use_global Use the global estimation method before the matching to perform "focussed search"?
 * \param mat If use_global is true, this keeps the global motion matrix.
 * \param rotation_correlation If use_global is true, this keeps rotation correlation coefficient.
 * \param translation_correlation If use_global is true, this keeps translation correlation coefficient.
 * \param used_max_distance If use_global is true, this contains the used search distance after the gme.
 * \return A Sparse weighted multi vectorfield containing all found matches.
 */
template <class T1, class T2, class MatchingFunctor>
SparseWeightedMultiVectorfield2D* matchFeaturesToImage(const vigra::MultiArrayView<2,T1>& src1,
                                                       const vigra::MultiArrayView<2,T2>& src2,
                                                       PointFeatureList2D const & features,
                                                       MatchingFunctor &  func,
                                                       unsigned int mask_width, unsigned int mask_height,
                                                       unsigned int max_distance,
                                                       unsigned int n_candidates,
                                                       bool use_global,
                                                       vigra::Matrix<double>& mat,
                                                       double & rotation_correlation, double & translation_correlation,
                                                       unsigned int & used_max_distance)
{
    vigra_precondition(src1.shape() == src2.shape(), "image shapes differ!");
    
    qDebug() << "Mask size: (" << mask_width<< ", " << mask_height << ")";
    
    using namespace ::std;
    using namespace ::vigra;
	
	unsigned int work_w   = (unsigned int) src1.width(),
                 work_h   = (unsigned int) src1.height();
	
	vigra::MultiArray<2,T2> s2_temp(src2.shape());
	s2_temp.init(0);
	
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
	
	used_max_distance = max(1, int(0.5 + max_distance - sqrt(mat(0,2)*mat(0,2) + mat(1,2)*mat(1,2))));
	
    unsigned int result_w = used_max_distance*2+mask_width+1,
			     result_h = used_max_distance*2+mask_height+1;
    
	///Create result image (will be used / updated for each features correlation)
	MultiArray<2,float>	result(result_w, result_h);
	
	//Create resulting vectorfield
	SparseWeightedMultiVectorfield2D* result_vf = new SparseWeightedMultiVectorfield2D;
	
	
	for(unsigned int i=0 ; i < features.size(); ++i)
    {
        unsigned int s1_x = vigra::round(features.position(i).x()),
                     s1_y = vigra::round(features.position(i).y());
		
		//Assure that source and transformed target coordinates are within mask bounds
		if(		s1_y  > mask_height/2	&& s1_y  < work_h-mask_height/2
			&&	s1_x  > mask_width/2	&& s1_x  < work_w-mask_width/2)
		{
			result.init(0);
			
			//Border threatment
			unsigned int search_upper = std::max((unsigned int) 0,		(unsigned int) (s1_y-used_max_distance-mask_height/2)),
                         search_left  = std::max((unsigned int) 0,		(unsigned int) (s1_x-used_max_distance-mask_width/2)),
                         search_lower = std::min(work_h,	(s1_y+used_max_distance+mask_height/2+1)),
                         search_right = std::min(work_w,	(s1_x+used_max_distance+mask_width/2+1)),
                         search_w = search_right - search_left,
                         search_h = search_lower - search_upper;
			
            MultiArrayView<2,float> s1(src1), s2(src2);
            
            //do the fast ncc
			func( s2.subarray( Shape2(search_left, search_upper), Shape2(search_right, search_lower)),
				  s1.subarray( Shape2(s1_x-mask_width/2, s1_y-mask_height/2),  Shape2(s1_x+mask_width/2+1, s1_y+mask_height/2+1)),
                  result.subarray(Shape2(0,0), Shape2(search_w,search_h)));
			
			
			unsigned int	result_w = search_right-search_left,
							result_h = search_lower-search_upper,
							max_x	 = result_w/2,
							max_y	 = result_h/2;
            
			typedef typename Vectorfield2D::PointType PointType;
			vector<PointType>	directions(n_candidates);
			vector<float>		weights(n_candidates);
			
            
                qDebug() << "\nSource: (" << search_w<<", " << search_h<< ")";
				for(unsigned int r_y=0; r_y<search_h; r_y++)
				{
                    qDebug() << "\n";
					for(unsigned int r_x=0; r_x<search_w; r_x++)
					{
                        qDebug() << s2.subarray( Shape2(search_left, search_upper), Shape2(search_right, search_lower))(r_x,r_y) << ", ";
                    }
                }
            
                qDebug() << "\nMask (" << mask_width<<", " << mask_height<< ")";
				for(unsigned int r_y=0; r_y<mask_height; r_y++)
				{
                    qDebug() << "\n";
					for(unsigned int r_x=0; r_x<mask_width; r_x++)
					{
                        qDebug() <<  s1.subarray( Shape2(s1_x-mask_width/2, s1_y-mask_height/2),  Shape2(s1_x+mask_width/2+1, s1_y+mask_height/2+1))(r_x,r_y) << ", ";
                    }
                }
            
                qDebug()<< "\nResult:";
				for(unsigned int r_y=0; r_y<result_h; r_y++)
				{
                    qDebug() << "\n";
					for(unsigned int r_x=0; r_x<result_w; r_x++)
					{
                        qDebug() << result(r_x,r_y) << ", ";
                    }
                }
            
			//collect N local maxima from the result image
			for(unsigned int c=0; c<n_candidates; c++)
			{
                qDebug()<< "\n";
				for(unsigned int r_y=0; r_y<result_h; r_y++)
				{
                    qDebug() << "\n";
					for(unsigned int r_x=0; r_x<result_w; r_x++)
					{
                        qDebug() << result(r_x,r_y) << ", ";
						if(	//	(r_x-result_w/2.0)*(r_x-result_w/2.0) + (r_y-result_h/2.0)*(r_y-result_h/2.0) < used_max_distance*used_max_distance
						   result(r_x,r_y) > result(max_x,max_y)
						   /*&&	result(r_x,r_y) > result(r_x-1,r_y-1)	&& result(r_x,r_y) > result(r_x,r_y-1)	&& result(r_x,r_y) > result(r_x+1,r_y-1)
						   &&	result(r_x,r_y) > result(r_x-1,r_y)												&& result(r_x,r_y) > result(r_x+1,r_y) 
						   &&	result(r_x,r_y) > result(r_x-1,r_y+1)	&& result(r_x,r_y) > result(r_x,r_y+1)	&& result(r_x,r_y) > result(r_x+1,r_y+1)
                            */)
	  					{
							max_x=r_x; max_y=r_y;
						}
					}
				}
				unsigned int s2_x = search_left  + max_x,
							 s2_y = search_upper + max_y;
				
				//s2 is in s1's coordinate system --> transform bach to I2's coords
				float	s2t_x = s2_x,//*mat(0,0) + s2_y*mat(0,1) + mat(0,2),
						s2t_y = s2_y;//s2_x*mat(1,0) + s2_y*mat(1,1) + mat(1,2);
				
				directions[c] = PointType(s2t_x - s1_x, s2t_y - s1_y);
				weights[c]   =  result(max_x,max_y);
				result(max_x,max_y)=0;
			}
			if(weights.size()>0)
			{
				result_vf->addVector(PointType(s1_x,s1_y),directions,weights);
			}
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
 * Helper class for the sorting of weighted target according to their weights.
 */
class WeightedTarget2D
{
	public:
        //Parameters
		int x, y;
		double weight;
    
        /**
         * The comparison operation.
         * A target is smaller/better if its weight is higher than that of another one.
         * 
         * \param rhs The other weighted target.
         * \return True, if the other weight is smaller.
         */
		bool operator < (const WeightedTarget2D& rhs) const
		{
			return rhs.weight < weight;
		}
};
    
/**
 * This class represents a functor for the (single) normalized correlation of an image
 * to another image of same size.
 */
class NormalizedCorrelationFunctor
{
    public:
        NormalizedCorrelationFunctor()
        {
        }
        
        template <class T1, class T2>
        double operator()(const vigra::MultiArrayView<2,T1> & src1,
                          const vigra::MultiArrayView<2,T2> & src2)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image shapes differ!");
            
            //find img1 mean
            vigra::FindAverage<double> average1;
            vigra::inspectImage(src1, average1);
            
            //find img2 mean
            vigra::FindAverage<double> average2;
            vigra::inspectImage(src2, average2);
            
            double	s1=0,s2=0,s11=0, s12=0, s22=0;
            
            
            
            auto i1 = src1.begin();
            auto i2 = src2.begin();
            
            for( ; i1 != src1.end() ; i1++, i2++ )
            {
                s1 = *i1;
                s2 = *i2;
                
                s12 += (s1-average1())*(s2-average2());
                s11 += (s1-average1())*(s1-average1());
                s22 += (s2-average2())*(s2-average2());
            }
            
            return s12/sqrt(s11*s22);
    }
};

/**
 * This class represents a functor for the (single) unnormalized correlation of an image
 * to another image of same size.
 */
class CorrelationFunctor
{
    public:
        CorrelationFunctor()
        {
        }
        
        template <class T1, class T2>
        double operator()(const vigra::MultiArrayView<2,T1> & src1,
                          const vigra::MultiArrayView<2,T2> & src2)
        {
            vigra_precondition(src1.shape() == src2.shape(), "image shapes differ!");
        
            double res=0;
            
            auto i1 = src1.begin();
            auto i2 = src2.begin();
            
            for( ; i1 != src1.end() ; i1++, i2++ )
            {
                res += (*i1) * (*i2);
            }
            return res/src1.size();
        }
};

/** 
 * Feature matching using features of the first image and features of the second image to search for
 * the N most likely features of the second image.
 * This function returns a (probability-)weighted 2-dimensional multi vectorfield holding the results.
 *
 * \param src1 The first image.
 * \param src2 The second image.
 * \param s1_features The features of the first image.
 * \param s2_features The features of the second image.
 * \param func The matching functor, which shall be used for comparison.
 * \param mask_width The width of the subimage, which will be compared for each feature.
 * \param mask_height The height of the subimage, which will be compared for each feature.
 * \param max_distance The maximal distance between the features of image 1 and image 2.
 * \param n_candidates The number of candidate matches, which shall be collected.
 * \param use_global Use the global estimation method before the matching to perform "focussed search"?
 * \param mat If use_global is true, this keeps the global motion matrix.
 * \param rotation_correlation If use_global is true, this keeps rotation correlation coefficient.
 * \param translation_correlation If use_global is true, this keeps translation correlation coefficient.
 * \param used_max_distance If use_global is true, this contains the used search distance after the gme.
 * \return A Sparse weighted multi vectorfield containing all found matches.
 */
template <class T1, class T2, class MatchingFunctor>
SparseWeightedMultiVectorfield2D* matchFeaturesToFeatures(const vigra::MultiArrayView<2,T1> & src1,
                                                          const vigra::MultiArrayView<2,T2> & src2,
                                                          PointFeatureList2D const & s1_features,
                                                          PointFeatureList2D const & s2_features,
                                                          MatchingFunctor & func,
                                                          unsigned int mask_width, unsigned int mask_height,
                                                          unsigned int max_distance,
                                                          unsigned int n_candidates,
                                                          bool use_global,
                                                          vigra::Matrix<double>& mat,
                                                          double & rotation_correlation, double & translation_correlation,
                                                          unsigned int & used_max_distance)
{
    vigra_precondition(src1.shape() == src2.shape(), "image shapes differ!");
    
    qDebug() << "Mask size: (" << mask_width<< ", " << mask_height << ")";
    
    using namespace ::std;
    using namespace ::vigra;
	
	unsigned int work_w   = (unsigned int) src1.width(),
                 work_h   = (unsigned int) src1.height();
    
	vigra::MultiArray<2,T2> s2_temp(src2.shape());
	s2_temp.init(0);
	
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
	
	used_max_distance = max(1,int(0.5 + max_distance - sqrt(mat(0,2)*mat(0,2) + mat(1,2)*mat(1,2))));
	
	//Create resulting vectorfield
	SparseWeightedMultiVectorfield2D*  result_vf = new SparseWeightedMultiVectorfield2D;
		
	for(unsigned int i=0 ; i < s1_features.size(); ++i)
	{ 
        //Source image point coordinates
        unsigned int s1_x = vigra::round(s1_features.position(i).x()),
					 s1_y = vigra::round(s1_features.position(i).y());
		
		//check if feature is inside mask bounds
		if(		s1_y > mask_height/2	&&	s1_y < (unsigned int)work_h-mask_height/2	
		   &&	s1_x > mask_width/2		&&	s1_x < (unsigned int)work_w-mask_width/2)
		{
			list<WeightedTarget2D>		candidates_list;
			
			for(unsigned int j=0 ; j < s2_features.size(); ++j)
			{ 
                //Destination image point coordinates
                int s2_x = vigra::round(s2_features.position(j).x()),
                    s2_y = vigra::round(s2_features.position(j).y());
                
                
                //s2 is in s1's coordinate system --> transform bach to I2's coords
                float	s2t_x = s2_x*mat(0,0) + s2_y*mat(0,1) + mat(0,2),
                        s2t_y = s2_x*mat(1,0) + s2_y*mat(1,1) + mat(1,2);
				
				//Assure that source and transformed target coordinates are within mask bounds (and within search space)
				if(		(s1_x-s2t_x)*(s1_x-s2t_x) + (s1_y-s2t_y)*(s1_y-s2t_y) < used_max_distance*used_max_distance
				   &&	s2t_y  > mask_height/2	&&	s2t_y  < work_h-mask_height/2	
				   &&	s2t_x  > mask_width/2	&&	s2t_x  < work_w-mask_width/2){
					
					//Potential candidate found! Now calculate the normalized cross correlation and add to list
					double weight =	func(	src1.subarray(  Shape2(s1_x-mask_width/2,s1_y-mask_height/2),
                                                            Shape2(s1_x+mask_width/2,s1_y+mask_height/2)),
											src2.subarray(  Shape2(vigra::round(s2t_x)-mask_width/2,vigra::round(s2t_y)-mask_height/2),
                                                            Shape2(vigra::round(s2t_x)+mask_width/2,vigra::round(s2t_y)+mask_height/2)) );
					
					WeightedTarget2D target;
					target.x=s2_x; 
					target.y=s2_y; 
					target.weight=weight;
					candidates_list.push_back(target);
				}
			}
			if(candidates_list.size()>0)
			{
				candidates_list.sort();
				
				typedef typename Vectorfield2D::PointType PointType;
				vector<PointType>	dirs(n_candidates);
				vector<float>		weights(n_candidates);
				
				
				//initialize vectors
				for(unsigned int c=0; c<n_candidates; ++c)
				{
					dirs[c] = PointType(0.0, 0.0);
					weights[c] = 0.0;
				}
				
				list<WeightedTarget2D>::iterator iter = candidates_list.begin();
				for(unsigned int c=0; c<n_candidates && iter!= candidates_list.end(); ++c, ++iter)
				{
					dirs[c] = PointType(iter->x-s1_x, iter->y-s1_y);
					weights[c] = iter->weight;
				}
				
				result_vf->addVector(PointType(s1_x,s1_y),dirs,weights);
			}
			
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
 * This function transforms a featurelist into a vector/list of corresponding shape contexts.
 *
 * \param features The feature list, which shall be transformed.
 * \param max_radius The maximal radius used for shape context creation.
 * \param angle_bins The count of bins for angular sampling for shape context creation.
 * \return The shape contexts of the input features.
 */
std::vector<vigra::MultiArray<2, unsigned int> > createShapeContexts(PointFeatureList2D const & features, double max_radius, int angle_bins)
{
    using namespace ::std;
    using namespace ::vigra;
    
	int radius_bins = int(log(max_radius))+1;
	
	vector<vigra::MultiArray<2, unsigned int> > shape_contexts;

	for(unsigned int i=0 ; i < features.size(); ++i)
    {
		//Source image point coordinates
		int s1_x = vigra::round(features.position(i).x()),
            s1_y = vigra::round(features.position(i).y());
		
		//create new shape context image for ith feature
		vigra::MultiArray<2, unsigned int> shape_context(angle_bins,radius_bins);
				 
		for(unsigned int j=0 ; j < features.size(); ++j)
        {
            //Destination image point coordinates
            int s2_x = vigra::round(features.position(j).x()),
                s2_y = vigra::round(features.position(j).y());
				 
			//Test if potential target is within radius distance
			float dist2 = (s1_x-s2_x)*(s1_x-s2_x) + (s1_y-s2_y)*(s1_y-s2_y);
			
			if(i!=j && dist2 < max_radius*max_radius)
			{
				float angle=std::atan2(float(s1_y-s2_y),float(s1_x-s2_x));
				shape_context(min(int((M_PI+angle)/(2*M_PI)*angle_bins),angle_bins-1), log(sqrt(dist2)))++;
			}
		}
		shape_contexts.push_back(shape_context);
	}
	return shape_contexts;
}

/**
 * Shape context matching functionality.
 * 
 * \param src1 The first shape context.
 * \param src2 The second shape context.
 */
template <class T1, class T2>
double shapecontext_cc(const vigra::MultiArray<2, T1> & src1, const vigra::MultiArray<2, T2> & src2)
{
    vigra_precondition(src1.shape() == src2.shape(), "image shapes differ!");
    
    double p1 = src1.template sum<double>(),
           p2 = src2.template sum<double>();
    
    
    auto i1 = src1.begin();
    auto i2 = src2.begin();
    
    double s1,s2, sum=0;
    
    for( ; i1 != src1.end(); ++i1, ++i2)
    {
        s1   = (*i1)/p1;
        s2   = (*i2)/p2;

        if(s1+s2 > 0)
        {
            sum += (s1-s2)*(s1-s2)/(s1+s2);
        }
    }
    return 1.0- 0.5*sum;
}

/** 
 * Feature matching using features of the first image and features of the second image to search for
 * the N most likely features of the second image. The features will therefore been transformed into
 * Shape Contexts structures.
 * This function returns a (probability-)weighted 2-dimensional multi vectorfield holding the results.
 *
 * \param src1 The first image.
 * \param src2 The second image.
 * \param s1_features The features of the first image.
 * \param s2_features The features of the second image.
 * \param func The matching functor, which shall be used for comparison.
 * \param mask_width The width of the subimage, which will be compared for each feature.
 * \param mask_height The height of the subimage, which will be compared for each feature.
 * \param max_distance The maximal distance between the features of image 1 and image 2.
 * \param n_candidates The number of candidate matches, which shall be collected.
 * \param use_global Use the global estimation method before the matching to perform "focussed search"?
 * \param mat If use_global is true, this keeps the global motion matrix.
 * \param rotation_correlation If use_global is true, this keeps rotation correlation coefficient.
 * \param translation_correlation If use_global is true, this keeps translation correlation coefficient.
 * \param used_max_distance If use_global is true, this contains the used search distance after the gme.
 * \return A Sparse weighted multi vectorfield containing all found matches.
 */
template <class T1, class T2>
SparseWeightedMultiVectorfield2D* matchFeaturesToFeaturesUsingShapeContext(const vigra::MultiArrayView<2,T1>& src1,
                                                                           const vigra::MultiArrayView<2,T2>& src2,
                                                                           PointFeatureList2D const & s1_features,
                                                                           PointFeatureList2D const & s2_features,
                                                                           unsigned int mask_width, unsigned int mask_height,
                                                                           unsigned int max_distance,
                                                                           unsigned int n_candidates,
                                                                           bool use_global,
                                                                           vigra::Matrix<double>& mat,
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

	used_max_distance = max(1, int(0.5 + max_distance- sqrt(mat(0,2)*mat(0,2) + mat(1,2)*mat(1,2))));
	
	
	//Create resulting vectorfield
	SparseWeightedMultiVectorfield2D* result_vf = new SparseWeightedMultiVectorfield2D;
	
	unsigned int max_radius = max(mask_width,mask_height)/2.0,
			     angle_bins = 8;
	
	//1. step: build shape context
	vector<vigra::MultiArray<2, unsigned int> >  shape_contexts1 = createShapeContexts(s1_features, max_radius, angle_bins),
                                                 shape_contexts2 = createShapeContexts(s2_features, max_radius, angle_bins);
	
	for(unsigned int i=0 ; i < s1_features.size(); ++i)
	{ 
        //Source image point coordinates
        int s1_x = vigra::round(s1_features.position(i).x()),
            s1_y = vigra::round(s1_features.position(i).y());
		
		//check if feature has an assigned shape context and is inside mask bounds
		if(	shape_contexts1[i](0,0)!=-1 )
        {
			list<WeightedTarget2D>		candidates_list;
			
			for(unsigned int j=0 ; j < s2_features.size(); ++j)
			{ 
                //Destination image point coordinates
                int s2_x = vigra::round(s2_features.position(j).x()),
                    s2_y = vigra::round(s2_features.position(j).y());
                
                //s2 is in s1's coordinate system --> transform bach to I2's coords
                float	s2t_x = s2_x*mat(0,0) + s2_y*mat(0,1) + mat(0,2),
                        s2t_y = s2_x*mat(1,0) + s2_y*mat(1,1) + mat(1,2);
				
				//Assure that source and transformed target coordinates are within search space
				if(	shape_contexts2[j](0,0)!=-1
					&&	(s1_x-s2t_x)*(s1_x-s2t_x) + (s1_y-s2t_y)*(s1_y-s2t_y) < used_max_distance*used_max_distance)
				{
					
					//Potential candidate found! Now calculate the normalized cross correlation of both shape_contexts
					double weight =	shapecontext_cc(shape_contexts1[i], shape_contexts2[j]);
					
					WeightedTarget2D target;
					target.x=s2_x; 
					target.y=s2_y; 
					target.weight=weight;
					candidates_list.push_back(target);
				}
			}
			if(candidates_list.size()>0){
				candidates_list.sort();
				
				typedef typename Vectorfield2D::PointType PointType;
				vector<PointType>	dirs(n_candidates);
				vector<float>		weights(n_candidates);
				
				
				 //initialize vectors
				for(unsigned int c=0; c<n_candidates; ++c)
				{
					dirs[c] = PointType(0,0);
					weights[c] = 0.0;
				}
				
				list<WeightedTarget2D>::iterator iter = candidates_list.begin();
				for(unsigned int c=0; c<n_candidates && iter!= candidates_list.end(); ++c, ++iter){
					dirs[c] = PointType(iter->x-s1_x, iter->y-s1_y);
					weights[c] = iter->weight;
				}
				
				result_vf->addVector(PointType(s1_x,s1_y),dirs,weights);
			}
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

} //end of namespace graipe

#endif //GRAIPE_FEATUREMATCHING_MATCHPOINTFEATURES_HXX
