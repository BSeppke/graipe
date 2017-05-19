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

#include "features2d/featurelist.hxx"

#define _USE_MATH_DEFINES
#include <math.h>

namespace graipe {

/**
 * Default constructor. Creates an empty point feature list.
 */
PointFeatureList2D::PointFeatureList2D()
{
}

/**
 * The typename of this feature list
 *
 * \return Always "PointFeatureList2D"
 */
QString PointFeatureList2D::typeName() const
{
	return "PointFeatureList2D";
}

/**
 * Returns the number of features in this list.
 *
 * \return The number of features in this list.
 */
unsigned int PointFeatureList2D::size() const
{
	return m_points.size();
}

/**
 * Completely erases this list of features. Does nothing if the list is locked.
 */
void PointFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_points.clear(); 
	updateModel();
}

/**
 * Getter for the position of a feature at a certain index.
 *
 * \param index The index of the feature inside the list.
 * \return The constant reference to the positon of the requested feature.
 */
const PointFeatureList2D::PointType& PointFeatureList2D::position(unsigned int index) const
{
	return m_points[index];		
}

/**
 * Setter for the position of a feature at a certain index. 
 * Replaces a features position at an index.
 *
 * \param index The index of the feature inside the list.
 * \param new_p The new positon of that feature.
 */
void PointFeatureList2D::setPosition(unsigned int index, const PointType& new_p)
{ 
    if(locked())
        return;
    
	m_points[index] = new_p;	
	updateModel();	
}

/**
 * Addition of a point feature to the list. This will append the given feature
 * at the end of the list of features.
 *
 * \param p The new feature.
 */
void PointFeatureList2D::addFeature(const PointType& p)
{
    if(locked())
        return;
    
	m_points.push_back(p);
	updateModel();
}

/**
 * Removal of a feature at a certain index.
 * Does nothing if the model is locked or the index is out of range.
 *
 * \param index The index of the feature inside the list.
 */
void PointFeatureList2D::removeFeature(unsigned int index)
{
    if(locked())
        return;
    
	if (index < (unsigned int)m_points.size() )
    {
        m_points.erase(m_points.begin()+index);
        updateModel();
    }
}

/**
 * The content's item header for the feature list serialization.
 * 
 * \return Always: "pos_x, pos_y"
 */
QString PointFeatureList2D::item_header() const
{
	return "pos_x, pos_y";
}

/**
 * Serialization of a single feature inside the list at a given index.
 * The feature will be serialized by means of comma separated values.
 * 
 * \param index Index of the feature to be serialized.
 * \return QString of the feature, ordered as: x, y.
 */
 QString PointFeatureList2D::serialize_item(unsigned int index) const
{
	return   QString::number(m_points[index].x(), 'g', 10) + ", "
           + QString::number(m_points[index].y(), 'g', 10);
}

/**
 * Deserialization/addition of a feature from a string to this list.
 *
 * \param serial A QString containing the serialization of the feature.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: x, y
 */
bool PointFeatureList2D::deserialize_item(const QString & serial)
{
    if(locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
	
	if(values.size() >= 2)
	{
		try
        {
			m_points.push_back(PointType(values[0].toFloat(), values[1].toFloat()));
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}

/**
 * Serialize the complete content of the featurelist to a QIODevice.
 * Mainly prints:
 *   item_header()
 * and for each feature:
 *   newline + serialize_item().
 *
 * \param out The output device for serialization.
 */
void PointFeatureList2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.writeTextElement("Legend", item_header());
    
	for(unsigned int i=0; i < size(); ++i)
    {
        xmlWriter.writeStartElement("Feature");
        xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeCharacters(serialize_item(i));
        xmlWriter.writeEndElement();
    }
}

/**
 * Deserializion of a  feature list from a QIODevice.
 * The first line is the header as given in item_header(), which is ignored however.
 * Each following line has to be one valide feature serialization.
 *
 * \param in The QIODevice, where we will read from.
 */
bool PointFeatureList2D::deserialize_content(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;

    //Clean up
	clear();
    updateModel();
    
    //Read the entries
    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "Feature")
        {
            if(!deserialize_item(xmlReader.readElementText()))
                return false;
        }
        else
        {
            xmlReader.skipCurrentElement();
        }
    }
    return true;
}




/**
 * Default constructor. Creates an empty weighted point feature list.
 */
WeightedPointFeatureList2D::WeightedPointFeatureList2D()
{
}

