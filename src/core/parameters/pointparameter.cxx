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

#include "core/parameters/pointparameter.hxx"

#include <QLabel>
#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the PointParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the PointParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param low           The lowest allowed value of this parameter.
 * \param upp           The highest allowed value of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
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

/**
 * Destructor of the PointParameter class.
 */
PointParameter::~PointParameter()
{
    //Also deletes other widget, since they are owned
    //by the assigned layout.
    if(m_delegate != NULL)
        delete m_delegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "PointParameter".
 */
QString  PointParameter::typeName() const
{
    return "PointParameter";
}

/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
QPoint PointParameter::lowerBound() const
{
    return m_min_value;//QPoint(m_spbXDelegate->minimum(), m_spbYDelegate->minimum());
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void PointParameter::setLowerBound(const QPoint& value)
{
    m_min_value = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMinimum(value.x());
        m_spbYDelegate->setMinimum(value.y());
    }
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
QPoint PointParameter::upperBound() const
{
    return m_max_value;//QPoint(m_spbXDelegate->maximum(), m_spbYDelegate->maximum());
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void PointParameter::setUpperBound(const QPoint& value)
{
    m_max_value = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMaximum(value.x());
        m_spbYDelegate->setMaximum(value.y());
    }
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void PointParameter::setRange(const QPoint& min_value, const QPoint& max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
QPoint PointParameter::value() const
{
    return m_value;//QPoint(m_spbXDelegate->value(), m_spbYDelegate->value());
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
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

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \return The value of the parameter converted to an QString.
 */
QString  PointParameter::toString() const
{
    return QString("(%1x%2)").arg(value().x()).arg(value().y());
}

/**
 * Serialization of the parameter's state to an output device.
 * Writes the following XML on the device:
 * 
 * <TYPENAME>
 *     <Name>NAME</Name>
 *     <x>X</x>
 *     <y>Y</y>
 * </TYPENAME>
 *
 * with TYPENAME = typeName(),
 *         NAME = name(),
 *            X = value().x(), and
 *            Y = value().y().
 *
 * \param xmlWriter The QXmlStreamWriter on which we serialize the parameter's state.
 */
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

/**
 * Deserialization of a parameter's state from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
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

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool PointParameter::isValid() const
{
    return m_value.x() >= m_min_value.x() && m_value.y() >= m_min_value.y()
        && m_value.x() <= m_max_value.x() && m_value.y() <= m_max_value.y();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
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

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
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

/**
 * @}
 */
