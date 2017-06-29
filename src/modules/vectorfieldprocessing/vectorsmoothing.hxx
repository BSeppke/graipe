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

#ifndef GRAIPE_VECTORFIELDPROCESSING_VECTORSMOOTHING_HXX
#define GRAIPE_VECTORFIELDPROCESSING_VECTORSMOOTHING_HXX

#include <vector>

#include <vigra/stdimage.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/gaussians.hxx>

#include "vectorfields/vectorfields.h"

namespace graipe {

/**
 * This function smooths a sparse weighted multi vectorfield using the neighbored vectors above a
 * certain weight and/or the vectorfields alternative directions. The smoothing will be controlled
 * by means of a (distance-weighted) Gaussian function with mean 0 and a scale of max_geo_distance/3.0.
 * The influence of each used vector for smoothing is scaled by its weight in addition.
 *
 * \param vectorfield The vectorfield to be smoothed.
 * \param max_iterations The max. count of smoothing iterations.
 * \param max_geo_distance The max. geometric distance between two points to be taken into account for smoothing.
 * \param min_weight The min. weight of points to be taken into account for smoothing.
 * \param useAllCandidates Use all candidate vectors for smoothing.
 * \return The new, smoothed vectorfield.
 */
SparseWeightedVectorfield2D* smoothVectorfield(SparseWeightedMultiVectorfield2D * vectorfield,
												 int max_iterations=10,
												 float max_geo_distance=10.0,
												 float min_weight=0.0,
												 bool useAllCandidates = true)
{
	
    int feature_count = vectorfield->size();					//count of features
	
	SparseWeightedVectorfield2D * result_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
	SparseWeightedVectorfield2D * work_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
		
    result_vectorfield->setGlobalMotion(vectorfield->globalMotion());
    work_vectorfield->setGlobalMotion(vectorfield->globalMotion());
	
	
	vigra::Gaussian<double> gauss( max_geo_distance/3.0 );
    
    //Prepare adjacency matrix
	std::vector<std::vector<int> > adjacency(feature_count);
    for (int i=0; i< feature_count; ++i)
	{
		result_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), vectorfield->weight(i));
		work_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), vectorfield->weight(i));
		
		for (int j=0; j<=i; ++j)
		{
            if(    (QPointFX(vectorfield->origin(i)-vectorfield->origin(j)).squaredLength()	< max_geo_distance*max_geo_distance)	// distance small enough
				&& (vectorfield->weight(i)>min_weight))							// corr > thresh
			{									// corr != NaN	
				adjacency[i].push_back(j); //j is neighboured to i and thus
				adjacency[j].push_back(i); //i is neighboured to j!
            }
        }
    }
	
	//Initialize the resulting vectorfield by using either all alternatives
	//or just the best vectors for the first iteration
	
    for (int i=0; i< feature_count; ++i)
	{
	    //calculate mean shifts using all neighbored features
		SparseWeightedVectorfield2D::PointType mean_direction;
		double	sum_wg = 0.0;
		double	sum_g = 0.0;
			
		for (unsigned int adj_idx=0; adj_idx<adjacency[i].size(); ++adj_idx)
		{
			unsigned int j = adjacency[i][adj_idx];
			
            //distance weighted mean needs a weight for each direction
            double g  = gauss(QPointFX(vectorfield->origin(i) - vectorfield->origin(j)).length());
            double w = vectorfield->weight(j);
            
            mean_direction	+= w*g*(vectorfield->direction(j)); 
            sum_wg			+= w*g;
            sum_g           += g;
			
			if (useAllCandidates)
            {
                for(unsigned int alt_idx = 0; alt_idx != vectorfield->alternatives(); ++alt_idx)
                {
                    //distance weighted mean needs a weight for each direction
                    double g  = gauss(QPointFX(vectorfield->origin(i) - vectorfield->origin(j)).length());
                    double w = vectorfield->altWeight(j,alt_idx);
                    
                    mean_direction	+= w*g*(vectorfield->altDirection(j,alt_idx));
                    sum_wg			+= w*g;
                    sum_g           += g;
                }
            }
		}
		if (sum_g != 0.0)
		{
			//save the new vector at current step
			mean_direction	/= sum_wg;
			sum_wg			/= sum_g;
		
			result_vectorfield->setDirection(i, mean_direction);
			result_vectorfield->setWeight(i, sum_wg);
		}
		else {
			result_vectorfield->setWeight(i, 0.0);
		}

	}
	
	// For all other iterations:
	// 1. swap result and temp_vf
	// 2. take the values out of the temp_vf
	// 3. write the smoothed values into result_vf 	
    for(int iteration=2; iteration<=max_iterations; iteration++) 
	{  
    	//Initialise current iteration step
		std::swap( work_vectorfield, result_vectorfield );
		
		//For all features
        for (int i=0; i< feature_count; ++i)
		{
		    //calculate mean shifts using all neighbored features
			SparseWeightedMultiVectorfield2D::PointType mean_direction;
			
			double	sum_wg = 0.0;
			double	sum_g = 0.0;
			
			for (unsigned int adj_idx=0; adj_idx<adjacency[i].size(); ++adj_idx)
			{
				unsigned int j = adjacency[i][adj_idx];
				
                //distance weighted mean needs a weight for each direction
                double g  = gauss(QPointFX(vectorfield->origin(i) - vectorfield->origin(j)).length());
				double w = work_vectorfield->weight(j);
				
				mean_direction	+= w*g*(work_vectorfield->direction(j)); 
				sum_wg			+= w*g;
				sum_g           += g;
			}
			
			if (sum_g != 0.0)
			{
				//save the new vector at current step
				mean_direction	/= sum_wg;
				sum_wg			/= sum_g;
				
				result_vectorfield->setDirection(i, mean_direction);
				result_vectorfield->setWeight(i, sum_wg);
			}
			else {
				result_vectorfield->setWeight(i, 0.0);
			}
		}
    }
	//Clean temp vf
	delete work_vectorfield;
	
	//Return result
	return result_vectorfield;
}

