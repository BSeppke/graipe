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

#include "core/parameters/enumparameter.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implementation file for the EnumParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the EnumParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param enum_names    All allowed enum labels of this parameter..
 * \param value         The initial (index, 0-starting) value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
EnumParameter::EnumParameter(const QString& name, const QStringList & enum_names, int value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_enum_names(enum_names),
    m_value(value),
    m_delegate(NULL)
{
}

/**
 * Destructor of the EnumParameter class
 */
EnumParameter::~EnumParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

/** 
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter. (-1 on error)
 */
int EnumParameter::value() const
{
	return m_value;// m_delegate->currentIndex();
}
    
/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void EnumParameter::setValue(int value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
    	m_delegate->setCurrentIndex(value);
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
QString EnumParameter::toString() const
{
    if (isValid())
    	return m_enum_names[value()];
    else
        return "";
}

/**
 * Serialization of the parameter's state to a xml stream.
 * Writes the following XML code by default:
 * 
 * <TYPENAME>
 *     <Name>NAME</Name>
 *     <Value>VALUETEXT</Value>
 * </TYPENAME>
 *
 * with TYPENAME = typeName(),
 *         NAME = name(), and
 *    VALUETEXT = QString::number(value()).
 *
 * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
 *                  parameter's type, name and value.
 */
void EnumParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Value", QString::number(value()));
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool EnumParameter::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
            while(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "Value")
                {
                    QString valueText =  xmlReader.readElementText();
                    
                    setValue(valueText.toInt());
                    return isValid();
                }
            }
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
        return false;
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "EnumParameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}
/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool EnumParameter::isValid() const
{
    return value() > -1 && value() < m_enum_names.size();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  EnumParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QComboBox;
        for(int v=0; v<m_enum_names.size(); ++v)
        {
            m_delegate->addItem(m_enum_names[v]);
        }
        m_delegate->setCurrentIndex(m_value);
    
        connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void EnumParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->currentIndex();
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */
