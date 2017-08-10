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

#ifndef GRAIPE_VECTORFIELDS_SPARSEVECTORFIELD_HXX
#define GRAIPE_VECTORFIELDS_SPARSEVECTORFIELD_HXX

#include "core/core.h"
#include "vectorfields/vectorfield.hxx"

#include <vector>

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *
 * @file
 * @brief Header file for sparse vectorfield classes
 */


/**
 * The base class of all sparse 2D vectorfields.
 * These vectorfields can be considered as lists of 2 points for each vector.
 * One point describing the origin, the other one the direction of the vector
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseVectorfield2D
:   public Vectorfield2D
{
	public:		
        /**
         * Default constructor. Creates an empty sparse vectorfield.
         */
        SparseVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a sparse vectorfield from another one.
         *
         * \param vf The other sparse vectorfield.
         */
		SparseVectorfield2D(const SparseVectorfield2D & vf);
	
        /**
         * The typename of this vector field.
         *
         * \return Always "SparseVectorfield2D"
         */
        QString typeName() const
        {
            return "SparseVectorfield2D";
        }
    
        /**
         * The size of this vectorfield. 
         * Implemented here, defined as pure virtual in base class.
         * 
         * \return The number of vectors in this vectorfield.
         */
		unsigned int size() const;
        
        /**
         * The removal of all vectors of this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * Does nothing if the model is locked.
         */
		void clear();
    
        /**
         * The origin/position of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \return The origin of the vector at the given index.
         */
        PointType origin(unsigned int index) const;
    
        /**
         * Set the origin/position of a vector at a given index in this vectorfield.
         * Does nothing if the model is locked.
         * 
         * \param index The index of the new origin.
         * \param new_o The new origin of the vector at that index.
         */
        void setOrigin(unsigned int index, const PointType& new_o);
    
        /**
         * The direction of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \return The direction of the vector at the given index.
         */
        PointType direction(unsigned int index) const;
    
        /**
         * Set the direction of a vector at a given index in this vectorfield.
         * Implemented here, defined as pure virtual in base class.
         * Does nothing if the model is locked.
         * 
         * \param index The index of the new direction.
         * \param new_d The new direction of the vector at that index.
         */
        void setDirection(unsigned int index, const PointType& new_p);

        /**
         * Add a vector to the sparse vectorfield.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir);
	
        /**
         * Removing a vector from the vector field at a given index
         * Does nothing if the model is locked.
         *
         * \param index. The index, where the vector shall be removed.
         */
        void removeVector(unsigned int index);
    
        /**
         * The content's item header for the vectorfield serialization.
         * 
         * \return Always: "pos_x, pos_y, dir_x, dir_y".
         */
		virtual QString csvHeader() const;
        
        /**
         * Serialization of a single vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y".
         */
		virtual QString itemToCSV(unsigned int index) const;
        
        /**
         * Deserialization/addition of a vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y
         */
		virtual bool itemFromCSV(const QString& serial);
        
        /**
         * Serialization of a single vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y".
         */
		virtual void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
        
        /**
         * Deserialization/addition of a vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y
         */
		virtual bool deserialize_item(QXmlStreamReader& xmlReader);
    
        /**
         * Serialize the complete content of the sparse vectorfield to an xml file.
         * Mainly prints:
         *   csvHeader
         * and for each vector:
         *   newline + serialize_item().
         *
         * \param out The output device for serialization.
         */
		virtual void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a  sparse vectorfield from an xml file.
         * The first line is the header as given in csvHeader, which is ignored however.
         * Each following line has to be one valid vector serialization.
         *
         * \param xmlReader The QXmlStreamReader, where we will read from.
         * \return True, if the content could be deserialized and the model is not locked.
         */
		virtual bool deserialize_content(QXmlStreamReader& xmlReader);
		
	protected:
        //Data containers for the origins and directions
        std::vector<PointType> m_origins;
		std::vector<PointType> m_directions;
};

