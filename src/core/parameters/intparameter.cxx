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

#include "core/parameters/intparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the IntParameter class
 * @}
 */

IntParameter::IntParameter(const QString& name, int low, int upp, int value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL)
{
}

IntParameter::~IntParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

int IntParameter::lowerBound() const
{
    return m_min_value;
}

void IntParameter::setLowerBound(int value)
{
    m_min_value = value;

    if(m_delegate != NULL)
        m_delegate->setMinimum(value);
}

int IntParameter::upperBound() const
{
    return m_max_value;
}

void IntParameter::setUpperBound(int value)
{
    m_max_value = value;

    if(m_delegate != NULL)
        m_delegate->setMaximum(value);
}

void IntParameter::setRange(int min_value, int max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

int IntParameter::value() const
{
    return m_value;
}

void IntParameter::setValue(int value)
{
    m_value = value;
    
    if(m_delegate != NULL)
        m_delegate->setValue(value);
}

QString  IntParameter::toString() const
{
	return QString("%1").arg(value());
}

bool IntParameter::fromString(QString& str)
{
    try
    {
        double val = str.toInt();
        setValue(val);
        
        return true;
    }
    catch (...)
    {
        qCritical() << "IntParameter deserialize: value could not be imported from: '" << str << "'";
    }
    return false;
}

bool IntParameter::isValid() const
{
    return value() >= lowerBound() && value() <= upperBound();
}

QWidget*  IntParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QSpinBox;
   
        m_delegate->setRange(lowerBound(), upperBound());
        m_delegate->setValue(value());
        
        connect(m_delegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

void IntParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->value();
        Parameter::updateValue();
    }
}

} //end of namespace graipe
