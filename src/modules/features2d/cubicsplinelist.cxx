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

#include "features2d/cubicsplinelist.hxx"

namespace graipe {

/**
 * @addtogroup graipe_features2d
 * @{
 *     @file
 *     @brief Implementation file for 2d cubic spline lists
 * @}
 */

CubicSplineList2D::CubicSplineList2D(Workspace* wsp)
: Model(wsp)
{
}

CubicSplineList2D::CubicSplineList2D(const CubicSplineList2D& spline_list)
: Model(spline_list)
{
	for (unsigned int i=0; i<spline_list.size(); ++i)
	{
		addSpline(spline_list.spline(i));
	}
}

unsigned int CubicSplineList2D::size() const
{
	return m_splines.size();
}

void CubicSplineList2D::clear()
{
    if(locked())
        return;
    
	m_splines.clear(); 
	emit modelChanged();
}

const CubicSplineList2D::CubicSplineType & CubicSplineList2D::spline(unsigned int index) const
{
	return m_splines[index];
}

void CubicSplineList2D::setSpline(unsigned int index, const CubicSplineType& new_spline)
{ 
    if(locked())
        return;
    
	m_splines[index] = new_spline;
	emit modelChanged();	
}

void CubicSplineList2D::addSpline(const CubicSplineType& spl)
{
    if(locked())
        return;
    
	m_splines.push_back(spl);
	emit modelChanged();
}

QString CubicSplineList2D::csvHeader() const
{
	return "dp0/dx, dp0/dy, p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y, dpN/dx, dpN/dy";
}

QString CubicSplineList2D::itemToCSV(unsigned int index) const
{

    CubicSplineType::PointType first_derivative = m_splines[index].derive(0),
                               last_derivative = m_splines[index].derive(1);
    CubicSplineType::PointListType points = m_splines[index].points() ;
    
    QString result = QString::number(first_derivative.x(), 'g', 10) + ", " + QString::number(first_derivative.y(), 'g', 10);
	
	for(int i=0; i < points.size(); ++i)
    {
		result +=    ", " + QString::number(points[i].x(), 'g', 10)
                   + ", " + QString::number(points[i].y(), 'g', 10);
    }
    
    result += ", " + QString::number(last_derivative.x(), 'g', 10) + ", " + QString::number(last_derivative.y(), 'g', 10);
	
    return result;
}

bool CubicSplineList2D::itemFromCSV(const QString & serial)
{
    if(locked())
        return false;
    
    CubicSplineType::PointType first_derivative, last_derivative;
    CubicSplineType::PointListType points;
    
    //try to split content into data entries
	QStringList values = serial.split(", ");
	
	if(values.size() >= 6)
	{
        try
        {
            points.clear();
            
            first_derivative.setX(values[0].toFloat());
            first_derivative.setY(values[1].toFloat());
            
			for(unsigned int i = 2; i<(unsigned int)values.size()-3; i+=2)
			{
				points.push_back(CubicSplineType::PointType(values[i+0].toFloat(), values[i+1].toFloat()));
			}
            
            last_derivative.setX(values[values.size()-2].toFloat());
            last_derivative.setY(values[values.size()-1].toFloat());
		}
		catch (std::exception & e)
        {
            qDebug().nospace() <<  "Error occured: " << e.what() << "\n" ;
			return false;
		}
		catch(...)
        {
			return false;
		}
	}
	
    m_splines.push_back(CubicSplineType(points, first_derivative, last_derivative));
    return true;
}

void CubicSplineList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    CubicSplineType::PointType first_derivative = m_splines[index].derive(0),
                               last_derivative = m_splines[index].derive(1);
    CubicSplineType::PointListType points = m_splines[index].points() ;
    
        xmlWriter.writeStartElement("Derivative");
            xmlWriter.writeAttribute("ID", "0");
            xmlWriter.writeTextElement("x", QString::number(first_derivative.x(), 'g', 10));
            xmlWriter.writeTextElement("y", QString::number(first_derivative.y(), 'g', 10));
        xmlWriter.writeEndElement();
    
	for(int i=0; i < points.size(); ++i)
    {
        xmlWriter.writeStartElement("Point");
            xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeTextElement("x", QString::number(points[i].x(), 'g', 10));
            xmlWriter.writeTextElement("y", QString::number(points[i].y(), 'g', 10));
        xmlWriter.writeEndElement();
    }
    
        xmlWriter.writeStartElement("Derivative");
            xmlWriter.writeAttribute("ID", QString::number(points.size()-1));
            xmlWriter.writeTextElement("x", QString::number(last_derivative.x(), 'g', 10));
            xmlWriter.writeTextElement("y", QString::number(last_derivative.y(), 'g', 10));
        xmlWriter.writeEndElement();
}

