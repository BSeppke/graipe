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

#ifndef GRAIPE_VECTORFIELDS_VECTORFIELD_HXX
#define GRAIPE_VECTORFIELDS_VECTORFIELD_HXX

#include "core/core.h"
#include "vectorfields/config.hxx"

namespace graipe {

/**
 * The base class of all 2D vectorfields. This can also be considered as an interface,
 * since all other vectorfields, either sparse or dense ones inherit from this class.
 */
class GRAIPE_VECTORFIELDS_EXPORT Vectorfield2D
:   public Model
{
	
    public:
		//The used point type
        typedef QPointFX PointType;
    
        /**
         * Default constructor. Creates an empty vectorfield.
         */
        Vectorfield2D(Environment* env);
    
        /**
         * Copy constructor. Creates a vectorfield from another one.
         *
         * \param vf The other vectorfield.
         */
        Vectorfield2D(const Vectorfield2D & vf);
    
        /**
         * The typename of this vector field.
         *
         * \return Always "Vectorfield2D"
         */
        QString typeName() const
        {
            return "Vectorfield2D";
        }
    
        /**
         * Getter for the scaling function of the vector length
         * from units/pixels to cm/s.
         *
         * \return The scaling value from pixels to cm/s.
         */
        double scale() const;
        
        /**
         * Setter for the scaling function of the vector length
         * from units/pixels to cm/s.
         * Should do nothing if the model is locked.
         *
         * \param scale The scaling value from pixels to cm/s.
         */
        void setScale(double scale);
    
        /**
         * Getter for the global motion of a vectorfield. Note that the
         * vectors are still containing the *complete* motion. So if you 
         * want to get the local motion only, you need to substract the
         * global motion from each vector.
         *
         * \return The global motion by means of an affine matrix.
         */
        QTransform globalMotion() const;
    
        /**
         * Setter for the global motion of a vectorfield. Note that the
         * vectors are still containing the *complete* motion after setting
         * the global motion using this function. If you
         * want to get the local motion only, you need to substract the
         * global motion from each vector.
         * Should do nothing if the model is locked.
         *
         * \param trans The global motion by means of an affine matrix.
         */
        void setGlobalMotion(const QTransform& trans);
	
        /**
         * The size of this vectorfield. 
         * Since this is just an interface without an actual strorage,
         * we leave the implementation open but mandatory for subclasses.
         * 
         * \return The number of vectors in this vectorfield.
         */
        virtual unsigned int size() const = 0;
    
        /**
         * The removal of all vectors of this vectorfield.
         * Since this is just an interface without an actual strorage,
         * we leave the implementation open but mandatory for subclasses.
         * Should do nothing if the model is locked.
         */
        virtual void clear() = 0;
    
        /**
         * The origin/position of a vector at a given index in this vectorfield.
         * Since this is just an interface without an actual strorage,
         * we leave the implementation open but mandatory for subclasses.
         * 
         * \param index The index of the vector.
         * \return The origin of the vector at the given index.
         */
        virtual PointType origin(unsigned int index) const = 0;
    
        /**
         * The direction of a vector at a given index in this vectorfield.
         * Since this is just an interface without an actual strorage,
         * we leave the implementation open but mandatory for subclasses.
         * 
         * \param index The index of the vector.
         * \return The direction of the vector at the given index.
         */
        virtual PointType direction(unsigned int index) const = 0;
    
        /**
         * Sets the direction of a vector at a given index in this vectorfield.
         * to a Point containing dx and dy. Since this is just an interface without an actual strorage,
         * we leave the implementation open but mandatory for subclasses.
         * Shoudl do nothing if the model is locked.
         * 
         * \param index The index of the vector.
         * \param new_d The new direction of the vector.
         * \return The direction of the vector at the given index.
         */
        virtual void setDirection(unsigned int index, const PointType& new_d) = 0;
    
    
        /**
         * The local direction of a vector at a given index w.r.t. the global
         * transformation matrix given in globalMotion().
         * 
         * \param index The index of the vector.
         * \return The local direction of the vector at the given index.
         */
        PointType localDirection(unsigned int index) const;
    
        /**
         * The local direction of a vector at a given index w.r.t. the global
         * transformation matrix given in globalMotion().
         * 
         * \param index The index of the vector.
         * \return The local direction of the vector at the given index.
         */
        PointType globalDirection(unsigned int index) const;
    
        /**
         * The squared length of a vector at a given index in this vectorfield.
         * 
         * \param index The index of the vector.
         * \return The squared length of the vector at the given index.
         */
        virtual float squaredLength(unsigned int index) const;
    
        /**
         * The length of a vector at a given index in this vectorfield.
         * 
         * \param index The index of the vector.
         * \return The length of the vector at the given index.
         */
        virtual float length(unsigned int index) const;
    
        /**
         * The angle of a vector at a given index in this vectorfield.
         * The result will be in degrees. 
         * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * 
         * \param index The index of the vector.
         * \return The angle of the vector at the given index.
         */
        virtual float angle(unsigned int index) const;
	
        /**
         * The target of a vector at a given index in this vectorfield.
         * Mainly origin(index) + direction(index).
         * 
         * \param index The index of the vector.
         * \return The direction of the vector at the given index.
         */
        virtual PointType target(unsigned int index) const;
    
        /**
         * Sets the target of a vector at a given index in this vectorfield.
         * to a point. Does nothing if the model is locked.
         * 
         * \param index The index of the vector.
         * \param new_t The new target of the vector.
         * \return The direction of the vector at the given index.
         */
        virtual void setTarget(unsigned int index, const PointType& new_t);
    
    protected:
        //Additional parameters:
        TransformParameter * m_global_motion;
        DoubleParameter    * m_scale;
};

} //end of namespace graipe

#endif
