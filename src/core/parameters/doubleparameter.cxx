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

#include "core/parameters/doubleparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the DoubleParameter class
 * @}
 */

DoubleParameter::DoubleParameter(const QString& name, double low, double upp, double value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL)
{
}

DoubleParameter::~DoubleParameter()
{
    if(m_delegate == NULL)
        delete m_delegate;
}

double DoubleParameter::lowerBound() const
{
    return m_min_value;//m_delegate->minimum();
}

void DoubleParameter::setLowerBound(double value)
{
    m_min_value = value;
    
    if(m_delegate != NULL)
        m_delegate->setMinimum(value);
}

double DoubleParameter::upperBound() const
{
    return m_max_value;//m_delegate->maximum();
}

void DoubleParameter::setUpperBound(double value)
{
    m_max_value = value;
    
    if(m_delegate != NULL)
        m_delegate->setMaximum(value);
}

void DoubleParameter::setRange(double min_value, double max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

double DoubleParameter::value() const
{
    return m_value;//m_delegate->value();
}

void DoubleParameter::setValue(double value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_delegate->setValue(value);
        Parameter::updateValue();
    }
}

QString DoubleParameter::toString() const
{
	return QString::number(value(),'g', 10);
}

bool DoubleParameter::fromString(QString& str)
{
    try
    {
        double val = str.toDouble();
        setValue(val);
        
        return true;
    }
    catch (...)
    {
        qCritical() << "DoubleParameter deserialize: value could not be imported from: '" << str << "'";
    }
    return false;
}

bool DoubleParameter::isValid() const
{
    return value() >= lowerBound() && value() <= upperBound();
}

QWidget*  DoubleParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QDoubleSpinBox;
    
        m_delegate->setDecimals(3);
        m_delegate->setRange(lowerBound(),upperBound());
        m_delegate->setValue(value());
        
        connect(m_delegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

void DoubleParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->value();
        Parameter::updateValue();
    }
}

} //end of namespace graipe
