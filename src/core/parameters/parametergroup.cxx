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

#include "core/parameters/parametergroup.hxx"
#include "core/impex.hxx"

#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the ParameterGroup class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the ParameterGroup class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter group.
 * \param items         A std::map from QString to Parameter pointers containing all parameters.
 * \param policy        Layout settings for the used QFormLayout.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
ParameterGroup::ParameterGroup(const QString&  name, storage_type items, QFormLayout::RowWrapPolicy policy, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_parameters(items),
    m_delegate(NULL),
    m_layout(NULL),
    m_policy(policy)
{
}

/**
 * Destructor of the ParameterGroup class. 
 * On destruction, every parameter of the group will be deleted, too.
 * as we assume to have the ownership of the added parameter pointers.
 */
ParameterGroup::~ParameterGroup()
{
    for(item_type item: m_parameters)
    {
         delete item.second;
    }
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "ParameterGroup".
 */
QString  ParameterGroup::typeName() const
{
	return "ParameterGroup";
}

/**
 * Add an already existing parameter to the ParameterGroup.
 *
 * \param id     The key for this parameter for easy access inside the group
 * \param param  The pointer to the existing parameter.
 * \param hidden If true, the parameter will be displayed by the group.
 * \return The index of the inserted parameter (size()-1)
 */
unsigned int ParameterGroup::addParameter(const QString& id, Parameter* param, bool hidden)
{
    unsigned int idx = (unsigned int)m_parameters.size();
    
    m_parameters.insert(item_type(id, param));
    
    if(!hidden)
    {
        m_parameter_order.push_back(id);
        
    }
    
    return idx;
}

/**
 * Accessor to a parameter specified by the (QString) id.
 *
 * \param id The id of the parameter.
 * \return The pointer to the parameter inside the group or NULL, if not found.
 */
Parameter* ParameterGroup::operator[](const QString& id)
{
    return m_parameters.at(id);
}

/**
 * Const accessor to a parameter specified by the (QString) id.
 *
 * \param id The id of the parameter.
 * \return The const pointer to the parameter inside the group or NULL, if not found.
 */
Parameter const * ParameterGroup::operator[](const QString& id) const
{
    return m_parameters.at(id);
}

/**
 * Const iterator pointing to the beginning of the QString -> parameter pointer
 * map storage. This may be used to support const for-looping in stl-style.
 *
 * \return The const iterator to the beginning of the QString -> parameter pointer map.
 */
ParameterGroup::storage_type::const_iterator ParameterGroup::begin() const
{
    return m_parameters.cbegin();
}

/**
 * Const iterator pointing after the end of the QString -> parameter pointer
 * map storage. This may be used to support const for-looping in stl-style.
 *
 * \return The const iterator after the end of the QString -> parameter pointer map.
 */
 ParameterGroup::storage_type::const_iterator ParameterGroup::end() const
{
    return m_parameters.cend();
}

/**
 * Iterator pointing to the beginning of the QString -> parameter pointer
 * map storage. This may be used to support for-looping in stl-style.
 *
 * \return The iterator to the beginning of the QString -> parameter pointer map.
 */
ParameterGroup::storage_type::iterator ParameterGroup::begin()
{
    return m_parameters.begin();
}

/**
 * Iterator pointing after the end of the QString -> parameter pointer
 * map storage. This may be used to support for-looping in stl-style.
 *
 * \return The iterator after the end of the QString -> parameter pointer map.
 */
ParameterGroup::storage_type::iterator ParameterGroup::end()
{
    return m_parameters.end();
}

/**
 * This function gives the parameter count of all existing parameters.
 *
 * \return The size of the QString -> parameter pointer map.
 */
unsigned int ParameterGroup::size() const
{
    return (unsigned int)m_parameters.size();
}

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \return The value of the parameter converted to an QString.
 */
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

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \param filter_types Only special parameters are given out - filtered by their type. 
 * \return The value of the parameter converted to an QString.
 */
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

/**
 * Serialization of the parameter's state to an output device.
 * This serializes each parameter in the group by means of its name and its serialization,
 * one per line, e.g. like:
 * "param1: StringParameter, bla"
 * "param2: PointParmaeter, ...."
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void ParameterGroup::serialize(QXmlStreamWriter& xmlWriter) const
{
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Parameters", QString::number(m_parameters.size()));
    
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        if (iter->second)
        {
            xmlWriter.writeStartElement("Parameter");
            xmlWriter.writeAttribute("ID",iter->first);
                iter->second->serialize(xmlWriter);
            xmlWriter.writeEndElement();
        }
    }
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
 * \return True, if the deserialization was successful, else false.
 */
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
                            
                            if(     xmlReader.name() == "Parameter"
                                &&  xmlReader.attributes().hasAttribute("ID"))
                            {
                                QString id = xmlReader.attributes().value("ID").toString();
                               
                                if(!m_parameters[id]->deserialize(xmlReader))
                                {
                                    throw std::runtime_error("Could not deserialize ID: " + id.toStdString());
                                }              
                                //Read until the </Parameter> comes...
                                while(true)
                                {
                                    if(!xmlReader.readNext())
                                    {
                                        return false;
                                    }
                                    
                                    if(xmlReader.isEndElement() && xmlReader.name() == "Parameter")
                                    {
                                        break;
                                    }
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
                throw std::runtime_error("Did not find typeName() in XML tree");
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
    
/**
 * This method is called after each (re-)assignment of the model list
 * e.g. after a call of the setModelList() function. 
 * It synchronizes the list of available models with the widget's list.
 */
void ParameterGroup::refresh()
{
    for(storage_type::iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        iter->second->refresh();
    }
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
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

/**
 * @}
 */
