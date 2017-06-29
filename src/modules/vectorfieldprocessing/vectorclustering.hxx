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

#ifndef GRAIPE_VECTORFIELDPROCESSING_VECTORCLUSTERING_HXX
#define GRAIPE_VECTORFIELDPROCESSING_VECTORCLUSTERING_HXX

#include <vector>

#include <vigra/stdimage.hxx>
#include <vigra/multi_array.hxx>
#include <vigra/tinyvector.hxx>

#include "vectorfields/vectorfields.h"
#include "features2d/features2d.h"

namespace graipe {
    
/**
 * Helper function to compute the four-dimensional distance between two unweighted
 * vectors. The direction component may be (over-) weighted with a factor.
 *
 * \param pos1x The x-position of the first vector.
 * \param pos1y The y-position of the first vector.
 * \param dir1x The x-direction of the first vector.
 * \param dir1y The y-direction of the first vector.
 * \param pos2x The x-position of the second vector.
 * \param pos2y The y-position of the second vector.
 * \param dir2x The x-direction of the second vector.
 * \param dir2y The y-direction of the second vector.
 * \param direction_weight weight for the directional component difference.
 * \return The weighted 4D difference of both vectors.
 */
inline float dist4D(float pos1x, float pos1y, float dir1x, float dir1y,
					float pos2x, float pos2y, float dir2x, float dir2y,
					float direction_weight)
{
	return		sqrt((pos1x-pos2x)*(pos1x-pos2x)+(pos1y-pos2y)*(pos1y-pos2y))
			+	direction_weight*sqrt((dir1x-dir2x)*(dir1x-dir2x)+(dir1y-dir2y)*(dir1y-dir2y));
}

/**
 * Helper function to compute the four-dimensional distance between two unweighted
 * vectors. The direction component may be (over-) weighted with a factor.
 *
 * \param pos1 The position of the first vector.
 * \param dir1 The direction of the first vector.
 * \param pos2 The position of the second vector.
 * \param dir2 The direction of the second vector.
 * \param direction_weight weight for the directional component difference.
 * \return The weighted 4D difference of both vectors.
 */
inline float dist4D(const Vectorfield2D::PointType & pos1, const PointFeatureList2D::PointType & dir1,
					const Vectorfield2D::PointType & pos2, const PointFeatureList2D::PointType & dir2,
					float direction_weight)
{
	return dist4D(pos1.x(), pos1.y(), dir1.x(), dir1.y(), pos2.x(), pos2.y(), dir2.x(), dir2.y(), direction_weight);
}

/**
 * Helper function for the comparison of two directions using the cross product.
 *
 * \param a The first direction vector.
 * \param b The second direction vector.
 * \return True if the a cross b is either larger than zero, or is equal to zero and
 *         the length of a is smaller than b's.
 */
inline bool comparePointAngles(const PointFeatureList2D::PointType& a, const PointFeatureList2D::PointType& b)
{
	//Cross product
	float c = a.cross(b);

	return c>0  || (c==0 && a.length() < b.length());
}

/**
 * Helper function to find out, if three consecutive points form a convex configuration.
 *
 * \param a The first point.
 * \param b The second point.
 * \param c The third point.
 * \return True if the line connecting a-b-c is convex.
 */
inline bool isConvex(const PointFeatureList2D::PointType& a, const PointFeatureList2D::PointType& b, const PointFeatureList2D::PointType& c)
{
	float area = QPointFX(b-a).cross(b-c);
	
	return area < 0 || (area==0 && QPointFX(a-c).length() < QPointFX(a-b).length() + QPointFX(c-b).length());
}

/**
 * Helper function to sort a point list by the corresponding angles. Will be used for the 
 * generation of a polygpn from a given point list.
 *
 * \param points The point list.
 * \return A copied and sorted point list, according to the angle w.r.t. the upper-left-most
 *         item of the point list.
 */
PointFeatureList2D sortPointListAngular(PointFeatureList2D& points)
{
	std::list<PointFeatureList2D::PointType> sorted_list;
	
	PointFeatureList2D sorted_features(points.workspace());
	
	if( points.size())
	{
		//Select point of min x and y-coordinate
		unsigned int min_idx=0;
		
		for (unsigned int i=1; i< points.size(); ++i)
		{
			if(	points.position(min_idx)< points.position(i) )
			{
				min_idx = i;
			}
		}
		
		//make relative positions to min_idx point
		for (unsigned int i=0; i< points.size(); ++i)
		{
			if(i != min_idx)
			{
				sorted_list.push_back(points.position(i) - points.position(min_idx));
			}
		}
		//sort the list by angles
		sorted_list.sort(comparePointAngles);
		
		//create final feature list:
		sorted_features.addFeature(points.position(min_idx));
		
        for(PointFeatureList2D::PointType p: sorted_list)
		{
			sorted_features.addFeature(p+points.position(min_idx));
		}
		
	}
	return sorted_features;
}

/**
 * Function to generate a polygon from a given point list. The polygon must
 * cover all points and might be forced to be convex using an additional parameter.
 *
 * \param points The point list, for which we want to generate the polygon.
 * \param convex If true, the resulting polygon will be convex.
 * \return The (hull) polygon for the point list.
 */
Polygon2D polygonFromPointList(const PointFeatureList2D& points, bool convex = true)
{
	Polygon2D result;
	if (points.size() == 1)
	{
		result.addPoint(points.position(0)+Polygon2D::PointType(-0.5,0.5));
		result.addPoint(points.position(0)+Polygon2D::PointType(-0.5,-0.5));
		result.addPoint(points.position(0)+Polygon2D::PointType(0.5,-0.5));
		result.addPoint(points.position(0)+Polygon2D::PointType(0.5,0.5));
		result.addPoint(points.position(0)+Polygon2D::PointType(-0.5,0.5));
	}
	else if (points.size() == 2)
	{
		result.addPoint(points.position(0));
		result.addPoint(points.position(1));
		result.addPoint((points.position(0)+points.position(1))/2.0+Polygon2D::PointType(1,1));
		result.addPoint(points.position(0));
	}
	else if( points.size() > 3)
	{
		if(convex)
		{
			std::list<Polygon2D::PointType> stack;
			
			stack.push_back(points.position(0));
			stack.push_back(points.position(1));
			
			unsigned int i=2, n = points.size();
			
			while (i<n)
			{
				if(isConvex(*(--(--stack.end())),
							*(--stack.end()),
							points.position(i))
				   || stack.size() == 1)
				{
					stack.push_back(points.position(i));
					i++;
				}
				else 
				{ 
					if(stack.size()>1)
						stack.pop_back();
				}
			}	
			for(Polygon2D::PointType p: stack)
			{
				result.addPoint(p);
			}
			result.addPoint(stack.front());
		}
		else
		{
			for (unsigned int  i=0; i< points.size(); ++i)
			{
				result.addPoint(points.position(i));
			}
			result.addPoint(points.position(0));
		}
	}
	return result;
}


/** 
 * This function uses an already clustered vectorfield as input, which is given by a weighted
 * vectorfield, where each weight corresponds to a cluster id. It will apply the polygon erstimation
 * for each cluster and return the list of polygons. The weight of each resulting polygon is given
 * by the mean 4d distance of all included vectors w.r.t. the center of gravity.
 *
 * \param vectorfield The vectorfield, which has already been clustered (weight = cluster id).
 * \param direction_weight weight for the directional component difference.
 * \return A list of weighted polygons, each representing one cluster. 
 */
WeightedPolygonList2D* polygonsFromClusteredVectorfield(SparseWeightedVectorfield2D* vectorfield, float direction_weight)
{
	WeightedPolygonList2D* result = new WeightedPolygonList2D(vectorfield->workspace());
	
	unsigned int vector_count = vectorfield->size();					//count of vectors
	unsigned int cluster_count = 0;
	
	
	//find clusters
	for (unsigned int i=0; i< vector_count; ++i)
	{
        cluster_count = std::max(cluster_count,(unsigned int)vectorfield->weight(i));
	}
	
	//create temp caches
	std::vector<PointFeatureList2D*> point_lists(cluster_count);
	std::vector<PointFeatureList2D::PointType> pos(cluster_count), dir(cluster_count);
    std::vector<float> vec_count(cluster_count), vec_diff(cluster_count);
	
	// For each cluster ->estimate mean vector
	for (unsigned int i=0; i< vector_count; ++i)
	{
		unsigned int cluster_idx = vectorfield->weight(i)-1;
		pos[cluster_idx] += vectorfield->origin(i);
		dir[cluster_idx] += vectorfield->direction(i);
		vec_count[cluster_idx]++;
	}
	for (unsigned int cluster_idx=0; cluster_idx< cluster_count; ++cluster_idx)
	{
		//initialise point Lists for polygon creation
		point_lists[cluster_idx] = new PointFeatureList2D(vectorfield->workspace());
		
		pos[cluster_idx] /= vec_count[cluster_idx];
		dir[cluster_idx] /= vec_count[cluster_idx];
	}
	
	//for each cluster -> estimate square differences between mean and each vector
	for (unsigned int i=0; i<vector_count; ++i)
	{
		unsigned int cluster_idx = vectorfield->weight(i)-1;
		vec_diff[cluster_idx] += dist4D(pos[cluster_idx], dir[cluster_idx],
										vectorfield->origin(i), vectorfield->direction(i),
										direction_weight);
		
		//Add points to corresponding lists
		point_lists[cluster_idx]->addFeature(vectorfield->origin(i));
	}
	for (unsigned int cluster_idx=0; cluster_idx< cluster_count; ++cluster_idx)
	{
		vec_diff[cluster_idx] /=vec_count[cluster_idx];
		result->addPolygon(polygonFromPointList(sortPointListAngular(*point_lists[cluster_idx])), vec_diff[cluster_idx]);
		
		delete point_lists[cluster_idx];
		point_lists[cluster_idx] = 0;
	}
	
	return result;
}

/**
 * Greedy vectorfield clustering algorithm. This implements a
 * basic, greedy clustering algorithm. It starts with the first vector and collects
 * new vectors until the radius threshold is reached. It then selects the next
 * (far-most) non-assigned vector and repeats the loop until no more assignments are
 * possible. 
 *
 * \param vectorfield The vectorfield to be thresholded.
 * \param max_4d_distance The maximum (4d) distance to be used for clustering. Defaults to 10.
 * \param min_weight The minimal weight of the vectors. Defaults to 0.
 * \param direction_weight weight for the directional component difference.
 * \param use_local Only use the local part of the vectors for clustering.
 * \return A vector conataining the results: 
 *          First item:  The resulting (clustered) vectorfield (weight = cluster id).
 *          Second item: The vectorfield of the cluster centres. Defaults to 1.
 *          Third item:  The list of weighted polygons for the cluster results. Defaults to false.
 */
template <class Vectorfield_Type>
std::vector<Model*> clusterVectorfieldGreedy(Vectorfield_Type * vectorfield,
                                             float max_4d_distance=10.0, float min_weight=0.0, float direction_weight=1.0,
                                             bool use_local=false)
{
    int feature_count = vectorfield->size();					//count of features
	
	SparseWeightedVectorfield2D * result_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
	SparseWeightedVectorfield2D * cluster_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
	
    result_vectorfield->setGlobalMotion(vectorfield->globalMotion());
    cluster_vectorfield->setGlobalMotion(vectorfield->globalMotion());
    
	std::vector<Model*> result;
	result.push_back(result_vectorfield);
	result.push_back(cluster_vectorfield);
	
	std::vector<int> cluster_id(feature_count);
	int assigned_count=0;
	
	for (int i=0; i< feature_count; ++i)
	{
		cluster_id[i] = -1;
	}
	
	//initialze with first feature
	cluster_id[0] = 0;
	cluster_vectorfield->addVector(vectorfield->origin(0),vectorfield->direction(0), vectorfield->weight(0));
	
	assigned_count=1;
	bool did_assignment = true;
	
    while (did_assignment) 
	{
		did_assignment = false;
		
		//Find the closest neighbor for the current cluster centre
		for (int i=0; i< feature_count; ++i)
		{
			if (cluster_id[i] != -1 || vectorfield->weight(i) < min_weight)
				continue;
			
			//initialize with maximal allowed distance
			float min_cluster_distance = max_4d_distance;
			int min_cluster_id = -1;
			
			for(unsigned int c=0; c<cluster_vectorfield->size(); c++)
			{
				float dist =   dist4D(cluster_vectorfield->origin(c), use_local ? cluster_vectorfield->localDirection(c) : cluster_vectorfield->direction(c),
									  vectorfield->origin(i),         use_local ? vectorfield->localDirection(i): vectorfield->direction(i),
									  direction_weight);
				
				if (dist<min_cluster_distance)
				{
					min_cluster_distance = dist;
					min_cluster_id = c;
				}
			}
			
			if (min_cluster_id != -1)
			{
				//qDebug() << "\t\tAdding " << i << " to cluster " << min_cluster_id << "\n"
				//<< "\t\t origin:    " << cluster_vectorfield->origin(min_cluster_id) << "\n"
				//<< "\t\t direction: " << cluster_vectorfield->direction(min_cluster_id)  << "\n"
				//<< "\t\t weight:    " <<  cluster_vectorfield->weight(min_cluster_id) << "\n\n";
				
				//Cluster gefunden:
				cluster_id[i] = min_cluster_id;
				assigned_count++;
				did_assignment=true;
				
				//cog und richtung für dieses Cluster neu berechnen
				unsigned int cluster_count = 0;
				for(unsigned int c=0; c<cluster_id.size(); c++)
				{
					if (cluster_id[c] == min_cluster_id)
					{
						cluster_count++;
					}
				}
				
				//qDebug()	<< "cluster count found: " << cluster_count << "\n"
				//		<< "\t\t new origin:    " << cluster_vectorfield->origin(min_cluster_id)*(cluster_count-1)/cluster_count
				//								+ vectorfield->origin(i)/cluster_count << "\n"
				//		<< "\t\t new direction: " << cluster_vectorfield->direction(min_cluster_id)*(cluster_count-1)/cluster_count
				//								+ vectorfield->direction(i)/cluster_count << "\n"
				//		<< "\t\t new weight:    " <<  cluster_vectorfield->weight(min_cluster_id)*(cluster_count-1)/cluster_count
				//								+ vectorfield->weight(i)/cluster_count << "\n\n";
				
				cluster_vectorfield->setOrigin(min_cluster_id,	  cluster_vectorfield->origin(min_cluster_id)*(cluster_count-1)/cluster_count
											   + vectorfield->origin(i)/cluster_count);
				cluster_vectorfield->setDirection(min_cluster_id,	cluster_vectorfield->direction(min_cluster_id)*(cluster_count-1)/cluster_count
												  + vectorfield->direction(i)/cluster_count);
				cluster_vectorfield->setWeight(min_cluster_id,	  cluster_vectorfield->weight(min_cluster_id)*(cluster_count-1)/cluster_count
											   + vectorfield->weight(i)/cluster_count);
			}
		}
		//add new cluster
		if(!did_assignment)
		{
			
			float max_cluster_distance = 0;
			int  new_feature_id = -1;
			//find the far-most non-assigned vector w.r.t. all already known cluster centers
			for (int i=0; i< feature_count; ++i)
			{
				if (cluster_id[i] != -1 || vectorfield->weight(i) < min_weight)
					continue;
				
				for(unsigned int c=0; c<cluster_vectorfield->size(); c++)
				{
					float dist =   dist4D(cluster_vectorfield->origin(c), use_local ? cluster_vectorfield->localDirection(c) : cluster_vectorfield->direction(c),
									      vectorfield->origin(i),         use_local ? vectorfield->localDirection(i): vectorfield->direction(i),
										  direction_weight);
					
					if ( dist>max_cluster_distance)
					{
						max_cluster_distance = dist;
						new_feature_id = i;
					}
				}
			}
			
			if (new_feature_id != -1)
			{
				did_assignment = true;
				//qDebug()	<< "\tAdding " << new_feature_id << " to new cluster " << cluster_vectorfield->size() << "\n"
				//		<< "\t\t origin:    " << vectorfield->origin(new_feature_id) << "\n"
				//		<< "\t\t direction: " << vectorfield->direction(new_feature_id)  << "\n"
				//		<< "\t\t weight:    " <<  vectorfield->weight(new_feature_id) << "\n\n";
				
				//add new feature
				cluster_id[new_feature_id] = cluster_vectorfield->size();
				cluster_vectorfield->addVector(vectorfield->origin(new_feature_id),vectorfield->direction(new_feature_id),vectorfield->weight(new_feature_id));
				assigned_count++;
			}
		}
		//qDebug() << "assigned " << assigned_count << " of " << feature_count << " elements to " <<  cluster_vectorfield->size() << " clusters\n";
	}
	
	for (int i=0; i< feature_count; ++i)
	{
		if (vectorfield->weight(i) < min_weight)
			continue;
		
		//qDebug() << "adding vector " << i  << " with weight: " << cluster_id[i]+1 << "\n";
		result_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), cluster_id[i]+1);
	}
    
	result.push_back(polygonsFromClusteredVectorfield(result_vectorfield, direction_weight));
	return result;
}

/**
 * K-means vectorfield clustering algorithm. This implements the well known
 * clustering algorithm for vectorfield. It uses the weighted 4d vector-distance
 * for distance measurement between the vectors.
 *
 * \param vectorfield The vectorfield to be thresholded.
 * \param k The count of resulting clusters. Defaults to 10.
 * \param min_weight The minimal weight of the vectors. Defaults to 0.
 * \param direction_weight weight for the directional component difference. Defaults to 1.
 * \param use_local Only use the local part of the vectors for clustering. Defaults to false.
 * \return A vector conataining the results: 
 *          First item:  The resulting (clustered) vectorfield (weight = cluster id).
 *          Second item: The vectorfield of the cluster centres.
 *          Third item:  The list of weighted polygons for the cluster results.
 */
template <class Vectorfield_Type>
std::vector<Model*> clusterVectorfieldKMeans(Vectorfield_Type * vectorfield, unsigned int k=10, float min_weight=0.0, float direction_weight =1.0,
											 bool use_local = false)
{
	typedef SparseWeightedVectorfield2D::PointType Point2D;
    
	unsigned int feature_count = vectorfield->size();					//count of features
		
	SparseWeightedVectorfield2D * result_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
	SparseWeightedVectorfield2D * cluster_vectorfield = new SparseWeightedVectorfield2D(vectorfield->workspace());
    
    result_vectorfield->setGlobalMotion(vectorfield->globalMotion());
    cluster_vectorfield->setGlobalMotion(vectorfield->globalMotion());
    
	std::vector<Model*> result;
	result.push_back(result_vectorfield);
	result.push_back(cluster_vectorfield);
	
	//qDebug() << "computing real feature count\n";
	
	unsigned int real_feature_count=0;
	for (unsigned int i=0; i< feature_count; ++i)
	{
		if(vectorfield->weight(i) >= min_weight)
			real_feature_count++;
	}
    k = std::min(real_feature_count,k);
	
	
	//qDebug() << "preparing cluster assignment array\n";
	std::vector<int> cluster_id(feature_count);
	for (unsigned int i=0; i< feature_count; ++i)
	{
		cluster_id[i] = -1;
	}
	
	//qDebug() << "initial selection of " << k << " seeds\n";
	//initial selection of n seeds
	while( cluster_vectorfield->size() != k)
	{
		unsigned int rand_id = rand() % feature_count;
		
		if(cluster_id[rand_id] == -1 && vectorfield->weight(rand_id) >= min_weight)
		{
			//qDebug() << "\tadding cluster from vector " << rand_id << "\n";
			cluster_id[rand_id] =  cluster_vectorfield->size();
			cluster_vectorfield->addVector(vectorfield->origin(rand_id), vectorfield->direction(rand_id),vectorfield->weight(rand_id));
		}
	}
	
	bool assignment_changed=true;
	
    while (assignment_changed) 
	{
		assignment_changed=false;
		
		//1. Prepare new clusters for next run...
		for(unsigned int c=0; c<cluster_vectorfield->size(); c++)
		{
			//qDebug() << "moving cluster center from " << cluster_vectorfield->origin(c) << " to ";
			//cog und richtung für dieses Cluster neu berechnen
			cluster_vectorfield->setOrigin(c,Point2D(0,0));
			cluster_vectorfield->setDirection(c,Point2D(0,0));
			cluster_vectorfield->setWeight(c,0);
			
			unsigned int cluster_count = 0;
			for(unsigned int i=0; i<cluster_id.size(); i++)
			{
				if (cluster_id[i] == c)
				{
					cluster_count++;
					cluster_vectorfield->setOrigin(c,	  cluster_vectorfield->origin(c) + vectorfield->origin(i));
					cluster_vectorfield->setDirection(c,  cluster_vectorfield->direction(c) + vectorfield->direction(i));
					cluster_vectorfield->setWeight(c,	  cluster_vectorfield->weight(c) + vectorfield->weight(i));
				}
			}
			if(cluster_count !=0)
			{
				cluster_vectorfield->setOrigin(c,	  cluster_vectorfield->origin(c)/(float)cluster_count);
				cluster_vectorfield->setDirection(c,  cluster_vectorfield->direction(c)/(float)cluster_count);
				cluster_vectorfield->setWeight(c,	  cluster_vectorfield->weight(c)/(float)cluster_count);
				//qDebug() << cluster_vectorfield->origin(c) << "\n";
			}
		}

		
		//2. Finde das nächste Clusterzentrum für jeden Vektor
		for (unsigned int i=0; i< feature_count; ++i)
		{
			if (vectorfield->weight(i) < min_weight)
				continue;
			
			float min_cluster_distance = vigra::NumericTraits<float>::max();
			int min_cluster_id = -1;
			
			for(unsigned int c=0; c<cluster_vectorfield->size(); c++)
			{
				float dist =   dist4D(cluster_vectorfield->origin(c), use_local ? cluster_vectorfield->localDirection(c) : cluster_vectorfield->direction(c),
									  vectorfield->origin(i),         use_local ? vectorfield->localDirection(i): vectorfield->direction(i),
									  direction_weight);
				
				if ( dist<min_cluster_distance)
				{
					min_cluster_distance = dist;
					min_cluster_id = c;
				}
			}
				
			//neuer Cluster gefunden:
			if(cluster_id[i] != min_cluster_id)
			{
				//qDebug() << "\tmoving " << i << " from cluster " << cluster_id[i] << " to cluster " << min_cluster_id << "\n";
				assignment_changed=true;
				cluster_id[i] = min_cluster_id;
			}
		}	
	}
	
	for (unsigned int i=0; i< feature_count; ++i)
	{
		if (vectorfield->weight(i) < min_weight)
			continue;
		
		//qDebug() << "adding vector " << i  << " with weight: " << cluster_id[i]+1 << "\n"; 
		result_vectorfield->addVector(vectorfield->origin(i), vectorfield->direction(i), cluster_id[i]+1);
	}
    
	result.push_back(polygonsFromClusteredVectorfield(result_vectorfield, direction_weight));
	return result;
}

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDPROCESSING_VECTORCLUSTERING_HXX
