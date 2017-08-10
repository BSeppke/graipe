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

#include "core/parameters/parametergroup.hxx"
#include "core/impex.hxx"

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the ParameterGroup class
 * @}
 */

ParameterGroup::ParameterGroup(const QString&  name, storage_type items, QFormLayout::RowWrapPolicy policy, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_parameters(items),
    m_delegate(NULL),
    m_layout(NULL),
    m_policy(policy)
{
}

ParameterGroup::~ParameterGroup()
{
    for(item_type item: m_parameters)
    {
        delete item.second;
    }
}

std::vector<Model*> ParameterGroup::needsModels() const
{
    std::vector<Model*> modelList;
    
    //For each parameter
    for(item_type item : m_parameters)
    {
        //For each of the linked models
        for(Model* m : item.second->needsModels())
        {
            //Is it already in the modelList?
            auto iter = std::find(modelList.begin(), modelList.end(), m);
            
            //If not: Add it!
            if(iter == modelList.end())
            {
                modelList.push_back(m);
            }
        }
    }
    return modelList;
}

unsigned int ParameterGroup::addParameter(const QString& id, Parameter* param, bool hidden)
{
    unsigned int idx = (unsigned int)m_parameters.size();
    
    param->setID(id);
    m_parameters.insert(item_type(id, param));
    
    if(!hidden)
    {
        m_parameter_order.push_back(id);
        
    }
    
    return idx;
}

Parameter* ParameterGroup::operator[](const QString& id)
{
    return m_parameters.at(id);
}

Parameter const * ParameterGroup::operator[](const QString& id) const
{
    return m_parameters.at(id);
}

ParameterGroup::storage_type::const_iterator ParameterGroup::begin() const
{
    return m_parameters.cbegin();
}

 ParameterGroup::storage_type::const_iterator ParameterGroup::end() const
{
    return m_parameters.cend();
}

ParameterGroup::storage_type::iterator ParameterGroup::begin()
{
    return m_parameters.begin();
}

ParameterGroup::storage_type::iterator ParameterGroup::end()
{
    return m_parameters.end();
}

unsigned int ParameterGroup::size() const
{
    return (unsigned int)m_parameters.size();
}

QString ParameterGroup::toString() const
{
    QString report;
    
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        Parameter * p = iter->second;
        if (p)
        {
            report += p->name() + ": " + p->toString() + QString("\n");
        }
    }
    
    return report;
}

QString ParameterGroup::valueText(const QString & filter_types) const
{
    QString report;
    
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        Parameter * p = iter->second;
        if (p)
        {
            if(!filter_types.contains(p->typeName()))
            {
                report += p->name() + ": " + p->toString() + QString("\n");
            }
        }
    }
    return report;
}

void ParameterGroup::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Parameters", QString::number(m_parameters.size()));
    
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        if (iter->second)
        {
            iter->second->serialize(xmlWriter);
        }
    }
    xmlWriter.writeEndElement();
}

bool ParameterGroup::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if (xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == typeName())
            {
                for(int j=0; j!=2; ++j)
                {
                    xmlReader.readNextStartElement();
                    
                    if(xmlReader.name() == "Name")
                    {
                        setName(xmlReader.readElementText());
                    }
                    if(xmlReader.name() == "Parameters")
                    {
                        int parameter_count = xmlReader.readElementText().toInt();
                        
                        if(parameter_count != m_parameters.size())
                        {
                            throw std::runtime_error("Parameter count mismatch");
                        }
                        
                        for (int i=0; i!= parameter_count; ++i)
                        {
                            xmlReader.readNextStartElement();
                            
                            if(xmlReader.attributes().hasAttribute("ID"))
                            {
                                QString id = xmlReader.attributes().value("ID").toString();
                               
                                if(!m_parameters[id]->deserialize(xmlReader))
                                {
                                    throw std::runtime_error("Could not deserialize ID: " + id.toStdString());
                                }
                            }

                        }                            
                        //Read until </ParameterGroup> comes...
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
                        return true;
                    }
                }
            }
            else
            {
                throw std::runtime_error("Did not find typeName() or id() in XML tree");
            }
        }
        else
        {
            throw std::runtime_error("Did not find any start element in XML tree");
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "ParameterGroup::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
    return true;
}

QWidget * ParameterGroup::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_layout =  new QFormLayout(m_delegate),
    
        m_layout->setRowWrapPolicy(m_policy);
        m_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        
        for( QString id : m_parameter_order)
        {
            Parameter* param = m_parameters.at(id);
            QWidget* p_delegate = param->delegate();
            
            if(p_delegate)
            {
                m_layout->addRow(param->name(), p_delegate);
                p_delegate->setMaximumSize(9999,9999);
                p_delegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
                connect(param, SIGNAL(valueChanged()), this, SLOT(updateValue()));
            }
        }
        Parameter::initConnections();

    }
    return m_delegate;
}

} //end of namespace graipe
