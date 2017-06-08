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

#include "vectorfields/vectorfield.hxx"

namespace graipe {

/**
 * Default constructor. Creates an empty vectorfield.
 */
Vectorfield2D::Vectorfield2D()
:  m_global_motion(new TransformParameter("Global motion matrix:")),
   m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
{
    m_parameters->addParameter("globalMotion", m_global_motion);
    m_parameters->addParameter("scale", m_scale);
}

/**
 * Copy constructor. Creates a vectorfield from another one.
 *
 * \param vf The other vectorfield.
 */
Vectorfield2D::Vectorfield2D(const Vectorfield2D & vf)
: Model(vf),
  m_global_motion(new TransformParameter("Global motion matrix:")),
  m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
{
    m_parameters->addParameter("globalMotion", m_global_motion);
    m_parameters->addParameter("scale", m_scale);
}

/**
 * Getter for the scaling function of the vector length
 * from units/pixels to cm/s.
 *
 * \return The scaling value from pixels to cm/s.
 */
double Vectorfield2D::scale() const
{
	return m_scale->value();
}

/**
 * Setter for the scaling function of the vector length
 * from units/pixels to cm/s.
 * Does nothing if the model is locked.
 *
 * \param scale The scaling value from pixels to cm/s.
 */
void Vectorfield2D::setScale(double scale)
{
    if(locked())
        return;

	m_scale->setValue(scale);
    updateModel();
}

/**
 * Getter for the global motion of a vectorfield. Note that the
 * vectors are still containing the *complete* motion. So if you 
 * want to get the local motion only, you need to substract the
 * global motion from each vector.
 *
 * \return The global motion by means of an affine matrix.
 */
QTransform Vectorfield2D::globalMotion() const
{
	return m_global_motion->value();
}

/**
 * Setter for the global motion of a vectorfield. Note that the
 * vectors are still containing the *complete* motion after setting
 * the global motion using this function. If you
 * want to get the local motion only, you need to substract the
 * global motion from each vector.
 * Does nothing if the model is locked.
 *
 * \param trans The global motion by means of an affine matrix.
 */
void Vectorfield2D::setGlobalMotion(const QTransform &  trans)
{
    if (locked())
        return;
    
	m_global_motion->setValue(trans);
    updateModel();
}

/**
 * The local direction of a vector at a given index w.r.t. the global
 * transformation matrix given in globalMotion().
 * 
 * \param index The index of the vector.
 * \return The local direction of the vector at the given index.
 */
Vectorfield2D::PointType Vectorfield2D::localDirection(unsigned int index) const
{
	return direction(index) - globalDirection(index);
}

/**
 * The local direction of a vector at a given index w.r.t. the global
 * transformation matrix given in globalMotion().
 * 
 * \param index The index of the vector.
 * \return The local direction of the vector at the given index.
 */
Vectorfield2D::PointType Vectorfield2D::globalDirection(unsigned int index) const
{
	return globalMotion().map(origin(index)) - origin(index);
}

/**
 * The squared length of a vector at a given index in this vectorfield.
 * 
 * \param index The index of the vector.
 * \return The squared length of the vector at the given index.
 */
float Vectorfield2D::squaredLength(unsigned int index) const
{
    return direction(index).squaredLength();
}

/**
 * The length of a vector at a given index in this vectorfield.
 * 
 * \param index The index of the vector.
 * \return The length of the vector at the given index.
 */
float Vectorfield2D::length(unsigned int index) const
{
    return direction(index).length();
}

/**
 * The angle of a vector at a given index in this vectorfield.
 * The result will be in degrees. 
 * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
 * 
 * \param index The index of the vector.
 * \return The angle of the vector at the given index.
 */
float Vectorfield2D::angle(unsigned int index) const
{
    return direction(index).angle();
}

/**
 * The target of a vector at a given index in this vectorfield.
 * Mainly origin(index) + direction(index).
 * 
 * \param index The index of the vector.
 * \return The direction of the vector at the given index.
 */
Vectorfield2D::PointType Vectorfield2D::target(unsigned int index) const
{	
	return origin(index) + direction(index);	
}

/**
 * Sets the target of a vector at a given index in this vectorfield.
 * to a point. Since this is just an interface without an actual strorage,
 * we leave the implementation open but mandatory for subclasses.
 * Does nothing if the model is locked.
 * 
 * \param index The index of the vector.
 * \param new_t The new target of the vector.
 * \return The direction of the vector at the given index.
 */
void Vectorfield2D::setTarget(unsigned int index, const PointType& new_t)
{
    if(locked())
        return;
    
	setDirection(index, new_t - origin(index));
    //SetDirection needs to call updateModel()!
}
    
} //end of namespace graipe
