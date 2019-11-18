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

#include "core/parameters/stringparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the StringParameter class
 * @}
 */

StringParameter::StringParameter(const QString& name, const QString& value, unsigned int columns, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_columns(columns),
    m_delegate(NULL)
{
}

StringParameter::~StringParameter()
{
   if(m_delegate != NULL)
        delete m_delegate;
}

QString StringParameter::value()  const
{
	return m_value;
}

void StringParameter::setValue(const QString & value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_delegate->setText(value);
        Parameter::updateValue();
    }
}

QString  StringParameter::toString() const
{
	return value();
}

bool StringParameter::fromString(QString& str)
{
    setValue(str);
    return true;
}

bool StringParameter::isValid() const
{
    return true;
}

QWidget*  StringParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QLineEdit;
        m_delegate->setMinimumWidth(m_columns * m_delegate->fontMetrics().horizontalAdvance("X"));
        m_delegate->setText(value());
        
        connect(m_delegate, SIGNAL(textChanged(const QString&)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

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
