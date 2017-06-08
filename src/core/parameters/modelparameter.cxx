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

#include "core/parameters/modelparameter.hxx"
#include "core/globals.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implementation file for the ModelParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the ModelParameter class with a setting of the
 * most important values directly.
 *
 * \param name           The name (label) of this parameter.
 * \param allowed_models A vector containing all currently available models.
 * \param type_filter    A QString to restrict the model list to certain model types.
 * \param value          The initial value of this parameter.
 * \param parent         If given (!= NULL), this parameter has a parent and will
 *                       be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
 */
ModelParameter::ModelParameter(const QString &name, QString type_filter, Model* value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_delegate(NULL),
    m_type_filter(type_filter)
{
    if(models.size())
	{
		m_allowed_values.clear();
		
		for(Model * model: models)
		{
            
			if(m_type_filter.isEmpty() || m_type_filter.contains(model->typeName()))
			{
				m_allowed_values.push_back(model);
			}
		}
	}
    
    setValue(value);
    
}

/**
 * The destructor of the ModelParameter class
 */
ModelParameter::~ModelParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
Model* ModelParameter::value() const
{
    if(isValid())
    {
        return m_allowed_values[m_model_idx];
    }
    else
    {
        return NULL;
    }
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void ModelParameter::setValue(Model* value)
{
    bool found = false;
    
    for(int i=0; i<m_allowed_values.size(); ++i)
    {
        if(m_allowed_values[i] == value)
        {
            found = true;
            m_model_idx=i;
            break;
        }
    }
    
    if (found && m_delegate != NULL)
    {
        m_delegate->setCurrentIndex(m_model_idx);
        Parameter::updateValue();
    }
    
    if (!found)
    {
        qDebug("ModelParameter::setValue() failed due to unknown/not allowed model value");
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
QString ModelParameter::toString() const
{ 
	return value()->id();
}

/**
 * Deserialization of a parameter's state from a string.
 *
 * \param str the input QString.
 * \return True, if the deserialization was successful, else false.
 */
bool ModelParameter::fromString(QString& str)
{
    for(Model* allowed: m_allowed_values)
    {
        if (allowed->id() == str)
        {
            setValue(allowed);
            return true;
        }
    }
    
    qDebug() << "ModelParameter deserialize: filename does not match any given. Was: '" << str << "'";
    return false;
}

/**
 * This function locks the parameters value. 
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
void ModelParameter::lock()
{
    if(value())
    {
        m_lock = value()->lock();
    }
}

/**
 * This function unlocks the parameters value.
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
void ModelParameter::unlock()
{
    if(value())
    {
        value()->unlock(m_lock);
    }
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool ModelParameter::isValid() const
{
    return (m_model_idx >= 0 && m_model_idx < m_allowed_values.size());
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  ModelParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QComboBox;
        
        int i=0;
        
		for(Model * model: m_allowed_values)
		{
			m_delegate->addItem(model->shortName());
            m_delegate->setItemData(i++, model->description(), Qt::ToolTipRole);
		}
    
        connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ModelParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_model_idx = m_delegate->currentIndex();
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */
