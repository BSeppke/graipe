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

#include "core/parameters/longstringparameter.hxx"

#include <QFontMetrics>
#include <QtDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implemenation file for the LongStringParameter class
 * @}
 */

LongStringParameter::LongStringParameter(const QString& name, const QString& value, unsigned int columns, unsigned int lines, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_columns(columns),
    m_lines(lines),
    m_delegate(NULL)
{
}

LongStringParameter::~LongStringParameter()
{
    if(m_delegate != NULL)
        delete m_delegate;
}

QString LongStringParameter::value()  const
{
    return m_value;
}

void LongStringParameter::setValue(const QString & value)
{
    m_value = value;
    
    if(m_delegate != NULL)
    {
        m_delegate->setPlainText(value);
    }
}

QString  LongStringParameter::toString() const
{
    return value();
}

bool LongStringParameter::fromString(QString& str)
{
    setValue(str);
    return true;
}

bool LongStringParameter::isValid() const
{
    return true;
}

QWidget*  LongStringParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QPlainTextEdit;
    
        m_delegate->setMinimumWidth(m_columns * m_delegate->fontMetrics().horizontalAdvance("X"));
        m_delegate->setMinimumHeight(m_lines * m_delegate->fontMetrics().lineSpacing());
        m_delegate->setPlainText(m_value);
    
        connect(m_delegate, SIGNAL(textChanged()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

void LongStringParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value = m_delegate->toPlainText();
        Parameter::updateValue();
    }
}

} //end of namespace graipe
