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

#include "features2d/polygonlist.hxx"

namespace graipe {

/**
 * Default constructor. Constructs an empty list of polygons.
 */
PolygonList2D::PolygonList2D()
{
}

/**
 * Copy constructor. Creates a copy from another list of polygons.
 *
 * \param poly_list The other PolygonList2D.
 */
PolygonList2D::PolygonList2D(const PolygonList2D& poly_list)
: Model(poly_list)
{
	for (unsigned int i=0; i<poly_list.size(); ++i)
	{
		addPolygon(poly_list.polygon(i));
	}
}

/**
 * Returns the typeName of a polygon list.
 *
 * \return Always "PolygonList2D".
 */
QString PolygonList2D::typeName() const
{
	return "PolygonList2D";
}
    
/**
 * Returns the number of polygons in this list.
 *
 * \return The number of polygons in this list.
 */
unsigned int PolygonList2D::size() const
{
    return m_polys.size();
}
    
/**
 * Completely erases this list of polygons. Does nothing if the list is locked.
 */
void PolygonList2D::clear()
{
    if(locked())
        return;
    
    m_polys.clear();
    updateModel();
}
    
/*
 * Constant access a polygon inside this list at a given index. May throw
 * an error, if the index is out of range.
 *
 * \param index The index of the polygon in the list.
 */
const PolygonList2D::PolygonType & PolygonList2D::polygon(unsigned int index) const
{
	return m_polys[index];
}

/*
 * Reset/replace a polygon inside this list at a given index. If the index is
 * out of range or the model is locked, this function will do nothing.
 *
 * \param index The index of the replaced polygon in the list.
 * \param new_p The replacement polygon.
 */
void PolygonList2D::setPolygon(unsigned int index, const PolygonType& new_p)
{ 
    if(locked())
        return;
    
	m_polys[index] = new_p;
	emit modelChanged();	
}

/*
 * Add a polygon at the end of this list.
 * If the model is locked, this function will do nothing.
 *
 * \param p The replacement polygon.
 */
void PolygonList2D::addPolygon(const PolygonType& poly)
{
    if(locked())
        return;
    
	m_polys.push_back(poly);
	emit modelChanged();
}

/**
 * Return the item header for this list of polygons.
 *
 * \return Always "p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y".
 */
QString PolygonList2D::item_header() const
{
	return "p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y";
}

/**
 * Serialization of one polygon at a given list index to a string. This function will
 * throw an error if the index is out of range.
 *
 * \param index The index of the polygon to be serialized.
 * \return A QString containing the searialization of the polygon.
 */
QString PolygonList2D::serialize_item(unsigned int index) const
{
    QString result;
	
	for(unsigned int i=0; i < size()-1; ++i)
    {
        PolygonType::PointType p = polygon(index)[i];
        
        result += QString::number(p.x(), 'g', 10) + ", " + QString::number(p.y(), 'g', 10) + ", ";
	}
    if (size())
    {
        PolygonType::PointType p = polygon(index)[size()-1];
        result += QString::number(p.x(), 'g', 10) + ", " + QString::number(p.y(), 'g', 10);
    }
	return result;
}

/**
 * Deserialization/addition of a polygon from a string to this list.
 *
 * \param serial A QString containing the searialization of the polygon.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: p0_x, p0_y, ... , pN_x, pN_y
 */
bool PolygonList2D::deserialize_item(const QString & serial)
{
    if(locked())
        return false;
    
    //try to split content into data entries
	QStringList values = serial.split(", ");
	
    typedef PolygonType::PointType PointType;
    
	if(values.size() >= 2)
	{
		//create new polygon and add points (according to serial)
        PolygonType new_p;
            
        for(unsigned int i = 0 ; i<(unsigned int)values.size()-1; i+=2)
		{
			new_p.addPoint(PointType(values[i].toFloat(), values[i+1].toFloat()));
		}
        
        m_polys.push_back(new_p);
		return true;
	}
    return false;
}

/**
 * Serialization the list of polygons to a QIODevice.
 * The first line is the header as given in item_header(). Each following
 * line represents one polygon serialization.
 *
 * \param out The QIODevice, where we will put our output on.
 */
void PolygonList2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
//TODO!!!
/*	write_on_device(item_header(), out);
	
    for(unsigned int index=0; index < size()-1; ++index)
    {
		write_on_device("\n" + serialize_item(index), out);
	}
    */
}

/**
 * Deserializion of a list of polygons from a QIODevice.
 * The first line is the header as given in item_header(), which is ignored however.
 * Each following line has to be one valid polygon serialization.
 *
 * \param in The QIODevice, where we will read from.
 */
bool PolygonList2D::deserialize_content(QXmlStreamReader& xmlReader)
{
//TODO!!!
/**    if(locked())
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
        QString line = QString(in.readLine());
        
        //ignore comments and empty lines
        if(!line.isEmpty() && !line.startsWith(";"))
        {
            if (!deserialize_item(line))
            {
                qCritical() << "PolygonList2D::deserialize_content: Polygon could not be deserialized from: '" << line << "'";
                return false;
            }
        }
    }
    return true;
    */
    return false;
}




