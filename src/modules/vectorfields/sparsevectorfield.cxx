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

#include "vectorfields/sparsevectorfield.hxx"
#include "core/basicstatistics.hxx"

namespace graipe {

/**
 * Default constructor. Creates an empty sparse vectorfield.
 */
SparseVectorfield2D::SparseVectorfield2D()
:	Vectorfield2D()
{
}

/**
 * Copy constructor. Creates a sparse vectorfield from another one.
 *
 * \param vf The other sparse vectorfield.
 */
SparseVectorfield2D::SparseVectorfield2D(const SparseVectorfield2D & vf)
:	Vectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		addVector(vf.origin(i),vf.direction(i));
	}
}


/**
 * The typename of this vector field.
 *
 * \return Always "SparseVectorfield2D"
 */
QString SparseVectorfield2D::typeName() const
{ 
	return QString("SparseVectorfield2D");
}

/**
 * The size of this vectorfield. 
 * Implemented here, defined as pure virtual in base class.
 * 
 * \return The number of vectors in this vectorfield.
 */
unsigned int SparseVectorfield2D::size() const
{
	return (unsigned int)m_origins.size();
}

/**
 * The removal of all vectors of this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * Does nothing if the model is locked.
 */
void SparseVectorfield2D::clear()
{
    if(locked())
        return;
    
	m_origins.clear();
	m_directions.clear();
	
	updateModel();
}

/**
 * The origin/position of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * May throw an error, if the index is out of bounds.
 * 
 * \param index The index of the vector.
 * \return The origin of the vector at the given index.
 */
SparseVectorfield2D::PointType SparseVectorfield2D::origin(unsigned int index) const
{	
	return m_origins[index];			
}

/**
 * Set the origin/position of a vector at a given index in this vectorfield.
 * Does nothing if the model is locked.
 * 
 * \param index The index of the new origin.
 * \param new_o The new origin of the vector at that index.
 */
void SparseVectorfield2D::setOrigin(unsigned int index, const PointType& new_p)
{	
    if(locked())
        return;
    
	m_origins[index] = new_p;
	updateModel();	
}

/**
 * The direction of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * May throw an error, if the index is out of bounds.
 * 
 * \param index The index of the vector.
 * \return The direction of the vector at the given index.
 */
SparseVectorfield2D::PointType SparseVectorfield2D::direction(unsigned int index) const
{	
	return m_directions[index];	
}
/**
 * Set the direction of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * Does nothing if the model is locked.
 * 
 * \param index The index of the new direction.
 * \param new_d The new direction of the vector at that index.
 */
void SparseVectorfield2D::setDirection(unsigned int index, const PointType& new_p)
{
    if(locked())
        return;
    
	m_directions[index] = new_p;
	updateModel();
}


/**
 * Add a vector to the sparse vectorfield.
 * Does nothing if the model is locked.
 *
 * \param orig The origin of the new vector.
 * \param dir  The direction of the new vector.
 */
void SparseVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{		
    if(locked())
        return;
    
	m_origins.push_back(orig); 
	m_directions.push_back(dir);
	updateModel();
}

/**
 * Removing a vector from the vector field at a given index.
 * If the index is out of bounds or the model is locked, 
 * this will not remove any vector.
 *
 * \param index. The index, where the vector shall be removed.
 */
void SparseVectorfield2D::removeVector(unsigned int index)
{
    if(locked())
        return;
    
	if (index < m_origins.size() )
    {
        m_origins.erase(m_origins.begin()+index);
        m_directions.erase(m_directions.begin()+index);
        updateModel();
    }
}

/**
 * The content's item header for the vectorfieldserialization.
 * 
 * \return Always: "pos_x, pos_y, dir_x, dir_y".
 */
QString SparseVectorfield2D::item_header() const
{
	return "pos_x, pos_y, dir_x, dir_y";
}

/**
 * Serialization of a single vector inside the list at a given index.
 * The vector will be serialized by means of comma separated values.
 * 
 * \param index Index of the vector to be serialized.
 * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y".
 */
