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

#include "features/polygon.hxx"

namespace graipe {
   
/**
 * Virtual destructor of this class, needed due to non-final QVector class.
 */
Polygon2D::~Polygon2D()
{
}
    
/**
 * The typename of this Polygon2D class
 *
 * \return Always: "Polygon2D"
 */
QString Polygon2D::typeName() const
{
	return "Polygon2D";
}

/**
 * Check if the polygon is closed.
 *
 * \return true if the polygon is closed (first == last point)
 */
bool Polygon2D::isClosed() const
{
	return (front() == back());
}

/**
 * Check if a point lies inside the polygon.
 *
 * \param p The point to be checked if inside this polygon.
 * \return true if the given point is inside the polygon and the
 *         polygon is closed.
 */
bool Polygon2D::isInside(const PointType& p) const
{
    return contains(p);
}

/**
 * The area included by the polygon.
 *
 * \return 0, if the polygon is not closed, else the area.
 */
float Polygon2D::area() const
{
    double area=0.0;
    unsigned int i, j=size()-1;
    
    if(!isClosed())
        return 0;
    
    for (i=0; i<(unsigned int)size(); i++)
    {
        area+=(at(j).x() + at(i).x())*(at(j).y()-at(i).y());
        j=i;
    }
    
    return area*.5;
}

/**
 * Add a point to the polygon
 *
 * \param p The point to be added to this polygon.
 */
void Polygon2D::addPoint(const PointType& new_p)
{
	*this << new_p;
}
    
} //End of namespace graipe
