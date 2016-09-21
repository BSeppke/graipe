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

#include "core/parameters/multimodelparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the MultiModelParameter class with a setting of the
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
MultiModelParameter::MultiModelParameter(const QString& name, const std::vector<Model*> * allowed_values, QString type_filter, std::vector<Model*> *  /*value*/, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_lstDelegate(NULL),
    m_allowed_values(*allowed_values),
	m_type_filter(type_filter)
{
}

/**
 * Destructor of the MultiModel class
 */
MultiModelParameter::~MultiModelParameter()
{
    if(m_lstDelegate != NULL)
    {
        delete m_lstDelegate;
        m_lstDelegate=NULL;
    }
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "MultiModelParameter".
 */
QString  MultiModelParameter::typeName() const
{
	return "MultiModelParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
const std::vector<Model*>& MultiModelParameter::value() const
{ 
	return m_selected_values;	
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void MultiModelParameter::setValue(const std::vector<Model*>& values)
{
	m_selected_values.clear();
    
	foreach( Model* model, values )
	{
		bool found = false;
		unsigned int i=0;
		
		foreach(Model* allowed_model, m_allowed_values)
		{
			
			if (model == allowed_model)
			{
				found = true;
				break;
			}
			++i;
		}
		
		if (found)
		{
			m_selected_values.push_back(model);
            if(m_lstDelegate != NULL)
            {
                m_lstDelegate->item(i)->setSelected(true);
            }
		}
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
QString MultiModelParameter::valueText() const
{ 
	QString res;
	foreach(Model* model, m_selected_values)
	{
		res += model->name() +", "; 
	}
	return res.left(res.length()-2);
}

/**
 * This method is called after each (re-)assignment of the model list
 * e.g. after a call of the setModelList() function. 
 * It synchronizes the list of available models with the widget's list.
 */
void MultiModelParameter::refresh()
{
	if(m_modelList && m_lstDelegate)
	{
		m_allowed_values.clear();
		m_lstDelegate->clear();
		
		foreach( Model* model, *m_modelList)
		{
			if( m_type_filter.contains(model->typeName()))
			{
				m_lstDelegate->addItem(model->shortName());
				m_lstDelegate->item(m_lstDelegate->count()-1)->setToolTip(model->description());
				m_allowed_values.push_back(model);
			}
		}
		m_selected_values.clear();
	}
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void MultiModelParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    
    foreach(Model* model, m_selected_values)
    {
        write_on_device(", " + encode_string(model->filename()), out);
    }
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool MultiModelParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    m_selected_values.clear();

    QString content(in.readLine().trimmed());
    
    QStringList model_filenames = content.split(", ");
    
    foreach( QString model_filename, model_filenames )
    {
        bool found = false;
        unsigned int i=0;
        
        foreach(Model* allowed_model, m_allowed_values)
        {
            
            if (decode_string(model_filename) == allowed_model->filename())
            {
                found = true;
                break;
            }
            ++i;
        }
        
        if (found)
        {
            m_selected_values.push_back(m_allowed_values[i]);
            if(m_lstDelegate != NULL)
            {
                m_lstDelegate->item(i)->setSelected(true);
            }
        }
        else
        {
            qDebug("MultiModelParameter deserialize: not found");
            return false;
        }
    }
    return true;
}
    
/**
 * This function locks the parameters value. 
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
void MultiModelParameter::lock()
{	
    m_locks.clear();
    
	foreach( Model* model, m_selected_values )
	{
        unsigned int model_lock = model->lock();
        m_locks.push_back(model_lock);
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
void MultiModelParameter::unlock()
{	
	for( unsigned int i=0; i < (unsigned int)m_selected_values.size(); ++i)
	{
        m_selected_values[i]->unlock(m_locks[i]);
    }
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
    
bool MultiModelParameter::isValid() const
{
	return m_lstDelegate && m_lstDelegate->isEnabled() && !m_selected_values.empty();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  MultiModelParameter::delegate()
{
    if(m_lstDelegate == NULL)
    {
        m_lstDelegate = new QListWidget;
        m_lstDelegate->setSelectionMode(QAbstractItemView::MultiSelection);
	
        refresh();
        initConnections();
	}
    return m_lstDelegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void MultiModelParameter::updateValue()
{
    if(m_lstDelegate != NULL)
    {
        m_selected_values.clear();

        for( unsigned int i=0; i < (unsigned int)m_lstDelegate->count(); ++i)
        {
            if (m_lstDelegate->item(i)->isSelected())
            {
                m_selected_values.push_back(m_allowed_values[i]);
            }
        }
        Parameter::updateValue();
    }
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void MultiModelParameter::initConnections()
{
    connect(m_lstDelegate, SIGNAL(itemSelectionChanged()), this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