QString SparseVectorfield2D::serialize_item(unsigned int index) const
{
	return   QString::number(m_origins[index].x(), 'g', 10)    + ", "
           + QString::number(m_origins[index].y(), 'g', 10)    + ", "
           + QString::number(m_directions[index].x(), 'g', 10) + ", "
           + QString::number(m_directions[index].y(), 'g', 10);
}

/**
 * Deserialization/addition of a vector from a string to this list.
 *
 * \param serial A QString containing the serialization of the vector.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y
 */
bool SparseVectorfield2D::deserialize_item(const QString & serial)
{
    if(locked())
        return false;
        
	//try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 4)
	{
		try
		{
			addVector(PointType(values[0].toFloat(), values[1].toFloat()),
					  PointType(values[2].toFloat(), values[3].toFloat()));
			return true;
		}
		catch(...)
		{
			qDebug() << QString("Error while importing SparseVectorfield2D: ") <<  serial;
			return false;
		}
	}
	return false;
}

/**
 * Serialize the complete content of the sparse vectorfield to an xml file.
 * Mainly prints:
 *   item_header()
 * and for each vector:
 *   newline + serialize_item().
 *
 * \param out The output device for serialization.
 */
void SparseVectorfield2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.writeTextElement("Legend", item_header());
    
	for(unsigned int i=0; i < size(); ++i)
    {
        xmlWriter.writeStartElement("Vector");
        xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeCharacters(serialize_item(i));
        xmlWriter.writeEndElement();
    }
}

/**
 * Deserialization of a  sparse vectorfield from an xml file.
 * The first line is the header as given in item_header(), which is ignored however.
 * Each following line has to be one valid vector serialization.
 * Does nothing if the model is locked.
 *
 * \param xmlReader The QXmlStreamReader, where we will read from.
 * \return True, if the content could be deserialized and the model is not locked.
 */
bool SparseVectorfield2D::deserialize_content(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;

    //Clean up
	clear();
    updateModel();
    
    //Read the entries
    while(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "Vector")
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
 * Default constructor. Creates an empty weighted sparse vectorfield.
 */
SparseWeightedVectorfield2D::SparseWeightedVectorfield2D()
{
}

/**
 * Copy constructor. Creates a weighted sparse vectorfield from another one.
 *
 * \param vf The other weighted sparse vectorfield.
 */
SparseWeightedVectorfield2D::SparseWeightedVectorfield2D(const SparseWeightedVectorfield2D & vf)
:	SparseVectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		m_weights.push_back(vf.weight(i));
	}
}

/**
 * The typename of this vectorfield.
 *
 * \return Always "SparseWeightedVectorfield2D"
 */
QString SparseWeightedVectorfield2D::typeName() const
{ 
	return QString("SparseWeightedVectorfield2D");
}

/**
 * The removal of all vectors of this vectorfield.
 * Specialized for this class.
 * Does nothing if the model is locked.
 */
void SparseWeightedVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_weights.clear();
    SparseVectorfield2D::clear();
}

/**
 * Getter for the  weight of a vector at a given index. 
 * May throw an error, if the index is out of bounds.
 *
 * \param index The index of the vector, for which we want the weight.
 * \return The weight of that vector.
 */
float SparseWeightedVectorfield2D::weight(unsigned int index) const
{	
	return m_weights[index];	
}

/**
 * Setter for the  weight of a vector at a given index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the vector, for which we want the weight.
 * \param new_w The new weight of that vector.
 */
void SparseWeightedVectorfield2D::setWeight(unsigned int index, float new_w)
{
	m_weights[index]= new_w;	
	
	updateModel();
}

/**
 * Add a vector to the weighted sparse vectorfield.
 * Does nothing if the model is locked.
 *
 * \param orig The origin of the new vector.
 * \param dir  The direction of the new vector.
 */
void SparseWeightedVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
    addVector(orig, dir, 0);
}

/**
 * Add a weighted vector to the weighted sparse vectorfield.
 * Does nothing if the model is locked.
 *
 * \param orig The origin of the new vector.
 * \param dir  The direction of the new vector.
 * \param w  The weight of the new vector.
 */
void SparseWeightedVectorfield2D::addVector(const PointType& orig, const PointType& dir, float w)
{
    if (locked())
        return;
    
    m_weights.push_back(w);
    SparseVectorfield2D::addVector(orig, dir);
}

/**
 * Removing a vector from the vector field at a given index
 * Does nothing if the model is locked or the index is out of bounds.
 *
 * \param index. The index, where the vector shall be removed.
 */
void SparseWeightedVectorfield2D::removeVector(unsigned int index)
{
    if (locked())
        return;
    
	if (index < m_weights.size() )
    {
        m_weights.erase(m_weights.begin()+index);        
    }
    SparseVectorfield2D::removeVector(index);
}

/**
 * The content's item header for the weighted vectorfield serialization.
 * 
 * \return Always: "pos_x, pos_y, dir_x, dir_y, weight".
 */
QString SparseWeightedVectorfield2D::item_header() const
{
	return SparseVectorfield2D::item_header() + ", weight";
}

/**
 * Serialization of a single weighted vector inside the list at a given index.
 * The vector will be serialized by means of comma separated values.
 * 
 * \param index Index of the vector to be serialized.
 * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight".
 */
QString SparseWeightedVectorfield2D::serialize_item(unsigned int index) const
{
	return SparseVectorfield2D::serialize_item(index) + ", "
           + QString::number(m_weights[index], 'g', 10);
}

/**
 * Deserialization/addition of a weihgted vector from a string to this list.
 *
 * \param serial A QString containing the serialization of the vector.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, weight
 */
bool SparseWeightedVectorfield2D::deserialize_item(const QString & serial)
{
    if (locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 5)
	{
		try
		{
			SparseVectorfield2D::deserialize_item(serial);
			m_weights.push_back(values[4].toFloat());
			return true;
		}
		catch(...)
		{
			qDebug() << QString("Error while importing SparseWeightedVectorfield2D: ") << serial;
			return false;
		}
	}
	return false;
}



		
		
		
		








		
		
	
/**
 * Default constructor. Creates an empty sparse multi vectorfield.
 */
SparseMultiVectorfield2D::SparseMultiVectorfield2D()
:	SparseVectorfield2D(),
    m_alternatives(new IntParameter("number of alternative directions",0,1000,10))
{
    m_parameters->addParameter("alternatives", m_alternatives);
}
	
 /**
 * Copy constructor. Creates a sparse multi vectorfield from another one.
 *
 * \param vf The other sparse multi vectorfield.
 */
SparseMultiVectorfield2D::SparseMultiVectorfield2D(const SparseMultiVectorfield2D & vf)
:	SparseVectorfield2D(vf),
    m_alternatives(new IntParameter("number of alternative directions",0,1000,10))
{
    m_parameters->addParameter("alternatives", m_alternatives);
    
    m_alt_directions.resize(vf.size());
	
    for( unsigned int i=0; i < vf.size(); ++i)
	{
        std::vector<PointType> new_vec(vf.alternatives());
        
        for (unsigned int alt_index=0; alt_index<vf.alternatives(); ++alt_index)
        {
            new_vec[alt_index] = vf.altDirection(i,alt_index);
        }
        m_alt_directions[i] = new_vec;
    }
}

/**
 * The typename of this vectorfield.
 *
 * \return Always "SparseMultiVectorfield2D"
 */
QString SparseMultiVectorfield2D::typeName() const
{ 
	return	QString("SparseMultiVectorfield2D");
}

/**
 * The removal of all vectors of this vectorfield.
 * Specialized for this class.
 * Does nothing if the model is locked.
 */
void SparseMultiVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_alt_directions.clear();
    SparseVectorfield2D::clear();
}
/**
 * Getter for the  number of alternative directions for a 
 * sparse multi vectorfield. Note that the overall direction count is:
 * alternatives+1, since the original direction stays untouched.
 *
 * \return The number of alternative directions of a vector.
 */
unsigned int SparseMultiVectorfield2D::alternatives() const
{
	return m_alternatives->value();
}

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
void SparseMultiVectorfield2D::setAlternatives(unsigned int alternatives)
{
    if(locked())
        return;
    
    updateModel();
}

/**
 * The direction of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the altivative direction.
 * \return The alternative direction of the vector at the given indices.
 */
SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altDirection(unsigned int index, unsigned int alt_index) const
{
	return (m_alt_directions[index])[alt_index];
}

/**
 * Sets the direction of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * Does nothing if the model is locked.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the altivative direction.
 * \param new_d The new alternative direction of the vector at the given indices.
 */
void SparseMultiVectorfield2D::setAltDirection(unsigned int index, unsigned int alt_index, const PointType& new_d)
{
    if(locked())
        return;
    
	(m_alt_directions[index])[alt_index] = new_d;
	updateModel();
}

/**
 * The local direction of a vector at a given index in this vectorfield.
 * May throw an error, if the index is out of bounds.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the alternative local direction.
 * \return The alternative local direction of the vector at the given indices.
 */
SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altLocalDirection(unsigned int index, unsigned int alt_index) const
{	
	return altDirection(index, alt_index) - altGlobalDirection(index, alt_index);
}

/**
 * The global direction of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the alternative global direction.
 * \return The alternative global direction of the vector at the given indices.
 */
SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altGlobalDirection(unsigned int index, unsigned int alt_index) const
{	
	return globalMotion().map(origin(index)) - origin(index);
}

/**
 * The squared length of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the alternative squared length.
 * \return The alternative squared length of the vector at the given indeices.
 */
float SparseMultiVectorfield2D::altSquaredLength(unsigned int index, unsigned int alt_index) const
{
    return altDirection(index, alt_index).squaredLength();
}

/**
 * The length of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the alternative length.
 * \return The alternative length of the vector at the given indices.
 */
float SparseMultiVectorfield2D::altLength(unsigned int index, unsigned int alt_index) const
{
    return altDirection(index, alt_index).length();
}

/**
 * The angle of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 *
 * \param index The index of the vector.
 * \param alt_index The index of the alternative angle.
 * \return The alternative angle of the vector at the given indices.
 */
float SparseMultiVectorfield2D::altAngle(unsigned int index, unsigned int alt_index) const
{
	return altDirection(index, alt_index).angle();
}

/**
 * The target of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 *
 * \param index The index of the vector.
 * \param alt_index The index of the alternative target.
 * \return The alternative target of the vector at the given incides.
 */
SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altTarget(unsigned int index, unsigned int alt_index) const
{	
	return origin(index) + altDirection(index, alt_index);
}
/**
 * Sets the target of a vector at a given index in this vectorfield.
 * May throw an error, if the index or the alt_index is out of bounds.
 * Does nothing if the model is locked.
 * 
 * \param index The index of the vector.
 * \param alt_index The index of the altivative target.
 * \param new_t The new alternative target of the vector at the given indices.
 */
void SparseMultiVectorfield2D::setAltTarget(unsigned int index, unsigned int alt_index, const PointType& new_t)
{	
	setAltDirection(index, alt_index, new_t - origin(index));
}

/**
 * Add a vector to the sparse multi vectorfield. This method adds empty alternative
 * directions, given by (0,0) to the alternative list.
 * Does nothing if the model is locked.
 *
 * \param orig The origin of the new vector.
 * \param dir  The direction of the new vector.
 */
void SparseMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
	if(locked())
        return;
    
    addVector(orig, dir, std::vector<PointType>(alternatives()));
}	
   
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
void SparseMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, const std::vector<SparseMultiVectorfield2D::PointType>& dirs)
{
	if(locked())
        return;
    
    std::vector<PointType> new_vec(alternatives());
    
    unsigned int min_count = std::min(alternatives(), (unsigned int)dirs.size());
    
    for (unsigned int alt_index=0; alt_index<min_count; ++alt_index)
    {
        new_vec[alt_index] = dirs[alt_index];
    }
    m_alt_directions.push_back(new_vec);
    
    SparseVectorfield2D::addVector(orig,dir);
}	

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
void SparseMultiVectorfield2D::addVector(const PointType& orig, const std::vector<PointType>& all_dirs)
{
	if(locked())
        return;
    
    Q_ASSERT(all_dirs.size() > 0);
    
    std::vector<PointType> alt_dirs = all_dirs;
    alt_dirs.erase(alt_dirs.begin());
    
    addVector(orig, all_dirs.front(), alt_dirs);
}

/**
 * Removing a vector from the vector field at a given index
 * Does nothing if the model is locked or the index is out of bounds.
 *
 * \param index. The index, where the vector shall be removed.
 */
void SparseMultiVectorfield2D::removeVector(unsigned int index)
{		
	if(locked())
        return;
    
	if (index < m_alt_directions.size() )
    {
        m_alt_directions.erase(m_alt_directions.begin()+index);     
        SparseVectorfield2D::removeVector(index);
    }
}

/**
 * The content's item header for the multi vectorfield serialization.
 * 
 * \return Always: "pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y".
 */
QString SparseMultiVectorfield2D::item_header() const
{
    QString result =  SparseVectorfield2D::item_header();
	
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
			result += QString(", alt%1_dir_x, alt%2_dir_y").arg(i).arg(i);
	}
	
    return result;
}

/**
 * Serialization of a single multi vector inside the list at a given index.
 * The vector will be serialized by means of comma separated values.
 * 
 * \param index Index of the vector to be serialized.
 * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y".
 */
QString SparseMultiVectorfield2D::serialize_item(unsigned int index) const
{
	QString result = SparseVectorfield2D::serialize_item(index);
    
	for( unsigned int i = 1; i<=alternatives(); ++i)
	{
        const PointType & dir = altDirection(index, i);
		result += ", " + QString::number(dir.x(), 'g', 10) + ", " + QString::number(dir.y(), 'g', 10) ;
	}
	return result;
}
		
/**
 * Deserialization/addition of a multi vector from a string to this list.
 *
 * \param serial A QString containing the serialization of the vector.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization is ordered as: pos_x, pos_y, dir_x, dir_y, alt0_dir_x, alt0_dir_y, ... , altN_dir_x, altN_dir_y
 */
bool SparseMultiVectorfield2D::deserialize_item(const QString & serial)
{
    //try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 4)
	{
		try
		{
			if(SparseVectorfield2D::deserialize_item(serial))
            {
                //The parameters should have been deserialized before, so we can use the properties:
                std::vector<PointType> alt_dirs(alternatives());
			
                for(unsigned int a=0; a<alternatives() && 4+a*2<(unsigned int)values.size(); ++a)
                {
                    (alt_dirs[a]).setX(values[4+a*2].toFloat());
                    (alt_dirs[a]).setY(values[4+a*2+1].toFloat());
                }
                m_alt_directions.push_back(alt_dirs);
                return true;
            }
		}
		catch(...)
		{
			qDebug() << QString("Error while importing SparseMultiVectorfield2D: ") << serial;
			return false;
		}
	}
	return false;
}

/**
 * This slot is called, whenever some parameter is changed.
 * It rearranges the size of the direction alternatives' vector.
 */
void SparseMultiVectorfield2D::updateModel()
{
    for( std::vector<PointType>& vec : m_alt_directions)
    {
        std::vector<PointType> new_vec(alternatives());
        
        for (unsigned int i=0; i<alternatives(); ++i)
        {
            new_vec[i] = vec[i];
        }
        vec = new_vec;
    }
    SparseVectorfield2D::updateModel();
}
		
		









		
//A sparse vectorfield with multiple weighted targets for each vector
SparseWeightedMultiVectorfield2D::SparseWeightedMultiVectorfield2D(const SparseWeightedMultiVectorfield2D & vf)
:	SparseMultiVectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		m_weights.push_back(vf.weight(i));
        m_alt_weights.push_back(std::vector<float>(vf.alternatives()));
	}
}
SparseWeightedMultiVectorfield2D::SparseWeightedMultiVectorfield2D()
{
}

