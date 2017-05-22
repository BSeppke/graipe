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

#include "core/model.hxx"
#include "core/impex.hxx"
#include "core/parameters.hxx"

#include <cmath>

#include <QtDebug>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implementation file for the base types of all data: The Model, ModelList and RasteredModel classes.
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default/empty contructor of the Model class
 */
Model::Model()
:  QObject(),
    Serializable(),
    m_name(new StringParameter("Name:", "", 20, NULL)),
    m_description(new LongStringParameter("Description:", "", 20, 6, NULL)),
    m_save_filename(new LongStringParameter("Filename:", "", 20, 3, NULL)),
    m_ul(new PointParameter("Local upper-left (px.):", QPoint(0,0), QPoint(100000,100000), QPoint(0,0), NULL)),
    m_lr(new PointParameter("Local lower-right (px.):", QPoint(0,0),QPoint(100000,100000), QPoint(0,0), NULL)),
    m_global_ul(new PointFParameter("Global upper-left (deg.):", QPointF(-180,-90), QPointF(180,90), QPointF(0,0), NULL)),
    m_global_lr(new PointFParameter("Global lower-right (deg.):", QPointF(-180,-90),QPointF(180,90), QPointF(0,0), NULL)),
    m_parameters(new ParameterGroup("Model Properties", ParameterGroup::storage_type(), QFormLayout::WrapAllRows))
{
    m_name->setValue(QString("New ") + typeName());
    m_description->setValue(QString("This new ") + typeName() + " has been created on " + QDateTime::currentDateTime().toString());
    
    m_parameters->addParameter("name", m_name);
    m_parameters->addParameter("descr", m_description);
    
                                                            //hidden==true
    m_parameters->addParameter("filename", m_save_filename, true);
    
    m_parameters->addParameter("ul", m_ul);
    m_parameters->addParameter("lr", m_lr);
    
    m_parameters->addParameter("global_ul", m_global_ul);
    m_parameters->addParameter("global_lr", m_global_lr);
    
    connect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
}

/**
 * Copy contructor of the Model class
 *
 * \param model The other model, from which the parameters will be copied.
 */
Model::Model(const Model& model)
:	QObject(),
    Serializable(),
    m_name(new StringParameter("Name:", model.name(),20, NULL)),
    m_description(new LongStringParameter("Description:", model.description(), 20, 6, NULL)),
    m_save_filename(new LongStringParameter("Filename:", model.filename(), 20, 3, NULL)),
    m_ul(new PointParameter("Local upper-left:", QPoint(0,0), QPoint(100000,100000), QPoint(model.left(), model.top()), NULL)),
    m_lr(new PointParameter("Local lower-right:", QPoint(0,0),QPoint(100000,100000), QPoint(model.right(), model.bottom()), NULL)),
    m_global_ul(new PointFParameter("Global upper-left (deg.):", QPointF(-180,-90), QPointF(180,90), QPointF(model.globalLeft(), model.globalTop()), NULL)),
    m_global_lr(new PointFParameter("Global lower-right (deg.):", QPointF(-180,-90),QPointF(180,90), QPointF(model.globalRight(), model.globalBottom()), NULL)),
    m_parameters(new ParameterGroup("Model Properties",ParameterGroup::storage_type(), QFormLayout::WrapAllRows))
{
    m_parameters->addParameter("name", m_name);
    m_parameters->addParameter("descr", m_description);
    
                                                            //Hidden
    m_parameters->addParameter("filename", m_save_filename, true);
    
    m_parameters->addParameter("ul", m_ul);
    m_parameters->addParameter("lr", m_lr);
    
    m_parameters->addParameter("global_ul", m_global_ul);
    m_parameters->addParameter("global_lr", m_global_lr);
    
    connect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
}

/**
 * Destructor of the Model class
 */
Model::~Model()
{
    //Delete the parameters
    delete m_parameters;
}

/**
 * Const accessor for the (full) model name. This returns the complete name.
 *
 * \return The name of the model.
 */
QString Model::name() const
{
    return m_name->value();
}

/**
 * The short model name. The short name is restricted to a certain amount of digits
 * and will be filled with "..."
 *
 * \param length The length of the short QString (default=60).
 * \return The short name of the model.
 */
QString Model::shortName(unsigned int length) const
{
	if(name().length() < (int)length)
		return name();
	
	if(name().length() < (int)length + 10)
		return name().left(length-3) + "...";
	
	return name().left(length/2+length%2) + "..." + name().right(length/2-3-1);
}

/**
 * Set the model's name to a new QString.
 *
 * \param new_name The new name of the model.
 */
void Model::setName (const QString& new_name)
{
    if(locked())
        return;
    
    m_name->setValue(new_name);
    updateModel();
}

/**
 * Const accessor for the model description QString. 
 *
 * \return The description of the model.
 */
QString Model::description() const
{
	return m_description->value();
}

/**
 * Set the model's description to a new QString.
 *
 * \param new_description The new description of the model.
 */
void Model::setDescription(const QString & new_description)
{
    if(locked())
        return;
    
    m_description->setValue(new_description);
    updateModel();
}

/**
 * Const accessor for the model filename QString.
 *
 * \return The filename of the model.
 */
QString Model::filename() const
{
	return m_save_filename->value();
}

/**
 * Set the model's filename to a new QString.
 *
 * \param new_filename The new filename of the model.
 */
void Model::setFilename(const QString & new_filename)
{
    if(locked())
        return;
    
    m_save_filename->setValue(new_filename);
    m_filename = new_filename;
    
    updateModel();
}

/**
 * Const accessor for the left (x-coordinate) of a Model.
 *
 * \return The left boundary of the Model.
 */
float Model::left() const 
{
	return m_ul->value().x();
}

/**
 * Set the left (x-coordinate) of a Model to a given value.
 *
 * \param new_left The new left boundary of the Model.
 */
void Model::setLeft(float new_left)
{
    if(locked())
        return;
    
    m_ul->setValue(QPoint(new_left, right()));
    updateModel();
}

/**
 * Const accessor for the top (y-coordinate) of a Model.
 *
 * \return The top boundary of the Model.
 */
float Model::top() const
{
        return m_ul->value().y();
}

/**
 * Set the top (y-coordinate) of a Model to a given value.
 *
 * \param new_top The new top boundary of the Model.
 */
void Model::setTop(float new_top)
{
    if(locked())
        return;
    
    m_ul->setValue(QPoint(left(), new_top));
    updateModel();
}

/**
 * Const accessor for the right (x-coordinate) of a Model.
 *
 * \return The right boundary of the Model.
 */
float Model::right() const 
{
        return m_lr->value().x();
}

/**
 * Set the right (x-coordinate) of a Model to a given value.
 *
 * \param new_right The new right boundary of the Model.
 */
void Model::setRight(float new_right)
{
    if(locked())
        return;
    
    m_lr->setValue(QPoint(new_right, bottom()));
    updateModel();
}

/**
 * Const accessor for the bottom (y-coordinate) of a Model.
 *
 * \return The bottom boundary of the Model.
 */
float Model::bottom() const
{
    return m_lr->value().y();
}

/**
 * Set the bottom (y-coordinate) of a Model to a given value.
 *
 * \param new_bottom The new bottom boundary of the Model.
 */
void Model::setBottom(float new_bottom)
{
    if(locked())
        return;
    
    m_lr->setValue(QPoint(right(), new_bottom));
    updateModel();
}

/**
 * Const accessor to the width (right-left) of the model.
 *
 * \return The width of the model.
 */
unsigned int Model::width() const
{
    return fabs(right()-left());
}

/**
 * Const accessor to the height (bottom-top) of the model.
 *
 * \return The height of the model.
 */
unsigned int Model::height() const
{
    return fabs(bottom()-top());
}

/**
 * Const accessor for the global left (x-coordinate) of a Model.
 *
 * \return The global left boundary of the Model.
 */
float Model::globalLeft() const
{
	return m_global_ul->value().x();
}

/**
 * Set the global left (x-coordinate) of a Model to a given value.
 *
 * \param new_left The new global left boundary of the Model.
 */
void Model::setGlobalLeft(float new_left)
{
    if(locked())
        return;
    
    m_global_ul->setValue(QPointF(new_left,globalTop()));
    updateModel();
}

/**
 * Const accessor for the global top (y-coordinate) of a Model.
 *
 * \return The global top boundary of the Model.
 */
float Model::globalTop() const
{
    return m_global_ul->value().y();
}

/**
 * Set the global top (y-coordinate) of a Model to a given value.
 *
 * \param new_top The new global top boundary of the Model.
 */
void Model::setGlobalTop(float new_top)
{
    if(locked())
        return;
    
    m_global_ul->setValue(QPointF(globalLeft(), new_top));
    updateModel();
}

/**
 * Const accessor for the global right (x-coordinate) of a Model.
 *
 * \return The global right boundary of the Model.
 */
float Model::globalRight() const
{
    return m_global_lr->value().x();
}

/**
 * Set the global right (x-coordinate) of a Model to a given value.
 *
 * \param new_right The new global right boundary of the Model.
 */
void Model::setGlobalRight(float new_right)
{
    if(locked())
        return;
    
    m_global_lr->setValue(QPointF(new_right, bottom()));
    updateModel();
}

/**
 * Const accessor for the global bottom (y-coordinate) of a Model.
 *
 * \return The global bottom boundary of the Model.
 */
float Model::globalBottom() const
{
    return m_global_lr->value().y();
}

/**
 * Set the global bottom (y-coordinate) of a Model to a given value.
 *
 * \param new_bottom The new global bottom boundary of the Model.
 */
void Model::setGlobalBottom(float new_bottom)
{
    if(locked())
        return;
    
    m_global_lr->setValue(QPointF(globalRight(), new_bottom));
    updateModel();
}
    
/**
 * Only models with valid boundaries may be visualized in geometric view mode!
 *
 * \return Returns true, if the lowerRight value is larger than the upperLeft value.
 */
bool Model::isViewable() const
{
    QPointF d_spt    = m_lr->value() - m_ul->value();
    
    return !d_spt.isNull();
}

/** Only models with valid boundaries may be visualized in geographic view mode!
 *
 * \return Returns true, if the globalLowerRight value is larger than the upperLeft value.
 */
bool Model::isGeoViewable() const
{
    QPointF d_geo    = m_global_lr->value() - m_global_ul->value();
    
    return !d_geo.isNull();
}

/**
 * Convenience function to get the local transformation in Qt style.
 *
 * \return The local translation matrix of the model.
 */
QTransform Model::localTransformation() const
{
	return QTransform::fromTranslate(left(), top());
}

/**
 * Convenience function to get the global transformation in Qt style.
 *
 * \return The global translation matrix of the model.
 */
QTransform Model::globalTransformation() const
{
	return  QTransform::fromTranslate(globalLeft(), -globalTop());
}

/**
 * Const copy model's geometry information to another model.
 *
 * \param other The other model.
 */
void Model::copyGeometry(Model& other) const
{
	//ensure constness
	if(this != &other)
	{
		other.setLeft(left());
		other.setTop(top());
		other.setRight(right());
		other.setBottom(bottom());
		
		other.setGlobalLeft(globalLeft());
		other.setGlobalTop(globalTop());
		other.setGlobalRight(globalRight());
		other.setGlobalBottom(globalBottom());
	}
}

/**
 * Const copy model's complete metadata to another model.
 *
 * \param other The other model.
 */
void Model::copyMetadata(Model& other) const
{
	//ensure constness
	if(this != &other)
	{
		copyGeometry(other);
		
		other.setName(name());
		other.setDescription(description());
		other.setFilename(filename());
	}
}

/**
 * Const copy model's complete data (and metadata) to another model.
 *
 * \param other The other model.
 */
void Model::copyData(Model& other) const
{
	//ensure constness
	if(this != &other)
	{
		copyMetadata(other);
	}
}

/**
 * The type of this model (same for every instance).
 *
 * \return "Model"
 */
QString Model::typeName() const
{
	return "Model";
}

/**
 * This function serializes a complete Model to an output device.
 * To do so, it serializes header first, then the content, like this:
 *      serialize_header()
 *      [Content]
 *      serialize_content();
 *
 * \param out The output device for the serialization.
 */
