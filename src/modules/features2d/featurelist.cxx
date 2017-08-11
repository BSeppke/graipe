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

#include "features2d/featurelist.hxx"

#define _USE_MATH_DEFINES
#include <math.h>

namespace graipe {

/**
 * @addtogroup graipe_features2d
 * @{
 *     @file
 *     @brief Implementation file for 2d feature lists
 * @}
 */

PointFeatureList2D::PointFeatureList2D(Workspace* wsp)
: Model(wsp)
{
}

unsigned int PointFeatureList2D::size() const
{
	return m_points.size();
}

void PointFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_points.clear(); 
	updateModel();
}

const PointFeatureList2D::PointType& PointFeatureList2D::position(unsigned int index) const
{
	return m_points[index];		
}

void PointFeatureList2D::setPosition(unsigned int index, const PointType& new_p)
{ 
    if(locked())
        return;
    
	m_points[index] = new_p;	
	updateModel();	
}

void PointFeatureList2D::addFeature(const PointType& p)
{
    if(locked())
        return;
    
	m_points.push_back(p);
	updateModel();
}

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

QString PointFeatureList2D::csvHeader() const
{
	return "pos_x, pos_y";
}

QString PointFeatureList2D::itemToCSV(unsigned int index) const
{
	return   QString::number(m_points[index].x(), 'g', 10) + ", "
           + QString::number(m_points[index].y(), 'g', 10);
}

bool PointFeatureList2D::itemFromCSV(const QString & serial)
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

void PointFeatureList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.writeTextElement("x", QString::number(m_points[index].x(), 'g', 10));
    xmlWriter.writeTextElement("y", QString::number(m_points[index].y(), 'g', 10));
}

bool PointFeatureList2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    PointType new_p;
    
    //Read two start elements
    for(int i=0; i<2; ++i)
    {
        if (xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == "x")
            {
                new_p.setX(xmlReader.readElementText().toFloat());
            }
            else if (xmlReader.name() == "y")
            {
                new_p.setY(xmlReader.readElementText().toFloat());
            }
            else
            {
                qWarning() << "Searching for x and y tags, but found:" << xmlReader.name();
                return false;
            }
        }
        else
        {
            qWarning() << "Did not find at least two start elements";
            return false;
        }
    }
    m_points.push_back(new_p);
    return true;
}

void PointFeatureList2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
	for(unsigned int i=0; i < size(); ++i)
    {
        xmlWriter.writeStartElement("Feature");
        xmlWriter.writeAttribute("ID", QString::number(i));
            serialize_item(i, xmlWriter);
        xmlWriter.writeEndElement();
    }
}

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
            if(!deserialize_item(xmlReader))
                return false;
        }
        else
        {
            qWarning() << "Found non 'Feature' tag in serialization of elements";
            return false;
        }
        //Read until </Feature> comes...
        while(true)
        {
            if(!xmlReader.readNext())
            {
                return false;
            }
            
            if(xmlReader.isEndElement() && xmlReader.name() == "Feature")
            {
                break;
            }
        }

    }
    return true;
}










WeightedPointFeatureList2D::WeightedPointFeatureList2D(Workspace* wsp)
:   PointFeatureList2D(wsp)
{
}

void WeightedPointFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_weights.clear();
    
	PointFeatureList2D::clear();
}

float WeightedPointFeatureList2D::weight(unsigned int index) const
{
	return m_weights[index];		
}

void WeightedPointFeatureList2D::setWeight(unsigned int index, float new_w)
{
    if(locked())
        return;
    
	m_weights[index]= new_w;	
	updateModel();
}

void WeightedPointFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0);
}

void WeightedPointFeatureList2D::addFeature(const PointType& p,float weight)
{
    if(locked())
        return;
    
    m_weights.push_back(weight);
    PointFeatureList2D::addFeature(p);
}

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

QString WeightedPointFeatureList2D::csvHeader() const
{
	return PointFeatureList2D::csvHeader() + ", weight";
}