/**
 * This function relaxes a sparse weighted multi vectorfield using the neighbored vectors above a
 * certain weight and/or the vectorfields alternative directions. The relaxation will be controlled
 * by means of a (distance-weighted) Gaussian function with mean 0 and a scale of max_geo_distance/3.0.
 * The influence of each used vector for smoothing is scaled by its weight in addition. The procedure will 
 * first find a representative using (all alternatives +) the neighbored vectors and then select the best-most
 * fitting alternative instead the original one.
 *
 * \param vectorfield The vectorfield to be smoothed.
 * \param max_iterations The max. count of smoothing iterations.
 * \param max_geo_distance The max. geometric distance between two points to be taken into account for smoothing.
 * \param min_weight The min. weight of points to be taken into account for smoothing.
 * \param useAllCandidates Use all candidate vectors for smoothing.
 * \return The new, relaxed vectorfield.
 */
SparseWeightedVectorfield2D * relaxVectorfield(SparseWeightedMultiVectorfield2D * vectorfield,
													  int max_iterations=10,
													  float max_geo_distance=10.0,
													  float min_weight=0.0,
													  bool useAllCandidates=true)
{
	int feature_count = vectorfield->size();					//count of features

	SparseWeightedVectorfield2D * result_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
	SparseWeightedVectorfield2D * work_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());

	result_vectorfield->setGlobalMotion(vectorfield->globalMotion());
	work_vectorfield->setGlobalMotion(vectorfield->globalMotion());
	
	vigra::Gaussian<double> gauss( max_geo_distance/3.0 );

	//Prepare adjacency matrix
	std::vector<std::vector<int> > adjacency(feature_count);
	for (int i=0; i< feature_count; ++i)
	{
		result_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), vectorfield->weight(i));
		work_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), vectorfield->weight(i));
		
		for (int j=0; j<=i; ++j)
		{
			if(    (QPointFX(vectorfield->origin(i)-vectorfield->origin(j)).squaredLength()	< max_geo_distance*max_geo_distance)	// distance small enough
			   && (vectorfield->weight(i)>min_weight))							// corr > thresh
			{									// corr != NaN	
				adjacency[i].push_back(j); //j is neighboured to i and thus
				adjacency[j].push_back(i); //i is neighboured to j!
			}
		}
	}
	
	//Initialize the resulting vectorfield by using either all alternatives
	//or just the best vectors for the first iteration
	for (int i=0; i< feature_count; ++i)
	{
		//calculate mean shifts using all neighbored features
		SparseWeightedVectorfield2D::PointType mean_direction;
		double	sum_wg = 0.0;
		double	sum_g = 0.0;
		
		for (unsigned int adj_idx=0; adj_idx<adjacency[i].size(); ++adj_idx)
		{
			unsigned int j = adjacency[i][adj_idx];
					
            //distance weighted mean needs a weight for each direction
            double g  = gauss(QPointFX(vectorfield->origin(i) - vectorfield->origin(j)).length());
            double w = vectorfield->weight(j);
            
            mean_direction	+= w*g*(vectorfield->direction(j)); 
            //sum_wg			+= w*g;
            sum_g           += g;
            
			if (useAllCandidates)
			{
            	for(unsigned int alt_idx = 0; alt_idx != vectorfield->alternatives(); ++alt_idx)
				{	
					
                    //distance weighted mean needs a weight for each direction
                    double g  = gauss(QPointFX(vectorfield->origin(i) - vectorfield->origin(j)).length());
					double w = vectorfield->altWeight(j,alt_idx);
					
					mean_direction	+= w*g*(vectorfield->altDirection(j,alt_idx));
					sum_wg			+= w*g;
					sum_g           += g;
				}
            }
		}
		
		if (sum_g != 0.0)
		{
			//save the new vector at current step
			mean_direction	/= sum_wg;
			//sum_wg			/= sum_g;
		
			//Instead of smoothing the current vector -> select best alternative
			//result_vectorfield->setDirection(i, mean_direction);
			//result_vectorfield->setWeight(i, sum_wg);
		
			//find best fitting vector out of the candidates
			int best_idx=0;
			double best_fit=0;
        
            double fit =   vectorfield->weight(i)
                         * vectorfield->direction(i).dot(mean_direction)	// This formula calculates the cosine
                         / QPointFX(vectorfield->direction(i)).length()		// of the angle between the current alternative
                         / QPointFX(mean_direction).length();				// vector and the representative (value of 1 = 0°)
        
            if(fit > best_fit)
            {
                best_fit = fit;
                best_idx=-1;
            }
            
			for (unsigned int alt_idx=0; alt_idx<vectorfield->alternatives(); ++alt_idx)
			{
				fit =   vectorfield->altWeight(i,alt_idx)
							 * vectorfield->altDirection(i,alt_idx).dot(mean_direction)	// This formula calculates the cosine
							 / QPointFX(vectorfield->altDirection(i,alt_idx)).length()	// of the angle between the current alternative
							 / QPointFX(mean_direction).length();						// vector and the representative (value of 1 = 0°)
			
				if(fit > best_fit)
				{
					best_fit = fit;
					best_idx=alt_idx;
				}
			}
            
            if(best_idx == -1)
            {
                result_vectorfield->setDirection(i,vectorfield->direction(i));
                result_vectorfield->setWeight(i,vectorfield->weight(i));
            }
            else
            {
                result_vectorfield->setDirection(i,vectorfield->altDirection(i,best_idx));
                result_vectorfield->setWeight(i,vectorfield->altWeight(i,best_idx));
            }
		}
		else
		{
			result_vectorfield->setWeight(i, 0.0);
		}
	}
	
	// For all other iterations:
	// 1. swap result and temp_vf
	// 2. take the values out of the temp_vf
	// 3. write the smoothed values into result_vf 	
    for(int iteration=2; iteration<=max_iterations; iteration++) 
	{  
    	//Initialise current iteration step
		std::swap( work_vectorfield, result_vectorfield );
		
		//For all features
        for (int i=0; i< feature_count; ++i)
		{
		    //calculate mean shifts using all neighbored features
			SparseWeightedMultiVectorfield2D::PointType mean_direction;
			
			double	sum_wg = 0.0;
			double	sum_g = 0.0;
			
			for (unsigned int adj_idx=0; adj_idx<adjacency[i].size(); ++adj_idx)
			{
				unsigned int j = adjacency[i][adj_idx];
				
				//distance weighted mean needs a weight for each direction
				double g  = gauss(QPointFX(work_vectorfield->origin(i) - work_vectorfield->origin(j)).length());
				double w = work_vectorfield->weight(j);
				
				mean_direction	+= w*g*(work_vectorfield->direction(j)); 
				//sum_wg			+= w*g;
				sum_g           += g;
			}
			
			if (sum_g != 0.0)
			{
				//save the new vector at current step
				mean_direction	/= sum_wg;
				sum_wg			/= sum_g;
				
					
				//save the new vector at current step
				mean_direction	/= sum_wg;
				//sum_wg			/= sum_g;
				
				//find best fitting vector out of the candidates
				int best_idx=0;
				double best_fit=0;
            
                double fit =   vectorfield->weight(i)
                             * vectorfield->direction(i).dot(mean_direction)	// This formula calculates the cosine
                             / QPointFX(vectorfield->direction(i)).length()		// of the angle between the current alternative
                             / QPointFX(mean_direction).length();				// vector and the representative (value of 1 = 0°)
            
                if(fit > best_fit)
                {
                    best_fit = fit;
                    best_idx=-1;
                }
                
                for (unsigned int alt_idx=0; alt_idx<vectorfield->alternatives(); ++alt_idx)
                {
                    fit = vectorfield->altWeight(i,alt_idx)
                                 * vectorfield->altDirection(i,alt_idx).dot(mean_direction)	// This formula calculates the cosine
                                 / QPointFX(vectorfield->altDirection(i,alt_idx)).length()	// of the angle between the current alternative
                                 / QPointFX(mean_direction).length();						// vector and the representative (value of 1 = 0°)
                
                    if(fit > best_fit)
                    {
                        best_fit = fit;
                        best_idx=alt_idx;
                    }
                }
                
                if(best_idx == -1)
                {
                    result_vectorfield->setDirection(i,vectorfield->direction(i));
                    result_vectorfield->setWeight(i,vectorfield->weight(i));
                }
                else
                {
                    result_vectorfield->setDirection(i,vectorfield->altDirection(i,best_idx));
                    result_vectorfield->setWeight(i,vectorfield->altWeight(i,best_idx));
                }
			}
			else
			{
				result_vectorfield->setWeight(i, 0.0);
			}
		}

    }
	//Clean temp vf
	delete work_vectorfield;
	
	//Return result
	return result_vectorfield;
}

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDPROCESSING_VECTORSMOOTHING_HXX
