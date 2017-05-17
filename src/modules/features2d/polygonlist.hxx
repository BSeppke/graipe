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

#ifndef GRAIPE_FEATURES2D_POLYGONLIST_HXX
#define GRAIPE_FEATURES2D_POLYGONLIST_HXX

#include "core/model.hxx"
#include "features2d/polygon.hxx"
#include "features2d/config.hxx"

#include <QVector>

namespace graipe {

/**
 * This class represents a list of polygons. It conformes to a
 * graipe::Model and may thus be widely used.
 */
class GRAIPE_FEATURES2D_EXPORT PolygonList2D 
:	public Model
{
	public:
		//The used point type
		typedef Polygon2D PolygonType;
    
        /**
         * Default constructor. Constructs an empty list of polygons.
         */
		PolygonList2D();
    
        /**
         * Copy constructor. Creates a copy from another list of polygons.
         *
         * \param poly_list The other PolygonList2D.
         */
		PolygonList2D(const PolygonList2D& poly_list);
    
        /**
         * Returns the typeName of a polygon list.
         *
         * \return Always "PolygonList2D".
         */
		QString typeName() const;
    
        /**
         * Returns the number of polygons in this list.
         *
         * \return The number of polygons in this list.
         */
        virtual unsigned int size() const;
    
        /**
         * Completely erases this list of polygons. Does nothing if the list is locked.
         */
        virtual void clear();
    
        /*
         * Constant access a polygon inside this list at a given index. May throw
         * an error, if the index is out of range.
         *
         * \param index The index of the polygon in the list.
         */
		virtual const PolygonType & polygon(unsigned int index) const;
    
        /*
         * Reset/replace a polygon inside this list at a given index. If the index is
         * out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced polygon in the list.
         * \param new_p The replacement polygon.
         */
		virtual void setPolygon(unsigned int index, const PolygonType& new_p);
    
        /*
         * Add a polygon at the end of this list.
         * If the model is locked, this function will do nothing.
         *
         * \param p The replacement polygon.
         */
		virtual void addPolygon(const PolygonType& poly);
	
		/**
         * Return the item header for this list of polygons.
         *
         * \return Always "p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y".
         */
		virtual QString item_header() const;
    
        /**
         * Serialization of one polygon at a given list index to a string. This function will
         * throw an error if the index is out of range.
         *
         * \param index The index of the polygon to be serialized.
         * \return A QString containing the searialization of the polygon.
         */
        virtual QString serialize_item(unsigned int index) const;
    
        /**
         * Deserialization/addition of a polygon from a string to this list.
         *
         * \param serial A QString containing the searialization of the polygon.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: p0_x, p0_y, ... , pN_x, pN_y
         */
        virtual bool deserialize_item(const QString & serial);
    
        /**
         * Serialization the list of polygons to a QIODevice.
         * The first line is the header as given in item_header(). Each following
         * line represents one polygon serialization.
         *
         * \param out The QIODevice, where we will put our output on.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserializion of a list of polygons from a QIODevice.
         * The first line is the header as given in item_header(), which is ignored however.
         * Each following line has to be one valid polygon serialization.
         *
         * \param in The QIODevice, where we will read from.
         */
		bool deserialize_content(QIODevice& in);
    
    protected:
        //The polygons
        QVector<PolygonType> m_polys;
};




/**
 * This class extends the list of polygons.
 * For each polygon, it keeps an additional weight, which may indicate
 * a fitting error or anything else.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPolygonList2D 
:	public PolygonList2D
{
    public:
        /**
         * Default constructor. Constructs an empty list of weighted polygons.
         */
        WeightedPolygonList2D();
    
        /**
         * Copy constructor. Creates a copy from another list of weighted polygons.
         *
         * \param poly_list The other WeightedPolygonList2D.
         */
        WeightedPolygonList2D(const WeightedPolygonList2D& poly_list);
        
        /**
         * Returns the typeName of a weighted polygon list.
         *
         * \return Always "WeightedPolygonList2D".
         */
        QString typeName() const;
    
        /**
         * Getter of the weight of a polygon at a given index. May throw an error,
         * if the index is out of bounds.
         * 
         * \param index The index, for qhich we query the weight.
         * \return The weight at the given index.
         */
        float weight(unsigned int index) const;
    
        /**
         * Setter of the weight of a polygon at a given index. If the index is
         * out of range or the model is locked, this function will do nothing.
         *
         * \param index The index of the replaced weight in the list.
         * \param new_w The replacement weight.
         */
        void setWeight(unsigned int index, float new_w);
        
        /*
         * Reset/replace a polygon inside this list at a given index. The weight of the reset
         * polygon will be set to zero. If the index is out of range or the model is locked, 
         * this function will do nothing.
         *
         * \param index The index of the replaced polygon in the list.
         * \param new_p The replacement polygon.
         */
        void setPolygon(unsigned int index, const PolygonType& new_p);
    
        /**
         * Reset/replace a polygon and a weight inside this list at a given index.
         * If the index is out of range or the model is locked,
         * this function will do nothing.
         *
         * \param index The index of the replaced polygon in the list.
         * \param new_p The replacement polygon.
         * \param new_w The replacement weight.
         */
		virtual void setPolygon(unsigned int index, const PolygonType& new_p, float new_w);
    
        /*
         * Add a polygon at the end of this list.
         * If the model is locked, this function will do nothing.
         *
         * \param poly The new polygon.
         */
		virtual void addPolygon(const PolygonType& poly);
    
        /*
         * Add a weighted polygon at the end of this list.
         * If the model is locked, this function will do nothing.
         *
         * \param poly The new polygon.
         * \param w The new weight.
         */
		virtual void addPolygon(const PolygonType& poly, float weight);
	
		/**
         * Return the item header for this list of polygons.
         *
         * \return Always "weight, p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y".
         */
		virtual QString item_header() const;
    
        /**
         * Serialization of one polygon at a given list index to a string. This function will
         * throw an error if the index is out of range.
         *
         * \param index The index of the polygon to be serialized.
         * \return A QString containing the searialization of the polygon.
         */
        virtual QString serialize_item(unsigned int index) const;
    
        /**
         * Deserialization/addition of a polygon from a string to this list.
         *
         * \param serial A QString containing the searialization of the polygon.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: p0_x, p0_y, ... , pN_x, pN_y
         */
        virtual bool deserialize_item(const QString & serial);
    
    protected:
        //The weights
        QVector<float> m_weights;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_POLYGONLIST_HXX
