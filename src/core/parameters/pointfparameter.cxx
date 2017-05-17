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

#include "core/parameters/pointfparameter.hxx"

#include <QLabel>
#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the PointFParameter class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the PointFParameter class with a setting of the
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
PointFParameter::PointFParameter(const QString& name, QPointF low, QPointF upp, QPointF value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_value(value),
    m_min_value(low),
    m_max_value(upp),
    m_delegate(NULL),
    m_dsbXDelegate(NULL),
    m_dsbYDelegate(NULL)
{
}

/**
 * Destructor of the PointFParameter class.
 */
PointFParameter::~PointFParameter()
{
    //Also deletes other widget, since they are owned
    //by the assigned layout.
    if(m_delegate != NULL)
        delete m_delegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "PointFParameter".
 */
QString  PointFParameter::typeName() const
{
    return "PointFParameter";
}
/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
QPointF PointFParameter::lowerBound() const
{
    return m_min_value;//QPointF(m_dsbXDelegate->minimum(), m_dsbYDelegate->minimum());
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void PointFParameter::setLowerBound(const QPointF& value)
{
    m_min_value = value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setMinimum(value.x());
        m_dsbYDelegate->setMinimum(value.y());
    }
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
QPointF PointFParameter::upperBound() const
{
    return m_max_value;//QPointF(m_dsbXDelegate->maximum(), m_dsbYDelegate->maximum());
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void PointFParameter::setUpperBound(const QPointF& value)
{
    m_max_value = value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setMaximum(value.x());
        m_dsbYDelegate->setMaximum(value.y());
    }
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void PointFParameter::setRange(const QPointF& min_value, const QPointF& max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
QPointF PointFParameter::value() const
{
    return m_value;//QPointF(m_dsbXDelegate->value(), m_dsbYDelegate->value());
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void PointFParameter::setValue(const QPointF& value)
{
    m_value=value;
    
    if(m_delegate)
    {
        m_dsbXDelegate->setValue(value.x());
        m_dsbYDelegate->setValue(value.y());
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
QString  PointFParameter::valueText() const
{
    return QString("(") + QString::number(value().x(),'g', 10) + "x" + QString::number(value().y(),'g', 10) + ")";
}
/**
 * Serialization of the parameter's state to an output device.
 * Writes the following XML on the device:
 * 
 * <MAGICID>
 *     <Name>NAME</Name>
 *     <x>X</x>
 *     <y>Y</y>
 * </MAGICID>
 *
 * with MAGICID = magicID(),
 *         NAME = name(),
 *            X = value().x(), and
 *            Y = value().y().
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void PointFParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(magicID());
    xmlWriter.writeTextElement("Name", name());
    xmlWriter.writeTextElement("x", QString::number(value().x(),'g', 10));
    xmlWriter.writeTextElement("y", QString::number(value().y(),'g', 10));
    xmlWriter.writeEndElement();

}
/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
 * \return True, if the deserialization was successful, else false.
 */
bool PointFParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    QString content(in.readLine().trimmed());
    
    //Cut off "(" and ")"
    QString tmp = content.mid(1, content.size()-2);
    
    //Split QString
    QStringList xy = tmp.split("x");
    
    //Check and read in data if possible
    if(xy.size()==2)
    {
        try
        {
            setValue(QPointF(xy[0].toFloat(),xy[1].toFloat()));
            return true;
        }
        catch (...)
        {
            qDebug() << "PointFParameter deserialize: point could not be imported from file. Was: '" << content << "'";
        }
    }
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool PointFParameter::isValid() const
{
    return m_value.x() >= m_min_value.x() && m_value.y() >= m_min_value.y()
        && m_value.x() <= m_max_value.x() && m_value.y() <= m_max_value.y();}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  PointFParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_dsbXDelegate = new QDoubleSpinBox;
        m_dsbYDelegate = new QDoubleSpinBox;
  
        m_dsbXDelegate->setMaximumSize(9999,9999);
        m_dsbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_dsbXDelegate->setRange(m_min_value.x(), m_max_value.x());
        m_dsbXDelegate->setValue(m_value.x());
        
        m_dsbYDelegate->setMaximumSize(9999,9999);
        m_dsbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        m_dsbYDelegate->setRange(m_min_value.y(), m_max_value.y());
        m_dsbYDelegate->setValue(m_value.y());
  
        QHBoxLayout * layout = new QHBoxLayout(m_delegate);
        
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(new QLabel("x:"));
        layout->addWidget(m_dsbXDelegate);
        layout->addWidget(new QLabel("y:"));
        layout->addWidget(m_dsbYDelegate);
    
        
        connect(m_dsbXDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        connect(m_dsbYDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void PointFParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_delegate != NULL)
    {
        m_value.setX(m_dsbXDelegate->value());
        m_value.setY(m_dsbYDelegate->value());
        Parameter::updateValue();
    }
}

} //end of namespace graipe

/**
 * @}
 */