QString SparseWeightedMultiVectorfield2D::typeName() const
{
	return	QString("SparseWeightedMultiVectorfield2D");
}

/**
 * The removal of all vectors of this vectorfield.
 * Specialized for this class.
 * Does nothing if the model is locked.
 */
void SparseWeightedMultiVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_weights.clear();
    m_alt_weights.clear();
    SparseMultiVectorfield2D::clear();
}

/**
 * Getter for the weight of a vector at a given index.
 * May throw an error, if the index is out of bounds.
 *
 * \param index The index of the vector, for which we want the weight.
 * \return The weight of that vector.
 */
float SparseWeightedMultiVectorfield2D::weight(unsigned int index) const
{
	return m_weights[index];
}

/**
 * Getter for the alternate weight of a vector at a given index and alt_index.
 * May throw an error, if the indices are out of bounds.
 *
 * \param index The index of the vector, for which we want the weight.
 * \param alt_index The index of the vector, for which we want the weight.
 * \return The weight of that (alternate) direction.
 */
float SparseWeightedMultiVectorfield2D::altWeight(unsigned int index, unsigned int alt_index) const
{
	return (m_alt_weights[index])[alt_index];
}

/**
 * Setter for the  weight of a vector at a given index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the vector, for which we want the weight.
 * \param weight The new weight of that direction.
 */
void SparseWeightedMultiVectorfield2D::setWeight(unsigned int index,  float weight)
{
    if(locked())
        return;
        
	m_weights[index] = weight;
	updateModel();
}

/**
 * Setter for the alternate weight of a vector at a given index and alt_index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the vector, for which we want the weight.
 * \param alt_index The index of the vector, for which we want the weight.
 * \param weight The new weight of that (alternate) direction.
 */
void SparseWeightedMultiVectorfield2D::setAltWeight(unsigned int index,  unsigned int alt_index, float weight)
{
    if(locked())
        return;
        
	(m_alt_weights[index])[alt_index] = weight;
	updateModel();
}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
    if(locked())
        return;
    
    addVector(orig, dir, 0.0, std::vector<PointType>(alternatives()), std::vector<float>(alternatives()));
}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, float weight)
{
    if(locked())
        return;
    
    addVector(orig, dir, weight, std::vector<PointType>(alternatives()), std::vector<float>(alternatives()));
}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, const std::vector<PointType>& alt_dirs)
{
    if(locked())
        return;
    
    addVector(orig, dir, 0.0, alt_dirs, std::vector<float>(alternatives()));

}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, float weight, const std::vector<PointType>& alt_dirs)
{
    if(locked())
        return;
    
    addVector(orig, dir, weight, alt_dirs, std::vector<float>(alternatives()));
}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, float weight, const std::vector<PointType>& alt_dirs, const std::vector<float>& alt_weights)
{
    if(locked())
        return;
    
    m_weights.push_back(weight);
    
    std::vector<float> new_vec(alternatives());
    
    unsigned int min_count = std::min(alternatives(), (unsigned int)alt_weights.size());
    
    for (unsigned int alt_index=0; alt_index<min_count; ++alt_index)
    {
        new_vec[alt_index] = alt_weights[alt_index];
    }
    m_alt_weights.push_back(new_vec);
    
    SparseMultiVectorfield2D::addVector(orig, dir, alt_dirs);
}

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
void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const std::vector<PointType>& all_dirs, const std::vector<float>& all_weights)
{
	if(locked())
        return;
    
    Q_ASSERT(all_dirs.size() > 0);
    Q_ASSERT(all_weights.size() > 0);
    
    std::vector<PointType> alt_dirs = all_dirs;
    alt_dirs.erase(alt_dirs.begin());
    std::vector<float> alt_weights = all_weights;
    alt_weights.erase(alt_weights.begin());
    
    addVector(orig, all_dirs.front(),all_weights.front(), alt_dirs, alt_weights);
}


