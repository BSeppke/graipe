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

#include "vectorfields/densevectorfield.hxx"
#include "core/basicstatistics.hxx"

namespace graipe {
   
/**
 * Default constructor. Creates an empty dense vectorfield.
 */
DenseVectorfield2D::DenseVectorfield2D(Environment* env)
: Vectorfield2D(env)
{
}

/**
 * Copy constructor. Creates a dense vectorfield from another one.
 *
 * \param vf The other dense vectorfield.
 */
DenseVectorfield2D::DenseVectorfield2D(const DenseVectorfield2D& vf)
:	Vectorfield2D(vf)
{
	m_u = vf.u();
	m_v = vf.v();
}

/**
 * Create an empty dense vectorfield of a given 2D shape .
 *
 * \param shape The 2D shape, which, contains (width, height).
 */
DenseVectorfield2D::DenseVectorfield2D(const DiffType& shape, Environment* env)
:   Vectorfield2D(env),
	m_u(ArrayType(shape)),
	m_v(ArrayType(shape))
{
	setLeft(0); setRight(shape[0]);
	setTop(0);  setBottom(shape[1]);
}

/**
 * Create an empty dense vectorfield of a given 2D shape.
 *
 * \param width The width of the new dense vectorfield.
 * \param height The height of the new dense vectorfield.
 */
DenseVectorfield2D::DenseVectorfield2D(int width, int height, Environment* env)
:   Vectorfield2D(env),
	m_u(ArrayType(width,height)),
    m_v(ArrayType(width,height))
{
	setLeft(0); setRight(width);
	setTop(0);  setBottom(height);
}

/**
 * Create a dense vectorfield from two arrays, which need to contain.
 * the vectors direction in x- (u) and y- (v) direction. All data will
 * be copied.
 *
 * \param u The x-directions of the vectors.
 * \param v The y-directions of the vectors.
 */
DenseVectorfield2D::DenseVectorfield2D(const ArrayViewType & u, const ArrayViewType & v, Environment* env)
:   Vectorfield2D(env),
	m_u(u),
	m_v(v)
{
	setLeft(0); setRight(u.width());
	setTop(0);  setBottom(u.height());
}

/**
 * The size of this vectorfield. 
 * Implemented here, defined as pure virtual in base class.
 * 
 * \return The number of vectors in this vectorfield (width x height).
 */
unsigned int DenseVectorfield2D::size() const
{
	return (unsigned int)m_u.size();
}

/**
 * This does not remove all the vectors, but resets their directions to zero.
 * Does nothing if the model is locked.
 */
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

/**
 * Checks if a position is inside this vectorfield
 *
 * \param d The position to be checked.
 */
bool DenseVectorfield2D::isInside(const DiffType & d)
{
	return m_u.isInside(d);
}
    

/**
 * Checks if a position is inside this vectorfield
 *
 * \param x The x-component of the position.
 * \param y The y-component of the position.
 */
bool DenseVectorfield2D::isInside(unsigned int x, unsigned int y)
{
    return isInside(DiffType(x,y));
}

/**
 * The origin/position of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * May throw an error, if the index is out of bounds.
 *
 * ATTENTION: Since the origin is not naturally defined for dense vectorfields
 *            one should not use this function, if possible.
 * 
 * \param index The index of the vector.
 * \return The origin of the vector at the given index.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::origin(unsigned int index) const
{	
	return DenseVectorfield2D::PointType(indexToX(index),indexToY(index));
}

/**
 * The direction of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * May throw an error, if the index is out of bounds.
 *
 * ATTENTION: Since neither PointTypes are not stored inside the arrays
 *            nor are 2D indexes used, use this function with care.
 * 
 * \param index The index of the vector.
 * \return The direction of the vector at the given index.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::direction(unsigned int index) const
{	
	return direction(indexToX(index), indexToY(index));
}
       
/**
 * The direction of a vector at a given position in this vectorfield.
 * May throw an error, if the position is out of bounds.
 *
 * ATTENTION: Since no PointTypes are not stored inside the arrays,
 *            use this function with care.
 * 
 * \param orig The position of the vector.
 * \return The direction of the vector at the given position.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::direction(const PointType& orig) const
{
    return direction(orig.x(), orig.y());
}
/**
 * The direction of a vector at a given position in this vectorfield.
 * May throw an error, if the position is out of bounds.
 *
 * ATTENTION: Since no PointTypes are not stored inside the arrays,
 *            use this function with care.
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The direction of the vector at the given position.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::direction(unsigned int x, unsigned int y) const
{
    return PointType(m_u(x,y), m_v(x,y));
}

/**
 * Set the direction of a vector at a given index in this vectorfield.
 * Implemented here, defined as pure virtual in base class.
 * Does nothing if the model is locked.
 *
 * ATTENTION: Since the arrays are 2D indexed, prefer using the alternative
 *            function.
 * 
 * \param index The index of the new direction.
 * \param new_d The new direction of the vector.
 */
void DenseVectorfield2D::setDirection(unsigned int index, const PointType& new_d)
{
    setDirection(indexToX(index), indexToY(index), new_d);
}

/**
 * Set the direction of a vector at a given position in this vectorfield.
 * Does nothing if the model is locked.
 * 
 * \param orig The position of the vector.
 * \param new_d The new direction of the vector.
 */
void DenseVectorfield2D::setDirection(const PointType& orig, const PointType& new_d)
{
    setDirection(orig.x(), orig.y(), new_d);
}

/**
 * Set the direction of a vector at a given position in this vectorfield.
 * Does nothing if the model is locked.
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \param new_d The new direction of the vector.
 */
void DenseVectorfield2D::setDirection(unsigned int x, unsigned int y, const PointType& new_d)
{
    if(locked())
        return;
    
    m_u(x,y) = new_d.x();
    m_v(x,y) = new_d.y();
    
    updateModel();
}
    
/**
 * The local direction of a vector at a given position w.r.t. the global
 * transformation matrix given in globalMotion().
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The local direction of the vector at the given index.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::localDirection(unsigned int x, unsigned int y) const
{	
	return direction(x,y) - globalDirection(x,y);
}

/**
 * The local direction of a vector at a given position w.r.t. the global
 * transformation matrix given in globalMotion().
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The local direction of the vector at the given index.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::globalDirection(unsigned int x, unsigned int y) const
{	
	return globalMotion().map(PointType(x,y)) - PointType(x,y);
}

    
/**
 * The squared length of a vector at a given index in this vectorfield.
 * 
 * \param index The index of the vector.
 * \return The squared length of the vector at the given index.
 */
float DenseVectorfield2D::squaredLength(unsigned int index) const
{
    return squaredLength(indexToX(index), indexToY(index));
}

/**
 * The squared length of a vector at a given position in this vectorfield.
 * 
 * \param orig The position of the vector.
 * \return The squared length of the vector at the given index.
 */
float DenseVectorfield2D::squaredLength(const PointType& orig) const
{
    return squaredLength(orig.x(), orig.y());
}

/**
 * The squared length of a vector at a given position in this vectorfield.
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The squared length of the vector.
 */
float DenseVectorfield2D::squaredLength(unsigned int x, unsigned int y) const
{
    return direction(x,y).squaredLength();
}

/**
 * The length of a vector at a given index in this vectorfield.
 * 
 * \param index The index of the vector.
 * \return The length of the vector.
 */
float DenseVectorfield2D::length(unsigned int index) const
{
    return length(indexToX(index), indexToY(index));
}

/**
 * The length of a vector at a given position in this vectorfield.
 * 
 * \param orig The position of the vector.
 * \return The length of the vector.
 */
float DenseVectorfield2D::length(const PointType& orig) const
{
    return length(orig.x(), orig.y());
}

/**
 * The length of a vector at a given position in this vectorfield.
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The length of the vector.
 */
float DenseVectorfield2D::length(unsigned int x, unsigned int y) const
{
    return direction(x,y).length();
}

/**
 * The angle of a vector at a given position in this vectorfield.
 * The result will be in degrees. 
 * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The angle of the vector.
 */
float DenseVectorfield2D::angle(unsigned int index) const
{
    return angle(indexToX(index), indexToY(index));
}

/**
 * The angle of a vector at a given position in this vectorfield.
 * The result will be in degrees. 
 * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
 * 
 * \param orig The position of the vector.
 * \return The angle of the vector.
 */
float DenseVectorfield2D::angle(const PointType& orig) const
{
     return angle(orig.x(), orig.y());
}

/**
 * The angle of a vector at a given position in this vectorfield.
 * The result will be in degrees. 
 * (0 = 3h, 90 = 6h, 180=9h, 270=12h).
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The angle of the vector at the given index.
 */
float DenseVectorfield2D::angle(unsigned int x, unsigned int y) const
{
    return direction(x,y).angle();
}


/**
 * The target of a vector at a given index in this vectorfield.
 * Mainly origin(index) + direction(index).
 * 
 * \param index The index of the vector.
 * \return The direction of the vector at the given index.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::target(unsigned int index) const
{
    return target(indexToX(index), indexToY(index));
}

/**
 * The target of a vector at a given position in this vectorfield.
 * Mainly origin(orig) + direction(orig).
 * 
 * \param orig The position of the vector.
 * \return The direction of the vector.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::target(const PointType& orig) const
{
    return target(orig.x(), orig.y());
}

/**
 * The target of a vector at a given position in this vectorfield.
 * Mainly origin(x,y) + direction(x,y).
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The target of the vector.
 */
DenseVectorfield2D::PointType DenseVectorfield2D::target(unsigned int x, unsigned int y) const
{
    return PointType(x, y) + direction(x,y);
}

/**
 * Sets the target of a vector at a given index in this vectorfield.
 * to a point. Does nothing if the model is locked.
 * 
 * \param index The index of the vector.
 * \param new_t The new target of the vector.
 */
void DenseVectorfield2D::setTarget(unsigned int index, const PointType& new_t)
{
    setTarget(indexToX(index), indexToY(index), new_t);
}

/**
 * Sets the target of a vector at a given position in this vectorfield.
 * to a point. Does nothing if the model is locked.
 * 
 * \param orig The position of the vector.
 * \param new_t The new target of the vector.
 */
void DenseVectorfield2D::setTarget(const PointType& orig, const PointType& new_t)
{
    setTarget(orig.x(), orig.y(), new_t);
}

/**
 * Sets the target of a vector at a given position in this vectorfield.
 * to a point. Does nothing if the model is locked.
 * 
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \param new_t The new target of the vector.
 */
void DenseVectorfield2D::setTarget(unsigned int x, unsigned int y, const PointType& new_t)
{
    if(locked())
        return;
    
    m_u(x,y) = new_t.x() - x;
    m_v(x,y) = new_t.y() - y;
    
    updateModel();
}

/**
 * Constant reading access to the x-component of the direction
 * vector. Use this access method for the best performance.
 *
 * \return A constant view on the array data for the x-direction.
 */
const DenseVectorfield2D::ArrayViewType& DenseVectorfield2D::u() const
{
	return m_u;
}

/**
 * Constant reading access to the x-component of the direction vector.
 * Does nothing if the model is locked or shapes mismatch.
 * Use this access method for the best performance.
 *
 * \return new_u A constant view on the new array data for the x-direction.
 */
void DenseVectorfield2D::setU(const ArrayViewType& new_u)
{
    if(locked() || m_u.shape() != new_u.shape())
        return;
    
	m_u = new_u;
}

/**
 * Constant reading access to the y-component of the direction
 * vector. Use this access method for the best performance.
 *
 * \return A constant view on the array data for the y-direction.
 */
const DenseVectorfield2D::ArrayViewType& DenseVectorfield2D::v() const
{
	return m_v;
}

/**
 * Setter for the y-component of the direction vector.
 * Does nothing if the model is locked or shapes mismatch.
 * Use this access method for the best performance.
 *
 * \param new_v A constant view on the new array data for the y-direction.
 */
void DenseVectorfield2D::setV(const ArrayViewType& new_v)
{
    if(locked() || m_v.shape() != new_v.shape())
        return;
    
	m_v = new_v;
}

/**
 * Serialize the complete content of the dense vectorfield to an xml file.
 * The serialization is just a binary stream of m_u followed by m_v.
 *
 * \param out The output device for serialization.
 */
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

/**
 * Deserialization of a  dense vectorfield from an xml file.
 * Since the serialization is just a binary stream of m_u and m_v
 * and we already know the size, it is quite easy the deserialize.
 *
 * \param xmlReader The QXmlStreamReader, where we will read from.
 * \return True, if the content could be deserialized and the model is not locked.
 */
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

/**
 * Specialization of Model's updateModel procedure.
 * Is called everytime, the model changes.
 */
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

/**
 * Index conversion: From (x,y) position -> flat 1D index.
 *
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The flat 1D index.
 */
unsigned int DenseVectorfield2D::xyToIdx(unsigned int x, unsigned int y) const
{
	return (unsigned int)(y*m_u.width()+x);
}

/**
 * Index conversion: From a flat 1D index -> the x-component of the position.
 *
 * \param index The flat 1D index.
 * \return The x-component of the position.
 */
unsigned int DenseVectorfield2D::indexToX(unsigned int index) const
{
	return index % m_u.width();
}

/**
 * Index conversion: From a flat 1D index -> the y-component of the position.
 *
 * \param index The flat 1D index.
 * \return The y-component of the position.
 */
unsigned int DenseVectorfield2D::indexToY(unsigned int index) const
{
	return (unsigned int)(index / m_u.width());
}






    
/**
 * Default constructor. Creates an empty dense weighted vectorfield.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(Environment* env)
: DenseVectorfield2D(env)
{
}

/**
 * Copy constructor. Creates a dense weighted vectorfield from another one.
 *
 * \param vf The other dense weighted vectorfield.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DenseWeightedVectorfield2D & vf)
:	DenseVectorfield2D(vf)
{
	m_w = vf.w();
}

/**
 * Copy constructor. Creates a dense weighted vectorfield from a non weighted one.
 * The weights will be zero initialized.
 *
 * \param vf The other dense vectorfield.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DenseVectorfield2D & vf)
:	DenseVectorfield2D(vf),
	m_w(ArrayType(vf.u().shape()))
{
	m_w = 0;
}

/**
 * Create an empty dense weighted vectorfield of a given 2D shape .
 *
 * \param shape The 2D shape, which, contains (width, height).
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const DiffType& shape, Environment* env)
:	DenseVectorfield2D(shape, env),
	m_w(ArrayType(shape))
{
}

/**
 * Create an empty dense weighted vectorfield of a given 2D shape.
 *
 * \param width The width of the new dense weighted vectorfield.
 * \param height The height of the new dense weighted vectorfield.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(int width, int height, Environment* env)
:	DenseVectorfield2D(width, height, env),
	m_w(ArrayType(width, height))
{
}

/**
 * Create a dense weighed vectorfield from two arrays, which need to contain.
 * the vectors direction in x- (u) and y- (v) direction. All data will
 * be copied. The weights will be zero initialized.
 *
 * \param u The x-directions of the vectors.
 * \param v The y-directions of the vectors.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, Environment* env)
:	DenseVectorfield2D(u, v, env),
	m_w(ArrayType(u.width(), u.height()))
{	
}


/**
 * Create a dense weighted vectorfield from two arrays, which need to contain.
 * the vectors direction in x- (u) and y- (v) direction. All data will
 * be copied.
 *
 * \param u The x-directions of the vectors.
 * \param v The y-directions of the vectors.
 * \param w The weights of the vectors.
 */
DenseWeightedVectorfield2D::DenseWeightedVectorfield2D(const ArrayViewType& u, const ArrayViewType& v, const ArrayViewType& w, Environment* env)
:	DenseVectorfield2D(u, v, env),
	m_w(w)
{	
}

/**
 * This does not remove all the vectors, but resets their directions 
 * and weights to zero.
 * Does nothing if the model is locked.
 */
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

/**
 * Getter for the  weight of a vector at a given index. 
 * May throw an error, if the index is out of bounds.
 *
 * \param index The index of the vector, for which we want the weight.
 * \return The weight of that vector.
 */
float DenseWeightedVectorfield2D::weight(unsigned int index) const
{
	return m_w(indexToX(index), indexToY(index));
}

/**
 * Getter for the  weight of a vector at a given position.
 * May throw an error, if the index is out of bounds.
 *
 * \param orig The position of the vector.
 * \return The weight of that vector.
 */
float DenseWeightedVectorfield2D::weight(const PointType& orig) const
{
    return weight(orig.x(), orig.y());
}

/**
 * Getter for the  weight of a vector at a given position.
 * May throw an error, if the index is out of bounds.
 *
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \return The weight of that vector.
 */
float DenseWeightedVectorfield2D::weight(unsigned int x, unsigned int y) const
{
	return m_w(x,y);
}

		
/**
 * Setter for the  weight of a vector at a given index.
 * Does nothing if the model is locked.
 *
 * \param index The index of the vector, for which we want the weight.
 * \param new_w The new weight of that vector.
 */
void DenseWeightedVectorfield2D::setWeight(unsigned int index, float new_w)
{
	setWeight(indexToX(index), indexToY(index), new_w);
}

/**
 * Setter for the  weight of a vector at a given position.
 * Does nothing if the model is locked.
 *
 * \param orig The position of the vector.
 * \param new_w The new weight of that vector.
 */
void DenseWeightedVectorfield2D::setWeight(const PointType& orig, float new_w)
{
    setWeight(orig.x(), orig.y(), new_w);
}

/**
 * Setter for the  weight of a vector at a given position.
 * Does nothing if the model is locked.
 *
 * \param x The x position of the vector.
 * \param y The y position of the vector.
 * \param new_w The new weight of that vector.
 */
void DenseWeightedVectorfield2D::setWeight(unsigned int x, unsigned int y, float new_w)
{
    if(locked())
        return;
    
	m_w(x,y) = new_w;
	updateModel();
}

/**
 * Serialize the complete content of the dense weighted vectorfield to an xml file.
 * The serialization is just a binary stream of m_u followed by m_v and m_w.
 *
 * \param out The output device for serialization.
 */
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

/**
 * Deserialization of a  dense vectorfield from an xml file.
 * Since the serialization is just a binary stream of m_u, m_v and m_w
 * and we already know the size, it is quite easy the deserialize.
 *
 * \param xmlReader The QXmlStreamReader, where we will read from.
 * \return True, if the content could be deserialized and the model is not locked.
 */
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


/**
 * Constant reading access to the weights of each direction
 * vector. Use this access method for the best performance.
 *
 * \return A constant view on the array data for the weights.
 */
const DenseWeightedVectorfield2D::ArrayViewType & DenseWeightedVectorfield2D::w() const
{
	return m_w;
}

/**
 * Setter for the weights of the direction vectors.
 * Does nothing if the model is locked or shapes mismatch.
 * Use this access method for the best performance.
 *
 * \param new_w A constant view on the new array data for the weights.
 */
void DenseWeightedVectorfield2D::setW(const ArrayViewType& new_w)
{
    if(locked() || m_w.shape() != new_w.shape())
        return;
    
	m_w = new_w;
}

/**
 * Specialization of Model's updateModel procedure.
 * Is called everytime, the model changes.
 */
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
