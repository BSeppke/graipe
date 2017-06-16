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

#include "core/parameters/boolparameter.hxx"
#include "core/model.hxx"

#include <QtDebug>

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
Parameter::Parameter( Environment* env)
:	m_name(""),
    m_parent(NULL),
    m_invert_parent(false),
    m_environment(env)
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
Parameter::Parameter(const QString&  name, Parameter* parent, bool invert_parent, Environment* env)
:	m_name(name), 
    m_parent(parent),
    m_invert_parent(invert_parent),
    m_environment(env)
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
 * The name of this parameter. This name is used as a label for the parameter.
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
 * The value converted to a QString. Needs to be specified for inheriting classes.
 * This is the default method for the value serialization performed by
 * serialize.
 *
 * \return The value of the parameter converted to an QString, here "".
 */
QString Parameter::toString() const
{
	return "";
}

/**
 * Sets the value using a QString. 
 * This is the default method for the value deserialization performed by
 * deserialize.
 *
 * \param str The value of the parameter converted to an QString
 * \return True, if the value could be restored. Here, always true.
 */
bool Parameter::fromString(QString& str)
{
    return true;
}

/**
 * Serialization of the parameter's state to a xml stream.
 * Writes the following XML code by default:
 * 
 * <TYPENAME>
 *     <Name>NAME</Name>
 *     <Value>VALUETEXT</Value>
 * </TYPENAME>
 *
 * with TYPENAME = typeName(),
 *         NAME = name(), and
 *    VALUETEXT = toString().
 *
 * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
 *                  parameter's type, name and value.
 */
void Parameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("Value", toString());
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an xml stream.
 *
 * \param xmlReader The QXmlStreamReader from which we read.
 * \return True, if the deserialization was successful, else false.
 */
bool Parameter::deserialize(QXmlStreamReader& xmlReader)
{
    bool success = false;
    
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
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
 * This function indicates whether the value of a parameter is a Model* or 
 * many of them or needs one at least. These parameters need to access the
 * global 'models' variable, too!
 *
 * \return A filled vector, if the parameter's value is related to a Model*.
 *         An empty vector by default.
 */
std::vector<Model*> Parameter::needsModels() const
{
    return std::vector<Model*>();
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
