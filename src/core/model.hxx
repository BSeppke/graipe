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

#ifndef GRAIPE_CORE_MODEL_HXX
#define GRAIPE_CORE_MODEL_HXX

#include "core/config.hxx"
#include "core/serializable.hxx"

#include <QString>
#include <QVector>
#include <QTransform>
#include <QObject>
#include <QtDebug>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Header file for the base types of all data: The Model, ModelList and RasteredModel classes.
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

//Forward declaration of used parameter types:
class Parameter;
class ParameterGroup;
class StringParameter;
class LongStringParameter;
class PointParameter;
class PointFParameter;

/**
 * This is the base class of all objects we are working
 * with in the GRAIPE environment. If you want to introduce your own
 * type into the framework, you have to inherit from this class or its
 * inheritants.
 *
 * The very basic definition of each model is a two-dimensional shape,
 * which may be aligned in local (left, top, right, bottom) coordinates
 * as well as in global corrdinats (e.g. world coords). For now, we 
 * assume a model to be rectified aligned to each coordinate space.
 *
 * Since the model provides signals and slots, it inherits from 
 * QObject as well as from Serializable for import/export reasons.
 *
 * A model also holds it lock-status w.r.t. to read-only locks, e.g.
 * to ensure no editing while an algorithm runs on this model. The 
 * locking is implemented by means of a ticketing system. For each
 * lock-request, the locker gets a random id, which he needs to pass for
 * a successful unlocking to the model.
 */
