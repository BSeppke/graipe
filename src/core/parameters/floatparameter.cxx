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

#include "core/parameters/floatparameter.hxx"

#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the FloatParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the FloatParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param low           The lowest allowed value of this parameter.
 * \param upp           The highest allowed value of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
FloatParameter::FloatParameter(const QString& name, float low, float upp, float value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL)
{
}

/**
 * Destructor of the FloatParameter class.
 */
FloatParameter::~FloatParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "FloatParameter".
 */
QString  FloatParameter::typeName() const
{
	return "FloatParameter";
}

/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
float FloatParameter::lowerBound() const
{
    return m_min_value;
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void FloatParameter::setLowerBound(float value)
{
    m_min_value = value;
    
    if(m_delegate != NULL)
        m_delegate->setMinimum(value);
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
float FloatParameter::upperBound() const
{
    return m_max_value;
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void FloatParameter::setUpperBound(float value)
{
    m_max_value = value;
    
    if(m_delegate != NULL)
        m_delegate->setMaximum(value);
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void FloatParameter::setRange(float min_value, float max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
float FloatParameter::value() const
{
    return m_value;//m_delegate->value();
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void FloatParameter::setValue(float value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_delegate->setValue(value);
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
QString  FloatParameter::toString() const
{
	return QString::number(value(),'g', 10);
}

/**
 * Deserialization of a parameter's state from a string.
 *
 * \param str the input QString.
 * \return True, if the deserialization was successful, else false.
 */
bool FloatParameter::fromString(QString& str)
{
    try
    {
        double val = str.toFloat();
        setValue(val);
        
        return true;
    }
    catch (...)
    {
        qCritical() << "FloatParameter deserialize: value could not be imported from: '" << str << "'";
    }
    return false;
}


/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool FloatParameter::isValid() const
{
    return value() >= lowerBound() && value() <= upperBound();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  FloatParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QDoubleSpinBox;
   
        m_delegate->setDecimals(3);
        m_delegate->setRange(lowerBound(), upperBound());
        m_delegate->setValue(value());
        
        connect(m_delegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void FloatParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->value();
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */

