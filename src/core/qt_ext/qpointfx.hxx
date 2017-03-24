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

#ifndef GRAIPE_CORE_QPOINTFX_HXX
#define GRAIPE_CORE_QPOINTFX_HXX

#include "core/config.hxx"

#include <QPointF>
#include <QString>
#include <QVector>

namespace graipe {
  
/** 
 * This class extends the floating point precise QPointF by means of comparison,
 * ordering and arithmetic functions.
 */
class GRAIPE_CORE_EXPORT QPointFX
    : public QPointF
{
public:
    /**
     * Default constructor - Creates empty QPointF
     */
    QPointFX();

    /**
     * Copy constructor - Creates QPointFX from QPointF
     */
    QPointFX(const QPointF & p);

    /**
     * Convenience constructor - Creates QPointFX form x,y-coords
     */
    QPointFX(float x, float y);

    /**
     * Angle from origin (0,0) to QPointFX in degrees
     *
     * \return the angle in degrees
     */
    float angle() const;

    /**
     * Squared euclidean distance from origin (0,0) to QPointFX
     *
     * \return (x*x + y*y)
     */
    float squaredLength() const;
    /**
     * Euclidean distance from origin (0,0) to QPointFX
     * \return sqrt(x*x + y*y)
     */
    float length() const;

    /**
     * Ordering of points in standard order: A point is smaller
     * if its y < other's y or y = other's y and x < other's x
     *
     * \param other PointF to compare against
     * \return true if this is the smaller point
     */
    bool operator<(const QPointF& other) const;
        
    /**
     * Ordering of points in standard order: A point is larger
     * if its y > other's y or y = other's y and x > other's x
     *
     * \param other PointF to compare against
     * \return true if this is the larger point
     */
    bool operator>(const QPointF& other) const;

    /**
     * Substraction of points
     * Basic coordinate-wise substraction
     *
     * \param other point, which will be substracted
     * \result the this - other point
     */
    QPointFX operator-(const QPointF& other) const;

    /**
     * Addition of points
     * Basic coordinate-wise addition
     *
     * \param other point, which will be added
     * \result the this + other point
     */
    QPointFX operator+(const QPointF& other) const;
        
    /**
     * Alterative (index) accessor to x and y coordinate
     * x is [0], y is [1]. Please note:
     * This function throws an error for idx > 1 !
     *
     * \param idx the index (0=x, 1=y, >1= throw error)
     * \return the x or y coordinate or an error
     */
    float operator[](unsigned int idx) const;

    /**
     * Dot product between two points:
     * As easy as x*other's x + y*other's y
     *
     * \param other the other point
     * \return the dot product
     */
    float dot(const QPointF& other) const;

    /**
     * Cross product between two points:
     * As easy as x*other's y - y*other's x
     *
     * \param other the other point
     * \return the cross product
     */
    float cross(const QPointF& other) const;
};

} //end of namespace graipe

#endif