/**
 * Default constructor. Constructs an empty list of weighted polygons.
 */
WeightedPolygonList2D::WeightedPolygonList2D()
{

}

/**
 * Copy constructor. Creates a copy from another list of weighted polygons.
 *
 * \param poly_list The other WeightedPolygonList2D.
 */
WeightedPolygonList2D::WeightedPolygonList2D(const WeightedPolygonList2D& poly_list)
: PolygonList2D(poly_list)
{
	for (unsigned int i=0; i<poly_list.size(); ++i)
	{
		m_weights.push_back(poly_list.weight(i));
	}
}

/**
 * Returns the typeName of a weighted polygon list.
 *
 * \return Always "WeightedPolygonList2D".
 */
QString WeightedPolygonList2D::typeName() const
{
	return "WeightedPolygonList2D";
}

/**
 * Getter of the weight of a polygon at a given index. May throw an error,
 * if the index is out of bounds.
 * 
 * \param index The index, for qhich we query the weight.
 * \return The weight at the given index.
 */
float WeightedPolygonList2D::weight(unsigned int index) const
{
	return m_weights[index];
}

/**
 * Setter of the weight of a polygon at a given index. If the index is
 * out of range or the model is locked, this function will do nothing.
 *
 * \param index The index of the replaced weight in the list.
 * \param new_w The replacement weight.
 */
void WeightedPolygonList2D::setWeight(unsigned int index, float new_w)
{
	m_weights[index] = new_w;
}

/*
 * Reset/replace a polygon inside this list at a given index. The weight of the reset
 * polygon will be set to zero. If the index is out of range or the model is locked, 
 * this function will do nothing.
 *
 * \param index The index of the replaced polygon in the list.
 * \param new_p The replacement polygon.
 */
void WeightedPolygonList2D::setPolygon(unsigned int index, const PolygonType& new_p)
{
    setPolygon(index, new_p, 0);
}

/**
 * Reset/replace a polygon and a weight inside this list at a given index.
 * If the index is out of range or the model is locked,
 * this function will do nothing.
 *
 * \param index The index of the replaced polygon in the list.
 * \param new_p The replacement polygon.
 * \param new_w The replacement weight.
 */
void WeightedPolygonList2D::setPolygon(unsigned int index, const PolygonType& new_p, float new_w)
{
    if(locked())
        return;
    
    PolygonList2D::setPolygon(index, new_p);
    setWeight(index, new_w);
}

/*
 * Add a polygon at the end of this list.
 * If the model is locked, this function will do nothing.
 *
 * \param poly The new polygon.
 */
void WeightedPolygonList2D::addPolygon(const PolygonType& poly)
{
    addPolygon(poly, 0);
}

/*
 * Add a weighted polygon at the end of this list.
 * If the model is locked, this function will do nothing.
 *
 * \param poly The new polygon.
 * \param w The new weight.
 */
void WeightedPolygonList2D::addPolygon(const PolygonType& poly, float w)
{
    if(locked())
        return;
    
    PolygonList2D::addPolygon(poly);
    m_weights.push_back(w);
}

/**
 * Return the item header for this list of polygons.
 *
 * \return Always "weight, p0_x, p0_y, p1_x, p1_y, ... , pN_x, pN_y".
 */
QString WeightedPolygonList2D::item_header() const
{
	return "weight, " + PolygonList2D::item_header();
}

/**
 * Serialization of one polygon at a given list index to a string. This function will
 * throw an error if the index is out of range.
 *
 * \param index The index of the polygon to be serialized.
 * \return A QString containing the searialization of the polygon.
 */
QString WeightedPolygonList2D::serialize_item(unsigned int index) const
{
    return QString::number(weight(index), 'g', 10) + ", " + PolygonList2D::serialize_item(index);
}

/**
 * Deserialization/addition of a polygon from a string to this list.
 *
 * \param serial A QString containing the searialization of the polygon.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization should be given as: p0_x, p0_y, ... , pN_x, pN_y
 */
bool WeightedPolygonList2D::deserialize_item(const QString & serial)
{

    if(locked())
        return false;    
    
	//try to split into weight and content
    QStringList weight_content = split_string_once(serial, ", ");
	
	if(weight_content.size()==2)
    {
		try
        {
			bool res = PolygonList2D::deserialize_item(weight_content[1]);
			if (res)
            {
                m_weights.push_back(weight_content[0].toFloat());
			}
            return res;
		}
		catch(...)
        {
		
            qCritical("WeightedPolygonList2D deserialize_item: Unable to deserialize '%s','%s' as a weighted point!", weight_content[1].toStdString().c_str(), weight_content[0].toStdString().c_str());
            return false;
		}
	}
	return false;
}
    
} //End of namespace graipe
