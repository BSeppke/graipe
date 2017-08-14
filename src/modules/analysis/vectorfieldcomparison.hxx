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

#ifndef GRAIPE_ANALYSIS_VECTORFIELDCOMPARISON_HXX
#define GRAIPE_ANALYSIS_VECTORFIELDCOMPARISON_HXX

#include "vectorfields/vectorfields.h"
#include "features2d/features2d.h"

#include <vigra/splineimageview.hxx>

namespace graipe {

/**
 * @addtogroup graipe_analysis
 * @{
 *
 * @file
 * @brief Comparison functors and functions for the (vectorfields) analysis module
 */

/**
 * A small helper class to define weighted indexes.
 */
class WeightedIndex
{
	public:
		/**
         * A target is smaller/better if its weight is smaller than that of another one.
         * 
         * \param rhs The other target
         * \return True, if my weight is smaller than the others
         */
		bool operator<(const WeightedIndex& rhs) const
        {
			return weight < rhs.weight;
		}
    
        //Members, directly accessible from outside
    
        /** The index **/
        unsigned int idx;
        /** the weight **/
		double weight;
};




/** 
 * A functor class interface for comparison of directions with
 * respect to their error/difference measures.
 */
class VectorDirectionErrorFunctor
{
    public:
        /**
         * The functor call for comparison.
         *
         * \param dir1_x x-direction of vector 1.
         * \param dir1_y y-direction of vector 1.
         * \param dir2_x x-direction of vector 2.
         * \param dir2_y y-direction of vector 2.
         * \return The error between both vectors in degrees.
         */
        virtual float operator()(double dir1_x, double dir1_y, double dir2_x, double dir2_y) const = 0;
    
        /**
         * Returns the short (abbrev.) name of this functor.
         */
        virtual QString shortName() const = 0;
    
        /**
         * Returns the (full) name of this functor.
         */
        virtual QString name() const = 0;
    
        /**
         * Returns the units name of this functor.
         */
        virtual QString units() const = 0;
};




/** 
 * A functor class for comparison of directions with
 * respect to their average angular error.
 */
class AverageAngularErrorFunctor
:   public VectorDirectionErrorFunctor
{
    public:
        /**
         * The functor call for average angle comparison.
         *
         * \param dir1_x x-direction of vector 1.
         * \param dir1_y y-direction of vector 1.
         * \param dir2_x x-direction of vector 2.
         * \param dir2_y y-direction of vector 2.
         * \return The angular difference between both vectors in degrees.
         *         If one of the vectors has no length, 0 is returned.
         */
        float operator()(double dir1_x, double dir1_y, double dir2_x, double dir2_y) const
        {
            double l1 = sqrt(	dir1_x*dir1_x + dir1_y*dir1_y);
            double l2 = sqrt(	dir2_x*dir2_x + dir2_y*dir2_y);
            
            if(l1 != 0 && l2 != 0)
            {
                return acos(  (dir1_x*dir2_x + dir1_y*dir2_y) / ( l1 * l2))*180.0/M_PI;
            }
            else 
            {
                //if one or both are points, no angular error can be determined...
                return 0;
            }
        }
    
        /**
         * Returns the short (abbrev.) name of this functor.
         *
         * \return Always: "AAE"
         */
        QString shortName() const
        {
            return "AAE";
        }
    
        /**
         * Returns the (full) name of this functor.
         *
         * \return Always: "Average angular error"
         */
        QString name() const
        {
            return "Average angular error";
        }
    
        /**
         * Returns the units name of this functor.
         *
         * \return Always: "°"
         */
        QString units() const
        {
            return "°";
        }
};




/** 
 * A functor class for comparison of directions with
 * respect to their average velocity error.
 */
class AverageVelocityErrorFunctor
:   public VectorDirectionErrorFunctor
{
    public:
        /**
         * The functor call for average angle comparison.
         *
         * \param dir1_x x-direction of vector 1.
         * \param dir1_y y-direction of vector 1.
         * \param dir2_x x-direction of vector 2.
         * \param dir2_y y-direction of vector 2.
         * \return The velocity difference between both vectors in cm/s.
         */
        float operator()(double dir1_x, double dir1_y, double dir2_x, double dir2_y) const
        {
            return sqrt((dir1_x-dir2_x)*(dir1_x-dir2_x) + (dir1_y-dir2_y)*(dir1_y-dir2_y));
        }
    
        /**
         * Returns the short (abbrev.) name of this functor.
         *
         * \return Always: "AVE"
         */
        QString shortName() const
        {
            return "AVE";
        }
    
        /**
         * Returns the (full) name of this functor.
         *
         * \return Always: "Average velocity error"
         */
        QString name() const
        {
            return "Average velocity error";
        }
    
        /**
         * Returns the units name of this functor.
         *
         * \return Always: "°"
         */
        QString units() const
        {
            return "cm/s°";
        }
};

/**
 * Comparison of any given vectorfield w.r.t. a dense vectorfield
 * Since the dense vectorfield may not directly cover the positions of the given vectorfield,
 * a spline interpolation will be used to estimate the dense vectorfields direction at the 
 * other vectorfield's positions. The order of the spline interpolation can by set by means of
 * a template parameter: int SPLINE_ORDER
 *
 * \param vf Vectorfield of any kind, which shall be compared.
 * \param reference_vf Dense reference vectorfield, which forms the base for comparision.
 * \param error_measure The functor for error estimation.
 * \param report The reference to a QString, in which the report will be stored.
 * \return A weighted point list containing the comparision results for each vector of vf.
 */
template <int SPLINE_ORDER>
WeightedPointFeatureList2D* compareVectorfieldToDenseModel(Vectorfield2D* vf,
                                                           DenseVectorfield2D* reference_vf,
                                                           VectorDirectionErrorFunctor& error_measure,
                                                           QString & report)
{
	WeightedPointFeatureList2D* comparison = new WeightedPointFeatureList2D(vf->workspace());
	
	double error=0, error2=0;
	double single_error;
	
	unsigned int  count=0;
	
	bool invertible;
	
	const QTransform	t_vf			= vf->globalTransformation(),
						t_reference_vf	= reference_vf->globalTransformation(),
						inv_t_ref_vf	= t_reference_vf.inverted(&invertible);
	
	vigra_precondition(invertible, "Transformation matrix of second vectorfield is not invertible!");
	
    vigra::SplineImageView<SPLINE_ORDER, float> spi_u(srcImageRange(reference_vf->u()));
    vigra::SplineImageView<SPLINE_ORDER, float> spi_v(srcImageRange(reference_vf->v()));

	for (unsigned int l1=0; l1 < vf->size(); ++l1)
	{
		QPointF p_tar = t_vf.map(vf->target(l1)),
				p_ori = t_vf.map(vf->origin(l1));
		
		double	dir_x = p_tar.x()-p_ori.x(),
				dir_y = p_tar.y()-p_ori.y();
				
		QPointF ref_img_p = inv_t_ref_vf.map(p_ori);
		
		//get direction information in image coordinates
		double	reference_dir_x = spi_u(ref_img_p.x(), ref_img_p.y()),
				reference_dir_y = spi_v(ref_img_p.x(), ref_img_p.y());
		
		//prject direction information to world coordinates
		QPointF	r_tar = t_reference_vf.map(ref_img_p + QPointF(reference_dir_x, reference_dir_y)),
				r_ori = t_reference_vf.map(ref_img_p);
		
		reference_dir_x = r_tar.x()-r_ori.x();
		reference_dir_y = r_tar.y()-r_ori.y();
		
		if(vf->scale() != 0 && reference_vf->scale() != 0)
		{
			single_error = error_measure(dir_x*vf->scale(),						dir_y*vf->scale(), 
										 reference_dir_x*reference_vf->scale(),	reference_dir_y*reference_vf->scale());
		}
		else
		{
			single_error = error_measure(dir_x,				dir_y,
										 reference_dir_x,	reference_dir_y);
		}
		
		//qDebug() << "status: " << (l1*100.0 / vf->size()) << "% err:" << single_error << " " << error_measure.units() << "\n";
		
		comparison->addFeature(vf->origin(l1), single_error);
		
		error  += single_error;	
		error2 += single_error*single_error;
		count++;
	}
	
	report += QString("%1 error    is: %2 %3\n").arg(error_measure.shortName()).arg(error/count).arg(error_measure.units())
            + QString("%1 variance is: %2 %3^2\n").arg(error_measure.shortName()).arg((error2 - ((error*error)/count))/count).arg(error_measure.units())
            + QString("%1 std.dev. is: %2 %3\n").arg(error_measure.shortName()).arg(sqrt((error2 - ((error*error)/count))/count)).arg(error_measure.units());
	
	return comparison;
}


/**
 * Comparison of any given vectorfield w.r.t. any other kind of vectorfield
 * Since the second vectorfield may not be dense, we provide an additional parameter, which
 * determines how many points (in the neighborhood) will bei used for comparison. Each point will be
 * weighted for the comparions by means of an inverse distance weight.
 *
 * \param vf Vectorfield of any kind, which shall be compared.
 * \param reference_vf The reference vectorfield, which forms the base for comparision.
 * \param n_nearest_neighbors The number of nearest neighbors in reference_vf for comparision.
 * \param error_measure The functor for error estimation.
 * \param report The reference to a QString, in which the report will be stored.
 * \return A weighted point list containing the comparision results for each vector of vf.
 */
WeightedPointFeatureList2D* compareVectorfieldsGeneric( Vectorfield2D* vf,
                                                        const Vectorfield2D* reference_vf,
                                                        unsigned int n_nearest_neighbors,
                                                        VectorDirectionErrorFunctor& error_measure,
                                                        QString & report)
{
	WeightedPointFeatureList2D* comparison = new WeightedPointFeatureList2D(vf->workspace());
	
	double error=0, error2=0;
	double single_error;
	
	unsigned int  count=0;
	
	const QTransform	t_vf			= vf->globalTransformation(),
						t_reference_vf	= reference_vf->globalTransformation();
	
	for (unsigned int l1=0; l1 < vf->size(); ++l1)
	{
        std::list<WeightedIndex> candidates;
		
		//find out distance for each vector of reference vf
		for (unsigned int l2=0; l2 < reference_vf->size(); ++l2)
		{
			QPointF p1 = t_vf.map(vf->origin(l1)),
					p2 = t_reference_vf.map(reference_vf->origin(l2));
			
			double cur_dist = sqrt( pow(p1.x()-p2.x(),2) +  pow(p1.y()-p2.y(),2));
			WeightedIndex wi; wi.idx = l2; wi.weight = cur_dist;
			candidates.push_back(wi);
		}
		//sort all by distance
		candidates.sort();
			
		double	mean_reference_dir_x=0, mean_reference_dir_y=0, 
				sum_w=0;
		
        std::list<WeightedIndex>::iterator c_iter = candidates.begin();
		
		unsigned int c = 0;
		for ( ; c_iter != candidates.end() && c<n_nearest_neighbors; ++c, ++c_iter)
		{
			double weight =   1.0/(1.0 + c_iter->weight);
			
			QPointF p1 = t_reference_vf.map(reference_vf->target(c_iter->idx)),
					p2 = t_reference_vf.map(reference_vf->origin(c_iter->idx));
			
			mean_reference_dir_x+=weight*(p1.x()-p2.x());	//weighted x-diff
			mean_reference_dir_y+=weight*(p1.y()-p2.y());	//weighted y-diff
			sum_w+=weight;									//weight
		}
		
		mean_reference_dir_x /=sum_w;
		mean_reference_dir_y /=sum_w;
		
		
		QPointF p1 = t_vf.map(vf->target(l1)),
				p2 = t_vf.map(vf->origin(l1));
		
		double	dir_x = p1.x()-p2.x(),
				dir_y = p1.y()-p2.y();
		
		
		if(vf->scale() != 0 && reference_vf->scale() != 0)
		{
			single_error = error_measure(dir_x*vf->scale(),								dir_y*vf->scale(), 
										 mean_reference_dir_x*reference_vf->scale(),	mean_reference_dir_y*reference_vf->scale());
		}
		else
		{
			single_error = error_measure(dir_x,					dir_y,
										 mean_reference_dir_x,	mean_reference_dir_y);
		}
			
		//qDebug() << "status: " << (l1*100.0 / vf->size()) << "% err:" << single_error << " " << error_measure.units() << "\n";
		
		comparison->addFeature(vf->origin(l1), single_error);
		
		error  += single_error;	
		error2 += single_error*single_error;
		count++;
	}
	
	report += QString("%1 error    is: %2 %3\n").arg(error_measure.shortName()).arg(error/count).arg(error_measure.units())
            + QString("%1 variance is: %2 %3^2\n").arg(error_measure.shortName()).arg((error2 - ((error*error)/count))/count).arg(error_measure.units())
            + QString("%1 std.dev. is: %2 %3\n").arg(error_measure.shortName()).arg(sqrt((error2 - ((error*error)/count))/count)).arg(error_measure.units());
	
	return comparison;
}

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_ANALYSIS_VECTORFIELDCOMPARISON
