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

#include <QtDebug>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

/**
 * @file
 * @brief Implementation file for the BoolParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the BoolParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
BoolParameter::BoolParameter(const QString& name, bool value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
	m_delegate(NULL)
{    
}

/**
 * The destructor of the BoolParameter class.
 */
BoolParameter::~BoolParameter()
{
    if(m_delegate!=NULL)
        delete m_delegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "BoolParameter".
 */
QString BoolParameter::typeName()
{
	return "BoolParameter";
}

/**
 * The current value of this parameter in the correct, most special type
 *
 * \return The value of this parameter.
 */
bool BoolParameter::value()  const
{
	return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void BoolParameter::setValue(bool value)
{
    m_value = value;
    
    if(m_delegate!=NULL)
        m_delegate->setChecked(value);
   
    Parameter::updateValue();
}

/**
 * The value converted to a QString. Either true or false.
 *
 * \return The value of the parameter converted to an QString
 */
QString BoolParameter::toString() const
{
    return (value()?"true":"false");
}

/**
 * Deserialization of a parameter's state from a string.
 *
 * \param str The input QString.
 * \return True, if the deserialization was successful, else false.
 */
bool BoolParameter::fromString(QString& str)
{
    if (str == "true" || str == "false")
    {
        setValue(str == "true");
        return true;
    }
    else
    {
        qDebug() << "BoolParameter deserialize: value has to be either 'true' or 'false' in file, but found: " << str;
    }
    return false;
}
    
/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool BoolParameter::isValid() const
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
QWidget*  BoolParameter::delegate()
{
    if (m_delegate == NULL)
    {
        m_delegate = new QCheckBox;
        m_delegate->setChecked(value());
        
        connect(m_delegate, SIGNAL(clicked()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void BoolParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->isChecked();
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */
