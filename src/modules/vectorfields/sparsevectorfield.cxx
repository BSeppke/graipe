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

#include "vectorfields/sparsevectorfield.hxx"
#include "core/basicstatistics.hxx"

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *     @file
 *     @brief Implementation file for sparse vectorfield classes
 * @}
 */

SparseVectorfield2D::SparseVectorfield2D(Workspace* wsp)
:	Vectorfield2D(wsp)
{
}

SparseVectorfield2D::SparseVectorfield2D(const SparseVectorfield2D & vf)
:	Vectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		addVector(vf.origin(i),vf.direction(i));
	}
}

unsigned int SparseVectorfield2D::size() const
{
	return (unsigned int)m_origins.size();
}

void SparseVectorfield2D::clear()
{
    if(locked())
        return;
    
	m_origins.clear();
	m_directions.clear();
	
	updateModel();
}

SparseVectorfield2D::PointType SparseVectorfield2D::origin(unsigned int index) const
{	
	return m_origins[index];			
}

void SparseVectorfield2D::setOrigin(unsigned int index, const PointType& new_p)
{	
    if(locked())
        return;
    
	m_origins[index] = new_p;
	updateModel();	
}

SparseVectorfield2D::PointType SparseVectorfield2D::direction(unsigned int index) const
{	
	return m_directions[index];	
}

void SparseVectorfield2D::setDirection(unsigned int index, const PointType& new_p)
{
    if(locked())
        return;
    
	m_directions[index] = new_p;
	updateModel();
}

void SparseVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{		
    if(locked())
        return;
    
	m_origins.push_back(orig); 
	m_directions.push_back(dir);
	updateModel();
}

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

QString SparseVectorfield2D::csvHeader() const
{
	return "pos_x, pos_y, dir_x, dir_y";
}

QString SparseVectorfield2D::itemToCSV(unsigned int index) const
{
	return   QString::number(m_origins[index].x(), 'g', 10)    + ", "
           + QString::number(m_origins[index].y(), 'g', 10)    + ", "
           + QString::number(m_directions[index].x(), 'g', 10) + ", "
           + QString::number(m_directions[index].y(), 'g', 10);
}

bool SparseVectorfield2D::itemFromCSV(const QString & serial)
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

void SparseVectorfield2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.writeTextElement("x", QString::number(m_origins[index].x(), 'g', 10));
    xmlWriter.writeTextElement("y",  QString::number(m_origins[index].y(), 'g', 10));
    xmlWriter.writeTextElement("u", QString::number(m_directions[index].x(), 'g', 10));
    xmlWriter.writeTextElement("v", QString::number(m_directions[index].y(), 'g', 10));
}

bool SparseVectorfield2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if(locked())
        return false;
    
    PointType ori;
    PointType dir;
    
    for(int i=0; i!=4; i++)
    {
        if(xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == "x")
            {
                ori.setX(xmlReader.readElementText().toFloat());
            }
            if(xmlReader.name() == "y")
            {
                ori.setY(xmlReader.readElementText().toFloat());
            }
            if(xmlReader.name() == "u")
            {
                dir.setX(xmlReader.readElementText().toFloat());
            }
            if(xmlReader.name() == "v")
            {
                dir.setY(xmlReader.readElementText().toFloat());
            }
        }
        else
        {
            return false;
        }
    }
    m_origins.push_back(ori);
    m_directions.push_back(dir);
    
    return true;
}

void SparseVectorfield2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
	for(unsigned int i=0; i < size(); ++i)
    {
        xmlWriter.writeStartElement("Vector2D");
        xmlWriter.writeAttribute("ID", QString::number(i));
            serialize_item(i, xmlWriter);
        xmlWriter.writeEndElement();
    }
}

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
        if(xmlReader.name() == "Vector2D")
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





















SparseWeightedVectorfield2D::SparseWeightedVectorfield2D(Workspace* wsp)
: SparseVectorfield2D(wsp)
{
}

SparseWeightedVectorfield2D::SparseWeightedVectorfield2D(const SparseWeightedVectorfield2D & vf)
:	SparseVectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		m_weights.push_back(vf.weight(i));
	}
}

void SparseWeightedVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_weights.clear();
    SparseVectorfield2D::clear();
}

float SparseWeightedVectorfield2D::weight(unsigned int index) const
{	
	return m_weights[index];	
}

void SparseWeightedVectorfield2D::setWeight(unsigned int index, float new_w)
{
	m_weights[index]= new_w;	
	
	updateModel();
}

void SparseWeightedVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
    addVector(orig, dir, 0);
}

void SparseWeightedVectorfield2D::addVector(const PointType& orig, const PointType& dir, float w)
{
    if (locked())
        return;
    
    m_weights.push_back(w);
    SparseVectorfield2D::addVector(orig, dir);
}

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

QString SparseWeightedVectorfield2D::csvHeader() const
{
	return SparseVectorfield2D::csvHeader() + ", weight";
}

QString SparseWeightedVectorfield2D::itemToCSV(unsigned int index) const
{
	return SparseVectorfield2D::itemToCSV(index) + ", "
           + QString::number(m_weights[index], 'g', 10);
}

bool SparseWeightedVectorfield2D::itemFromCSV(const QString & serial)
{
    if (locked())
        return false;
    
	//try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 5)
	{
		try
		{
			SparseVectorfield2D::itemFromCSV(serial);
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

void SparseWeightedVectorfield2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.writeTextElement("w", QString::number(m_weights[index], 'g', 10));
}

bool SparseWeightedVectorfield2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;
    
    if(!SparseVectorfield2D::deserialize_item(xmlReader))
    {
        return false;
    }
    
    if(xmlReader.readNextStartElement())
    {
        if(xmlReader.name() == "w")
        {
            m_weights.push_back(xmlReader.readElementText().toFloat());
            return true;
        }
    }
    return false;
}



		
		
		
		








		
		


SparseMultiVectorfield2D::SparseMultiVectorfield2D(Workspace* wsp)
:	SparseVectorfield2D(wsp),
    m_alternatives(new IntParameter("number of alternative directions",0,1000,10))
{
    m_parameters->addParameter("alternatives", m_alternatives);
}

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

void SparseMultiVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_alt_directions.clear();
    SparseVectorfield2D::clear();
}

unsigned int SparseMultiVectorfield2D::alternatives() const
{
	return m_alternatives->value();
}

void SparseMultiVectorfield2D::setAlternatives(unsigned int alternatives)
{
    if(locked())
        return;
    
    updateModel();
}

SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altDirection(unsigned int index, unsigned int alt_index) const
{
	return (m_alt_directions[index])[alt_index];
}

void SparseMultiVectorfield2D::setAltDirection(unsigned int index, unsigned int alt_index, const PointType& new_d)
{
    if(locked())
        return;
    
	(m_alt_directions[index])[alt_index] = new_d;
	updateModel();
}

SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altLocalDirection(unsigned int index, unsigned int alt_index) const
{	
	return altDirection(index, alt_index) - altGlobalDirection(index, alt_index);
}

SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altGlobalDirection(unsigned int index, unsigned int alt_index) const
{	
	return globalMotion().map(origin(index)) - origin(index);
}

float SparseMultiVectorfield2D::altSquaredLength(unsigned int index, unsigned int alt_index) const
{
    return altDirection(index, alt_index).squaredLength();
}

float SparseMultiVectorfield2D::altLength(unsigned int index, unsigned int alt_index) const
{
    return altDirection(index, alt_index).length();
}

float SparseMultiVectorfield2D::altAngle(unsigned int index, unsigned int alt_index) const
{
	return altDirection(index, alt_index).angle();
}

SparseMultiVectorfield2D::PointType SparseMultiVectorfield2D::altTarget(unsigned int index, unsigned int alt_index) const
{	
	return origin(index) + altDirection(index, alt_index);
}

void SparseMultiVectorfield2D::setAltTarget(unsigned int index, unsigned int alt_index, const PointType& new_t)
{	
	setAltDirection(index, alt_index, new_t - origin(index));
}

void SparseMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
	if(locked())
        return;
    
    addVector(orig, dir, std::vector<PointType>(alternatives()));
}

void SparseMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, const std::vector<PointType>& alt_dirs)
{
	if(locked())
        return;
    
    std::vector<PointType> new_vec(alternatives());
    
    unsigned int min_count = std::min(alternatives(), (unsigned int)alt_dirs.size());
    
    for (unsigned int alt_index=0; alt_index<min_count; ++alt_index)
    {
        new_vec[alt_index] = alt_dirs[alt_index];
    }
    m_alt_directions.push_back(new_vec);
    
    SparseVectorfield2D::addVector(orig,dir);
}

void SparseMultiVectorfield2D::addVector(const PointType& orig, const std::vector<PointType>& all_dirs)
{
	if(locked())
        return;
    
    Q_ASSERT(all_dirs.size() > 0);
    
    std::vector<PointType> alt_dirs = all_dirs;
    alt_dirs.erase(alt_dirs.begin());
    
    addVector(orig, all_dirs.front(), alt_dirs);
}

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

QString SparseMultiVectorfield2D::csvHeader() const
{
    QString result =  SparseVectorfield2D::csvHeader();
	
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
			result += QString(", alt%1_dir_x, alt%2_dir_y").arg(i).arg(i);
	}
	
    return result;
}

QString SparseMultiVectorfield2D::itemToCSV(unsigned int index) const
{
	QString result = SparseVectorfield2D::itemToCSV(index);
    
	for( unsigned int i = 1; i<=alternatives(); ++i)
	{
        const PointType & dir = altDirection(index, i);
		result += ", " + QString::number(dir.x(), 'g', 10) + ", " + QString::number(dir.y(), 'g', 10) ;
	}
	return result;
}

bool SparseMultiVectorfield2D::itemFromCSV(const QString & serial)
{
    //try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 4)
	{
		try
		{
			if(SparseVectorfield2D::itemFromCSV(serial))
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

void SparseMultiVectorfield2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    SparseVectorfield2D::serialize_item(index, xmlWriter);
    
    xmlWriter.writeTextElement("altDirections", QString::number(alternatives()));
    
    for( unsigned int i = 1; i<=alternatives(); ++i)
	{
        const PointType & dir = altDirection(index, i);
        
        xmlWriter.writeStartElement("altDirection");
            xmlWriter.writeAttribute("ID", QString::number(i));
        
            xmlWriter.writeTextElement("u", QString::number(dir.x(), 'g', 10));
            xmlWriter.writeTextElement("v", QString::number(dir.y(), 'g', 10));
        xmlWriter.writeEndElement();
	}
}

bool SparseMultiVectorfield2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;
    
    if(!SparseVectorfield2D::deserialize_item(xmlReader))
    {
        return false;
    }

    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "altDirections"
        &&  xmlReader.readElementText().toInt() == alternatives())
    {
        //The parameters should have been deserialized before, so we can use the properties:
        std::vector<PointType> alt_dirs(alternatives());
        
        for( unsigned int i = 1; i<=alternatives(); ++i)
        {
            if(     xmlReader.readNextStartElement()
                &&  xmlReader.name() =="altDirection"
                &&  xmlReader.attributes().hasAttribute("ID")
                &&  xmlReader.attributes().value("ID").toInt() == i)
            {
                PointType dir;
        
                for(int j=0; j!=2; ++j)
                {
                    if(xmlReader.readNextStartElement())
                    {
                        if(xmlReader.name() =="u")
                        {
                            dir.setX(xmlReader.readElementText().toFloat());
                        }
                        if(xmlReader.name() =="v")
                        {
                            dir.setY(xmlReader.readElementText().toFloat());
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                alt_dirs.push_back(dir);
            }
            else
            {
                return false;
            }
        }
        
        m_alt_directions.push_back(alt_dirs);
        return true;
    }
    else
    {
        return false;
    }
}

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
		
		









		


SparseWeightedMultiVectorfield2D::SparseWeightedMultiVectorfield2D(Workspace* wsp)
: SparseMultiVectorfield2D(wsp)
{
}

SparseWeightedMultiVectorfield2D::SparseWeightedMultiVectorfield2D(const SparseWeightedMultiVectorfield2D & vf)
:	SparseMultiVectorfield2D(vf)
{
	for( unsigned int i=0; i < vf.size(); ++i)
	{
		m_weights.push_back(vf.weight(i));
        m_alt_weights.push_back(std::vector<float>(vf.alternatives()));
	}
}

void SparseWeightedMultiVectorfield2D::clear()
{
    if (locked())
        return;
    
    m_weights.clear();
    m_alt_weights.clear();
    SparseMultiVectorfield2D::clear();
}

float SparseWeightedMultiVectorfield2D::weight(unsigned int index) const
{
	return m_weights[index];
}

float SparseWeightedMultiVectorfield2D::altWeight(unsigned int index, unsigned int alt_index) const
{
	return (m_alt_weights[index])[alt_index];
}

void SparseWeightedMultiVectorfield2D::setWeight(unsigned int index,  float weight)
{
    if(locked())
        return;
        
	m_weights[index] = weight;
	updateModel();
}

void SparseWeightedMultiVectorfield2D::setAltWeight(unsigned int index,  unsigned int alt_index, float weight)
{
    if(locked())
        return;
        
	(m_alt_weights[index])[alt_index] = weight;
	updateModel();
}

void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir)
{
    if(locked())
        return;
    
    addVector(orig, dir, 0.0, std::vector<PointType>(alternatives()), std::vector<float>(alternatives()));
}

void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, float weight)
{
    if(locked())
        return;
    
    addVector(orig, dir, weight, std::vector<PointType>(alternatives()), std::vector<float>(alternatives()));
}

void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, const std::vector<PointType>& alt_dirs)
{
    if(locked())
        return;
    
    addVector(orig, dir, 0.0, alt_dirs, std::vector<float>(alternatives()));

}

void SparseWeightedMultiVectorfield2D::addVector(const PointType& orig, const PointType& dir, float weight, const std::vector<PointType>& alt_dirs)
{
    if(locked())
        return;
    
    addVector(orig, dir, weight, alt_dirs, std::vector<float>(alternatives()));
}

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

QString SparseWeightedMultiVectorfield2D::csvHeader() const
{
    QString result =  SparseVectorfield2D::csvHeader() + ", weight";
	
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
        result += QString(", alt%1_dir_x, alt%2_dir_y, alt%3_weight").arg(i).arg(i).arg(i);
	}
	
    return result;
}

QString SparseWeightedMultiVectorfield2D::itemToCSV(unsigned int index) const
{
	QString result = SparseVectorfield2D::itemToCSV(index) + ", " + QString::number(weight(index), 'g', 10);
    
	for( unsigned int i = 0; i<alternatives(); ++i)
	{
		result += ", " + QString::number(altDirection(index, i).x(), 'g', 10)
                + ", " + QString::number(altDirection(index, i).y(), 'g', 10)
                + ", " + QString::number(altWeight(index, i), 'g', 10);
	}
	return result;
}

bool SparseWeightedMultiVectorfield2D::itemFromCSV(const QString & serial)
{
    //try to split content into data entries
	QStringList values = serial.split(", ");
	if(values.size() >= 5)
	{
		try
		{
			if(SparseVectorfield2D::itemFromCSV(serial))
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

void SparseWeightedMultiVectorfield2D::serialize_item(unsigned int index, QXmlStreamWriter& xmlWriter) const
{
    SparseVectorfield2D::serialize_item(index, xmlWriter);
    
    xmlWriter.writeTextElement("w", QString::number(weight(index), 'g', 10));
    xmlWriter.writeTextElement("altDirections", QString::number(alternatives()));
    
    for(unsigned int i = 1; i<=alternatives(); ++i)
	{
        const PointType & dir = altDirection(index, i);
        
        xmlWriter.writeStartElement("altDirection");
            xmlWriter.writeAttribute("ID", QString::number(i));
        
            xmlWriter.writeTextElement("u", QString::number(dir.x(), 'g', 10));
            xmlWriter.writeTextElement("v", QString::number(dir.y(), 'g', 10));
            xmlWriter.writeTextElement("w", QString::number(m_alt_weights[index][i-1], 'g', 10));
        xmlWriter.writeEndElement();
	}
}

bool SparseWeightedMultiVectorfield2D::deserialize_item(QXmlStreamReader& xmlReader)
{
    if (locked())
        return false;
    
    if(!SparseVectorfield2D::deserialize_item(xmlReader))
    {
        return false;
    }

    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "w")
    {
        m_weights.push_back(xmlReader.readElementText().toFloat());
    }
    else
    {
        return false;
    }
    
    if(     xmlReader.readNextStartElement()
        &&  xmlReader.name() == "altDirections"
        &&  xmlReader.readElementText().toInt() == alternatives())
    {
        //The parameters should have been deserialized before, so we can use the properties:
        std::vector<PointType> alt_dirs(alternatives());
        std::vector<float> alt_weights(alternatives());
        
        for( unsigned int i = 1; i<=alternatives(); ++i)
        {
            if(     xmlReader.readNextStartElement()
                &&  xmlReader.name() =="altDirection"
                &&  xmlReader.attributes().hasAttribute("ID")
                &&  xmlReader.attributes().value("ID").toInt() == i)
            {
                PointType dir;
                float w;
                
                for(int j=0; j!=3; ++j)
                {
                    if(xmlReader.readNextStartElement())
                    {
                        if(xmlReader.name() =="u")
                        {
                            dir.setX(xmlReader.readElementText().toFloat());
                        }
                        if(xmlReader.name() =="v")
                        {
                            dir.setY(xmlReader.readElementText().toFloat());
                        }
                        if(xmlReader.name() =="w")
                        {
                            w = xmlReader.readElementText().toFloat();
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                alt_dirs.push_back(dir);
                alt_weights.push_back(w);
            }
            else
            {
                return false;
            }
        }
        
        m_alt_directions.push_back(alt_dirs);
        m_alt_weights.push_back(alt_weights);
        return true;
    }
    else
    {
        return false;
    }
}

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
