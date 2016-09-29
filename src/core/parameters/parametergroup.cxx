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
        if (item.second != NULL)
        {
            delete item.second;
            item.second=NULL;
        }
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
    
    if(m_delegate != NULL && !hidden)
    {
        param->delegate()->setMaximumSize(9999,9999);
        param->delegate()->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred));
        m_layout->addRow(param->name(), param->delegate());
        connect(param, SIGNAL(valueChanged()), this, SLOT(updateValue()));
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
QString ParameterGroup::valueText() const
{
    QString report;
    
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        Parameter * p = iter->second;
        if (p)
        {
            report += p->name() + ": " + p->valueText() + QString("\n");
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
                report += p->name() + ": " + p->valueText() + QString("\n");
            }
        }
    }
    return report;
}

/**
 * The magicID of this parameter class. 
 * Implemented to fullfil the Serializable interface.
 *
 * \return "", since a parameter group does not use any magicIDs.
 */
QString ParameterGroup::magicID() const
{
    return "";
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void ParameterGroup::serialize(QIODevice& out) const
{
    for(storage_type::const_iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
    {
        if (iter->second)
        {
            write_on_device(iter->first + ": ", out);
            iter->second->serialize(out);
            write_on_device("\n", out);
        }
    }
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool ParameterGroup::deserialize(QIODevice& in)
{
    while(!in.atEnd())
    {
        //1. Try to get the Id of the parameter
        QString id = read_from_device_until(in, ": ");
        
        if(id.isEmpty())
            break;
        
        id = id.left(id.size()-2);
        
        //2. Find the correct Parameter for this id
        for(storage_type::iterator iter = m_parameters.begin();  iter != m_parameters.end(); ++iter)
        {
            //3. Parameter found in m_parameters
            if (iter->first == id)
            {
                //4. Try to serialize the found parameter using the serial
                if(!iter->second->deserialize(in))
                {
                    qCritical() << "ParameterGroup deserialize: Unable to deserialize '" << id << "' for parameter '" << iter->second->name() << "'";
                    return false;
                }
            }
        }
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
        iter->second->setModelList(m_modelList);
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
        m_layout = new QFormLayout(m_delegate);
        
        m_layout->setRowWrapPolicy(m_policy);
        m_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        
        for( QString id : m_parameter_order)
        {
            Parameter* param = m_parameters.at(id);
            
            if(param->delegate())
            {
                param->delegate()->setMaximumSize(9999,9999);
                param->delegate()->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
                
                if(!param->isHidden())
                {
                    m_layout->addRow(param->name(), param->delegate());
                }
                connect(param, SIGNAL(valueChanged()), this, SLOT(updateValue()));
            }
        }
    }
    
    return m_delegate;
}

} //end of namespace graipe
