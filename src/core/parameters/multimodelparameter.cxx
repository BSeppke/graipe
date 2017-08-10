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

#include "core/parameters/multimodelparameter.hxx"
#include "core/workspace.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Implmentation file for the MultiModelParameter class
 */

/**
 * Default constructor of the MultiModelParameter class with a setting of the
 * most important values directly.
 *
 * \param name           The name (label) of this parameter.
 * \param allowed_models A vector containing all currently available models.
 * \param type_filter    A QString to restrict the model list to certain model types.
 * \param parent         If given (!= NULL), this parameter has a parent and will
 *                       be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent  If true, the enables/disabled dependency to the parent will be swapped.
 */
MultiModelParameter::MultiModelParameter(const QString& name, QString type_filter, Parameter* parent, bool invert_parent, Workspace* wsp)
:	Parameter(name, parent, invert_parent, wsp),
    m_delegate(NULL),
    m_allowed_values(wsp->models),
	m_type_filter(type_filter)
{
    if(wsp != NULL && wsp->models.size())
	{
		m_allowed_values.clear();
		
		for(Model* model: wsp->models)
		{
			if( m_type_filter.contains(model->typeName()))
			{
				m_allowed_values.push_back(model);
			}
		}
	}
}

/**
 * Destructor of the MultiModel class
 */
MultiModelParameter::~MultiModelParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
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
        if(m_delegate != NULL)
        {
            m_delegate->item(i)->setSelected(found);
        }
        i++;
    }
    if(m_delegate != NULL)
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
 * Serialization of the parameter's state to a xml stream.
 * Writes the following XML code by default:
 * 
 * <MultiModelParameter>
 *     <Name>NAME</Name>
 *     <Values>N</Value>
 *     <Value ID="0">VALUE_0_ID</Value>
 *     ...
 *     <Value ID="N-1">VALUE_N-1_ID</Value>
 * </MultiModelParameter>
 *
 * with     NAME = name(),
 *             N = QString::number(value().size()), and
 *    VALUE_0_ID = values()[0]->id().
 *
 * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
 *                  parameter's type, name and value.
 */
void MultiModelParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID",id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Values", QString::number(value().size()));
    int i=0;
    for(const Model* model: value())
    {
        xmlWriter.writeStartElement("Value");
        xmlWriter.writeAttribute("ID", QString::number(i++));
            xmlWriter.writeCharacters(model->id());
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool MultiModelParameter::deserialize(QXmlStreamReader& xmlReader)
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
                    QString id =  xmlReader.readElementText();
                    
                    int i=0;
                    
                    for(const Model* allowed_model: m_allowed_values)
                    {
                       if (id == allowed_model->id())
                       {
                            m_delegate->item(i)->setSelected(true);
                            break;
                        }
                        ++i;
                    }
                    if(i==m_allowed_values.size())
                    {
                        throw std::runtime_error("Did not find a model with id: " + id.toStdString());
                    }
                }
            }
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
 * This function indicates whether the value of a parameter is a Model* or 
 * many of them or needs one at least. These parameters need to access the
 * global 'models' variable, too!
 *
 * \return A filled vector, if the parameter's value is related to a Model*.
 *         An empty vector by default.
 */
std::vector<Model*> MultiModelParameter::needsModels() const
{
    return value();
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
    if(m_delegate == NULL)
    {
        m_delegate = new QListWidget;
        
        m_delegate->setSelectionMode(QAbstractItemView::MultiSelection);
        
        for(Model* model: m_allowed_values)
		{
			m_delegate->addItem(model->shortName());
            m_delegate->item(m_delegate->count()-1)->setToolTip(model->description());
		}
    
        connect(m_delegate, SIGNAL(selectionChanged()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void MultiModelParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_model_idxs.clear();
        
        for(int i=0; i<m_delegate->count(); ++i)
        {
            if(m_delegate->item(i)->isSelected())
            {
                m_model_idxs.push_back(i);
            }
        }
        Parameter::updateValue();
    }
}

/**
 * @}
 */


} //end of namespace graipe