/**
 * Extension of the base class for sparse 2D vectorfields.
 * This class lets you assign one weight for each vector.
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedVectorfield2D : public SparseVectorfield2D
{		
	public:	
        /**
         * Default constructor. Creates an empty weighted sparse vectorfield.
         */
        SparseWeightedVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a weighted sparse vectorfield from another one.
         *
         * \param vf The other weighted sparse vectorfield.
         */
		SparseWeightedVectorfield2D(const SparseWeightedVectorfield2D & vf);
		
        /**
         * The typename of this vectorfield.
         *
         * \return Always "SparseWeightedVectorfield2D"
         */
        QString typeName() const
        {
            return "SparseWeightedVectorfield2D";
        }
    
        /**
         * The removal of all vectors of this vectorfield.
         * Specialized for this class.
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
         * Setter for the  weight of a vector at a given index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the vector, for which we want the weight.
         * \param new_w The new weight of that vector.
         */
		virtual void setWeight(unsigned int index, float new_w);
		
        /**
         * Add a vector to the weighted sparse vectorfield.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir);
    
        /**
         * Add a weighted vector to the weighted sparse vectorfield.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param w  The weight of the new vector.
         */
		virtual void addVector(const PointType& orig, const PointType& dir, float w);
	
        /**
         * Removing a vector from the vector field at a given index
         * Does nothing if the model is locked or the index is out of bounds.
         *
         * \param index. The index, where the vector shall be removed.
         */
        void removeVector(unsigned int index);
    
        /**
         * The content's item header for the weighted vectorfield serialization.
         * 
         * \return Always: "pos_x, pos_y, dir_x, dir_y, weight".
         */
		QString csvHeader() const;
        
        /**
         * Serialization of a single weighted vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight".
         */
		QString itemToCSV(unsigned int index) const;
        
        /**
         * Deserialization/addition of a weighted vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, weight
         */
		bool itemFromCSV(const QString& serial);
        
        /**
         * Serialization of a single weighted vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight".
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
        
        /**
         * Deserialization/addition of a weighted vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, weight
         */
		bool deserialize_item(QXmlStreamReader& xmlWriter);
		
	protected:
        //Storage of the weights
		std::vector<float> m_weights;
};

