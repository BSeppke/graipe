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

#ifndef GRAIPE_VECTORFIELDS_DENSEVECTORFIELD_HXX
#define GRAIPE_VECTORFIELDS_DENSEVECTORFIELD_HXX

#include "core/core.h"
#include "vectorfields/vectorfield.hxx"

#include "vigra/multi_array.hxx"

namespace graipe {

/**
 * This class represents a dense vectorfield. Instead of keeping
 * each single vector by means of position and direction, we use two 2D
 * arrays here and store the motion component in x direction (m_u) and in
 * y direction (m_v) repectively.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseVectorfield2D
:   public Vectorfield2D
{	
	public:
        //Some convenient typedefs:
        typedef vigra::MultiArray<2,float> ArrayType;
        typedef vigra::MultiArrayView<2,float> ArrayViewType;
        typedef ArrayType::difference_type DiffType;
        
        /**
         * Default constructor. Creates an empty dense vectorfield.
         */
        DenseVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a dense vectorfield from another one.
         *
         * \param vf The other dense vectorfield.
         */
        DenseVectorfield2D(const DenseVectorfield2D & vf);
		
        /**
         * Create an empty dense vectorfield of a given 2D shape .
         *
         * \param shape The 2D shape, which, contains (width, height).
         */
		DenseVectorfield2D(const DiffType& shape, Workspace* wsp);
    
        /**
         * Create an empty dense vectorfield of a given 2D shape.
         *
         * \param width The width of the new dense vectorfield.
         * \param height The height of the new dense vectorfield.
         */
		DenseVectorfield2D(int width, int height, Workspace* wsp);
    
