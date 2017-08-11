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

#include "core/parameters/boolparameter.hxx"
#include "core/model.hxx"

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the general Parameter class
 * @}
 */

Parameter::Parameter()
:	m_name(""),
    m_parent(NULL),
    m_invert_parent(false)
{
}

Parameter::Parameter(const QString&  name, Parameter* parent, bool invert_parent)
:	m_name(name), 
    m_parent(parent),
    m_invert_parent(invert_parent)
{
}

Parameter::~Parameter()
{
    //Nothing to do here, since no widget was created earlier.
}

QString Parameter::name() const
{
	return m_name;
}

void Parameter::setName(const QString& name)
{
	m_name = name; 
}

Parameter*  Parameter::parent()
{
    return m_parent;
}

bool Parameter::invertParent() const
{
    return m_invert_parent;
}

QString Parameter::toString() const
{
	return "";
}

bool Parameter::fromString(QString& str)
{
    return true;
}

void Parameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Value", toString());
    xmlWriter.writeEndElement();
}

bool Parameter::deserialize(QXmlStreamReader& xmlReader)
{
    bool success = false;
    
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
            for(int i=0; i!=2; ++i)
            {
                xmlReader.readNextStartElement();
            
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "Value")
                {
                    QString valueText =  xmlReader.readElementText();
                    success = fromString(valueText);
                }
            }
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
            return success;
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}

void Parameter::lock()
{
}

void Parameter::unlock()
{
}

bool Parameter::isValid() const
{
    return false;
}

std::vector<Model*> Parameter::needsModels() const
{
    return std::vector<Model*>();
}

QWidget*  Parameter::delegate()
{
    return NULL;
}

void Parameter::updateValue()
{
    emit valueChanged();
}

void Parameter::initConnections()
{
    if(parent()!=NULL && parent()->typeName()=="BoolParameter")
    {
        BoolParameter* parent_param = static_cast<BoolParameter*>(parent());
        if(parent_param != NULL)
        {
            QCheckBox* parent_widget = static_cast<QCheckBox*> (parent_param->delegate());
            if (parent_widget != NULL && delegate() != NULL)
            {
                if(m_invert_parent)
                {
                    connect(parent_widget, SIGNAL(clicked(bool)), delegate(), SLOT(setDisabled(bool)));
                    delegate()->setEnabled(!parent_param->value());
                    
                }
                else
                {
                    connect(parent_widget, SIGNAL(clicked(bool)), delegate(), SLOT(setEnabled(bool)));
                    delegate()->setEnabled(parent_param->value());
                }
            }
        }
    }
}
 
} //end of namespace graipe