/**
 * The typename of this feature list
 *
 * \return Always "WeightedPointFeatureList2D"
 */
QString WeightedPointFeatureList2D::typeName() const
{
	return "WeightedPointFeatureList2D";
}

/**
 * Completely erases this list of weighted features. Does nothing if the list is locked.
 */
void WeightedPointFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_weights.clear();
    
	PointFeatureList2D::clear();
}

/**
 * Getter for the weight of a feature at a certain index.
 *
 * \param index The index of the feature inside the list.
 * \return The weight of the requested feature.
 */
float WeightedPointFeatureList2D::weight(unsigned int index) const
{
	return m_weights[index];		
}

/**
 * Setter for the weight of a feature at a certain index.
 * Replaces a feature's weight at an index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the feature inside the list.
 * \param new_w The new weight of that feature.
 */
void WeightedPointFeatureList2D::setWeight(unsigned int index, float new_w)
{
    if(locked())
        return;
    
	m_weights[index]= new_w;	
	updateModel();
}
	
/**
 * Addition of a point feature to the list. This will append the given feature
 * at the end of the list of features and assign it with a weight of zero.
 * Does nothing if the model is locked.
 *
 * \param p The new feature.
 */
void WeightedPointFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0);
}

/**
 * Addition of a weighted feature to the list. This will append the given feature
 * at the end of the list of features.
 * Does nothing if the model is locked.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 */
void WeightedPointFeatureList2D::addFeature(const PointType& p,float weight)
{
    if(locked())
        return;
    
    m_weights.push_back(weight);
    PointFeatureList2D::addFeature(p);
}

/**
 * Specialized removal of a feature at a certain index.
 * Does nothing if the model is locked or the index is out of range.
 *
 * \param index The index of the feature inside the list.
 */
void WeightedPointFeatureList2D::removeFeature(unsigned int index)
{
    if(locked())
        return;
    
    
	if (index < (unsigned int)m_weights.size() )
    {
        m_weights.erase(m_weights.begin()+index);
        PointFeatureList2D::removeFeature(index);
    }
}

/**
 * The content's item header for the weighted feature list serialization.
 * 
 * \return Always: "pos_x, pos_y, weight"
 */
QString WeightedPointFeatureList2D::item_header() const
{
	return PointFeatureList2D::item_header() + ", weight";
}
    
/**
 * Serialization of a single weighted feature inside the list at a given index.
 * The weighted feature will be serialized by means of comma separated values.
 * 
 * \param index Index of the weighted feature to be serialized.
 * \return QString of the weighted feature, ordered as: x, y, weight.
 */
QString WeightedPointFeatureList2D::serialize_item(unsigned int index) const
{
	return PointFeatureList2D::serialize_item(index) + ", " + QString::number(m_weights[index], 'g', 10);
}

/**
 * Deserialization/addition of a weighted feature from a string to this list.
 *
 * \param serial A QString containing the serialization of the weighted feature.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: x, y, weight
 */
bool WeightedPointFeatureList2D::deserialize_item(const QString & serial)
{
    if(locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
    
	if(values.size() >= 3)
    {
		try
        {
            m_weights.push_back(values[2].toFloat());
            PointFeatureList2D::deserialize_item(serial);
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}




/**
 * Default constructor. Creates an empty edgel feature list.
 */
EdgelFeatureList2D::EdgelFeatureList2D()
{
}

/**
 * The typename of this feature list
 *
 * \return Always "EdgelFeatureList2D"
 */
QString EdgelFeatureList2D::typeName() const
{
	return "EdgelFeatureList2D";
}

/**
 * Completely erases this list of edgel features. Does nothing if the list is locked.
 */
void EdgelFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_orientations.clear();
    
	WeightedPointFeatureList2D::clear();
}

/**
 * Getter for the angle of a feature at a certain index.
 * The angle is returned in degrees, with respect to the clock:
 *   (0 = 3h, 90 = 6h, 180=9h, 270=12h).
 *
 * \param index The index of the feature inside the list.
 * \return The angle of the requested feature.
 */
float EdgelFeatureList2D::angle(unsigned int index) const
{
	return m_orientations[index]*180.0/M_PI;
}

/**
 * Getter for the orientation of a feature at a certain index.
 * The orientation is returned in radians, with respect to the clock:
 *   (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
 *
 * \param index The index of the feature inside the list.
 * \return The orientation of the requested feature.
 */
float EdgelFeatureList2D::orientation(unsigned int index) const
{
	return m_orientations[index];		
}

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
void EdgelFeatureList2D::setOrientation(unsigned int index, float new_o)
{
    if(locked())
        return;
    
	m_orientations[index]= new_o;
	updateModel();
}
	
/**
 * Addition of a point feature to the list. This will append the given feature
 * at the end of the list of features and assign it with a weight and an
 * orientation of zero.
 *
 * \param p The new feature.
 */
void EdgelFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0, 0);
}

/**
 * Addition of a weighted feature to the list. This will append the given weighted feature
 * at the end of the list of features and assign it with zero orientation.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 */
void EdgelFeatureList2D::addFeature(const PointType& p, float weight)
{
    addFeature(p, weight, 0);
}
/**
 * Addition of an edgel feature to the list. This will append the given edgel feature
 * at the end of the list of features.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
 */
void EdgelFeatureList2D::addFeature(const PointType& p,float weight, float orientation)
{
    if(locked())
        return;
    
    m_orientations.push_back(orientation);
    WeightedPointFeatureList2D::addFeature(p, weight);
}

/**
 * Specialized removal of a feature at a certain index.
 * Does nothing if the model is locked or the index is out of range.
 *
 * \param index The index of the feature inside the list.
 */
void EdgelFeatureList2D::removeFeature(unsigned int index)
{
    if(locked())
        return;
    
	if (index < (unsigned int)m_orientations.size() )
    {
        m_orientations.erase(m_orientations.begin()+index);
        WeightedPointFeatureList2D::removeFeature(index);
    }
}

/**
 * The content's item header for the edgel feature list serialization.
 * 
 * \return Always: "pos_x, pos_y, weight, orientation"
 */
QString EdgelFeatureList2D::item_header() const
{
	return WeightedPointFeatureList2D::item_header() + ", orientation";
}

/**
 * Serialization of a single edgel feature inside the list at a given index.
 * The edgelfeature will be serialized by means of comma separated values.
 * 
 * \param index Index of the edgel feature to be serialized.
 * \return QString of the edgel feature, ordered  as: x, y, weight, orientation.
 */
QString EdgelFeatureList2D::serialize_item(unsigned int index) const
{
	return WeightedPointFeatureList2D::serialize_item(index) + ", " + QString::number(m_orientations[index], 'g', 10);
}

/**
 * Deserialization/addition of a edgel feature from a string to this list.
 *
 * \param serial A QString containing the serialization of the edgel feature.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: x, y, weight, orientation
 */
bool EdgelFeatureList2D::deserialize_item(const QString & serial)
{	
    if(locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 4)
    {
		try
        {
            m_orientations.push_back(values[3].toFloat());
            WeightedPointFeatureList2D::deserialize_item(serial);
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}





/**
 * Default constructor. Creates an empty SIFT feature list.
 */
SIFTFeatureList2D::SIFTFeatureList2D()
{
}

/**
 * The typename of this feature list
 *
 * \return Always "SIFTFeatureList2D"
 */
QString SIFTFeatureList2D::typeName() const
{
	return "SIFTFeatureList2D";
}

/**
 * Completely erases this list of SIFT features. Does nothing if the list is locked.
 */
void SIFTFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_scales.clear();
    m_descriptors.clear();
    
	EdgelFeatureList2D::clear();
}

/**
 * Getter for the scale of a feature at a certain index.
 *
 * \param index The index of the feature inside the list.
 * \return The scale (in scale-space sigma) of the requested feature.
 */
float SIFTFeatureList2D::scale(unsigned int index) const
{
	return m_scales[index];
}

/**
 * Setter for the scale of a feature at a certain index.
 * Replaces a feature's scale at an index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the feature inside the list.
 * \param new_s The new scale (in scale-space sigma) of of that feature.
 */
void SIFTFeatureList2D::setScale(unsigned int index, float new_s)
{
    if(locked())
        return;
    
	m_scales[index] = new_s;
	updateModel();
}

/**
 * Getter for the descriptor of a feature at a certain index.
 *
 * \param index The index of the feature inside the list.
 * \return The descriptor of the requested feature.
 */
const QVector<float>& SIFTFeatureList2D::descriptor(unsigned int index) const
{
	return m_descriptors[index];
}

/**
 * Setter for the descriptor of a feature at a certain index.
 * Replaces a feature's descriptor at an index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the feature inside the list.
 * \param new_d The new descriptor of that feature.
 */
void SIFTFeatureList2D::setDescriptor(unsigned int index, const QVector<float> & new_d)
{
    if(locked())
        return;
    
	m_descriptors[index] = new_d;
	updateModel();
}

/**
 * Addition of a point feature to the list. This will append the given feature
 * at the end of the list of features and assign it with a weight, an
 * orientation and a scale of zero and with an empty descriptor.
 *
 * \param p The new feature.
 */
void SIFTFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0, 0, 0, QVector<float>());
}
        
/**
 * Addition of a weighted feature to the list. This will append the given weighted feature
 * at the end of the list of features and assign it with an
 * orientation and a scale of zero and with an empty descriptor.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 */
void SIFTFeatureList2D::addFeature(const PointType& p, float weight)
{
    addFeature(p, weight, 0, 0, QVector<float>());
    
}
/**
 * Addition of an edgel feature to the list. This will append the given edgel feature
 * at the end of the list of features.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
 */
void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation)
{
    addFeature(p, weight, orientation, 0, QVector<float>());
}

/**
 * Addition of a SIFT feature to the list. This will append the given edgel feature
 * at the end of the list of features and assign it with an empty descriptor.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
 * \param scale The scale (in scale-space sigma) of the SIFT feature.
 */
void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation, float scale)
{
    addFeature(p, weight, orientation, scale, QVector<float>());
    
}

/**
 * Addition of a SIFT feature to the list. This will append the given edgel feature
 * at the end of the list of features.
 *
 * \param p The new feature.
 * \param weight The weight of the new feature.
 * \param orientation The orientation of the new feature (0 = 3h, pi/2 = 6h, pi=9h, 3pi/2=12h).
 * \param scale The scale (in scale-space sigma) of the SIFT feature.
 * \param descr The SIFT descriptor of the feature
 */
void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation, float scale, const QVector<float> & desc)
{
    if(locked())
        return;
    
	m_scales.push_back(scale);
    m_descriptors.push_back(desc);
    
    EdgelFeatureList2D::addFeature(p, weight, orientation);
}

/**
 * Specialized removal of a feature at a certain index.
 * Does nothing if the model is locked or the index is out of range.
 *
 * \param index The index of the feature inside the list.
 */
void SIFTFeatureList2D::removeFeature(unsigned int index)
{
    if(locked())
        return;
    
	if (index <(unsigned int) m_scales.size() )
    {
        m_scales.erase(m_scales.begin()+index);
        m_descriptors.erase(m_descriptors.begin()+index);
        EdgelFeatureList2D::removeFeature(index);
    }
}

/**
 * The content's item header for the SIFT feature list serialization.
 * 
 * \return Always: "x, y, weight, orientation, scale, descr_0, ..., descr_N"
 */
QString SIFTFeatureList2D::item_header() const
{
	return EdgelFeatureList2D::item_header() + ", scale, descr_0, ..., descr_N";
}

/**
 * Serialization of a single SIFT feature inside the list at a given index.
 * The SIFT will be serialized by means of comma separated values.
 * 
 * \param index Index of the SIFT feature to be serialized.
 * \return QString of the edgel feature, ordered  as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
 */
QString SIFTFeatureList2D::serialize_item(unsigned int index) const
{
	QString result = QString("%1, %2").arg(EdgelFeatureList2D::serialize_item(index)).arg(m_scales[index]);
    
    for(unsigned int i=0; i< (unsigned int)m_descriptors[index].size(); ++i)
    {
		result += ", " + QString::number((m_descriptors[index])[i], 'g', 10);
	}
	return result;

}
    
/**
 * Deserialization/addition of a SIFT feature from a string to this list.
 *
 * \param serial A QString containing the serialization of the SIFT feature.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: x, y, weight, orientation, scale, descr_0, ..., descr_N.
 */
bool SIFTFeatureList2D::deserialize_item(const QString & serial)
{
    if(locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 5)
    {
		try
        {
			m_scales.push_back(values[4].toFloat());
			
            QVector<float> desc;
            
            if(values.size()>5)
            {
                desc.resize(values.size()-5);
			
                for(int i=5; i< values.size(); ++i)
                {
                    desc[i-5] = values[i].toFloat();
                }
            }
            
            m_descriptors.push_back(desc);
            
            EdgelFeatureList2D::deserialize_item(serial);
			
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}

} //End of namespace graipe
