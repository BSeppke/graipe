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
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implmentation file for the MultiModelParameter class
 *
 * @addtogroup core
 * @{
 */

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
    m_lstDelegate(new QListWidget),
    m_allowed_values(*allowed_values),
	m_type_filter(type_filter)
{
    m_lstDelegate->setSelectionMode(QAbstractItemView::MultiSelection);
    refresh();
    
    connect(m_lstDelegate, SIGNAL(selectionChanged()), this, SLOT(updateValue()));
    Parameter::initConnections();
}

/**
 * Destructor of the MultiModel class
 */
MultiModelParameter::~MultiModelParameter()
{
    if(m_lstDelegate != NULL)
        delete m_lstDelegate;
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
std::vector<Model*> MultiModelParameter::value() const
{
    std::vector<Model*> selected_models;
    
    for(int i : m_model_idxs)
    {
        selected_models.push_back(m_allowed_values[i]);
    }
    
    return selected_models;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void MultiModelParameter::setValue(const std::vector<Model*>& value)
{
   m_model_idxs.clear();
    
    unsigned int i=0;
    
    for(const Model* allowed_model: m_allowed_values)
    {
        bool found = false;
        
        for(const Model* model: value)
        {
            if( allowed_model == model)
            {
                found=true;
                break;
            }
        }
        m_model_idxs.push_back(i);
        if(m_lstDelegate != NULL)
        {
            m_lstDelegate->item(i)->setSelected(found);
        }
        i++;
    }
    if(m_lstDelegate != NULL)
    {
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
QString MultiModelParameter::toString() const
{ 
	QString res;
    
    for(const Model* model: value())
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
	if(m_modelList != NULL)
	{
		m_allowed_values.clear();
		
		for(Model* model: *m_modelList)
		{
			if( m_type_filter.contains(model->typeName()))
			{
				m_allowed_values.push_back(model);
			}
		}
	}
    
	if(m_lstDelegate != NULL)
	{
		m_lstDelegate->clear();
		
		for(Model* model: m_allowed_values)
		{
			m_lstDelegate->addItem(model->shortName());
            m_lstDelegate->item(m_lstDelegate->count()-1)->setToolTip(model->description());
		}
	}
}

/**
 * Serialization of the parameter's state to an output device.
 * Writes comman-separated model list the output device, containing the filename
 * for each model, like:
 * "MultModelParameter, file1.bla, file2.blubb"
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void MultiModelParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(magicID());
    xmlWriter.writeTextElement("Name", name());
    int i=0;
    for(const Model* model: value())
    {
        xmlWriter.writeStartElement("Value");
        xmlWriter.writeAttribute("ID", QString::number(i++));
            xmlWriter.writeCharacters(model->filename());
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
 * \return True, if the deserialization was successful, else false.
 */
bool MultiModelParameter::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if (xmlReader.readNextStartElement())
        {
            if(xmlReader.name() == magicID())
            {
                while(xmlReader.readNextStartElement())
                {
                    if(xmlReader.name() == "Name")
                    {
                        setName(xmlReader.readElementText());
                    }
                    if(xmlReader.name() == "Value")
                    {
                        QString filename =  xmlReader.readElementText();
                        
                        int i=0;
                        
                        for(const Model* allowed_model: m_allowed_values)
                        {
                           if (filename == allowed_model->filename())
                           {
                                m_lstDelegate->item(i)->setSelected(true);
                                break;
                            }
                            ++i;
                        }
                        if(i==m_allowed_values.size())
                        {
                            throw std::runtime_error("Did not find a model with filename: " + filename.toStdString());
                        }
                    }
                }
            }
        }
        else
        {
            throw std::runtime_error("Did not find magicID in XML tree");
        }
        throw std::runtime_error("Did not find any start element in XML tree");
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for magicID: " << magicID() << "Error: " << e.what();
        return false;
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
    
	for(Model* model: value())
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
    unsigned int i=0;

	for(Model* model: value())
	{
        model->unlock(m_locks[i]);
        ++i;
    }
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
    
bool MultiModelParameter::isValid() const
{
	return true;
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
    
        connect(m_lstDelegate, SIGNAL(selectionChanged()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_lstDelegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void MultiModelParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_lstDelegate != NULL)
    {
        m_model_idxs.clear();
        
        for(int i=0; i<m_lstDelegate->count(); ++i)
        {
            if(m_lstDelegate->item(i)->isSelected())
            {
                m_model_idxs.push_back(i);
            }
        }
        Parameter::updateValue();
    }
}


} //end of namespace graipe

/**
 * @}
 */
