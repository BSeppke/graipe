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

#include <QtDebug>

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
ModelParameter::ModelParameter(const QString &name, const std::vector<Model*> * rs_object_stack, QString type_filter, Model* value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_cmbDelegate(NULL),
    m_type_filter(type_filter),
    m_value(value)
{
	m_modelList = rs_object_stack;
}

/**
 * The destructor of the ModelParameter class
 */
ModelParameter::~ModelParameter()
{
    if(m_cmbDelegate != NULL)
    {
        delete m_cmbDelegate;
        m_cmbDelegate=NULL;
    }
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "ModelParameter".
 */
QString  ModelParameter::typeName() const
{
	return "ModelParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
Model* ModelParameter::value() const
{ 
	return  m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void ModelParameter::setValue(Model* value)
{
    bool found = false;
    unsigned int i=0;
    
    foreach(Model* allowed, m_allowed_values)
    {
        if (allowed == value)
        {
            m_value = value;
            
            if(m_cmbDelegate != NULL)
            {
                m_cmbDelegate->setCurrentIndex(i);
            }
            found = true;
        }
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
QString ModelParameter::valueText() const
{ 
	return value()->name();
}

/**
 * This method is called after each (re-)assignment of the model list
 * e.g. after a call of the setModelList() function. 
 * It synchronizes the list of available models with the widget's list.
 */
void ModelParameter::refresh()
{
	if(m_modelList && m_cmbDelegate)
	{
		m_allowed_values.clear();
		m_cmbDelegate->clear();
		
		foreach( Model* model, *m_modelList)
		{
            
			if(m_type_filter.isEmpty() || m_type_filter.contains(model->typeName()))
			{
				m_cmbDelegate->addItem(model->shortName());
				m_cmbDelegate->setItemData(m_cmbDelegate->count()-1, model->description(), Qt::ToolTipRole);
				m_allowed_values.push_back(model);
			}
		}
        
        if(m_allowed_values.size())
        {
            m_value = m_allowed_values[0];
            m_cmbDelegate->setCurrentIndex(0);
        }
	}
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void ModelParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + encode_string(value()->filename()), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool ModelParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    bool found = false;
    unsigned int i=0;
    
    QString content(in.readLine().trimmed());
    
    foreach(Model* allowed, m_allowed_values)
    {
        if (allowed->filename() == decode_string(content))
        {
            m_value = allowed;
            
            if(m_cmbDelegate != NULL)
            {
                m_cmbDelegate->setCurrentIndex(i);
            }
            found = true;
        }
        i++;
    }
    
    if (!found)
    {
        qDebug() << "ModelParameter deserialize: filename does not match any given. Was: '" << content << "'";
    }
    
    return found;
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
    if(m_value)
    {
        m_lock = m_value->lock();
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
    if(m_value)
    {
        m_value->unlock(m_lock);
    }
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool ModelParameter::isValid() const
{
    return m_allowed_values.size();
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
	if(m_cmbDelegate==NULL)
	{
        m_cmbDelegate = new QComboBox;
		refresh();
        initConnections();
    }
    return m_cmbDelegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ModelParameter::updateValue()
{
    if(m_cmbDelegate != NULL)
    {
        unsigned int idx = m_cmbDelegate->currentIndex();
        
        if(idx<m_allowed_values.size())
        {
            m_value = m_allowed_values[m_cmbDelegate->currentIndex()];
            Parameter::updateValue();
        }
    }
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void ModelParameter::initConnections()
{
    connect(m_cmbDelegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe