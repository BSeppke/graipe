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

#include "core/model.hxx"
#include "core/impex.hxx"
#include "core/parameters.hxx"
#include "core/workspace.hxx"

#include <cmath>
#include <algorithm>

#include <QtDebug>
#include <QXmlStreamWriter>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the base types of all data: The Model, ModelList and RasteredModel classes.
 * @}
 */

Model::Model(Workspace* wsp)
:  QObject(),
    Serializable(),
    m_name(new StringParameter("Name:", "", 20, NULL)),
    m_description(new LongStringParameter("Description:", "", 20, 6, NULL)),
    m_ul(new PointParameter("Local upper-left (px.):", QPoint(0,0), QPoint(100000,100000), QPoint(0,0), NULL)),
    m_lr(new PointParameter("Local lower-right (px.):", QPoint(0,0),QPoint(100000,100000), QPoint(0,0), NULL)),
    m_global_ul(new PointFParameter("Global upper-left (deg.):", QPointF(-180,-90), QPointF(180,90), QPointF(0,0), NULL)),
    m_global_lr(new PointFParameter("Global lower-right (deg.):", QPointF(-180,-90),QPointF(180,90), QPointF(0,0), NULL)),
    m_parameters(new ParameterGroup("Model Properties", ParameterGroup::storage_type(), QFormLayout::WrapAllRows)),
    m_workspace(wsp)
{
    m_name->setValue(QString("New ") + typeName());
    m_description->setValue(QString("This new ") + typeName() + " has been created on " + QDateTime::currentDateTime().toString());
    
    m_parameters->addParameter("name", m_name);
    m_parameters->addParameter("descr", m_description);
    
    m_parameters->addParameter("ul", m_ul);
    m_parameters->addParameter("lr", m_lr);
    
    m_parameters->addParameter("global_ul", m_global_ul);
    m_parameters->addParameter("global_lr", m_global_lr);
    
    connect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
    
    //Add to global Models list
    workspace()->models.push_back(this);
}

Model::Model(const Model& model)
:	QObject(),
    Serializable(),
    m_name(new StringParameter("Name:", model.name(),20, NULL)),
    m_description(new LongStringParameter("Description:", model.description(), 20, 6, NULL)),
    m_ul(new PointParameter("Local upper-left:", QPoint(0,0), QPoint(100000,100000), QPoint(model.left(), model.top()), NULL)),
    m_lr(new PointParameter("Local lower-right:", QPoint(0,0),QPoint(100000,100000), QPoint(model.right(), model.bottom()), NULL)),
    m_global_ul(new PointFParameter("Global upper-left (deg.):", QPointF(-180,-90), QPointF(180,90), QPointF(model.globalLeft(), model.globalTop()), NULL)),
    m_global_lr(new PointFParameter("Global lower-right (deg.):", QPointF(-180,-90),QPointF(180,90), QPointF(model.globalRight(), model.globalBottom()), NULL)),
    m_parameters(new ParameterGroup("Model Properties",ParameterGroup::storage_type(), QFormLayout::WrapAllRows))
{
    m_parameters->addParameter("name", m_name);
    m_parameters->addParameter("descr", m_description);
    
    m_parameters->addParameter("ul", m_ul);
    m_parameters->addParameter("lr", m_lr);
    
    m_parameters->addParameter("global_ul", m_global_ul);
    m_parameters->addParameter("global_lr", m_global_lr);
    
    connect(m_parameters, SIGNAL(valueChanged()), this, SLOT(updateModel()));
    
    //Add to global Models list
    workspace()->models.push_back(this);
}

Model::~Model()
{
    //Delete the parameters
    delete m_parameters;
    
    //Remove from global models list
    workspace()->models.erase(std::remove(workspace()->models.begin(), workspace()->models.end(), this), workspace()->models.end());
}

QString Model::name() const
{
    return m_name->value();
}

QString Model::shortName(unsigned int length) const
{
	if(name().length() < (int)length)
		return name();
	
	if(name().length() < (int)length + 10)
		return name().left(length-3) + "...";
	
	return name().left(length/2+length%2) + "..." + name().right(length/2-3-1);
}

void Model::setName (const QString& new_name)
{
    if(locked())
        return;
    
    m_name->setValue(new_name);
    updateModel();
}

QString Model::description() const
{
	return m_description->value();
}

void Model::setDescription(const QString & new_description)
{
    if(locked())
        return;
    
    m_description->setValue(new_description);
    updateModel();
}

float Model::left() const 
{
	return m_ul->value().x();
}

void Model::setLeft(float new_left)
{
    if(locked())
        return;
    
    m_ul->setValue(QPoint(new_left, right()));
    updateModel();
}

float Model::top() const
{
        return m_ul->value().y();
}

void Model::setTop(float new_top)
{
    if(locked())
        return;
    
    m_ul->setValue(QPoint(left(), new_top));
    updateModel();
}

float Model::right() const 
{
        return m_lr->value().x();
}

void Model::setRight(float new_right)
{
    if(locked())
        return;
    
    m_lr->setValue(QPoint(new_right, bottom()));
    updateModel();
}

float Model::bottom() const
{
    return m_lr->value().y();
}

void Model::setBottom(float new_bottom)
{
    if(locked())
        return;
    
    m_lr->setValue(QPoint(right(), new_bottom));
    updateModel();
}

unsigned int Model::width() const
{
    return fabs(right()-left());
}

unsigned int Model::height() const
{
    return fabs(bottom()-top());
}

float Model::globalLeft() const
{
	return m_global_ul->value().x();
}

void Model::setGlobalLeft(float new_left)
{
    if(locked())
        return;
    
    m_global_ul->setValue(QPointF(new_left,globalTop()));
    updateModel();
}