void Model::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(4);
    
    xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement(typeName());
        xmlWriter.writeAttribute("ID", filename());
            xmlWriter.writeStartElement("Header");
                serialize_header(xmlWriter);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("Content");
                serialize_content(xmlWriter);
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
}

/**
 * This function deserializes the model by means of its header and content
 *
 * \param  in The input device.
 * \return True, if the Model could be restored,
 */
bool Model::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if (xmlReader.readNextStartElement())
        {
            qDebug() << "Model::deserialize: readNextStartElement" << xmlReader.name();
            
            if(xmlReader.name() == typeName())
            {
                while(xmlReader.readNextStartElement())
                {
                    qDebug() << "Model::deserialize: readNextStartElement" << xmlReader.name();
            
                    if(xmlReader.name() == "Header")
                    {
                         if(!deserialize_header(xmlReader))
                         {
                            return false;
                        }
                        //Read until </Header> comes....
                        while(true)
                        {
                            if(!xmlReader.readNext())
                            {
                                return false;
                            }
                            
                            if(xmlReader.isEndElement() && xmlReader.name() == "Header")
                            {
                                break;
                            }
                        }
                    }
                    if(xmlReader.name() == "Content")
                    {
                        if(!deserialize_content(xmlReader))
                        {
                            return false;
                        }
                    }
                }
                return true;
            }
        }
        else
        {
            throw std::runtime_error("Did not find typeName() in XML tree");
        }
        throw std::runtime_error("Did not find any start element in XML tree");
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
    return  false;
}

/**
 * This function serializes the header of a model like this:
 *      typeName()
 *      m_parameters->serialize()
 *
 * \param out the output device.
 */
void Model::serialize_header(QXmlStreamWriter& xmlWriter) const
{
    m_parameters->serialize(xmlWriter);
}

/**
 * This function deserializes the Model's header.
 *
 * \param  in The input device.
 * \return True, if the Model's header could be restored,
 */
bool Model::deserialize_header(QXmlStreamReader& xmlReader)
{
    if(locked())
    {
        return false;
    }

    disconnect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
    
    bool res = m_parameters->deserialize(xmlReader);
    
    connect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
        
    return res;
}

/**
 * This function serializes the content of a model.
 * Has to be specialized, here always "none\n".
 *
 * \param out the output device.
 */
void Model::serialize_content(QXmlStreamWriter& xmlWriter) const
{
}

/**
 * This function deserializes the Model's content.
 *
 * \param  in The input device.
 * \return True, if the Model's content could be restored,
 */
bool Model::deserialize_content(QXmlStreamReader& xmlReader)
{
    return true;
}

/**
 * Models may be locked (to read only access, while algorithms are using them e.g.
 * This function can be used to query, if the Model is locked or not.
 *
 * \return True, if the model has been locked by somebody
 */
bool Model::locked() const
{
    return (m_locks.size() > 0);
}

/**
 * Models may be locked (to read only access), while algorithms are using them e.g.
 * This function can be used to query, how many locks are currently active.
 *
 * \return the number of locks currently active.
 */
unsigned int Model::lockedBy() const
{
    return m_locks.size();
}

/**
 * Put a lock request on the model. Since the locking is a secured operation,
 * each lock-requester will get a personal (random) unlock code by its request.
 * He has to take for this code, because otherwise, unlocking is impossible.
 *
 * \return The code needed for unlocking afterwards
 */
unsigned int Model::lock()
{
    unsigned int unlock_code = rand();
    m_locks.push_back(unlock_code);
    
	emit modelChanged();
    
    return unlock_code;
}

/**
 * Remove the locking of the model using your unlock code.
 *
 * \param unlock_code the code, which unlocks the lock.
 */
void Model::unlock(unsigned int unlock_code)
{    
    QVector<unsigned int>::iterator iter = std::find(m_locks.begin(), m_locks.end(), unlock_code);
    if(iter != m_locks.end())
    {
        m_locks.erase(iter);
        emit modelChanged();
    }
}

/**
 * Potentially non-const access to the parameters of the model.
 * These can be used to edit the model in a GUI!
 *
 * \return The parameters of this model
 */