QString WeightedPointFeatureList2D::itemToCSV(unsigned int index) const
{
	return PointFeatureList2D::itemToCSV(index) + ", " + QString::number(m_weights[index], 'g', 10);
}

bool WeightedPointFeatureList2D::itemFromCSV(const QString & serial)
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
            PointFeatureList2D::itemFromCSV(serial);
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}

void WeightedPointFeatureList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
	PointFeatureList2D::serialize_item(index, xmlWriter);
    xmlWriter.writeTextElement("weight", QString::number(m_weights[index], 'g', 10));
}

bool WeightedPointFeatureList2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if(locked())
        return false;
    
    if(!PointFeatureList2D::deserialize_item(xmlReader))
    {
        return false;
    }
	
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "weight")
    {
        m_weights.push_back(xmlReader.readElementText().toFloat());
        return true;
    }
    else
    {
        qWarning() << "Did not find a start element for feature weight";
        return false;
    }
}










EdgelFeatureList2D::EdgelFeatureList2D(Workspace* wsp)
: WeightedPointFeatureList2D(wsp)
{
}

void EdgelFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_orientations.clear();
    
	WeightedPointFeatureList2D::clear();
}

float EdgelFeatureList2D::angle(unsigned int index) const
{
	return m_orientations[index]*180.0/M_PI;
}

float EdgelFeatureList2D::orientation(unsigned int index) const
{
	return m_orientations[index];		
}

void EdgelFeatureList2D::setOrientation(unsigned int index, float new_o)
{
    if(locked())
        return;
    
	m_orientations[index]= new_o;
	updateModel();
}

void EdgelFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0, 0);
}

void EdgelFeatureList2D::addFeature(const PointType& p, float weight)
{
    addFeature(p, weight, 0);
}

void EdgelFeatureList2D::addFeature(const PointType& p,float weight, float orientation)
{
    if(locked())
        return;
    
    m_orientations.push_back(orientation);
    WeightedPointFeatureList2D::addFeature(p, weight);
}

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

QString EdgelFeatureList2D::csvHeader() const
{
	return WeightedPointFeatureList2D::csvHeader() + ", orientation";
}

QString EdgelFeatureList2D::itemToCSV(unsigned int index) const
{
	return WeightedPointFeatureList2D::itemToCSV(index) + ", " + QString::number(m_orientations[index], 'g', 10);
}

bool EdgelFeatureList2D::itemFromCSV(const QString & serial)
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
            WeightedPointFeatureList2D::itemFromCSV(serial);
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}

void EdgelFeatureList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
	WeightedPointFeatureList2D::serialize_item(index, xmlWriter);
    xmlWriter.writeTextElement("orientation", QString::number(m_orientations[index], 'g', 10));
}

bool EdgelFeatureList2D::deserialize_item(QXmlStreamReader& xmlReader)
{	
    if(locked())
        return false;
    
    if(!WeightedPointFeatureList2D::deserialize_item(xmlReader))
    {
        return false;
    }
	
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "orientation")
    {
        m_orientations.push_back(xmlReader.readElementText().toFloat());
        return true;
    }
    else
    {
        qWarning() << "Did not find a start element for feature orientation";
        return false;
    }
}










SIFTFeatureList2D::SIFTFeatureList2D(Workspace* wsp)
: EdgelFeatureList2D(wsp)
{
}

void SIFTFeatureList2D::clear()
{
    if(locked())
        return;
    
	m_scales.clear();
    m_descriptors.clear();
    
	EdgelFeatureList2D::clear();
}

float SIFTFeatureList2D::scale(unsigned int index) const
{
	return m_scales[index];
}

void SIFTFeatureList2D::setScale(unsigned int index, float new_s)
{
    if(locked())
        return;
    
	m_scales[index] = new_s;
	updateModel();
}

const QVector<float>& SIFTFeatureList2D::descriptor(unsigned int index) const
{
	return m_descriptors[index];
}

void SIFTFeatureList2D::setDescriptor(unsigned int index, const QVector<float> & new_d)
{
    if(locked())
        return;
    
	m_descriptors[index] = new_d;
	updateModel();
}

void SIFTFeatureList2D::addFeature(const PointType& p)
{
    addFeature(p, 0, 0, 0, QVector<float>());
}

void SIFTFeatureList2D::addFeature(const PointType& p, float weight)
{
    addFeature(p, weight, 0, 0, QVector<float>());
    
}

void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation)
{
    addFeature(p, weight, orientation, 0, QVector<float>());
}

void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation, float scale)
{
    addFeature(p, weight, orientation, scale, QVector<float>());
    
}

void SIFTFeatureList2D::addFeature(const PointType& p, float weight, float orientation, float scale, const QVector<float> & desc)
{
    if(locked())
        return;
    
	m_scales.push_back(scale);
    m_descriptors.push_back(desc);
    
    EdgelFeatureList2D::addFeature(p, weight, orientation);
}

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

QString SIFTFeatureList2D::csvHeader() const
{
	return EdgelFeatureList2D::csvHeader() + ", scale, descr_0, ..., descr_N";
}

QString SIFTFeatureList2D::itemToCSV(unsigned int index) const
{
	QString result = QString("%1, %2").arg(EdgelFeatureList2D::itemToCSV(index)).arg(m_scales[index]);
    
    for(unsigned int i=0; i< (unsigned int)m_descriptors[index].size(); ++i)
    {
		result += ", " + QString::number((m_descriptors[index])[i], 'g', 10);
	}
	return result;

}

bool SIFTFeatureList2D::itemFromCSV(const QString & serial)
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
            
            EdgelFeatureList2D::itemFromCSV(serial);
			
			return true;
		}
		catch(...)
        {
			return false;
		}
	}
	return false;
}

void SIFTFeatureList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
	EdgelFeatureList2D::serialize_item(index, xmlWriter);
    
    xmlWriter.writeTextElement("scale", QString::number(m_scales[index], 'g', 10));
    
    xmlWriter.writeStartElement("descriptor");
    xmlWriter.writeAttribute("size", QString::number(m_descriptors[index].size()));
    
    for(unsigned int i=0; i< (unsigned int)m_descriptors[index].size(); ++i)
    {
		xmlWriter.writeStartElement("value");
        xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeCharacters(QString::number((m_descriptors[index])[i], 'g', 10));
        xmlWriter.writeEndElement();
	}
}

bool SIFTFeatureList2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if(locked())
        return false;
    
    if(!EdgelFeatureList2D::deserialize_item(xmlReader))
    {
        return false;
    }
    //Read two more starting tags
	for(int i=0; i!=2; i++)
    {
        if(xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == "scale")
            {
                m_scales.push_back(xmlReader.readElementText().toFloat());
            }
            else if(   xmlReader.name() == "descriptor"
                    && xmlReader.attributes().hasAttribute("size"))
            {
                int d_size = xmlReader.attributes().value("size").toInt();
                QVector<float> desc(d_size);
                
                //Read the descriptor
                for(int d_i=0; d_i!=d_size; d_i++)
                {
                    if(     xmlReader.readNextStartElement()
                        &&  xmlReader.name() == "value")
                    {
                        desc[d_i] = xmlReader.readElementText().toFloat();
                    }
                    else
                    {
                        qWarning() << "Did not find enough descriptor fields, needed"  << xmlReader.attributes().value("size") << " stopped at: " << d_i ;
                        return false;
                    }
                }
                m_descriptors.push_back(desc);
            }
            else
            {
                qWarning() << "Did find a different start element for SIFT features:" <<  xmlReader.name();
                return false;
            }
        }
        else
        {
            qWarning() << "Did not find at least two more start element for SIFT features";
            return false;
        }
    }
    return true;
}

} //End of namespace graipe
