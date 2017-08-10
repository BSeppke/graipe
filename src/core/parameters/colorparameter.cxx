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

#include "core/parameters/colorparameter.hxx"

#include <QtDebug>
#include <QXmlStreamWriter>
#include <QObject>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the ColorParameter class
 * @}
 */

ColorParameter::ColorParameter(const QString& name, QColor value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_value(value),
    m_delegate(NULL)
{
}

ColorParameter::~ColorParameter()
{
    if(m_delegate!=NULL)
        delete m_delegate;
}

const QColor& ColorParameter::value() const
{
    return m_value;
}

void ColorParameter::setValue(const QColor& value)
{
    m_value = value;
    
    if(m_delegate!=NULL)
    {
        QPixmap p(32, 32);
        p.fill(value);
        m_delegate->setIcon(QIcon(p));
        m_delegate->setText(toString());
    
        Parameter::updateValue();
    }
}

QString  ColorParameter::toString() const
{
    return m_value.name(QColor::HexArgb);
}

bool ColorParameter::fromString(QString & str)
{
    QColor new_color(str);
        
    if(new_color.isValid())
    {
        setValue(new_color);
        return true;
    }
    else
    {
        qDebug() << "ColorParameter deserialize: value has to be in #AARRGGBB or #RRGGBB format, but found: " << str;
        return false;
    }
}

 bool ColorParameter::isValid() const
{
    return m_value.isValid();
}

QWidget*  ColorParameter::delegate()
{
    if (m_delegate == NULL)
    {
        m_delegate = new QPushButton(toString());
        
        QPixmap p(32, 32);
        p.fill(value());
        m_delegate->setIcon(QIcon(p));
    
        connect(m_delegate, SIGNAL(clicked()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    
    return m_delegate;
}

void ColorParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        QColor col = QColorDialog::getColor(m_value, m_delegate, name());
        
        if(col.isValid())
        {
            setValue(col);
        }
    }
}

} //end of namespace graipe
