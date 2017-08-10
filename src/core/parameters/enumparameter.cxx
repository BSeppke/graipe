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

#include "core/parameters/enumparameter.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>


namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the EnumParameter class
 * @}
 */

EnumParameter::EnumParameter(const QString& name, const QStringList & enum_names, int value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_enum_names(enum_names),
    m_value(value),
    m_delegate(NULL)
{
}

EnumParameter::~EnumParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

int EnumParameter::value() const
{
	return m_value;// m_delegate->currentIndex();
}

void EnumParameter::setValue(int value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
    	m_delegate->setCurrentIndex(value);
        Parameter::updateValue();
    }
}

QString EnumParameter::toString() const
{
    if (isValid())
    	return m_enum_names[value()];
    else
        return "";
}

void EnumParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Value", QString::number(value()));
    xmlWriter.writeEndElement();
}

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

bool EnumParameter::isValid() const
{
    return value() > -1 && value() < m_enum_names.size();
}

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
