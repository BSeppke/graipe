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

#ifndef GRAIPE_VECTORFIELDS_SPARSEVECTORFIELD_HXX
#define GRAIPE_VECTORFIELDS_SPARSEVECTORFIELD_HXX

#include "core/core.h"
#include "vectorfields/vectors.hxx"

#include <vector>

namespace graipe {

/**
 * The base class of all sparse 2D vectorfields. This can also be considered as an interface,
 * since all other vectorfields, either sparse or dense ones inherit from this class.
 */
template <class T>
class SparseVectorfield2DBase
:   public ItemListModel<T>
{
	
    public:
		//The used point type
        typedef typename T::PointType PointType;
    
        /**
         * Default constructor. Creates an empty vectorfield.
         */
        SparseVectorfield2DBase()
        :  m_global_motion(new TransformParameter("Global motion matrix:")),
           m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
        {
            Model::m_parameters->addParameter("globalMotion", m_global_motion);
            Model::m_parameters->addParameter("scale", m_scale);
        }
    
        /**
         * Copy constructor. Creates a vectorfield from another one.
         *
         * \param vf The other vectorfield.
         */
        SparseVectorfield2DBase(const SparseVectorfield2DBase<T> & vf)
        :   m_global_motion(new TransformParameter("Global motion matrix:")),
            m_scale(new DoubleParameter("scale (px->cm/s)",0,99999,10))
        {
            Model::m_parameters->addParameter("globalMotion", m_global_motion);
            Model::m_parameters->addParameter("scale", m_scale);
        }
        
        /**
         * Getter for the scaling function of the vector length
         * from units/pixels to cm/s.
         *
         * \return The scaling value from pixels to cm/s.
         */
        double scale() const
        {
            return m_scale->value();
        }
    
        /**
         * Setter for the scaling function of the vector length
         * from units/pixels to cm/s.
         * Should do nothing if the model is locked.
         *
         * \param scale The scaling value from pixels to cm/s.
         */
        void setScale(double scale)
        {
            if(Model::locked())
                return;

            m_scale->setValue(scale);
            Model::updateModel();
        }
    
        /**
         * Getter for the global motion of a vectorfield. Note that the
         * vectors are still containing the *complete* motion. So if you 
         * want to get the local motion only, you need to substract the
         * global motion from each vector.
         *
         * \return The global motion by means of an affine matrix.
         */
        QTransform globalMotion() const
        {
            return m_global_motion->value();
        }
    
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
        void setGlobalMotion(const QTransform& trans)
        {
            if (Model::locked())
                return;
            
            m_global_motion->setValue(trans);
            Model::updateModel();
        }
	
    protected:
        //Additional parameters:
        TransformParameter * m_global_motion;
        DoubleParameter    * m_scale;
};

/**
 * The simplest sparse 2D vectorfield.
 * These vectorfields can be considered as lists of 2 points for each vector.
 * One point describing the origin, the other one the direction of the vector
 */
typedef SparseVectorfield2DBase<Vector2D> SparseVectorfield2D;

/**
 * Extension of the simple class for sparse 2D vectorfields.
 * This class lets you assign one weight for each vector.
 */
typedef SparseVectorfield2DBase<WeightedVector2D> SparseWeightedVectorfield2D;


template <class T>
class SparseMultiVectorfield2DBase
:   public SparseVectorfield2DBase<T>
{
    public:
		//The used point type
        typedef typename SparseVectorfield2DBase<T>::PointType PointType;
    
        /**
         * Default constructor. Creates an empty vectorfield.
         */
        SparseMultiVectorfield2DBase()
        :   SparseVectorfield2DBase<T>(),
            m_alternatives(new IntParameter("number of alternative directions",0,1000,10))
        {
            SparseVectorfield2DBase<T>::m_parameters->addParameter("alternatives", m_alternatives);
        }
    
        /**
         * Copy constructor. Creates a vectorfield from another one.
         *
         * \param vf The other vectorfield.
         */
        SparseMultiVectorfield2DBase(const SparseVectorfield2DBase<T> & vf)
        :   SparseVectorfield2DBase<T>(),
            m_alternatives(new IntParameter("number of alternative directions",0,1000,10))
        {
            SparseVectorfield2DBase<T>::m_parameters->addParameter("alternatives", m_alternatives);
        }
        
        /**
         * Getter for the scaling function of the vector length
         * from units/pixels to cm/s.
         *
         * \return The scaling value from pixels to cm/s.
         */
        double alternatives() const
        {
            return m_alternatives->value();
        }
    
        /**
         * Setter for the scaling function of the vector length
         * from units/pixels to cm/s.
         * Should do nothing if the model is locked.
         *
         * \param scale The scaling value from pixels to cm/s.
         */
        void setAlternatives(int alternatives)
        {
            if(Model::locked())
                return;

            m_alternatives->setValue(alternatives);
            Model::updateModel();
        }
   
   protected:
        IntParameter* m_alternatives;
};

/**
 * Extension of the simple class for sparse 2D vectorfields.
 * This class lets you assign more than one direction for each vector.
 * Beside direction, altDirections hold these directions
 * other direction may be set.
 */
typedef SparseMultiVectorfield2DBase<MultiVector2D> SparseMultiVectorfield2D;

/**
 * A sparse vectorfield with multiple weighted directions for each vector
 * The weight for the assigned direction is stored in weight, the weights for the
 * altDirections are stored in altWeights
 */
typedef SparseMultiVectorfield2DBase<WeightedMultiVector2D> SparseWeightedMultiVectorfield2D;

} //end of namespace graipe

#endif
