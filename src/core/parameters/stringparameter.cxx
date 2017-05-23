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

#include "core/parameters/stringparameter.hxx"

#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the StringParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the StringParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param value         The initial value of this parameter.
 * \param columns       The width of the textfield (in chars).
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
StringParameter::StringParameter(const QString& name, const QString& value, unsigned int columns, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_columns(columns),
    m_delegate(NULL)
{
}

/**
 * Destructor of the StringParameter class.
 */
StringParameter::~StringParameter()
{
   if(m_delegate != NULL)
        delete m_delegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "StringParameter".
 */
QString  StringParameter::typeName() const
{
	return "StringParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
QString StringParameter::value()  const
{
	return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void StringParameter::setValue(const QString & value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_delegate->setText(value);
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
QString  StringParameter::toString() const
{
	return value();
}

/**
 * Sets the value using a QString. This is the default method, used by the desearialize .
 *
 * \param str The value of the parameter converted to an QString
 */
bool StringParameter::fromString(QString& str)
{
    setValue(str);
    return true;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool StringParameter::isValid() const
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
QWidget*  StringParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QLineEdit;
        m_delegate->setMinimumWidth(m_columns * m_delegate->fontMetrics().width("X"));
        m_delegate->setText(value());
        
        connect(m_delegate, SIGNAL(textChanged(const QString&)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}


/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void StringParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->text();
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */
