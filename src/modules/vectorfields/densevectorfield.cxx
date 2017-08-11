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

#include "vectorfields/densevectorfield.hxx"
#include "core/basicstatistics.hxx"

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *     @file
 *     @brief Implementation file for dense vectorfield classes
 * @}
 */

DenseVectorfield2D::DenseVectorfield2D(Workspace* wsp)
: Vectorfield2D(wsp)
{
}

DenseVectorfield2D::DenseVectorfield2D(const DenseVectorfield2D& vf)
:	Vectorfield2D(vf)
{
	m_u = vf.u();
	m_v = vf.v();
}

DenseVectorfield2D::DenseVectorfield2D(const DiffType& shape, Workspace* wsp)
:   Vectorfield2D(wsp),
	m_u(ArrayType(shape)),
	m_v(ArrayType(shape))
{
	setLeft(0); setRight(shape[0]);
	setTop(0);  setBottom(shape[1]);
}

DenseVectorfield2D::DenseVectorfield2D(int width, int height, Workspace* wsp)
:   Vectorfield2D(wsp),
	m_u(ArrayType(width,height)),
    m_v(ArrayType(width,height))
{
	setLeft(0); setRight(width);
	setTop(0);  setBottom(height);
}

DenseVectorfield2D::DenseVectorfield2D(const ArrayViewType & u, const ArrayViewType & v, Workspace* wsp)
:   Vectorfield2D(wsp),
	m_u(u),
	m_v(v)
{
	setLeft(0); setRight(u.width());
	setTop(0);  setBottom(u.height());
}

unsigned int DenseVectorfield2D::size() const
{
	return (unsigned int)m_u.size();
}

void DenseVectorfield2D::clear()
{
    if(locked())
        return;
    
    //TODO: Look for a way to avoid double updates
    
    //also adjusts the width and height of the member arrays
	updateModel();
    
    if(m_u.size())
        m_u=0;
    
    if(m_v.size())
        m_v=0;
        
    //note about update
	updateModel();
}

bool DenseVectorfield2D::isInside(const DiffType & d)
{
	return m_u.isInside(d);
}

bool DenseVectorfield2D::isInside(unsigned int x, unsigned int y)
{
    return isInside(DiffType(x,y));
}

DenseVectorfield2D::PointType DenseVectorfield2D::origin(unsigned int index) const
{	
	return DenseVectorfield2D::PointType(indexToX(index),indexToY(index));
}

DenseVectorfield2D::PointType DenseVectorfield2D::direction(unsigned int index) const
{	
	return direction(indexToX(index), indexToY(index));
}

DenseVectorfield2D::PointType DenseVectorfield2D::direction(const PointType& orig) const
{
    return direction(orig.x(), orig.y());
}

DenseVectorfield2D::PointType DenseVectorfield2D::direction(unsigned int x, unsigned int y) const
{
    return PointType(m_u(x,y), m_v(x,y));
}

void DenseVectorfield2D::setDirection(unsigned int index, const PointType& new_d)
{
    setDirection(indexToX(index), indexToY(index), new_d);
}

void DenseVectorfield2D::setDirection(const PointType& orig, const PointType& new_d)
{
    setDirection(orig.x(), orig.y(), new_d);
}

void DenseVectorfield2D::setDirection(unsigned int x, unsigned int y, const PointType& new_d)
{
    if(locked())
        return;
    
    m_u(x,y) = new_d.x();
    m_v(x,y) = new_d.y();
    
    updateModel();
}

DenseVectorfield2D::PointType DenseVectorfield2D::localDirection(unsigned int x, unsigned int y) const
{	
	return direction(x,y) - globalDirection(x,y);
}

DenseVectorfield2D::PointType DenseVectorfield2D::globalDirection(unsigned int x, unsigned int y) const
{	
	return globalMotion().map(PointType(x,y)) - PointType(x,y);
}

float DenseVectorfield2D::squaredLength(unsigned int index) const
{
    return squaredLength(indexToX(index), indexToY(index));
}

float DenseVectorfield2D::squaredLength(const PointType& orig) const
{
    return squaredLength(orig.x(), orig.y());
}

float DenseVectorfield2D::squaredLength(unsigned int x, unsigned int y) const
{
    return direction(x,y).squaredLength();
}

float DenseVectorfield2D::length(unsigned int index) const
{
    return length(indexToX(index), indexToY(index));
}

float DenseVectorfield2D::length(const PointType& orig) const
{
    return length(orig.x(), orig.y());
}

float DenseVectorfield2D::length(unsigned int x, unsigned int y) const
{
    return direction(x,y).length();
}

float DenseVectorfield2D::angle(unsigned int index) const
{
    return angle(indexToX(index), indexToY(index));
}

float DenseVectorfield2D::angle(const PointType& orig) const
{
     return angle(orig.x(), orig.y());
}

float DenseVectorfield2D::angle(unsigned int x, unsigned int y) const
{
    return direction(x,y).angle();
}

DenseVectorfield2D::PointType DenseVectorfield2D::target(unsigned int index) const
{
    return target(indexToX(index), indexToY(index));
}

DenseVectorfield2D::PointType DenseVectorfield2D::target(const PointType& orig) const
{
    return target(orig.x(), orig.y());
}

DenseVectorfield2D::PointType DenseVectorfield2D::target(unsigned int x, unsigned int y) const
{
    return PointType(x, y) + direction(x,y);
}

void DenseVectorfield2D::setTarget(unsigned int index, const PointType& new_t)
{
    setTarget(indexToX(index), indexToY(index), new_t);
}

void DenseVectorfield2D::setTarget(const PointType& orig, const PointType& new_t)
{
    setTarget(orig.x(), orig.y(), new_t);
}

void DenseVectorfield2D::setTarget(unsigned int x, unsigned int y, const PointType& new_t)
{
    if(locked())
        return;
    
    m_u(x,y) = new_t.x() - x;
    m_v(x,y) = new_t.y() - y;
    
    updateModel();
}

const DenseVectorfield2D::ArrayViewType& DenseVectorfield2D::u() const
{
	return m_u;
}

void DenseVectorfield2D::setU(const ArrayViewType& new_u)
{
    if(locked() || m_u.shape() != new_u.shape())
        return;
    
	m_u = new_u;
}

const DenseVectorfield2D::ArrayViewType& DenseVectorfield2D::v() const
{
	return m_v;
}

void DenseVectorfield2D::setV(const ArrayViewType& new_v)
{
    if(locked() || m_v.shape() != new_v.shape())
        return;
    
	m_v = new_v;
}

void DenseVectorfield2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
    try
    {
      
        qint64 channel_size = m_u.width()*m_u.height()*sizeof(ArrayType::value_type);

        QByteArray block((const char*)m_u.data(),channel_size);
            
        xmlWriter.writeStartElement("Channel");
        xmlWriter.writeAttribute("ID", "u");
        xmlWriter.writeAttribute("Encoding", "Base64");
            xmlWriter.writeCharacters(block.toBase64());
        xmlWriter.writeEndElement();
        
        block = QByteArray((const char*)m_v.data(),channel_size);
            
        xmlWriter.writeStartElement("Channel");
        xmlWriter.writeAttribute("ID", "v");
        xmlWriter.writeAttribute("Encoding", "Base64");
            xmlWriter.writeCharacters(block.toBase64());
        xmlWriter.writeEndElement();
    }
    catch(...)
    {
        qCritical() << "DenseVectorfield2D::serialize_content failed!";
    }
}

bool DenseVectorfield2D::deserialize_content(QXmlStreamReader& xmlReader)
{
    if(width() == 0 || height()==0)
    {
        qCritical("DenseVectorfield2D::deserialize_content: storage image has zero size!");
        return false;
    }
    
    m_u.reshape(DiffType(width(), height()));
    m_v.reshape(DiffType(width(), height()));
    
    qint64 channel_size = m_u.width()*m_u.height()*sizeof(ArrayType::value_type);
    
    try
    {
        for (int i=0; i!=2; ++i)
        {
            xmlReader.readNextStartElement();
            
            //qDebug() << "DenseVectorfield2D::deserialize_content: readNextStartElement" << xmlReader.name();
            
            if(xmlReader.name() == "Channel"
                && xmlReader.attributes().hasAttribute("ID")
                && xmlReader.attributes().hasAttribute("Encoding")
                && xmlReader.attributes().value("Encoding") == "Base64")
            {
                QString id = xmlReader.attributes().value("ID").toString();
                
                QByteArray block;
                block.append(xmlReader.readElementText());
                block = QByteArray::fromBase64(block);
                
                if (id  == "u")
                {
                    if(block.size() == channel_size)
                    {
                        memcpy((char*)m_u.data(), block.data(), channel_size);
                    }
                    else
                    {
                        throw std::runtime_error("Channel serialization was of wrong size in XML after Base64 decoding for u field.");
                    }
                }
                else if (id  == "v")
                {
                    if(block.size() == channel_size)
                    {
                        memcpy((char*)m_v.data(), block.data(), channel_size);
                    }
                    else
                    {
                        throw std::runtime_error("Channel serialization was of wrong size in XML after Base64 decoding for v field.");
                    }
                }
            
            }
            else
            {
                throw std::runtime_error("Did not find a correct channel element inXML tree");
            }
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "DenseVectorfield2D::deserialize_content failed! Error: " << e.what();
        return false;
    }
    return true;
}

void DenseVectorfield2D::updateModel()
{
    if(   (width() !=0 && (unsigned int)m_u.width() != width())
       || (height()!=0 && (unsigned int)m_u.height()!=height()))
    {
        m_u.reshape(DiffType(width(), height()));
        m_v.reshape(DiffType(width(), height()));
    }
    
    Model::updateModel();
}

unsigned int DenseVectorfield2D::xyToIdx(unsigned int x, unsigned int y) const
{
	return (unsigned int)(y*m_u.width()+x);
}

unsigned int DenseVectorfield2D::indexToX(unsigned int index) const
{
	return index % m_u.width();
}

unsigned int DenseVectorfield2D::indexToY(unsigned int index) const
{
	return (unsigned int)(index / m_u.width());
}







DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(Workspace* wsp)
: DenseVectorfield2D(wsp)
{
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DenseWeightedVectorfield2D & vf)
:	DenseVectorfield2D(vf)
{
	m_w = vf.w();
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DenseVectorfield2D & vf)
:	DenseVectorfield2D(vf),
	m_w(ArrayType(vf.u().shape()))
{
	m_w = 0;
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DiffType& shape, Workspace* wsp)
:	DenseVectorfield2D(shape, wsp),
	m_w(ArrayType(shape))
{
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(int width, int height, Workspace* wsp)
:	DenseVectorfield2D(width, height, wsp),
	m_w(ArrayType(width, height))
{
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, Workspace* wsp)
:	DenseVectorfield2D(u, v, wsp),
	m_w(ArrayType(u.width(), u.height()))
{	
}

DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, const ArrayViewType& w, Workspace* wsp)
:	DenseVectorfield2D(u, v, wsp),
	m_w(w)
{	
}

void DenseWeightedVectorfield2D::clear()
{
    if(locked())
        return;
    
    //also adjusts the width and height of the member arrays
	DenseVectorfield2D::clear();
    
    if(m_w.size() != 0 && m_w.size() != 0)
    {
        m_u=0;
    }
    
    //note about update
	updateModel();
}

float DenseWeightedVectorfield2D::weight(unsigned int index) const
{
	return m_w(indexToX(index), indexToY(index));
}

float DenseWeightedVectorfield2D::weight(const PointType& orig) const
{
    return weight(orig.x(), orig.y());
}

float DenseWeightedVectorfield2D::weight(unsigned int x, unsigned int y) const
{
	return m_w(x,y);
}

void DenseWeightedVectorfield2D::setWeight(unsigned int index, float new_w)
{
	setWeight(indexToX(index), indexToY(index), new_w);
}

void DenseWeightedVectorfield2D::setWeight(const PointType& orig, float new_w)
{
    setWeight(orig.x(), orig.y(), new_w);
}

void DenseWeightedVectorfield2D::setWeight(unsigned int x, unsigned int y, float new_w)
{
    if(locked())
        return;
    
	m_w(x,y) = new_w;
	updateModel();
}

void DenseWeightedVectorfield2D::serialize_content(QXmlStreamWriter& xmlWriter) const
{
    DenseVectorfield2D::serialize_content(xmlWriter);
    
    try
    {
        qint64 channel_size = m_w.width()*m_w.height()*sizeof(ArrayType::value_type);

        QByteArray block((const char*)m_w.data(),channel_size);
            
        xmlWriter.writeStartElement("Channel");
        xmlWriter.writeAttribute("ID", "w");
        xmlWriter.writeAttribute("Encoding", "Base64");
            xmlWriter.writeCharacters(block.toBase64());
        xmlWriter.writeEndElement();
    }
    catch(...)
    {
        qCritical() << "DenseWeightedVectorfield2D::serialize_content failed!";
    }
}

bool DenseWeightedVectorfield2D::deserialize_content(QXmlStreamReader& xmlReader)
{
    if( !DenseVectorfield2D::deserialize_content(xmlReader))
    {
        return false;
    }
    
    qint64 channel_size = m_u.width()*m_u.height()*sizeof(ArrayType::value_type);
    
    m_w.reshape(DiffType(width(), height()));
    
    if(width() == 0 || height()==0)
    {
        qCritical("DenseWeightedVectorfield2D::deserialize_content: storage image has zero size!");
        return false;
    }
    
    try
    {
        xmlReader.readNextStartElement();
            
        //qDebug() << "DenseVectorfield2D::deserialize_content: readNextStartElement" << xmlReader.name();
            
        if(xmlReader.name() == "Channel"
            && xmlReader.attributes().hasAttribute("ID")
            && xmlReader.attributes().value("ID") == "w"
            && xmlReader.attributes().hasAttribute("Encoding")
            && xmlReader.attributes().value("Encoding") == "Base64")
        {
            QByteArray block;
            block.append(xmlReader.readElementText());
            block = QByteArray::fromBase64(block);
            
            if(block.size() == channel_size)
            {
                memcpy((char*)m_w.data(), block.data(), channel_size);
            }
            else
            {
                throw std::runtime_error("Channel serialization was of wrong size in XML after Base64 decoding for w field.");
            }
        }
        else
        {
            throw std::runtime_error("Did not find a correct channel element inXML tree");
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "DenseWeightedVectorfield2D::deserialize_content failed! Error: " << e.what();
        return false;
    }
    return true;
}

const DenseWeightedVectorfield2D::ArrayViewType & DenseWeightedVectorfield2D::w() const
{
	return m_w;
}

void DenseWeightedVectorfield2D::setW(const ArrayViewType& new_w)
{
    if(locked() || m_w.shape() != new_w.shape())
        return;
    
	m_w = new_w;
}

void DenseWeightedVectorfield2D::updateModel()
{
    if(   (width() !=0 && (unsigned int)m_u.width() != width())
       || (height()!=0 && (unsigned int)m_u.height()!=height()))
    {
        m_w.reshape(DiffType(width(), height()));
    }
    
    DenseVectorfield2D::updateModel();
}

} //end of namespace graipe
