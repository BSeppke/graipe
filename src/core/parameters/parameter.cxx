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

#include "core/parameters/parameter.hxx"
#include "core/parameters/boolparameter.hxx"
#include "core/model.hxx"

#include <QtDebug>
#include <QCoreApplication>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implementation file for the general Parameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the Parameter class
 */
Parameter::Parameter()
{
}

/**
 * More usable constructor of the Parameter class with a setting of the 
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
Parameter::Parameter(const QString&  name, Parameter* parent, bool invert_parent)
:	m_name(name), 
    m_parent(parent),
    m_invert_parent(invert_parent)
{
}

/**
 * (Of course virtual) Destructor of the Parameter class.
 */
Parameter::~Parameter()
{
    //Nothing to do here, since no widget was created earlier.
}

/**
 * The (immutable) type name of this parameter class.
 * Implemented to fullfil the Serializable interface.
 *
 * \return "Parameter".
 */
QString  Parameter::typeName() const
{
	return "Parameter";
}

/**
 * The name of this parameter. This name is used a label for the parameter.
 *
 * \return The name of the parameter.
 */
QString Parameter::name() const
{
	return m_name;
}

/**
 * Writer for the name/label of this parameter.
 *
 * \param name The new name of the parameter.
 */
void Parameter::setName(const QString& name)
{
	m_name = name; 
}

/**
 * Potentially non-const access to the parent of this parameter. 
 *
 * \return The pointer to the parent parameter.
 */
Parameter*  Parameter::parent()
{
    return m_parent;
}

/**
 * Is the parent's checked state linked to setEnabled slot of this parameter's
 * widget delegate or is it inverted linked?
 *
 * \return True, if an inverted linkage is established.
 */
bool Parameter::invertParent() const
{
    return m_invert_parent;
}

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The value of the parameter converted to an QString
 */
QString Parameter::toString() const
{
	return "";
}

/**
 * Sets the value using a QString. This is the default method, used by the desearialize .
 *
 * \param str The value of the parameter converted to an QString
 */
bool Parameter::fromString(QString& str)
{
    return true;
}

/**
 * Serialization of the parameter's state to an output device.
 * Writes the following XML on the device:
 * 
 * <MAGICID>
 *     <Name>NAME</Name>
 *     <Value>VALUETEXT</Value>
 * </MAGICID>
 *
 * with MAGICID = typeName(),
 *         NAME = name(), and
 *    VALUETEXT = toString().
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void Parameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Value", toString());
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
 * \return True, if the deserialization was successful, else false.
 */
bool Parameter::deserialize(QXmlStreamReader& xmlReader)
{
    bool success = false;
    
    try
    {
        if (xmlReader.readNextStartElement())
        {            
            if(xmlReader.name() == typeName())
            {
                for(int i=0; i!=2; ++i)
                {
                    xmlReader.readNextStartElement();
                
                    if(xmlReader.name() == "Name")
                    {
                        setName(xmlReader.readElementText());
                    }
                    if(xmlReader.name() == "Value")
                    {
                        QString valueText =  xmlReader.readElementText();
                        success = fromString(valueText);
                    }
                }
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
                return success;
            }
        }
        else
        {
            throw std::runtime_error("Did not find typeName() in XML tree");
        }
        throw std::runtime_error("Did not find any start element in XML tree");
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
}

/**
 * Const access to the model list, which is currently assigned to 
 * this parameter.
 *
 * \return A pointer to the current model list.
 */
const std::vector<Model*> * Parameter::modelList() const
{
	return m_modelList;
}

/**
 * Writing access to the model list, which may be used to assign a new
 * or update the currently used model list of this parameter.
 *
 * \param new_model_list A pointer to the new model list.
 */
void Parameter::setModelList(const std::vector<Model*> * new_obj_stack)
{
	m_modelList = new_obj_stack;
	refresh();
}

/**
 * This method is called after each (re-)assignment of the model list
 * e.g. after a call of the setModelList() function. It may be implemented
 * by means of the subclasses to handle these updates.
 */
void Parameter::refresh()
{
}

/**
 * This function locks the parameters value. 
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
void Parameter::lock()
{
}

/**
 * This function unlocks the parameters value.
 * This means, that after a lock() call, only const acess to the parameter is 
 * possible until someone unlocks it. 
 * To work properly, the inner parameter class has to be designed accordingly.
 * As an example, you may look at the Model class, which supports locking and
 * unlocking - so do the parameter classes based on models!
 */
void Parameter::unlock()
{
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool Parameter::isValid() const
{
    return false;
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  Parameter::delegate()
{
    return NULL;
}

/**
 * This slot is called everytime, the delegate has changed. It may be used to synchronize
 * the internal value of the parameter with the current delegate's value. 
 * For the base class, it simply emits the valueChanged() signal.
 */
void Parameter::updateValue()
{
    emit valueChanged();
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void Parameter::initConnections()
{
    if(parent()!=NULL && parent()->typeName()=="BoolParameter")
    {
        BoolParameter* parent_param = static_cast<BoolParameter*>(parent());
        if(parent_param != NULL)
        {
            QCheckBox* parent_widget = static_cast<QCheckBox*> (parent_param->delegate());
            if (parent_widget != NULL && delegate() != NULL)
            {
                if(m_invert_parent)
                {
                    connect(parent_widget, SIGNAL(clicked(bool)), delegate(), SLOT(setDisabled(bool)));
                    delegate()->setEnabled(!parent_param->value());
                    
                }
                else
                {
                    connect(parent_widget, SIGNAL(clicked(bool)), delegate(), SLOT(setEnabled(bool)));
                    delegate()->setEnabled(parent_param->value());
                }
            }
        }
    }
    
}

} //end of namespace graipe

/**
 * @}
 */
