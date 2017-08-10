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
 *     @file
 *     @brief Implmentation file for the MultiModelParameter class
 * @}
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

MultiModelParameter::~MultiModelParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

std::vector<Model*> MultiModelParameter::value() const
{
    std::vector<Model*> selected_models;
    
    for(int i : m_model_idxs)
    {
        selected_models.push_back(m_allowed_values[i]);
    }
    
    return selected_models;
}

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

QString MultiModelParameter::toString() const
{ 
	QString res;
    
    for(const Model* model: value())
	{
		res += model->name() +", "; 
	}
	return res.left(res.length()-2);
}

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

void MultiModelParameter::lock()
{	
    m_locks.clear();
    
	for(Model* model: value())
	{
        unsigned int model_lock = model->lock();
        m_locks.push_back(model_lock);
	}
}

void MultiModelParameter::unlock()
{
    unsigned int i=0;

	for(Model* model: value())
	{
        model->unlock(m_locks[i]);
        ++i;
    }
}

bool MultiModelParameter::isValid() const
{
	return true;
}

std::vector<Model*> MultiModelParameter::needsModels() const
{
    return value();
}

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
