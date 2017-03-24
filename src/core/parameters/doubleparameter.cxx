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

#include "core/parameters/doubleparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the DoubleParameter class with a setting of the
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
DoubleParameter::DoubleParameter(const QString& name, double low, double upp, double value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_dsbDelegate(NULL)
{
}

/**
 * Destructor of the DoubleParameter class.
 */
DoubleParameter::~DoubleParameter()
{
    if(m_dsbDelegate == NULL)
        delete m_dsbDelegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "DoubleParameter".
 */
QString  DoubleParameter::typeName() const
{
	return "DoubleParameter";
}

/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
double DoubleParameter::lowerBound() const
{
    return m_min_value;//m_dsbDelegate->minimum();
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void DoubleParameter::setLowerBound(double value)
{
    m_min_value = value;
    
    if(m_dsbDelegate != NULL)
        m_dsbDelegate->setMinimum(value);
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
double DoubleParameter::upperBound() const
{
    return m_max_value;//m_dsbDelegate->maximum();
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void DoubleParameter::setUpperBound(double value)
{
    m_max_value = value;
    
    if(m_dsbDelegate != NULL)
        m_dsbDelegate->setMaximum(value);
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void DoubleParameter::setRange(double min_value, double max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
double DoubleParameter::value() const
{
    return m_value;//m_dsbDelegate->value();
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void DoubleParameter::setValue(double value)
{
    m_value = value;
    
    if(m_dsbDelegate != NULL)
    {
        m_dsbDelegate->setValue(value);
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
QString DoubleParameter::valueText() const
{
	return QString::number(value(),'g', 10);
}

/**
 * Serialization of the parameter's state to an output device.
 * Basically, just: "DoubleParameter, " + valueText()
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void DoubleParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + valueText(), out);
}

/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
 * \return True, if the deserialization was successful, else false.
 */
bool DoubleParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    QString content(in.readLine().trimmed());

    try
    {
        double val = content.toDouble();
        setValue(val);
        
        return true;
    }
    catch (...)
    {
        qCritical() << "DoubleParameter deserialize: value could not be imported from: '" << content << "'";
    }
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool DoubleParameter::isValid() const
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
QWidget*  DoubleParameter::delegate()
{
    if(m_dsbDelegate == NULL)
    {
        m_dsbDelegate = new QDoubleSpinBox;
    
        m_dsbDelegate->setDecimals(3);
        m_dsbDelegate->setRange(lowerBound(),upperBound());
        m_dsbDelegate->setValue(value());
        
        connect(m_dsbDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_dsbDelegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void DoubleParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_dsbDelegate != NULL)
    {
        m_value = m_dsbDelegate->value();
        Parameter::updateValue();
    }
}

} //end of namespace graipe
