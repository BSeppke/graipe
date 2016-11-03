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

#include "core/parameters/pointparameter.hxx"

#include <QLabel>
#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the PointParameter class with a setting of the
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
PointParameter::PointParameter(const QString& name, QPoint low, QPoint upp, QPoint value, Parameter* parent, bool invert_parent)
:   Parameter(name, parent, invert_parent),
    m_delegate(new QWidget),
    m_spbXDelegate(new QSpinBox),
    m_spbYDelegate(new QSpinBox)
{
    setRange(low, upp);
    setValue(value);
    
    m_spbXDelegate->setMaximumSize(9999,9999);
    m_spbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    
    m_spbYDelegate->setMaximumSize(9999,9999);
    m_spbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
    QHBoxLayout * layout = new QHBoxLayout(m_delegate);
    
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(new QLabel("x:"));
    layout->addWidget(m_spbXDelegate);
    layout->addWidget(new QLabel("y:"));
    layout->addWidget(m_spbYDelegate);
    
    connect(m_spbXDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
    connect(m_spbYDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
    initConnections();
}

/**
 * Destructor of the PointParameter class.
 */
PointParameter::~PointParameter()
{
    //Also deletes other widget, since they are owned
    //by the assigned layout.
    delete m_delegate;
    m_delegate=NULL;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "PointParameter".
 */
QString  PointParameter::typeName() const
{
    return "PointParameter";
}

/**
 * The lowest possible value of this parameter.
 *
 * \return The minimal value of this parameter.
 */
QPoint PointParameter::lowerBound() const
{
    return QPoint(m_spbXDelegate->minimum(), m_spbYDelegate->minimum());
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void PointParameter::setLowerBound(const QPoint& value)
{
    m_spbXDelegate->setMinimum(value.x());
    m_spbYDelegate->setMinimum(value.y());
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
QPoint PointParameter::upperBound() const
{
    return QPoint(m_spbXDelegate->maximum(), m_spbYDelegate->maximum());
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void PointParameter::setUpperBound(const QPoint& value)
{
    m_spbXDelegate->setMaximum(value.x());
    m_spbYDelegate->setMaximum(value.y());
}

/**
 * Writing accessor of the minimum and maximum value of this parameter.
 *
 * \param min_value The new minimum value of this parameter.
 * \param max_value The new maximum value of this parameter.
 */
void PointParameter::setRange(const QPoint& min_value, const QPoint& max_value)
{
    setLowerBound(min_value);
    setUpperBound(max_value);
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
QPoint PointParameter::value() const
{
    return QPoint(m_spbXDelegate->value(), m_spbYDelegate->value());
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void PointParameter::setValue(const QPoint& value)
{
    m_spbXDelegate->setValue(value.x());
    m_spbYDelegate->setValue(value.y());
    Parameter::updateValue();
}

/**
 * The value converted to a QString. Please note, that this can vary from the 
 * serialize() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars
 * inside the QString.
 *
 * \return The value of the parameter converted to an QString.
 */
QString  PointParameter::valueText() const
{
    return QString("(%1x%2)").arg(value().x()).arg(value().y());
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void PointParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", "+ valueText(), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool PointParameter::deserialize(QIODevice& in)
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
            setValue(QPoint(xy[0].toInt(),xy[1].toInt()));
            return true;
        }
        catch (...)
        {
            qDebug("PointParameter deserialize: point could not be imported from file");
        }
    }
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool PointParameter::isValid() const
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
QWidget*  PointParameter::delegate()
{
    return m_delegate;
}

} //end of namespace graipe
