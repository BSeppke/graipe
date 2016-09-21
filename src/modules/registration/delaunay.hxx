/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    Additional Copyright for most parts of the Delaunay lib, which    */
/*    I ported to use VIGRA style and types is due to:                  */
/*           2005, Sjaak Priester, Amsterdam (sjaak@sjaakpriester.nl)   */
/*    See his copyright notice below the GRAIPE preamble for more       */
/*    information and the license.                                      */
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

/************************************************************************/
/*  Copyright (C) 2004-2005 Sjaak Priester                              */
/*                                                                      */
/*  This is free software; you can redistribute it and/or modify        */
/*  it under the terms of the GNU General Public License as published   */
/*  by the Free Software Foundation; either version 2 of the License,   */
/*  or (at your option) any later version.                              */
/*                                                                      */
/*  This file is distributed in the hope that it will be useful,        */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of      */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       */
/*  GNU General Public License for more details.                        */
/*                                                                      */
/*  You should have received a copy of the GNU General Public License   */
/*  along with this application; if not, write to the Free Software     */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston,               */
/*  MA 02111-1307 USA                                                   */
/************************************************************************/

#ifndef GRAIPE_REGISTRATION_DELAUNAY_HXX
#define GRAIPE_REGISTRATION_DELAUNAY_HXX

#include <vector>
#include <set>
#include <algorithm>
#include <math.h>

#include "vigra/tinyvector.hxx"

namespace graipe {

/**
 * A templated Triangle class. The template argument defines the precision
 * of the points/vertices, which form the triangle. Default is double.
 */
template<typename T = double>
class Triangle
{
    public:
        //The used vertex type
        typedef vigra::TinyVector<T,2> VertexType;
    
        /**
         * Copy constructor. Creates a triangle from another one.
         * 
         * \param tri The other triangle.
         */
        Triangle(const Triangle& tri)
        : m_center(tri.m_center),
          m_r(tri.m_r),
          m_r2(tri.m_r2)
        {
            for (int i = 0; i < 3; i++) m_vertices[i] = tri.m_vertices[i];
        }
    
        /**
         * Single argument constructor. Takes pointers to three vertices and
         * creates a new triangle for these.
         *
         * \param p0 The first vertex point.
         * \param p1 The second vertex point.
         * \param p2 The third vertex point.
         */
        Triangle(const VertexType * p0, const VertexType * p1, const VertexType * p2)
        {
            m_vertices[0] = p0;
            m_vertices[1] = p1;
            m_vertices[2] = p2;
            setCircumCircle();
        }
    
        /**
         * Array argument constructor. Given an array of three vertices, this
         * constructor creates a new triagle for them.
         *
         * \param vertices The array of three vertices.
         */
        Triangle(const VertexType * vertices)
        {
            m_vertices[0] = vertices;
            m_vertices[1] = vertices+1;
            m_vertices[2] = vertices+2;
            setCircumCircle();
        }
    
        /**
         * Copy assignment operator. Copies the content of this triangle
         * to another one.
         *
         * \param tri The other triangle, which will be over-copied
         * \return This triangle.
         */
        const Triangle& operator=(const Triangle &tri) const
        {
            if (this != &tri) 
            {
                memcpy ((void*)this, &tri, sizeof(tri));
            } 
            return *this; 
        } 
    
        /**
         * Smaller comparison for two triangles. A triangle is smaller than another one, 
         * iff the center is smaller.
         *
         * \param tri The other triangle.
         * \return True, if the triangle is left of the other or on top of it.
         */
        bool operator<(const Triangle& tri) const
        {
            if (m_center[0] == tri.m_center[0]) return m_center[1] < tri.m_center[1];
            return m_center[0] < tri.m_center[0];
        }
    
        /**
         * Reading/Const access to the vertices of the triangle.
         * Since the index is not checked, this may only be used for 0<=i<=2.
         *
         * \param i Index of the vertex: Either 0,1,2.
         * \return Const pointer to the coresponding vertex.
         */
        const VertexType * vertex(unsigned int i) const
        {
            return m_vertices[i];
        }
        /**
         * Another handy left of definition for triangle.
         *
         * \param point Vertex to be checked for.
         * \return True if the vertex is to the right of the triangle's circumcircle.
         */
        bool isLeftOf(const VertexType& point) const
        {
            //
            return point[0] > (m_center[0] + m_r);
        }

        /**
         * Returns true if * itVertex is in the triangle's circumcircle.
         * A vertex exactly on the circle is also considered to be in the circle.
         *
         * \param point Vertex to be checked for.
         * \return True if the vertex is in the triangle's circumcircle.
         */
        bool cCEncompasses(const VertexType & point) const
        {

            return vigra::squaredNorm(point - m_center) <= m_r2;
        }
    