class GRAIPE_CORE_EXPORT Model
:	public QObject,
	public Serializable
{
	Q_OBJECT
	
    public:
        /**
         * Default/empty contructor of the Model class
         */
        Model();
    
        /**
         * Copy contructor of the Model class
         *
         * \param model The other model, from which the parameters will be copied.
         */
		Model(const Model& model);
    
        /**
         * Destructor of the Model class
         */
		virtual ~Model();
    
        /**
         * The (full) model name. This returns the complete name.
         *
         * \return The name of the model.
         */
        virtual QString name() const;
    
        /**
         * The short model name. The short name is restricted to a certain amount of digits
         * and will be filled with "..."
         *
         * \param length The length of the short QString (default=60).
         * \return The short name of the model.
         */
		virtual QString shortName(unsigned int length=60) const;

        /**
         * Set the models name to a new QString.
         *
         * \param new_name The new name of the model.
         */
        virtual void setName(const QString & new_name);
	
        /**
         * Const accessor for the model description QString. 
         *
         * \return The description of the model.
         */
		virtual QString description() const;
    
        /**
         * Set the model's description to a new QString.
         *
         * \param new_description The new description of the model.
         */
		virtual void setDescription(const QString & new_description);
	
        /**
         * Const accessor for the model filename QString.
         *
         * \return The filename of the model.
         */
		virtual QString filename() const;
    
        /**
         * Set the model's filename to a new QString.
         *
         * \param new_filename The new filename of the model.
         */
		virtual void setFilename(const QString & new_filename);
    
		/**
         * Const accessor for the left (x-coordinate) of a Model.
         *
         * \return The left boundary of the Model.
         */
		virtual float left() const;
    
        /**
         * Set the left (x-coordinate) of a Model to a given value.
         *
         * \param new_left The new left boundary of the Model.
         */
		virtual void setLeft(float new_left);

        /**
         * Const accessor for the top (y-coordinate) of a Model.
         *
         * \return The top boundary of the Model.
         */
		virtual float top() const;

        /**
         * Set the top (y-coordinate) of a Model to a given value.
         *
         * \param new_top The new top boundary of the Model.
         */
		virtual void setTop(float new_top);
    
        /**
         * Const accessor for the right (x-coordinate) of a Model.
         *
         * \return The right boundary of the Model.
         */
		virtual float right() const;
    
        /**
         * Set the right (x-coordinate) of a Model to a given value.
         *
         * \param new_right The new right boundary of the Model.
         */
		virtual void setRight(float new_right);
    
        /**
         * Const accessor for the bottom (y-coordinate) of a Model.
         *
         * \return The bottom boundary of the Model.
         */
		virtual float bottom() const;
 
        /**
         * Set the bottom (y-coordinate) of a Model to a given value.
         *
         * \param new_bottom The new bottom boundary of the Model.
         */
		virtual void setBottom(float new_bottom);

        /**
         * Const accessor to the width (right-left) of the model.
         *
         * \return The width of the model.
         */
        virtual unsigned int width() const;
    
        /**
         * Const accessor to the height (bottom-top) of the model.
         *
         * \return The height of the model.
         */
        virtual unsigned int height() const;
    
		/**
         * Const accessor for the global left (x-coordinate) of a Model.
         *
         * \return The global left boundary of the Model.
         */
		virtual float globalLeft() const;
    
        /**
         * Set the global left (x-coordinate) of a Model to a given value.
         *
         * \param new_left The new global left boundary of the Model.
         */
		virtual void setGlobalLeft(float new_left);

        /**
         * Const accessor for the global top (y-coordinate) of a Model.
         *
         * \return The global top boundary of the Model.
         */
		virtual float globalTop() const;

        /**
         * Set the global top (y-coordinate) of a Model to a given value.
         *
         * \param new_top The new global top boundary of the Model.
         */
		virtual void setGlobalTop(float new_top);
    
        /**
         * Const accessor for the global right (x-coordinate) of a Model.
         *
         * \return The global right boundary of the Model.
         */
		virtual float globalRight() const;
    
        /**
         * Set the global right (x-coordinate) of a Model to a given value.
         *
         * \param new_right The new global right boundary of the Model.
         */
		virtual void setGlobalRight(float new_right);
    
        /**
         * Const accessor for the global bottom (y-coordinate) of a Model.
         *
         * \return The global bottom boundary of the Model.
         */
		virtual float globalBottom() const;
 
        /**
         * Set the global bottom (y-coordinate) of a Model to a given value.
         *
         * \param new_bottom The new global bottom boundary of the Model.
         */
		virtual void setGlobalBottom(float new_bottom);
    
        /**
         * Only models with valid boundaries may be visualized in geometric view mode!
         *
         * \return Returns true, if the lowerRight value is larger than the upperLeft value.
         */
        virtual bool isViewable() const;
    
        /** Only models with valid boundaries may be visualized in geographic view mode!
         *
         * \return Returns true, if the globalLowerRight value is larger than the upperLeft value.
         */
        virtual bool isGeoViewable() const;
    
        /**
         * Convenience function to get the local transformation in Qt style.
         *
         * \return The local translation matrix of the model.
         */
        virtual QTransform localTransformation() const;
    
        /**
         * Convenience function to get the global transformation in Qt style.
         *
         * \return The global translation matrix of the model.
         */
        virtual QTransform globalTransformation() const;

        /**
         * Const copy model's geometry information to another model.
         *
         * \param other The other model.
         */
		virtual void copyGeometry(Model& other) const;
    
        /**
         * Const copy model's complete metadata to another model.
         *
         * \param other The other model.
         */
        virtual void copyMetadata(Model& other) const;
        /**
         * Const copy model's complete data (and metadata) to another model.
         *
         * \param other The other model.
         */
        virtual void copyData(Model& other) const;

        /**
         * The type of this model (same for every instance).
         *
         * \return "Model"
         */
        virtual QString typeName() const;
    
        /**
         * This function returns the automagically generated first header line for model
         * serialization.
         *
         * \return The first Header line, namely: "[Graipe::" + typeName() + "]"
         */
        QString magicID() const;
    
        /**
         * This function serializes a complete Model to a QString.
         * To do so, it serializes header first, then the content, like this:
         *      serialize_header()
         *      [Content]
         *      serialize_content();
         *
         * \param out The output device for the serialization.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * This function deserializes the model by means of its header and content
         *
         * \param  in The input device.
         * \return True, if the Model could be restored,
         */
        bool deserialize(QIODevice& in);
    
        /**
         * This function serializes the header of a model like this:
         *      magicID()
         *      m_parameters->serialize()
         *
         * \param out the output device.
         */
        virtual void serialize_header(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * This function deserializes the Model's header.
         *
         * \param  in The input device.
         * \return True, if the Model's header could be restored,
         */
        virtual bool deserialize_header(QIODevice& in);
    
        /**
         * This function serializes the content of a model.
         * Has to be specialized, here always "none\n".
         *
         * \param out the output device.
         */
        virtual void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * This function deserializes the Model's content.
         *
         * \param  in The input device.
         * \return True, if the Model's content could be restored,
         */
        virtual bool deserialize_content(QIODevice& in);
    
        /**
         * Models may be locked (to read only access), while algorithms are using them e.g.
         * This function can be used to query, if the Model is locked or not.
         *
         * \return True, if the model has been locked by somebody
         */
        bool locked() const;
        /**
         * Models may be locked (to read only access), while algorithms are using them e.g.
         * This function can be used to query, how many locks are currently active.
         *
         * \return the number of locks currently active.
         */
        unsigned int lockedBy() const;
    
        /**
         * Put a lock request on the model. Since the locking is a secured operation,
         * each lock-requester will get a personal (random) unlock code by its request.
         * He has to take for this code, because otherwise, unlocking is impossible.
         *
         * \return The code needed for unlocking afterwards
         */
        unsigned int lock();
    
        /** 
         * Remove the locking of the model using your unlock code.
         *
         * \param unlock_code the code, which unlocks the lock.
         */
        void unlock(unsigned int unlock_code);
    
        /**
         * Potentially non-const access to the parameters of the model.
         * These can be used to edit the model in a GUI!
         *
         * \return The parameters of this model
         */
        ParameterGroup* parameters();
    
    public slots:
        /**
         * This slot is called, whenever some parameter is changed.
         * It then emits simply the modelChanged signal to inform connected views etc.
         */
        virtual void updateModel();
    
	signals:
        /** Emit a model change to others **/
		void modelChanged();
    
    protected:
        //The parameters of this model:
        StringParameter	    * m_name;
        LongStringParameter	* m_description;
        LongStringParameter	* m_save_filename;
        PointParameter	    * m_ul, * m_lr;
        PointFParameter	    * m_global_ul, * m_global_lr;
    
        //A group for collecting all
        ParameterGroup      * m_parameters;

    private:
        //keeping track of the locks
        QVector<unsigned int> m_locks;
};

/**
 * This class acts like a basic container of templated elements
 * but fullfilles the Model interface
 */
template<class T>
class GRAIPE_CORE_EXPORT ItemListModel
    : public Model
{
    public:
        //Typedefs for convenience
        typedef T item_type;
        typedef QVector<T> container_type;
        typedef typename container_type::const_iterator const_iterator;
    
        /**
         * Default Constructor
         */
        ItemListModel()
        {
        }
    
        /**
         * Copy contructor (from another model)
         * \param other The other model.
         */
        ItemListModel(const ItemListModel<T>& other)
        : m_data(other.data())
        {
        }
    
        /**
         * Virtual destructor
         */
        virtual ~ItemListModel()
        {
        }

        /**
         * The type of this model (same for every instance oif same templates).
         *
         * \return T::typeName() + "List"
         */
        QString typeName() const
        {
            return T::typeName() + "List";
        }
    
        /**
         * Returns the element count of the internal item storage.
         * \return The size of the element list.
         */
        int size() const
        {
            return m_data.size();
        }
    
        /**
         * Clears all items in the list.
         */
        void clear()
        {
            m_data.clear();
            updateModel();
        }
    
        /**
         * Const iterator access to the begin of the list.
         *
         * \return A const iterator the the begin of the element list.
         */
        const_iterator begin() const
        {
            return m_data.begin();
        }
        
        /**
         * Const iterator access to the end of the list.
         *
         * \return A const iterator the the end of the element list.
         */
        const_iterator end() const
        {
            return m_data.end();
        }
    
        /**
         * Const access to the underlying data container a.k.a. the list of elements.
         *
         * \return Const reference to the data.
         */
        const container_type& data() const
        {
            return m_data;
        }

        /**
         * Const access to a single item at a given index.
         *
         * \param idx The index of that item in the list
         * \return Const item reference to the requested item.
         */
        const item_type& item(unsigned int idx) const
        {
            return m_data[idx];
        }
    
        /**
         * Removal of an item at a given index in the list.
         *
         * \param idx Index of the item to be removed.
         */
        void remove(unsigned int idx)
        {
            m_data.remove(idx);
            updateModel();
        }
     
        /**
         * Replacement of an item at a given index in the list.
         *
         * \param idx Index of the item to be replaced.
         * \param item The new item at the given index.
         */
        void replace(unsigned int idx, const item_type& item)
        {
            m_data.replace(idx,item);
            updateModel();
        }
    
        /**
         * Appending a new item at thje end of the list.
         *
         * \param item The new item, which will be appended.
         */
        void append(const item_type& item)
        {
            m_data.append(item);
            updateModel();
        }
    
        /**
         * Content serialization. This class serializes its content in a CSV-like manner:
         * Each item in the list is allowed to fill one line, line break will be inserted
         * ater each.
         *
         * \param out The output device, where the serialization will take place.
         */
        void serialize_content(QXmlStreamWriter& xmlWriter) const
        {
//TODO!!!            write_on_device(T::headerCSV(), out);
    /*
            for(const item_type& item : m_data)
            {
                write_on_device("\n" + item.toCSV(), out);
            }
            */
        }
    
        /**
         * Content deserialization. We assume, that this class serialized its content in a CSV-like manner:
         * Each item in the list is allowed to fill one line. Thus line breaks are used
         * to split the content into single items and deserialize them separately
         *
         * \param in The input device, where we read from.
         ± \return true, if the complete content could be deserialized from the input device.
         */
        bool deserialize_content(QIODevice& in)
        {
            if (locked())
                return false;
                
            //Read in header line and then throw it away immideately
            if(!in.atEnd())
                in.readLine();

            //Clean up
            clear();
            updateModel();

            //Read the entries
            while(!in.atEnd())
            {
                QString line(in.readLine());
                
                if(!line.isEmpty() && !line.startsWith(";"))
                {
                    item_type new_item;
                    if (!new_item.fromCSV(line))
                     {
                        qCritical() << typeName() << "::deserialize_content: Item could not be deserialized from: '" << line << "'";
                        return false;
                    }
                    append(new_item);
                }
            }
            return true;
        }
    
    protected:
        /** The data storage **/
        QVector<T> m_data;
};

/**
 * A rastered model extends the default model by the assumption of an underlying
 * Raster of a given size.
 */
class GRAIPE_CORE_EXPORT RasteredModel
    : public Model
{
    public:
        /**
         * Default/empty contructor of the RasteredModel class.
         */
        RasteredModel();
    
        /**
         * Copy contructor of the RasteredModel class.
         *
         * \param model The other model, from which the parameters will be copied.
         */
		RasteredModel(const RasteredModel& model);
    
        /**
         * Destructor of the RasteredModel class.
         */
		virtual ~RasteredModel();
    
        /**
         * Const accessor to the raster resolution in x-direction of the model.
         *
         * \return The raster width of the model.
         */
        unsigned int width() const;

        /**
         * Set the raster resolution in x-direction of the model to a new width.
         *
         * \param new_w The new raster width of the model.
         */
         virtual void setWidth(unsigned int new_w);
    
        /**
         * Const accessor to the raster resolution in y-direction of the model.
         *
         * \return The raster height of the model.
         */
        unsigned int height() const;
    
        /**
         * Set the raster resolution in y-direction of the model to a new height.
         *
         * \param new_h The new raster height of the model.
         */
        virtual void setHeight(unsigned int new_h);
    
        /**
         * Is equivalent to the question if the raster is of size 0x0.
         *
         * \return True, if the raster is of zero size.
         */
        virtual bool isEmpty() const;
    
        /**
         * Only models with valid boundaries may be visualized in geometric view mode!
         *
         * \return Returns true, if the lowerRight value is larger than the upperLeft value.
         */
        bool isViewable() const;
    
        /** Only models with valid boundaries may be visualized in geographic view mode!
         *
         * \return Returns true, if the globalLowerRight value is larger than the upperLeft value.
         */
        bool isGeoViewable() const;
    
        /**
         * Convenience function to get the local transformation in Qt style.
         *
         * \return The local translation matrix of the model scaled by the resolution.
         */
        QTransform localTransformation() const;
    
        /**
         * Convenience function to get the global transformation in Qt style.
         *
         * \return The global translation matrix of the model scaled by the resolution.
         */
        QTransform globalTransformation() const;
        
        /**
         * Const copy model's geometry information to another model.
         *
         * \param other The other model.
         */
		void copyGeometry(Model& other) const;
        /**
         * Const copy model's complete data (and metadata) to another model.
         *
         * \param other The other model.
         */
        void copyData(Model& other) const;
    
        /**
         * The type of this RasteredModel (same for every instance).
         *
         * \return "RasteredModel"
         */
        QString typeName() const;
    
    protected:
        /** The additional parameters of this model: **/
        PointParameter * m_size;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_MODEL_HXX