        /**
         * Create a dense vectorfield from two arrays, which need to contain.
         * the vectors direction in x- (u) and y- (v) direction. All data will
         * be copied.
         *
         * \param u The x-directions of the vectors.
         * \param v The y-directions of the vectors.
         */
		DenseVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, Workspace* wsp);
		
        /**
         * The typename of this vector field.
         *
         * \return Always "DenseVectorfield2D"
         */
		QString typeName() const
        {
            return "DenseVectorfield2D";
        }
		
        /**
         * The size of this vectorfield. 
         * Implemented here, defined as pure virtual in base class.
         * 
         * \return The number of vectors in this vectorfield (width x height).
         */
		unsigned int size() const;
        
        /**
         * This does not remove all the vectors, but resets their directions to zero.
         * Does nothing if the model is locked.
         */
		void clear();
    
        /**
         * Checks if a position is inside this vectorfield
         *
         * \param d The position to be checked.
         */
        bool isInside(const DiffType & d);
    
        /**
         * Checks if a position is inside this vectorfield
         *
         * \param x The x-component of the position.
         * \param y The y-component of the position.
         */
        bool isInside(unsigned int x, unsigned int y);
    
        /**
         * The origin/position of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * May throw an error, if the index is out of bounds.
         *
         * ATTENTION: Since the origin is not naturally defined for dense vectorfields
         *            one should not use this function, if possible.
         * 
         * \param index The index of the vector.
         * \return The origin of the vector at the given index.
         */
		PointType origin(unsigned int index) const;
    
        /**
         * The direction of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * May throw an error, if the index is out of bounds.
         *
         * ATTENTION: Since neither PointTypes are not stored inside the arrays
         *            nor are 2D indexes used, use this function with care.
         * 
         * \param index The index of the vector.
         * \return The direction of the vector at the given index.
         */
		PointType direction(unsigned int index) const;
        
        /**
         * The direction of a vector at a given position in this vectorfield.
         * May throw an error, if the position is out of bounds.
         *
         * ATTENTION: Since no PointTypes are not stored inside the arrays,
         *            use this function with care.
         * 
         * \param orig The position of the vector.
         * \return The direction of the vector at the given position.
         */
		virtual PointType direction(const PointType& orig) const;
        
        /**
         * The direction of a vector at a given position in this vectorfield.
         * May throw an error, if the position is out of bounds.
         *
         * ATTENTION: Since no PointTypes are not stored inside the arrays,
         *            use this function with care.
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The direction of the vector at the given position.
         */
		virtual PointType direction(unsigned int x, unsigned int y) const;
    
        /**
         * Set the direction of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * Does nothing if the model is locked.
         *
         * ATTENTION: Since the arrays are 2D indexed, prefer using the alternative
         *            function.
         * 
         * \param index The index of the new direction.
         * \param new_d The new direction of the vector.
         */
		void setDirection(unsigned int index, const PointType& new_d);
     
        /**
         * Set the direction of a vector at a given position in this vectorfield.
         * Does nothing if the model is locked.
         * 
         * \param orig The position of the vector.
         * \param new_d The new direction of the vector.
         */
		virtual void setDirection(const PointType& orig, const PointType& new_d);
    
        /**
         * Set the direction of a vector at a given position in this vectorfield.
         * Does nothing if the model is locked.
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \param new_d The new direction of the vector.
         */
		virtual void setDirection(unsigned int x, unsigned int y, const PointType& new_d);
    
        /**
         * The local direction of a vector at a given position w.r.t. the global
         * transformation matrix given in globalMotion().
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The local direction of the vector at the given index.
         */
        virtual PointType localDirection(unsigned int x, unsigned int y) const;
    
        /**
         * The local direction of a vector at a given position w.r.t. the global
         * transformation matrix given in globalMotion().
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The local direction of the vector at the given index.
         */
        virtual PointType globalDirection(unsigned int x, unsigned int y) const;
    
        /**
         * The squared length of a vector at a given index in this vectorfield.
         * 
         * \param index The index of the vector.
         * \return The squared length of the vector at the given index.
         */
        float squaredLength(unsigned int index) const;
    
        /**
         * The squared length of a vector at a given position in this vectorfield.
         * 
         * \param orig The position of the vector.
         * \return The squared length of the vector at the given index.
         */
        virtual float squaredLength(const PointType& orig) const;
    
        /**
         * The squared length of a vector at a given position in this vectorfield.
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The squared length of the vector.
         */
        virtual float squaredLength(unsigned int x, unsigned int y) const;
    
        /**
         * The length of a vector at a given index in this vectorfield.
         * 
         * \param index The index of the vector.
         * \return The length of the vector.
         */
        float length(unsigned int index) const;
    
        /**
         * The length of a vector at a given position in this vectorfield.
         * 
         * \param orig The position of the vector.
         * \return The length of the vector.
         */
        virtual float length(const PointType& orig) const;
    
        /**
         * The length of a vector at a given position in this vectorfield.
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The length of the vector.
         */
        virtual float length(unsigned int x, unsigned int y) const;
    
        /**
         * The angle of a vector at a given position in this vectorfield.
         * The result will be in degrees. 
         * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The angle of the vector.
         */
        float angle(unsigned int index) const;
    
        /**
         * The angle of a vector at a given position in this vectorfield.
         * The result will be in degrees. 
         * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * 
         * \param orig The position of the vector.
         * \return The angle of the vector.
         */
        virtual float angle(const PointType& orig) const;
    
        /**
         * The angle of a vector at a given position in this vectorfield.
         * The result will be in degrees. 
         * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The angle of the vector at the given index.
         */
        virtual float angle(unsigned int x, unsigned int y) const;
	
        /**
         * The target of a vector at a given index in this vectorfield.
         * Mainly origin(index) + direction(index).
         * 
         * \param index The index of the vector.
         * \return The direction of the vector at the given index.
         */
        PointType target(unsigned int index) const;
    
        /**
         * The target of a vector at a given position in this vectorfield.
         * Mainly origin(orig) + direction(orig).
         * 
         * \param orig The position of the vector.
         * \return The direction of the vector.
         */
        virtual PointType target(const PointType& orig) const;
    
        /**
         * The target of a vector at a given position in this vectorfield.
         * Mainly origin(x,y) + direction(x,y).
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The target of the vector.
         */
        virtual PointType target(unsigned int x, unsigned int y) const;
    
        /**
         * Sets the target of a vector at a given index in this vectorfield.
         * to a point. Does nothing if the model is locked.
         * 
         * \param index The index of the vector.
         * \param new_t The new target of the vector.
         */
        void setTarget(unsigned int index, const PointType& new_t);
        
        /**
         * Sets the target of a vector at a given position in this vectorfield.
         * to a point. Does nothing if the model is locked.
         * 
         * \param orig The position of the vector.
         * \param new_t The new target of the vector.
         */
        virtual void setTarget(const PointType& orig, const PointType& new_t);
    
        /**
         * Sets the target of a vector at a given position in this vectorfield.
         * to a point. Does nothing if the model is locked.
         * 
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \param new_t The new target of the vector.
         */
        virtual void setTarget(unsigned int x, unsigned int y, const PointType& new_t);
    
        /**
         * Constant reading access to the x-component of the direction
         * vector. Use this access method for the best performance.
         *
         * \return A constant view on the array data for the x-direction.
         */
		const ArrayViewType & u() const;
    
        /**
         * Setter for the y-component of the direction vector.
         * Does nothing if the model is locked or shapes mismatch.
         * Use this access method for the best performance.
         *
         * \param new_v A constant view on the new array data for the y-direction.
         */
        void setU(const ArrayViewType& new_v);
	
        /**
         * Constant reading access to the y-component of the direction
         * vector. Use this access method for the best performance.
         *
         * \return A constant view on the array data for the y-direction.
         */
		const ArrayViewType & v() const;
    
        /**
         * Setter for the y-component of the direction vector.
         * Does nothing if the model is locked or shapes mismatch.
         * Use this access method for the best performance.
         *
         * \param new_v A constant view on the new array data for the y-direction.
         */
        void setV(const ArrayViewType& new_v);
    
        /**
         * Serialize the complete content of the dense vectorfield to an xml file.
         * The serialization is just a binary stream of m_u followed by m_v.
         *
         * \param out The output device for serialization.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a  dense vectorfield from an xml file.
         * Since the serialization is just a binary stream of m_u and m_v
         * and we already know the size, it is quite easy the deserialize.
         *
         * \param xmlReader The QXmlStreamReader, where we will read from.
         * \return True, if the content could be deserialized and the model is not locked.
         */
		bool deserialize_content(QXmlStreamReader& xmlReader);
    
    protected slots:
        /**
         * Specialization of Model's updateModel procedure.
         * Is called everytime, the model changes.
         */
        void updateModel();
    
	protected:
        /**
         * Index conversion: From (x,y) position -> flat 1D index.
         *
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The flat 1D index.
         */
		unsigned int xyToIdx(unsigned int x, unsigned int y) const;
        
        /**
         * Index conversion: From a flat 1D index -> the x-component of the position.
         *
         * \param index The flat 1D index.
         * \return The x-component of the position.
         */
		unsigned int indexToX(unsigned int index) const;

        /**
         * Index conversion: From a flat 1D index -> the y-component of the position.
         *
         * \param index The flat 1D index.
         * \return The y-component of the position.
         */
		unsigned int indexToY(unsigned int index) const;
		
        //Data storage
		ArrayType m_u, m_v;
};