        /**
         * Check if a point is really inside the triangle. Uses the cCEncompasses
         * function to break early. A vertex exactly on the triangle is also considered
         * to be in the triangle.
         *
         * \param point Vertex to be checked for.
         * \return True if the vertex is in the triangle's circumcircle.
         */
        bool isInside(const VertexType & point) const
        {
            //Quick check to break as early as possible
            if(!cCEncompasses(point))
                return false;
            
            //Inside the encompassing circle - further tests necessary:
            // Compute direction vectors         
            VertexType v0 = *m_vertices[2] - *m_vertices[0];
            VertexType v1 = *m_vertices[1] - *m_vertices[0];
            VertexType v2 = point -          *m_vertices[0];
            
            // Compute dot products
            double	dot00 = dot(v0, v0),
                    dot01 = dot(v0, v1),
                    dot02 = dot(v0, v2),
                    dot11 = dot(v1, v1),
                    dot12 = dot(v1, v2);
            
            // Compute barycentric coordinates
            double	invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01),
                           u = (dot11 * dot02 - dot01 * dot12) * invDenom,
                           v = (dot00 * dot12 - dot01 * dot02) * invDenom;
            
            // Check if point is in triangle
            return (u >= 0) && (v >= 0) && (u + v <= 1);
        }

    protected:
        /**
         * Internal function to update the circumcircle based on the
         * currently set vertices. Updates m_center, m_r and m_r2.
         */
        void setCircumCircle()
        {
            T x0 = (*m_vertices[0])[0];
            T y0 = (*m_vertices[0])[1];

            T x1 = (*m_vertices[1])[0];
            T y1 = (*m_vertices[1])[1];

            T x2 = (*m_vertices[2])[0];
            T y2 = (*m_vertices[2])[1];

            T y10 = y1 - y0;
            T y21 = y2 - y1;

			double REAL_EPSILON = 1.192092896e-07F;	// = 2^-23; I've no idea why this is a good value, but GDI+ has it.

            bool b21zero = y21 > -REAL_EPSILON && y21 < REAL_EPSILON;

            if (y10 > -REAL_EPSILON && y10 < REAL_EPSILON)
            {
                if (b21zero)	// All three vertices are on one horizontal line.
                {
                    if (x1 > x0)
                    {
                        if (x2 > x1) x1 = x2;
                    }
                    else
                    {
                        if (x2 < x0) x0 = x2;
                    }
                    m_center[0] = (x0 + x1) * .5;
                    m_center[1] = y0;
                }
                else	// m_vertices[0] and m_vertices[1] are on one horizontal line.
                {
                    double m1 = - (x2 - x1) / y21;

                    double mx1 = (x1 + x2) * .5;
                    double my1 = (y1 + y2) * .5;

                    m_center[0] = (x0 + x1) * .5;
                    m_center[1] = m1 * (m_center[0] - mx1) + my1;
                }
            }
            else if (b21zero)	// m_vertices[1] and m_vertices[2] are on one horizontal line.
            {
                double m0 = - (x1 - x0) / y10;

                double mx0 = (x0 + x1) * .5;
                double my0 = (y0 + y1) * .5;

                m_center[0] = (x1 + x2) * .5;
                m_center[1] = m0 * (m_center[0] - mx0) + my0;
            }
            else	// 'Common' cases, no multiple vertices are on one horizontal line.
            {
                double m0 = - (x1 - x0) / y10;
                double m1 = - (x2 - x1) / y21;

                double mx0 = (x0 + x1) * .5;
                double my0 = (y0 + y1) * .5;

                double mx1 = (x1 + x2) * .5;
                double my1 = (y1 + y2) * .5;

                m_center[0] = (m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1);
                m_center[1] = m0 * (m_center[0] - mx0) + my0;
            }

            double dx = x0 - m_center[0];
            double dy = y0 - m_center[1];

            m_r2 = dx * dx + dy * dy;	// the radius of the circumcircle, squared
            m_r = sqrt(m_r2);	// the proper radius

            // Version 1.1: make m_r2 slightly higher to ensure that all edges
            // of co-circular vertices will be caught.
            // Note that this is a compromise. In fact, the algorithm isn't floatly
            // suited for very many co-circular vertices.
            m_r2 *= 1.000001f;
        }
    
   private:
        const vigra::TinyVector<T,2> * m_vertices[3];	// the three triangle vertices
        vigra::TinyVector<double,2>    m_center;		// center of circumcircle
        double m_r;                      // radius of circumcircle
        double m_r2;                     // radius of circumcircle, squared
};


typedef Triangle<double>::VertexType Vertex;
typedef std::vector<Vertex> VertexVector;
typedef std::vector<Vertex>::iterator VIterator;
typedef std::vector<Vertex>::const_iterator CVIterator;