ParameterGroup* Model::parameters()
{
    return m_parameters;
}

/**
 * This slot is called, whenever some parameter is changed.
 * It then emits simply the modelChanged signal to inform connected views etc.
 */
void Model::updateModel()
{
    emit modelChanged();
}



/**
 * Default/empty contructor of the RasteredModel class.
 */
RasteredModel::RasteredModel()
: Model(),
  m_size(new PointParameter("Raster size:", QPoint(0,0),QPoint(100000,100000), QPoint(0,0), NULL))
{
    m_parameters->addParameter("size", m_size);
}

/**
 * Copy contructor of the RasteredModel class.
 *
 * \param model The other model, from which the parameters will be copied.
 */
RasteredModel::RasteredModel(const RasteredModel& model)
: Model(model),
  m_size(new PointParameter("Raster size:", QPoint(0,0),QPoint(100000,100000), QPoint(model.width(), model.height()), NULL))
{
    m_parameters->addParameter("size", m_size);
}

/**
 * Destructor of the RasteredModel class.
 */
RasteredModel::~RasteredModel()
{
}

/**
 * Const accessor to the raster resolution in x-direction of the model.
 *
 * \return The raster width of the model.
 */
unsigned int RasteredModel::width() const
{
	return m_size->value().x();
}

/**
 * Set the raster resolution in x-direction of the model to a new width.
 *
 * \param new_w The new raster width of the model.
 */
void RasteredModel::setWidth(unsigned int new_w)
{
    if(locked())
        return;
    
    m_size->setValue(QPoint(new_w, height()));
    updateModel();
}

/**
 * Const accessor to the raster resolution in y-direction of the model.
 *
 * \return The raster height of the model.
 */
unsigned int RasteredModel::height() const
{
	return m_size->value().y();
}

/**
 * Set the raster resolution in y-direction of the model to a new height.
 *
 * \param new_h The new raster height of the model.
 */
void RasteredModel::setHeight(unsigned int new_h)
{
    if(locked())
        return;
    
    m_size->setValue(QPoint(width(), new_h));
    updateModel();
}

/**
 * Validity of models
 * Only valid models may be visualized!
 */
bool RasteredModel::isEmpty() const
{
    return m_size->value().isNull();
}

bool RasteredModel::isViewable() const
{
    return !isEmpty() && Model::isViewable();
}

bool RasteredModel::isGeoViewable() const
{
    return !isEmpty() && Model::isGeoViewable();
}

/**
 * Convenience function to get the local transformation in Qt style.
 *
 * \return The local translation matrix of the model scaled by the resolution.
 */
QTransform RasteredModel::localTransformation() const
{
	double scale_x = std::abs(right()  - left())/width();
    double scale_y = std::abs(bottom() - top())/height();
	
	return QTransform::fromScale(scale_x,scale_y) * Model::localTransformation();
}
/**
 * Convenience function to get the global transformation in Qt style.
 *
 * \return The global translation matrix of the model scaled by the resolution.
 */
QTransform RasteredModel::globalTransformation() const
{
	double scale_x = std::abs(globalRight() - globalLeft())/width();
	double scale_y = std::abs(globalBottom() - globalTop())/height();
	
	return QTransform::fromScale(scale_x,scale_y) * Model::globalTransformation();
}

/**
 * Const copy model's geometry information to another model.
 *
 * \param other The other model.
 */
void RasteredModel::copyGeometry(Model& other) const
{
    Model::copyGeometry(other);
    
	//ensure constness
	if(this != &other && other.typeName()==typeName())
	{   
        RasteredModel& raster_model = static_cast<RasteredModel&>(other);
        
        raster_model.setWidth(width());
        raster_model.setHeight(height());
	}
}
/**
 * Const copy model's complete data (and metadata) to another model.
 *
 * \param other The other model.
 */
void RasteredModel::copyData(Model& other) const
{
    Model::copyData(other);
}

/**
 * The type of this model (same for every instance).
 *
 * \return "RasteredModel"
 */
QString RasteredModel::typeName() const
{
	return "RasteredModel";
}

} //end of namespace graipe

/**
 * @}
 */