float Model::globalTop() const
{
    return m_global_ul->value().y();
}

void Model::setGlobalTop(float new_top)
{
    if(locked())
        return;
    
    m_global_ul->setValue(QPointF(globalLeft(), new_top));
    updateModel();
}

float Model::globalRight() const
{
    return m_global_lr->value().x();
}

void Model::setGlobalRight(float new_right)
{
    if(locked())
        return;
    
    m_global_lr->setValue(QPointF(new_right, bottom()));
    updateModel();
}

float Model::globalBottom() const
{
    return m_global_lr->value().y();
}

void Model::setGlobalBottom(float new_bottom)
{
    if(locked())
        return;
    
    m_global_lr->setValue(QPointF(globalRight(), new_bottom));
    updateModel();
}

bool Model::isViewable() const
{
    QPointF d_spt    = m_lr->value() - m_ul->value();
    
    return !d_spt.isNull();
}

bool Model::isGeoViewable() const
{
    QPointF d_geo    = m_global_lr->value() - m_global_ul->value();
    
    return !d_geo.isNull();
}

QTransform Model::localTransformation() const
{
	return QTransform::fromTranslate(left(), top());
}

QTransform Model::globalTransformation() const
{
	return  QTransform::fromTranslate(globalLeft(), -globalTop());
}

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

void Model::copyMetadata(Model& other) const
{
	//ensure constness
	if(this != &other)
	{
		copyGeometry(other);
		
		other.setName(name());
		other.setDescription(description());
	}
}

void Model::copyData(Model& other) const
{
	//ensure constness
	if(this != &other)
	{
		copyMetadata(other);
	}
}

void Model::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(4);
    
    bool fullFile = (xmlWriter.device()->pos() == 0);
    
    if (fullFile)
    {
        xmlWriter.writeStartDocument();
    }
        xmlWriter.writeStartElement(typeName());
        xmlWriter.writeAttribute("ID", id());
            xmlWriter.writeStartElement("Header");
                serialize_header(xmlWriter);
            xmlWriter.writeEndElement();
            xmlWriter.writeStartElement("Content");
                serialize_content(xmlWriter);
            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();    
    if (fullFile)
    {
        xmlWriter.writeEndDocument();
    }
}

bool Model::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        //Assume, that the deserialized has already read the start node:
        //if (xmlReader.readNextStartElement())
        //{
        //    qDebug() << "Model::deserialize: readNextStartElement" << xmlReader.name();
            
            if(     xmlReader.name() == typeName()
                &&  xmlReader.attributes().hasAttribute("ID"))
            {
                setID(xmlReader.attributes().value("ID").toString());
                
                while(xmlReader.readNextStartElement())
                {
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
            else
            {
                throw std::runtime_error("Did not find typeName() or id() in XML tree");
            }
        //}
        //else
        //{
        //  throw std::runtime_error("Did not find any start element in XML tree");
        //}
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
    return  false;
}

void Model::serialize_header(QXmlStreamWriter& xmlWriter) const
{
    m_parameters->serialize(xmlWriter);
}

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

void Model::serialize_content(QXmlStreamWriter& xmlWriter) const
{
}

bool Model::deserialize_content(QXmlStreamReader& xmlReader)
{
    return true;
}

bool Model::locked() const
{
    return (m_locks.size() > 0);
}

unsigned int Model::lockedBy() const
{
    return m_locks.size();
}

unsigned int Model::lock()
{
    unsigned int unlock_code = rand();
    m_locks.push_back(unlock_code);
    
	emit modelChanged();
    
    return unlock_code;
}

void Model::unlock(unsigned int unlock_code)
{    
    QVector<unsigned int>::iterator iter = std::find(m_locks.begin(), m_locks.end(), unlock_code);
    if(iter != m_locks.end())
    {
        m_locks.erase(iter);
        emit modelChanged();
    }
}

ParameterGroup* Model::parameters()
{
    return m_parameters;
}

void Model::updateModel()
{
    emit modelChanged();
}












RasteredModel::RasteredModel(Workspace* wsp)
: Model(wsp),
  m_size(new PointParameter("Raster size:", QPoint(0,0),QPoint(100000,100000), QPoint(0,0), NULL))
{
    m_parameters->addParameter("size", m_size);
}

RasteredModel::RasteredModel(const RasteredModel& model)
: Model(model),
  m_size(new PointParameter("Raster size:", QPoint(0,0),QPoint(100000,100000), QPoint(model.width(), model.height()), NULL))
{
    m_parameters->addParameter("size", m_size);
}

RasteredModel::~RasteredModel()
{
}

unsigned int RasteredModel::width() const
{
	return m_size->value().x();
}

void RasteredModel::setWidth(unsigned int new_w)
{
    if(locked())
        return;
    
    m_size->setValue(QPoint(new_w, height()));
    updateModel();
}

unsigned int RasteredModel::height() const
{
	return m_size->value().y();
}

void RasteredModel::setHeight(unsigned int new_h)
{
    if(locked())
        return;
    
    m_size->setValue(QPoint(width(), new_h));
    updateModel();
}

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

QTransform RasteredModel::localTransformation() const
{
	double scale_x = std::abs(right()  - left())/width();
    double scale_y = std::abs(bottom() - top())/height();
	
	return QTransform::fromScale(scale_x,scale_y) * Model::localTransformation();
}

QTransform RasteredModel::globalTransformation() const
{
	double scale_x = std::abs(globalRight() - globalLeft())/width();
	double scale_y = std::abs(globalBottom() - globalTop())/height();
	
	return QTransform::fromScale(scale_x,scale_y) * Model::globalTransformation();
}

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

void RasteredModel::copyData(Model& other) const
{
    Model::copyData(other);
}

} //end of namespace graipe