// Changed in verion 1.1: collect triangles in a multiset.
// In version 1.0, I used a set, preventing the creation of multiple
// triangles with identical center points. Therefore, more than three
// co-circular vertices yielded incorrect results. Thanks to Roger Labbe.
typedef std::multiset<Triangle<double>> TriangleSet;
typedef std::multiset<Triangle<double>>::iterator TIterator;
typedef std::multiset<Triangle<double>>::const_iterator CTIterator;


/**
 * Very basic implementation of an Edge class, which just keeps two vertices.
 */
class Edge
{
    public:
        /**
         * Copy constructor.
         *
         * \param e The other edge, from which the data will be copied.
         */
        Edge(const Edge& e)
        : m_pV0(e.v0()),
          m_pV1(e.v1())
        {
        }
    
        /**
         * Single argument constructor. Constructs an edge given two vertices.
         *
         * \param pV0 The first vertex.
         * \param pV1 The second vertex.
         */
        Edge(const Vertex * pV0, const Vertex * pV1)
        : m_pV0(pV0),
          m_pV1(pV1)
        {
        }
    
        /**
         * Smaller than comparison operator for edges.
         *
         * \return True if the pointers/addresses are smaller.
         */
        bool operator<(const Edge& e) const
        {
            if (m_pV0 == e.m_pV0)
            {
                return * m_pV1 < * e.m_pV1;
            }
            return * m_pV0 < * e.m_pV0;
        }
    
        /**
         * Reading/Const access to the first vertex of the edge.
         *
         * \return Const pointer to the first vertex.
         */
        const Vertex* v0() const
        {
            return m_pV0;
        }
    
        /**
         * Reading/Const access to the second vertex of the edge.
         *
         * \return Const pointer to the second vertex.
         */
        const Vertex* v1() const
        {
            return m_pV1;
        }
    
    private:
        const Vertex * m_pV0;
        const Vertex * m_pV1;
};

typedef std::set<Edge> EdgeSet;
typedef std::set<Edge>::iterator EdgeIterator;
typedef std::set<Edge>::const_iterator CEdgeIterator;

/**
 * Function object to check whether a triangle has one of the vertices in SuperTriangle.
 * operator() returns true if it does.
 */
class TriangleHasVertex
{
    public:
        TriangleHasVertex(const Vertex superTriangle[3])
        : m_pSuperTriangle(superTriangle)
        {
        }
    
        bool operator()(const Triangle<double>& tri) const
        {
            for (int i = 0; i < 3; i++)
            {
                const Vertex * p = tri.vertex(i);
                if (p >= m_pSuperTriangle && p < (m_pSuperTriangle + 3))
                {
                    return true;
                }
            }
            return false;
        }
    
    private:
        const Vertex * m_pSuperTriangle;
};

/**
 * Function object to check whether a triangle is 'completed', i.e. doesn't need to be checked
 * again in the algorithm, i.e. it won't be changed anymore.
 * Therefore it can be removed from the workset.
 * A triangle is completed if the circumcircle is completely to the left of the current vertex.
 * If a triangle is completed, it will be inserted in the output set, unless one or more of it's vertices
 * belong to the 'super triangle'.
 */
class TriangleIsCompleted
{
    public:
        TriangleIsCompleted(CVIterator itVertex, TriangleSet& output, const Vertex SuperTriangle[3])
        : m_itVertex(itVertex),
          m_output(output),
          m_pSuperTriangle(SuperTriangle)
        {
        }
    
        bool operator()(const Triangle<double>& tri) const
        {
            bool b = tri.isLeftOf(*m_itVertex);

            if (b)
            {
                TriangleHasVertex thv(m_pSuperTriangle);
                if (! thv(tri))
                {
                    m_output.insert(tri);
                }
            }
            return b;
        }

    private:
        CVIterator m_itVertex;
        TriangleSet& m_output;
        const Vertex * m_pSuperTriangle;
};

/**
 * Function object to check whether vertex is in circumcircle of triangle.
 * operator() returns true if it does.
 * The edges of a 'hot' triangle are stored in the EdgeSet edges.
 */
class VertexIsInCircumCircle
{
    public:
        VertexIsInCircumCircle(CVIterator itVertex, EdgeSet& edges) 
        : m_itVertex(itVertex),
          m_edges(edges)
        {}
        