/**
 * This class extense a dense vectorfield by means of a weight component, 
 * which is stored by means of a third array, namely m_w.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseWeightedVectorfield2D : public DenseVectorfield2D
{
	public:
        /**
         * Default constructor. Creates an empty dense weighted vectorfield.
         */
        DenseWeightedVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a dense weighted vectorfield from a non weighted one.
         * The weights will be zero initialized.
         *
         * \param vf The other dense vectorfield.
         */
        DenseWeightedVectorfield2D(const DenseVectorfield2D & vf);
    
        /**
         * Copy constructor. Creates a dense weighted vectorfield from another one.
         *
         * \param vf The other dense weighted vectorfield.
         */
        DenseWeightedVectorfield2D(const DenseWeightedVectorfield2D & vf);
		
        /**
         * Create an empty dense weighted vectorfield of a given 2D shape .
         *
         * \param shape The 2D shape, which, contains (width, height).
         */
		DenseWeightedVectorfield2D(const DiffType& shape, Workspace* wsp);
    
        /**
         * Create an empty dense weighted vectorfield of a given 2D shape.
         *
         * \param width The width of the new dense weighted vectorfield.
         * \param height The height of the new dense weighted vectorfield.
         */
		DenseWeightedVectorfield2D(int width, int height, Workspace* wsp);
    
        /**
         * Create a dense weighed vectorfield from two arrays, which need to contain.
         * the vectors direction in x- (u) and y- (v) direction. All data will
         * be copied. The weights will be zero initialized.
         *
         * \param u The x-directions of the vectors.
         * \param v The y-directions of the vectors.
         */
		DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, Workspace* wsp);
    
        /**
         * Create a dense weighted vectorfield from two arrays, which need to contain.
         * the vectors direction in x- (u) and y- (v) direction. All data will
         * be copied.
         *
         * \param u The x-directions of the vectors.
         * \param v The y-directions of the vectors.
         * \param w The weights of the vectors.
         */
		DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, const ArrayViewType& w, Workspace* wsp);
    
        /**
         * The typename of this vector field.
         *
         * \return Always "DenseWeightedVectorfield2D"
         */
		QString typeName() const
        {
            return "DenseWeightedVectorfield2D";
        }
        
        /**
         * This does not remove all the vectors, but resets their directions 
         * and weights to zero.
         * Does nothing if the model is locked.
         */
         void clear();
		
		/**
         * Getter for the  weight of a vector at a given index. 
         * May throw an error, if the index is out of bounds.
         *
         * \param index The index of the vector, for which we want the weight.
         * \return The weight of that vector.
         */
		virtual float weight(unsigned int index) const;
    
		/**
         * Getter for the  weight of a vector at a given position.
         * May throw an error, if the index is out of bounds.
         *
         * \param orig The position of the vector.
         * \return The weight of that vector.
         */
		virtual float weight(const PointType& orig) const;
    
		/**
         * Getter for the  weight of a vector at a given position.
         * May throw an error, if the index is out of bounds.
         *
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \return The weight of that vector.
         */
		virtual float weight(unsigned int x, unsigned int y) const;
		
		/**
         * Setter for the  weight of a vector at a given index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the vector, for which we want the weight.
         * \param new_w The new weight of that vector.
         */
		virtual void setWeight(unsigned int index, float new_w);
    
		/**
         * Setter for the  weight of a vector at a given position.
         * Does nothing if the model is locked.
         *
         * \param orig The position of the vector.
         * \param new_w The new weight of that vector.
         */
		virtual void setWeight(const PointType& orig, float new_w);
    
		/**
         * Setter for the  weight of a vector at a given position.
         * Does nothing if the model is locked.
         *
         * \param x The x position of the vector.
         * \param y The y position of the vector.
         * \param new_w The new weight of that vector.
         */
		virtual void setWeight(unsigned int x, unsigned int y, float new_w);
		
        /**
         * Serialize the complete content of the dense weighted vectorfield to an xml file.
         * The serialization is just a binary stream of m_u followed by m_v and m_w.
         *
         * \param out The output device for serialization.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a  dense vectorfield from an xml file.
         * Since the serialization is just a binary stream of m_u, m_v and m_w
         * and we already know the size, it is quite easy the deserialize.
         *
         * \param xmlReader The QXmlStreamReader, where we will read from.
         * \return True, if the content could be deserialized and the model is not locked.
         */
		bool deserialize_content(QXmlStreamReader& xmlReader);
		
        /**
         * Constant reading access to the weights of each direction
         * vector. Use this access method for the best performance.
         *
         * \return A constant view on the array data for the weights.
         */
		const ArrayViewType & w() const;
    
        /**
         * Setter for the weights of the direction vectors.
         * Does nothing if the model is locked or shapes mismatch.
         * Use this access method for the best performance.
         *
         * \param new_w A constant view on the new array data for the weights.
         */
        void setW(const DenseVectorfield2D::ArrayViewType& new_w);
    
    protected slots:
        /**
         * Specialization of Model's updateModel procedure.
         * Is called everytime, the model changes.
         */
        void updateModel();

    protected:
        //Storage for the weights
        ArrayType m_w;
};

} //end of namespace graipe

#endif
