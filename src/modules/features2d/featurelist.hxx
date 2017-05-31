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

#ifndef GRAIPE_FEATURES2D_FEATURELIST_HXX
#define GRAIPE_FEATURES2D_FEATURELIST_HXX

#include "core/model.hxx"
#include "core/qt_ext/qpointfx.hxx"

#include "features2d/config.hxx"

#include <QVector>

namespace graipe {

/**
 * Base class for collections for the different feature types.
 * This class provides the storage of an x,y tuple for each feature.
 */
class GRAIPE_FEATURES2D_EXPORT PointFeatureList2D 
:	public Model
{
	public:
		//The used point type
		typedef QPointFX PointType;
    
        /**
         * Default constructor. Creates an empty point feature list.
         */
		PointFeatureList2D();
		
        /**
         * The typename of this feature list
         *
         * \return Always "PointFeatureList2D"
         */
		virtual QString typeName() const;
		
        /**
         * Returns the number of features in this list.
         *
         * \return The number of features in this list.
         */
		virtual unsigned int size() const;
            
        /**
         * Completely erases this list of features. Does nothing if the list is locked.
         */
		virtual void clear();
		
        /**
         * Getter for the position of a feature at a certain index.
         *
         * \param index The index of the feature inside the list.
         * \return The constant reference to the positon of the requested feature.
         */
		virtual const PointType& position(unsigned int index) const;
    
        /**
         * Setter for the position of a feature at a certain index. 
         * Replaces a features position at an index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the feature inside the list.
         * \param new_p The new positon of that feature.
         */
		virtual void setPosition(unsigned int index, const PointType& new_p);
		
        /**
         * Addition of a point feature to the list. This will append the given feature
         * at the end of the list of features.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         */
		virtual void addFeature(const PointType& p);
		
        /**
         * Removal of a feature at a certain index.
         * Does nothing if the model is locked or the index is out of range.
         *
         * \param index The index of the feature inside the list.
         */
		virtual void removeFeature(unsigned int index);
		
        /**
         * The content's item header for the feature list serialization.
         * 
         * \return Always: "pos_x, pos_y"
         */
		virtual QString csvHeader() const;
        
        /**
         * Serialization of a single feature inside the list at a given index.
         * The feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the feature to be serialized.
         * \return QString of the feature, namely "x, y"
         */
		virtual QString itemToCSV(unsigned int index) const;
        
        /**
         * Deserialization/addition of a feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: x, y
         */
		virtual bool itemFromCSV(const QString& serial);
        
        /**
         * Serialization of a single feature inside the list at a given index.
         * The feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the feature to be serialized.
         * \return QString of the feature, namely "x, y"
         */
		virtual void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
        
        /**
         * Deserialization/addition of a feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization is ordered as: x, y
         */
		virtual bool deserialize_item(QXmlStreamReader& xmlReader);
    
        /**
         * Serialize the complete content of the featurelist to an xml file.
         * Mainly prints:
         *   csvHeader
         * and for each feature:
         *   newline + serialize_item().
         *
         * \param out The output device for serialization.
         */
		void serialize_content(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization of a  feature list from an xml file.
         * The first line is the header as given in csvHeader, which is ignored however.
         * Each following line has to be one valide feature serialization.
         * Does nothing if the model is locked.
         *
         * \param xmlReader The QXmlStreamReader, where we will read from.
         */
		bool deserialize_content(QXmlStreamReader& xmlReader);
	
	protected:
		//The pointlist
		QVector<PointType> m_points;
};




/**
 * Extension of the base class for point features.
 * This class provides the storage of an assigned weight for each
 * point feature.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPointFeatureList2D 
:	public PointFeatureList2D
{
	
	public:		
        /**
         * Default constructor. Creates an empty weighted point feature list.
         */
		WeightedPointFeatureList2D();
		
        /**
         * The typename of this feature list
         *
         * \return Always "WeightedPointFeatureList2D"
         */
        virtual QString typeName() const;
		
        /**
         * Completely erases this list of weighted features. Does nothing if the list is locked.
         */
		void clear();
    
        /**
         * Getter for the weight of a feature at a certain index.
         *
         * \param index The index of the feature inside the list.
         * \return The weight of the requested feature.
         */
		float weight(unsigned int index) const;
    
        /**
         * Setter for the weight of a feature at a certain index.
         * Replaces a feature's weight at an index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the feature inside the list.
         * \param new_w The new weight of that feature.
         */
		void setWeight(unsigned int index, float new_w);
    
        /**
         * Addition of a point feature to the list. This will append the given feature
         * at the end of the list of features and assign it with a weight of zero.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         */
        void addFeature(const PointType& p);
    
        /**
         * Addition of a weighted feature to the list. This will append the given feature
         * at the end of the list of features.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         */
        virtual void addFeature(const PointType& p, float weight);
    
        /**
         * Specialized removal of a feature at a certain index.
         * Does nothing if the model is locked or the index is out of range.
         *
         * \param index The index of the feature inside the list.
         */
		void removeFeature(unsigned int index);
    
        /**
         * The content's item header for the weighted feature list serialization.
         * 
         * \return Always: "pos_x, pos_y, weight"
         */
		QString csvHeader() const;
    
        /**
         * Serialization of a single weighted feature inside the list at a given index.
         * The weighted feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the weighted feature to be serialized.
         * \return QString of the weighted feature, ordered as: x, y, weight.
         */
		virtual QString itemToCSV(unsigned int index) const;
    
        /**
         * Deserialization/addition of a weighted feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the weighted feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight
         */
		bool itemFromCSV(const QString& serial);
    
        /**
         * Serialization of a single weighted feature inside the list at a given index.
         * The weighted feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the weighted feature to be serialized.
         * \return QString of the weighted feature, ordered as: x, y, weight.
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization/addition of a weighted feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the weighted feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight
         */
		bool deserialize_item(QXmlStreamReader& xmlReader);
    
		
	protected:
        //Additional weights
        QVector<float> m_weights;
};




/**
 * Extension of the class for weighted point features.
 * This class provides the storage of an assigned orientation for each
 * weighted point feature.
 */
class GRAIPE_FEATURES2D_EXPORT EdgelFeatureList2D 
:	public WeightedPointFeatureList2D
{
		
	public:		
        /**
         * Default constructor. Creates an empty edgel feature list.
         */
		EdgelFeatureList2D();
		
        /**
         * The typename of this feature list
         *
         * \return Always "EdgelFeatureList2D"
         */
		virtual QString typeName() const;
	
        /**
         * Completely erases this list of edgel features. Does nothing if the list is locked.
         */
		void clear();
    
        /**
         * Getter for the angle of a feature at a certain index.
         * The angle is returned in degrees, with respect to the clock:
         *   (0 = 3h, 90 = 6h, 180=9h, 270=12h).
         *
         * \param index The index of the feature inside the list.
         * \return The angle of the requested feature.
         */
		float angle(unsigned int index) const;
	
        /**
         * Getter for the orientation of a feature at a certain index.
         * The orientation is returned in radians, with respect to the clock:
         *   (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         *
         * \param index The index of the feature inside the list.
         * \return The orientation of the requested feature.
         */
		float orientation(unsigned int index) const;
    
        /**
         * Setter for the orientation of a feature at a certain index.
         * Replaces a feature's orientation at an index.
         * Does nothing if the model is locked.
         * The orientation shall be given in radians, with respect to the clock:
         *   (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         *
         * \param index The index of the feature inside the list.
         * \param new_o The new orientation of that feature.
         */
		void setOrientation(unsigned int index, float new_o);
		
        /**
         * Addition of a point feature to the list. This will append the given feature
         * at the end of the list of features and assign it with a weight and an
         * orientation of zero.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         */
        void addFeature(const PointType& p);
    
        /**
         * Addition of a weighted feature to the list. This will append the given weighted feature
         * at the end of the list of features and assign it with zero orientation.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         */
        void addFeature(const PointType& p, float weight);
    
        /**
         * Addition of an edgel feature to the list. This will append the given edgel feature
         * at the end of the list of features.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         */
        virtual void addFeature(const PointType& p, float weight, float orientation);
    
        /**
         * Specialized removal of a feature at a certain index.
         * Does nothing if the model is locked or the index is out of range.
         *
         * \param index The index of the feature inside the list.
         */
        void removeFeature(unsigned int index);
        
        /**
         * The content's item header for the edgel feature list serialization.
         * 
         * \return Always: "pos_x, pos_y, weight, orientation"
         */
		QString csvHeader() const;
    
        /**
         * Serialization of a single weighted feature inside the list at a given index.
         * The weighted feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the weighted feature to be serialized.
         * \return QString of the weighted feature, ordered as: x, y, weight, orientation.
         */
		virtual QString itemToCSV(unsigned int index) const;
    
        /**
         * Deserialization/addition of a weighted feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the weighted feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight, orientation
         */
		bool itemFromCSV(const QString& serial);
    
        /**
         * Serialization of a single weighted feature inside the list at a given index.
         * The weighted feature will be serialized by means of comma separated values.
         * 
         * \param index Index of the weighted feature to be serialized.
         * \return QString of the weighted feature, ordered as: x, y, weight, orientation.
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization/addition of a weighted feature from a string to this list.
         * Does nothing if the model is locked.
         *
         * \param serial A QString containing the serialization of the weighted feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight, orientation
         */
		bool deserialize_item(QXmlStreamReader& xmlReader);
		
	protected:
        //Aditional orientations
		QVector<float> m_orientations;
};




/**
 * Extension of the class for edgel features.
 * This class provides the storage of a scale and an assigned feature
 * descriptor for each edgel feature.
 */
class GRAIPE_FEATURES2D_EXPORT SIFTFeatureList2D 
:	public EdgelFeatureList2D
{
	public:
        /**
         * Default constructor. Creates an empty SIFT feature list.
         */
		SIFTFeatureList2D();
		
        /**
         * The typename of this feature list
         *
         * \return Always "SIFTFeatureList2D"
         */
		virtual QString typeName() const;
	
        /**
         * Completely erases this list of SIFT features. Does nothing if the list is locked.
         */
		void clear();
    
        /**
         * Getter for the scale of a feature at a certain index.
         *
         * \param index The index of the feature inside the list.
         * \return The scale (in scale-space sigma) of the requested feature.
         */
		float scale(unsigned int index) const;
    
        /**
         * Setter for the scale of a feature at a certain index.
         * Replaces a feature's scale at an index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the feature inside the list.
         * \param new_s The new scale (in scale-space sigma) of of that feature.
         */
		void setScale(unsigned int index, float new_s);
    
        /**
         * Getter for the descriptor of a feature at a certain index.
         *
         * \param index The index of the feature inside the list.
         * \return The descriptor of the requested feature.
         */
		const QVector<float>& descriptor(unsigned int index) const;
    
        /**
         * Setter for the descriptor of a feature at a certain index.
         * Replaces a feature's descriptor at an index.
         * Does nothing if the model is locked.
         *
         * \param index The index of the feature inside the list.
         * \param new_d The new descriptor of that feature.
         */
		void setDescriptor(unsigned int index, const QVector<float> & new_d);
		
        /**
         * Addition of a point feature to the list. This will append the given feature
         * at the end of the list of features and assign it with a weight, an
         * orientation and a scale of zero and with an empty descriptor.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         */
        void addFeature(const PointType& p);
    
        /**
         * Addition of a weighted feature to the list. This will append the given weighted feature
         * at the end of the list of features and assign it with an
         * orientation and a scale of zero and with an empty descriptor.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         */
        void addFeature(const PointType& p, float weight);
    
        /**
         * Addition of an edgel feature to the list. This will append the given edgel feature
         * at the end of the list of features.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         */
        virtual void addFeature(const PointType& p, float weight, float orientation);
    
        /**
         * Addition of a SIFT feature to the list. This will append the given edgel feature
         * at the end of the list of features and assign it with an empty descriptor.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         * \param scale The scale (in scale-space sigma) of the SIFT feature.
         */
        virtual void addFeature(const PointType& p, float weight, float orientation, float scale);
    
        /**
         * Addition of a SIFT feature to the list. This will append the given edgel feature
         * at the end of the list of features.
         * Does nothing if the model is locked.
         *
         * \param p The new feature.
         * \param weight The weight of the new feature.
         * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
         * \param scale The scale (in scale-space sigma) of the SIFT feature.
         * \param descr The SIFT descriptor of the feature
         */
        virtual void addFeature(const PointType& p, float weight, float orientation, float scale, const QVector<float> & descr);
        
        /**
         * Specialized removal of a feature at a certain index.
         * Does nothing if the model is locked or the index is out of range.
         *
         * \param index The index of the feature inside the list.
         */
		void removeFeature(unsigned int index);
    
        /**
         * The content's item header for the SIFT feature list serialization.
         * 
         * \return Always: "x, y, weight, orientation, scale, descr_0, ..., descr_N"
         */
		QString csvHeader() const;
    
        /**
         * Serialization of a single SIFT feature inside the list at a given index.
         * The SIFT will be serialized by means of comma separated values.
         * 
         * \param index Index of the SIFT feature to be serialized.
         * \return QString of the edgel feature, ordered  as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
         */
		QString itemToCSV(unsigned int index) const;
    
        /**
         * Deserialization/addition of a SIFT feature from a string to this list.
         *
         * \param serial A QString containing the serialization of the SIFT feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
         */
		bool itemFromCSV(const QString& serial);
    
        /**
         * Serialization of a single SIFT feature inside the list at a given index.
         * The SIFT will be serialized by means of comma separated values.
         * 
         * \param index Index of the SIFT feature to be serialized.
         * \return QString of the edgel feature, ordered  as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
         */
		void serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const;
    
        /**
         * Deserialization/addition of a SIFT feature from a string to this list.
         *
         * \param serial A QString containing the serialization of the SIFT feature.
         * \return True, if the item could be deserialized and the model is not locked.
         *         The serialization should be given as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
         */
		bool deserialize_item(QXmlStreamReader& xmlReader);
		
	protected:
        //Additional scales and feature descriptors
		QVector<float> m_scales;
		QVector<QVector<float> > m_descriptors;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_FEATURELIST_HXX