        bool operator()(const Triangle<double>& tri) const
        {
            bool b = tri.cCEncompasses(*m_itVertex);

            if (b)
            {
                handleEdge(tri.vertex(0), tri.vertex(1));
                handleEdge(tri.vertex(1), tri.vertex(2));
                handleEdge(tri.vertex(2), tri.vertex(0));
            }
            return b;
        }
    protected:
        void handleEdge(const Vertex * p0, const Vertex * p1) const
        {
            const Vertex * pVertex0(NULL);
            const Vertex * pVertex1(NULL);

            // Create a normalized edge, in which the smallest vertex comes first.
            if (* p0 < * p1)
            {
                pVertex0 = p0;
                pVertex1 = p1;
            }
            else
            {
                pVertex0 = p1;
                pVertex1 = p0;
            }

            Edge e(pVertex0, pVertex1);

            // Check if this edge is already in the buffer
            EdgeIterator found = m_edges.find(e);

            if (found == m_edges.end())
            {
                m_edges.insert(e);		// no, it isn't, so insert
            }
            else
            {
                m_edges.erase(found);							// yes, it is, so erase it to eliminate double edges
            }
        }

    private:
        CVIterator m_itVertex;
        EdgeSet& m_edges;
};

/**
 * All class definitions above may now be compined to formulate
 * the well known Delaunay triangluation procedure.
 *
 * \param vertices Unconnected vector of vertices/point, which shall be
 *                 Delauny triangulated.
 * \param output The (final) set of triangles which hold pointers to the input
 *                vertices.
 */
void delaunay_triangulation(const VertexVector& vertices, TriangleSet& output)
{
	if (vertices.size() < 3)
    {
        return;	// nothing to handle
    }
	
    // Determine the bounding box.
	CVIterator itVertex = vertices.begin();

	float xMin = (*itVertex)[0];
	float yMin = (*itVertex)[1];
	float xMax = xMin;
	float yMax = yMin;

	++itVertex;		// If we're here, we know that vertices is not empty.
	for (; itVertex != vertices.end(); itVertex++)
	{
		xMax = (*itVertex)[0];	// Vertices are sorted along the x-axis, so the last one stored will be the biggest.
		float y = (*itVertex)[1];
		if (y < yMin)
        {
            yMin = y;
        }
		if (y > yMax)
        {
            yMax = y;
        }
	}

	float dx = xMax - xMin;
	float dy = yMax - yMin;

	// Make the bounding box slightly bigger, just to feel safe.
	float ddx = dx * 0.01F;
	float ddy = dy * 0.01F;

	xMin -= ddx;
	xMax += ddx;
	dx += 2 * ddx;

	yMin -= ddy;
	yMax += ddy;
	dy += 2 * ddy;

	// Create a 'super triangle', encompassing all the vertices. We choose an equilateral triangle with horizontal base.
	// We could have made the 'super triangle' simply very big. However, the algorithm is quite sensitive to
	// rounding errors, so it's better to make the 'super triangle' just big enough, like we do here.
	Vertex vSuper[3];
    
    float dMax = (dx > dy) ? dx : dy; 
    float xMid = (xMax + xMin) / 2.0;
    float yMid = (yMax + yMin) / 2.0; 
    
    vSuper[0] = Vertex(xMid - 20 * dMax, yMid - dMax); 
    vSuper[1] = Vertex(xMid, yMid + 20 * dMax); 
    vSuper[2] = Vertex(xMid + 20 * dMax,yMid - dMax);
	
    TriangleSet workset;
	workset.insert(Triangle<>(vSuper));

	for (itVertex = vertices.begin(); itVertex != vertices.end(); itVertex++)
	{
		// First, remove all 'completed' triangles from the workset.
		// A triangle is 'completed' if its circumcircle is entirely to the left of the current vertex.
		// Vertices are sorted in x-direction (the set container does this automagically).
		// Unless they are part of the 'super triangle', copy the 'completed' triangles to the output.
		// The algorithm also works without this step, but it is an important optimalization for bigger numbers of vertices.
		// It makes the algorithm about five times faster for 2000 vertices, and for 10000 vertices,
		// it's thirty times faster. For smaller numbers, the difference is negligible.
		TIterator itEnd = remove_if(workset.begin(), workset.end(), TriangleIsCompleted(itVertex, output, vSuper));

		EdgeSet edges;

		// A triangle is 'hot' if the current vertex v is inside the circumcircle.
		// Remove all hot triangles, but keep their edges.
		itEnd = remove_if(workset.begin(), itEnd, VertexIsInCircumCircle(itVertex, edges));
		workset.erase(itEnd, workset.end());	// remove_if doesn't actually remove; we have to do this explicitly.

		// Create new triangles from the edges and the current vertex.
		for (EdgeIterator it = edges.begin(); it != edges.end(); it++)
        {
			workset.insert(Triangle<>(it->v0(), it->v1(), & (* itVertex)));
        }
	}

	// Finally, remove all the triangles belonging to the 'super triangle' and move the remaining
	// triangles tot the output; remove_copy_if lets us do that in one go.
	TIterator where = output.begin();
	remove_copy_if(workset.begin(), workset.end(), std::inserter(output, where), TriangleHasVertex(vSuper));
}


} //namespace graipe

#endif //GRAIPE_REGISTRATION_DELAUNAY_HXX
