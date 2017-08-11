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

#include "vectorfields/vectorfield.hxx"

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *     @file
 *     @brief Implementation file for generic 2d vectorfield base classes
 * @}
 */

Vectorfield2D::Vectorfield2D(Workspace* wsp)
:   Model(wsp),
    m_global_motion(new TransformParameter("Global motion matrix:")),
    m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
{
    m_parameters->addParameter("globalMotion", m_global_motion);
    m_parameters->addParameter("scale", m_scale);
}

Vectorfield2D::Vectorfield2D(const Vectorfield2D & vf)
: Model(vf),
  m_global_motion(new TransformParameter("Global motion matrix:")),
  m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
{
    m_parameters->addParameter("globalMotion", m_global_motion);
    m_parameters->addParameter("scale", m_scale);
}

double Vectorfield2D::scale() const
{
	return m_scale->value();
}

void Vectorfield2D::setScale(double scale)
{
    if(locked())
        return;

	m_scale->setValue(scale);
    updateModel();
}

QTransform Vectorfield2D::globalMotion() const
{
	return m_global_motion->value();
}

void Vectorfield2D::setGlobalMotion(const QTransform &  trans)
{
    if (locked())
        return;
    
	m_global_motion->setValue(trans);
    updateModel();
}

Vectorfield2D::PointType Vectorfield2D::localDirection(unsigned int index) const
{
	return direction(index) - globalDirection(index);
}

Vectorfield2D::PointType Vectorfield2D::globalDirection(unsigned int index) const
{
	return globalMotion().map(origin(index)) - origin(index);
}

float Vectorfield2D::squaredLength(unsigned int index) const
{
    return direction(index).squaredLength();
}

float Vectorfield2D::length(unsigned int index) const
{
    return direction(index).length();
}

float Vectorfield2D::angle(unsigned int index) const
{
    return direction(index).angle();
}

Vectorfield2D::PointType Vectorfield2D::target(unsigned int index) const
{	
	return origin(index) + direction(index);	
}

void Vectorfield2D::setTarget(unsigned int index, const PointType& new_t)
{
    if(locked())
        return;
    
	setDirection(index, new_t - origin(index));
    //SetDirection needs to call updateModel()!
}
    
} //end of namespace graipe
