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

#include "qpointfx.hxx"

#define _USE_MATH_DEFINES //To get M_PI in every case
#include <math.h>

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the QPointFX class
 * @}
 */

QPointFX::QPointFX()
: QPointF()
{
}

QPointFX::QPointFX(const QPointF & p)
: QPointF(p)
{
}

QPointFX::QPointFX(float x, float y)
: QPointF(x,y)
{
}

float QPointFX::angle() const
{
    return fmod(360 + atan2(this->y(), this->x())*180.0/M_PI, 360);
}

float QPointFX::squaredLength() const
{
    return this->x()*this->x()+this->y()*this->y();
}

float QPointFX::length() const
{
    return sqrt(this->squaredLength());
}

bool QPointFX::operator<(const QPointF& other) const
{
    return (    (this->x() <  other.x())
            ||  (this->x() == other.x() && (this->y() <  other.y())));
}

bool QPointFX::operator>(const QPointF& other) const
{
    return (    (this->x() >  other.x())
            ||  (this->x() == other.x() && (this->y() >  other.y())));
}

QPointFX QPointFX::operator-(const QPointF& other) const
{
    return QPointFX(this->x() - other.x(), this->y() - other.y());
}

QPointFX QPointFX::operator+(const QPointF& other) const
{
    return QPointFX(this->x() + other.x(), this->y() + other.y());
}

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

float QPointFX::dot(const QPointF& other) const
{
    return this->x()*other.x() + this->y()*other.y();
}

float QPointFX::cross(const QPointF& other) const
{
    return this->x()*other.y() - this->y()*other.x();
}

} //End of namespace graipe