/**
 * Removing a vector from the vector field at a given index
 * Does nothing if the model is locked or the index is out of bounds.
 *
 * \param index. The index, where the vector shall be removed.
 */
void SparseWeightedMultiVectorfield2D::removeVector(unsigned int index)
{
    if(locked())
        return;
    
	if (index < m_weights.size() )
    {
        m_weights.erase(m_weights.begin()+index);
        m_alt_weights.erase(m_alt_weights.begin()+index);
        SparseMultiVectorfield2D::removeVector(index);
    }
}

/**
 * The content's item header for the weighted multi vectorfield serialization.
 * 
 * \return Always: "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
 */
QString SparseWeightedMultiVectorfield2D::item_header() const
{
    QString result =  SparseVectorfield2D::item_header() + ", weight";
	
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
        result += QString(", alt%1_dir_x, alt%2_dir_y, alt%3_weight").arg(i).arg(i).arg(i);
	}
	
    return result;
}

/**
 * Serialization of a single weighted multi vector inside the list at a given index.
 * The vector will be serialized by means of comma separated values.
 * 
 * \param index Index of the vector to be serialized.
 * \return QString of the vector, namely "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
 */
QString SparseWeightedMultiVectorfield2D::serialize_item(unsigned int index) const
{
	QString result = SparseVectorfield2D::serialize_item(index) + ", " + QString::number(weight(index), 'g', 10);
    
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
		result += ", " + QString::number(altDirection(index, i).x(), 'g', 10)
                + ", " + QString::number(altDirection(index, i).y(), 'g', 10)
                + ", " + QString::number(altWeight(index, i), 'g', 10);
	}
	return result;
}

/**
 * Deserialization/addition of a weighted multi vector from a string to this list.
 *
 * \param serial A QString containing the serialization of the vector.
 * \return True, if the item could be deserialized and the model is not locked.
 *         The serialization is ordered as: "pos_x, pos_y, dir_x, dir_y, weight, alt0_dir_x, alt0_dir_y, alt0_weight, ... , altN_dir_x, altN_dir_y, altN_weight".
 */
bool SparseWeightedMultiVectorfield2D::deserialize_item(const QString & serial)
{
    //try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 5)
	{
		try
		{
			if(SparseVectorfield2D::deserialize_item(serial))
            {
                m_weights.push_back(values[4].toFloat());
                
                //The parameters should have been deserialized before, so we can use the properties:
                std::vector<PointType> alt_dirs(alternatives());
                std::vector<float> alt_weights(alternatives());
			
                for(unsigned int a=0; a<alternatives() && 5+a*3<(unsigned int)values.size(); ++a)
                {
                    (alt_dirs[a]).setX(values[5+a*3  ].toFloat());
                    (alt_dirs[a]).setY(values[5+a*3+1].toFloat());
                    alt_weights[a]  =  values[5+a*3+2].toFloat();
                }
                m_alt_directions.push_back(alt_dirs);
                m_alt_weights.push_back(alt_weights);
                
                return true;
            }
		}
		catch(...)
		{
			qDebug() << QString("Error while importing SparseWeightedMultiVectorfield2D: ") << serial;
			return false;
		}
	}
	return false;
}

/**
 * This slot is called, whenever some parameter is changed.
 * It rearranges the size of the weight alternatives' vector.
 */
void SparseWeightedMultiVectorfield2D::updateModel()
{
    for( std::vector<float>& vec : m_alt_weights)
    {
        std::vector<float> new_vec(alternatives());
        
        for (unsigned int i=0; i<alternatives(); ++i)
        {
            new_vec[i] = vec[i];
        }
        vec = new_vec;
    }
    SparseMultiVectorfield2D::updateModel();
}
    
} //end of namespace graipe