bool CubicSplineList2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if(locked())
        return false;
    
    CubicSplineType::PointType first_derivative, last_derivative;
    CubicSplineType::PointListType points;
	
    if(/*     xmlReader.readNextStartElement()
        &&*/  xmlReader.name() == "CubicSpline2D"
        &&  xmlReader.attributes().hasAttribute("Points"))
    {
        int size = xmlReader.attributes().value("Points").toInt();
        points.resize(size);
        
        if(     xmlReader.readNextStartElement()
            && xmlReader.name() == "Derivative"
            && xmlReader.attributes().hasAttribute("ID")
            && xmlReader.attributes().value("ID").toInt() == 0)
        {
            if(     xmlReader.readNextStartElement()
                &&  xmlReader.name() == "x")
            {
                first_derivative.setX(xmlReader.readElementText().toFloat());
            }
            if(     xmlReader.readNextStartElement()
                &&  xmlReader.name() == "y")
            {
                first_derivative.setY(xmlReader.readElementText().toFloat());
            }
        }
        
        for(int i=0; i!=size; ++i)
        {
            if(     xmlReader.readNextStartElement()
                &&  xmlReader.name() == "Point"
                &&  xmlReader.attributes().hasAttribute("ID")
                &&  xmlReader.attributes().value("ID").toInt() == i)
            {
                
                if(     xmlReader.readNextStartElement()
                    &&  xmlReader.name() == "x")
                {
                    points[i].setX(xmlReader.readElementText().toFloat());
                }
                if(     xmlReader.readNextStartElement()
                    &&  xmlReader.name() == "y")
                {
                    points[i].setY(xmlReader.readElementText().toFloat());
                }
            }
        }
        
        if(    xmlReader.readNextStartElement()
            && xmlReader.name() == "Derivative"
            && xmlReader.attributes().hasAttribute("ID")
            && xmlReader.attributes().value("ID").toInt() == size-1)
        {
            if(     xmlReader.readNextStartElement()
               &&   xmlReader.name() == "x")
            {
                last_derivative.setX(xmlReader.readElementText().toFloat());
            }
            if(     xmlReader.readNextStartElement()
               &&   xmlReader.name() == "y")
            {
                last_derivative.setY(xmlReader.readElementText().toFloat());
            }
        }
        
    }
    else
    {
        qWarning() << "Did not find matching start attribute";
        return false;
    }
    
    m_splines.push_back(CubicSplineType(points, first_derivative, last_derivative));
    return true;
}

void CubicSplineList2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
	for(unsigned int i=0; i < size(); ++i)
    {
        xmlWriter.writeStartElement("CubicSpline2D");
            xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeAttribute("Points", QString::number(m_splines[i].points().size()));
        
            serialize_item(i, xmlWriter);
        xmlWriter.writeEndElement();
    }
}

bool CubicSplineList2D::deserialize_content(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;

    //Clean up
	clear();
    updateModel();
    
    //Read the entries
    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "CubicSpline2D")
        {
            if(!deserialize_item(xmlReader))
                return false;
        }
        else
        {
            xmlReader.skipCurrentElement();
        }
    }
    return true;
}










WeightedCubicSplineList2D::WeightedCubicSplineList2D(Workspace* wsp)
: CubicSplineList2D(wsp)
{

}

WeightedCubicSplineList2D::WeightedCubicSplineList2D(const WeightedCubicSplineList2D& spline_list)
: CubicSplineList2D(spline_list)
{
	for (unsigned int i=0; i<spline_list.size(); ++i)
	{
		m_weights.push_back(spline_list.weight(i));
	}
}

float WeightedCubicSplineList2D::weight(unsigned int index) const
{
	return m_weights[index];
}

void WeightedCubicSplineList2D::setWeight(unsigned int index, float new_w)
{
	m_weights[index] = new_w;
}

void WeightedCubicSplineList2D::setSpline(unsigned int index, const CubicSplineType& new_spl)
{
    setSpline(index, new_spl, 0);
}

void WeightedCubicSplineList2D::setSpline(unsigned int index, const CubicSplineType& new_spl, float new_w)
{
    if(locked())
        return;
    
	CubicSplineList2D::setSpline(index, new_spl);
	setWeight(index, new_w);
}

void WeightedCubicSplineList2D::addSpline(const CubicSplineType& spl)
{
    addSpline(spl, 0);
}

void WeightedCubicSplineList2D::addSpline(const CubicSplineType& spl, float w)
{
    if(locked())
        return;
    
	CubicSplineList2D::addSpline(spl);
	m_weights.push_back(w);
}

QString WeightedCubicSplineList2D::csvHeader() const
{
    return "weight, " + CubicSplineList2D::csvHeader();
}

QString WeightedCubicSplineList2D::itemToCSV(unsigned int index) const
{
    return QString("%1, %2").arg(m_weights[index]).arg(CubicSplineList2D::itemToCSV(index));
}

bool WeightedCubicSplineList2D::itemFromCSV(const QString & serial)
{
    if(locked())
        return false;    
    
	//try to split into weight and content
    QStringList weight_content = split_string_once(serial, ", ");
	
	if(weight_content.size()==2)
    {
		try
        {
			bool res = CubicSplineList2D::itemFromCSV(weight_content[1]);
			if (res)
            {
                m_weights.push_back(weight_content[0].toFloat());
			}
            return res;
		}
		catch(...)
        {
		
            qCritical("WeightedCubicSplineList2D deserialize_item: Unable to deserialize '%s','%s' as a weighted point!", weight_content[1].toStdString().c_str(), weight_content[0].toStdString().c_str());
            return false;
		}
	}
	return false;
}

void WeightedCubicSplineList2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    CubicSplineList2D::serialize_item(index, xmlWriter);
    
    xmlWriter.writeTextElement("weight",  QString::number(m_weights[index], 'g', 10));
}

bool WeightedCubicSplineList2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if(locked())
        return false;    
    
	if (!CubicSplineList2D::deserialize_item(xmlReader))
    {
        return false;
    }
    
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "weight")
    {
        m_weights.push_back(xmlReader.readElementText().toFloat());
        return true;
    }
    return false;
}

} //End of namespace graipe