/**
 * Extension of the base class for sparse 2D vectorfields.
 * This class lets you assign more than one direction for each vector.
 * Beside the direction, which we consider to hava an alternative index of zero,
 * other direction may be set.
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseMultiVectorfield2D
:   public SparseVectorfield2D
{
	public:
        /**
         * Default constructor. Creates an empty sparse multi vectorfield.
         */
        SparseMultiVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a sparse multi vectorfield from another one.
         *
         * \param vf The other sparse multi vectorfield.
         */
		SparseMultiVectorfield2D(const SparseMultiVectorfield2D & vf);
		
        /**
         * The typename of this vectorfield.
         *
         * \return Always "SparseMultiVectorfield2D"
         */
        QString typeName() const
        {
            return "SparseMultiVectorfield2D";
        }
    
        /**
         * The removal of all vectors of this vectorfield.
         * Specialized for this class.
         * Does nothing if the model is locked.
         */
		void clear();
    
        /**
         * Getter for the  number of alternative directions for a 
         * sparse multi vectorfield. Note that the overall direction count is:
         * alternatives+1, since the original direction stays untouched.
         *
         * \return The number of alternative directions of a vector.
         */
		unsigned int alternatives() const;
    
        /**
         * Setter for the  number of alternative directions for a
         * sparse multi vectorfield. Note that the overall direction count is:
         * alternatives+1, since the original direction stays untouched.
         * Does nothing if the model is locked.
         * Rescales all (already existing alternatives into larger data containers
         * or cuts the contents, if the number of alternatives is decreased
         *
         * \param alternatives The number of alternative directions of a vector.
         */
		void setAlternatives(unsigned int alternatives);
		
        /**
         * The direction of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the alternative direction. If this is zero,
         *        it returns the (original) direction of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative direction.
         * \return The (alternative) direction of the vector at the given index.
         */
		virtual PointType altDirection(unsigned int index, unsigned int alt_index) const;
        
        /**
         * Sets the direction of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * Does nothing if the model is locked.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the altivative direction. If this is zero,
         *        it sets the (original) direction of the vector. If it is between
         *        1<=alt_index<=alterantives(), it sets the alternative direction.
         * \param new_d The new (alternative) direction of the vector at the given index.
         */
		virtual void setAltDirection(unsigned int index, unsigned int alt_index, const PointType& new_d);
    
        /**
         * The local direction of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the alternative local direction. If this is zero,
         *        it returns the (original) local direction of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative local direction.
         * \return The (alternative) local direction of the vector at the given index.
         */
		virtual PointType altLocalDirection(unsigned int index, unsigned int alt_index) const;
        
        /**
         * The global direction of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the alternative global direction. If this is zero,
         *        it returns the (original) global direction of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative global direction.
         * \return The (alternative) global direction of the vector at the given index.
         */
		virtual PointType altGlobalDirection(unsigned int index, unsigned int alt_index) const;
    
        /**
         * The squared length of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the alternative squared length. If this is zero,
         *        it returns the (original) squared length of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative squared length.
         * \return The (alternative) squared length of the vector at the given index.
         */
        virtual float altSquaredLength(unsigned int index, unsigned int alt_index) const;
    
        /**
         * The length of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         *
         * \param index The index of the vector.
         * \param alt_index The index of the alternative length. If this is zero,
         *        it returns the (original) length of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative length.
         * \return The (alternative) length of the vector at the given index.
         */
		virtual float altLength(unsigned int index, unsigned int alt_index) const;
            
        /**
         * The angle of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         *
         * \param index The index of the vector.
         * \param alt_index The index of the alternative angle. If this is zero,
         *        it returns the (original) angle of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative angle.
         * \return The (alternative) angle of the vector at the given index.
         */
		virtual float altAngle(unsigned int index, unsigned int alt_index) const;
        
        /**
         * The target of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         *
         * \param index The index of the vector.
         * \param alt_index The index of the alternative target. If this is zero,
         *        it returns the (original) target of the vector. If it is between
         *        1<=alt_index<=alternatives(), it returns the alternative target.
         * \return The (alternative) target of the vector at the given index.
         */
		virtual PointType altTarget(unsigned int index, unsigned int alt_index) const;
	
        /**
         * Sets the target of a vector at a given index in this vectorfield.
         * May throw an error, if the index is out of bounds.
         * Does nothing if the model is locked.
         * 
         * \param index The index of the vector.
         * \param alt_index The index of the altivative target. If this is zero,
         *        it sets the (original) target of the vector. If it is between
         *        1<=alt_index<=alterantives(), it sets the alternative target.
         * \param new_t The new (alternative) target of the vector at the given index.
         */
		virtual void setAltTarget(unsigned int index, unsigned int alt_index, const PointType& new_t);
		
        /**
         * Add a vector to the sparse multi vectorfield. This method adds empty alternative
         * directions, given by (0,0) to the alternative list.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir);
    
        /**
         * Add a vector to the sparse multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * If the list of alternatives is smaller than alternatives(), the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives(), the first alternative()
         * entries will be copied and the rest will be dismissed.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param alt_dirs  The alternative directions of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir, const std::vector<PointType>& alt_dirs);
    
        /**
         * Add a vector to the sparse multi vectorfield. This method adds a list of directions
         * (first) main direction, (others) alternative directions to the vectorfield.
         * If the list of directions is smaller than alternatives()+1, the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives()+1, the first alternative()
         * entries will be copied and the rest will be dismissed.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param alt_dirs  The alternative directions of the new vector.
         */
		void addVector(const PointType& orig, const std::vector<PointType>& all_dirs);
	
        /**
         * Removing a vector from the vector field at a given index
         * Does nothing if the model is locked or the index is out of bounds.
         *
         * \param index. The index, where the vector shall be removed.
         */
        void removeVector(unsigned int index);
    
        /**
         * The content's item header for the multi vectorfield serialization.
         * 
         * \return Always: "pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y".
         */
		QString csvHeader() const;
        
        /**
         * Serialization of a single multi vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y".
         */
		QString itemToCSV(unsigned int index) const;
        
        /**
         * Deserialization/addition of a multi vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y
         */
		bool itemFromCSV(const QString& serial);
    
        /**
         * Serialization of a single multi vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y".
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
        
        /**
         * Deserialization/addition of a multi vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y
         */
		bool deserialize_item(QXmlStreamReader& xmlWriter);
    
    protected slots:
        /**
         * This slot is called, whenever some parameter is changed.
         * It rearranges the size of the alternatives' vector.
         */
        void updateModel();
    
	protected:
        //Container for the alternative directions
		std::vector<std::vector<PointType> > m_alt_directions;
    
        //Additional parameters:
        IntParameter * m_alternatives;
};

