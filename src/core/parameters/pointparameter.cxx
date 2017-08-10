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

#include "core/parameters/pointparameter.hxx"

#include <QLabel>
#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the PointParameter class
 * @}
 */

PointParameter::PointParameter(const QString& name, QPoint low, QPoint upp, QPoint value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL),
    m_spbXDelegate(NULL),
    m_spbYDelegate(NULL)
{
}

PointParameter::~PointParameter()
{
    //Also deletes other widget, since they are owned
    //by the assigned layout.
    if(m_delegate != NULL)
        delete m_delegate;
}

QPoint PointParameter::lowerBound() const
{
    return m_min_value;//QPoint(m_spbXDelegate->minimum(), m_spbYDelegate->minimum());
}

void PointParameter::setLowerBound(const QPoint& value)
{
    m_min_value = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMinimum(value.x());
        m_spbYDelegate->setMinimum(value.y());
    }
}

QPoint PointParameter::upperBound() const
{
    return m_max_value;//QPoint(m_spbXDelegate->maximum(), m_spbYDelegate->maximum());
}

void PointParameter::setUpperBound(const QPoint& value)
{
    m_max_value = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMaximum(value.x());
        m_spbYDelegate->setMaximum(value.y());
    }
}

void PointParameter::setRange(const QPoint& min_value, const QPoint& max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

QPoint PointParameter::value() const
{
    return m_value;//QPoint(m_spbXDelegate->value(), m_spbYDelegate->value());
}

void PointParameter::setValue(const QPoint& value)
{
    m_value = value;

    if(m_delegate != NULL)
    {
        m_spbXDelegate->setValue(value.x());
        m_spbYDelegate->setValue(value.y());
        Parameter::updateValue();
    }
}

QString  PointParameter::toString() const
{
    return QString("(%1x%2)").arg(value().x()).arg(value().y());
}

void PointParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("x", QString::number(value().x()));
    xmlWriter.writeTextElement("y", QString::number(value().y()));
    xmlWriter.writeEndElement();
}

bool PointParameter::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
            QPoint p;
            
            for(int i=0; i!=3; ++i)
            {
                xmlReader.readNextStartElement();
                
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "x")
                {
                   p.setX(xmlReader.readElementText().toInt());
                }
                if(xmlReader.name() == "y")
                {
                   p.setY(xmlReader.readElementText().toInt());
                }
            }
            
            //Read until </PointParameter> comes....
            while(true)
            {
                if(!xmlReader.readNext())
                {
                    return false;
                }
                
                if(xmlReader.isEndElement() && xmlReader.name() == typeName())
                {
                    break;
                }
            }
            setValue(p);
            return true;
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
        throw std::runtime_error("Did not find any start element in XML tree");
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "PointParameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}

bool PointParameter::isValid() const
{
    return m_value.x() >= m_min_value.x() && m_value.y() >= m_min_value.y()
        && m_value.x() <= m_max_value.x() && m_value.y() <= m_max_value.y();
}

QWidget*  PointParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_spbXDelegate = new QSpinBox;
        m_spbYDelegate = new QSpinBox;

        m_spbXDelegate->setMaximumSize(9999,9999);
        m_spbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_spbXDelegate->setRange(m_min_value.x(), m_max_value.x());
        m_spbXDelegate->setValue(m_value.x());
        
        m_spbYDelegate->setMaximumSize(9999,9999);
        m_spbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_spbYDelegate->setRange(m_min_value.y(), m_max_value.y());
        m_spbYDelegate->setValue(m_value.y());
        
        QHBoxLayout * layout = new QHBoxLayout(m_delegate);
    
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(new QLabel("x:"));
        layout->addWidget(m_spbXDelegate);
        layout->addWidget(new QLabel("y:"));
        layout->addWidget(m_spbYDelegate);
    
        connect(m_spbXDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
        connect(m_spbYDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

void PointParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value.setX(m_spbXDelegate->value());
        m_value.setY(m_spbYDelegate->value());
        Parameter::updateValue();
    }
}

} //end of namespace graipe
