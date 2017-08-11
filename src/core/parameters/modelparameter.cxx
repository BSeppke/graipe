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

#include "core/parameters/modelparameter.hxx"
#include "core/workspace.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the ModelParameter class
 * @}
 */

ModelParameter::ModelParameter(const QString &name, QString type_filter, Parameter* parent, bool invert_parent, Workspace* wsp)
:   Parameter(name, parent, invert_parent),
    m_delegate(NULL),
    m_type_filter(type_filter)
{
    if(wsp!= NULL && wsp->models.size())
	{
		m_allowed_values.clear();
		
		for(Model * model: wsp->models)
		{
			if(m_type_filter.isEmpty() || m_type_filter.contains(model->typeName()))
			{
				m_allowed_values.push_back(model);
			}
		}
	}
    if(m_allowed_values.size() != 0)
    {
        setValue(m_allowed_values.front());
    }
}

ModelParameter::~ModelParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

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
        qDebug() << "ModelParameter::setValue() failed due to unknown/not allowed model value: " << (long int) value;
    }
}

QString ModelParameter::toString() const
{ 
	return value()->id();
}

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

void ModelParameter::lock()
{
    if(value())
    {
        m_lock = value()->lock();
    }
}

void ModelParameter::unlock()
{
    if(value())
    {
        value()->unlock(m_lock);
    }
}

bool ModelParameter::isValid() const
{
    return (m_model_idx >= 0 && m_model_idx < m_allowed_values.size());
}

std::vector<Model*> ModelParameter::needsModels() const
{
    std::vector<Model*> modelList;
    
    if(isValid())
    {
        modelList.push_back(value());
    }
    return modelList;
}

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
