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

#include "qpointfx.hxx"

#define _USE_MATH_DEFINES //To get M_PI in every case
#include <math.h>

#include <QtDebug>

namespace graipe {

/**
 * Default constructor - Creates empty QPointF
 */
QPointFX::QPointFX()
: QPointF()
{
}

/**
 * Copy constructor - Creates QPointFX from QPointF
 */
QPointFX::QPointFX(const QPointF & p)
: QPointF(p)
{
}

/**
 * Convenience constructor - Creates QPointFX form x,y-coords
 */
QPointFX::QPointFX(float x, float y)
: QPointF(x,y)
{
}

/**
 * Angle from origin (0,0) to QPointFX in degrees
 *
 * \return the angle of the QPointFX
 */
float QPointFX::angle() const
{
    return fmod(360 + atan2(this->y(), this->x())*180.0/M_PI, 360);
}

/**
 * Squared euclidean distance from origin (0,0) to QPointFX
 *
 * \return (x*x + y*y)
 */
float QPointFX::squaredLength() const
{
    return this->x()*this->x()+this->y()*this->y();
}

/**
 * Euclidean distance from origin (0,0) to QPointFX
 * \return sqrt(x*x + y*y)
 */
float QPointFX::length() const
{
    return sqrt(this->squaredLength());
}

/**
 * Ordering of points in standard order: A point is smaller
 * if its y < other's y or y = other's y and x < other's x
 *
 * \param other PointF to compare against
 * \return true if this is the smaller point
 */
bool QPointFX::operator<(const QPointF& other) const
{
    return (    (this->x() <  other.x())
            ||  (this->x() == other.x() && (this->y() <  other.y())));
}

/**
 * Ordering of points in standard order: A point is larger
 * if its y > other's y or y = other's y and x > other's x
 *
 * \param other PointF to compare against
 * \return true if this is the larger point
 */
bool QPointFX::operator>(const QPointF& other) const
{
    return (    (this->x() >  other.x())
            ||  (this->x() == other.x() && (this->y() >  other.y())));
}

/**
 * Substraction of points
 * Basic coordinate-wise substraction
 *
 * \param other point, which will be substracted
 * \result the this - other point
 */
QPointFX QPointFX::operator-(const QPointF& other) const
{
    return QPointFX(this->x() - other.x(), this->y() - other.y());
}

/**
 * Addition of points
 * Basic coordinate-wise addition
 *
 * \param other point, which will be added
 * \result the this + other point
 */
QPointFX QPointFX::operator+(const QPointF& other) const
{
    return QPointFX(this->x() + other.x(), this->y() + other.y());
}

/**
 * Alterative (index) accessor to x and y coordinate
 * x is [0], y is [1]. Please note:
 * This function throws an error for idx > 1 !
 *
 * \param idx the index (0=x, 1=y, >1= throw error)
 * \return the x or y coordinate or an error
 */
float QPointFX::operator[](unsigned int idx) const
{
    switch (idx)
    {
    case 0:
        return this->x();
    case 1:
        return this->y();
    }
    
    throw std::runtime_error("Error: QPointFX provides only [0] -> x and [1] -> y index operators.");
}

/**
 * Dot product between two points:
 * As easy as x*other's x + y*other's y
 *
 * \param other the other point
 * \return the dot product
 */
float QPointFX::dot(const QPointF& other) const
{
    return this->x()*other.x() + this->y()*other.y();
}

/**
 * Cross product between two points:
 * As easy as x*other's y - y*other's x
 *
 * \param other the other point
 * \return the cross product
 */
float QPointFX::cross(const QPointF& other) const
{
    return this->x()*other.y() - this->y()*other.x();
}

} //End of namespace graipe