//A sparse vectorfield with multiple weighted targets for each vector
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedMultiVectorfield2D : public SparseMultiVectorfield2D
{
	public:
        /**
         * Default constructor. Creates an empty sparse weighted multi vectorfield.
         */
        SparseWeightedMultiVectorfield2D(Workspace* wsp);
    
        /**
         * Copy constructor. Creates a sparse weighted multi vectorfield from another one.
         *
         * \param vf The other sparse weighted multi vectorfield.
         */
		SparseWeightedMultiVectorfield2D(const SparseWeightedMultiVectorfield2D & vf);
		
        /**
         * The typename of this vectorfield.
         *
         * \return Always "SparseWeightedMultiVectorfield2D"
         */
        QString typeName() const
        {
            return "SparseWeightedMultiVectorfield2D";
        }
		
        /**
         * The removal of all vectors of this vectorfield.
         * Specialized for this class.
         * Does nothing if the model is locked.
         */
		void clear();
		
		/**
         * Getter for the weight of a vector at a given index.
         * May throw an error, if the index is out of bounds.
         *
         * \param index The index of the vector, for which we want the weight.
         * \return The weight of that vector.
         */
		virtual float weight(unsigned int index) const;
		
        /**
         * Getter for the alternate weight of a vector at a given index and alt_index.
         * May throw an error, if the indices are out of bounds.
         *
         * \param index The index of the vector, for which we want the weight.
         * \param alt_index The index of the vector, for which we want the weight.
         * \return The weight of that (alternate) direction.
         */
		virtual float altWeight(unsigned int index, unsigned int alt_index) const;
    
        /**
         * Setter for the  weight of a vector at a given index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the vector, for which we want the weight.
         * \param weight The new weight of that direction.
         */
		virtual void setWeight(unsigned int index, float weight);

        /**
         * Setter for the alternate weight of a vector at a given index and alt_index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the vector, for which we want the weight.
         * \param alt_index The index of the vector, for which we want the weight.
         * \param weight The new weight of that (alternate) direction.
         */
		virtual void setAltWeight(unsigned int index, unsigned int alt_index, float weight);
    
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * All alternative directions will be set to (0,0).
         * The weight and the alternative weights will be set to zero.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir);
    
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * All alternative directions will be set to (0,0).
         * The alternative weights will be set to zero.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param weight  The weight of the new vector.
         */
        virtual void addVector(const PointType& orig, const PointType& dir, float weight);
    
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * If the list of alternatives is smaller than alternatives(), the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives(), the first alternative()
         * entries will be copied and the rest will be dismissed.
         * The weight and the alternative weights will be set to zero.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param alt_dirs  The alternative directions of the new vector.
         */
		void addVector(const PointType& orig, const PointType& dir, const std::vector<PointType>& alt_dirs);
    
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * If the list of alternatives is smaller than alternatives(), the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives(), the first alternative()
         * entries will be copied and the rest will be dismissed.
         * The alternative weights will be set to zero.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param weight The weight of the given direction.
         * \param alt_dirs  The alternative directions of the new vector.
         */
		virtual void addVector(const PointType& orig, const PointType& dir, float weight, const std::vector<PointType>& alt_dirs);
        
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a main direction
         * and given alternative directions to the alternative list.
         * If the list of alternatives is smaller than alternatives(), the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives(), the first alternative()
         * entries will be copied and the rest will be dismissed.
         * The same holds for the alternative weights.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param dir  The direction of the new vector.
         * \param weight The weight of the given direction.
         * \param alt_dirs  The alternative directions of the new vector.
         * \param alt_weights The alternative direction weights of the new vector.
         */
		virtual void addVector(const PointType& orig, const PointType& dir, float weight, const std::vector<PointType>& alt_dirs, const std::vector<float>& alt_weights);
    
        /**
         * Add a vector to the sparse weighted multi vectorfield. This method adds a all directions
         * (first) as main (other) as alternatives
         * If the list of alternatives is smaller than alternatives(), the first n entries
         * will be filled. 
         * If the list of alternatives is larger than alternatives(), the first alternative()
         * entries will be copied and the rest will be dismissed.
         * The same holds for the alternative weights.
         * Does nothing if the model is locked.
         *
         * \param orig The origin of the new vector.
         * \param all_dirs  The alternative directions of the new vector.
         * \param all_weights The alternative direction weights of the new vector.
         */
		virtual void addVector(const PointType& orig, const std::vector<PointType>& all_dirs, const std::vector<float>& all_weights);
	
        /**
         * Removing a vector from the vector field at a given index
         * Does nothing if the model is locked or the index is out of bounds.
         *
         * \param index. The index, where the vector shall be removed.
         */
        void removeVector(unsigned int index);
    
        /**
         * The content's item header for the weighted multi vectorfield serialization.
         * 
         * \return Always: "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
         */
		QString csvHeader() const;
        
        /**
         * Serialization of a single weighted multi vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
         */
		QString itemToCSV(unsigned int index) const;
        
        /**
         * Deserialization/addition of a weighted multi vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
         */
		bool itemFromCSV(const QString& serial);
        
        /**
         * Serialization of a single weighted multi vector inside the list at a given index.
         * The vector will be serialized by means of comma separated values.
         * 
         * \param index Index of the vector to be serialized.
         * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
        
        /**
         * Deserialization/addition of a weighted multi vector from a string to this list.
         *
         * \param serial A QString containing the serialization of the vector.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
         */
		bool deserialize_item(QXmlStreamReader& xmlWriter);
   
    protected slots:
        /**
         * This slot is called, whenever some parameter is changed.
         * It rearranges the size of the alternatives' vector.
         */
        void updateModel();
        		
	protected:
        //Storage for the weights and the alternative weights of the vectorfield
		std::vector<float> m_weights;
		std::vector<std::vector<float> > m_alt_weights;
};

/**
 * @}
 */

} //end of namespace graipe

#endif
