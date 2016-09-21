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
    m_delegate(NULL),
    m_spbXDelegate(NULL),
    m_spbYDelegate(NULL),
    m_value(value),
    m_min(low),
    m_max(upp)
{
}

/**
 * Destructor of the PointParameter class.
 */
PointParameter::~PointParameter()
{
    if(m_delegate != NULL)
    {
        delete m_spbXDelegate;
        m_spbXDelegate=NULL;
        
        delete m_spbYDelegate;
        m_spbYDelegate=NULL;
        
        delete m_delegate;
        m_delegate=NULL;
        
    }
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
const QPoint& PointParameter::lowerBound() const
{
    return m_min;
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void PointParameter::setLowerBound(const QPoint& value)
{
    m_min = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMinimum(value.x());
        m_spbYDelegate->setMinimum(value.y());
    }
}

/**
 * The highest possible value of this parameter.
 *
 * \return The maximal value of this parameter.
 */
const QPoint& PointParameter::upperBound() const
{
    return m_max;
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void PointParameter::setUpperBound(const QPoint& value)
{
    m_max = value;
    
    if(m_delegate != NULL)
    {
        m_spbXDelegate->setMaximum(value.x());
        m_spbYDelegate->setMaximum(value.y());
    }
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
const QPoint& PointParameter::value() const
{
    return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void PointParameter::setValue(const QPoint& value)
{
    if(   (value.x() >= lowerBound().x() && value.y() >= lowerBound().y())
       && (value.x() <  upperBound().x() && value.y() <  upperBound().y()))
    {
        m_value =  value;
        
        if(m_delegate != NULL)
        {
            m_spbXDelegate->setValue(value.x());
            m_spbYDelegate->setValue(value.y());
        }
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
QString  PointParameter::valueText() const
{
    return QString("(%1x%2)").arg(m_value.x()).arg(m_value.y());
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
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_spbXDelegate = new QSpinBox;
        m_spbYDelegate = new QSpinBox;
        
        m_spbXDelegate->setMinimum(m_min.x());
        m_spbXDelegate->setMaximum(m_max.x());
        m_spbXDelegate->setValue(m_value.x());
        m_spbXDelegate->setMaximumSize(9999,9999);
        m_spbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        m_spbYDelegate->setMinimum(m_min.y());
        m_spbYDelegate->setMaximum(m_max.y());
        m_spbYDelegate->setValue(m_value.y());
        m_spbYDelegate->setMaximumSize(9999,9999);
        m_spbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        QHBoxLayout * layout = new QHBoxLayout;
        m_delegate->setLayout(layout);
        
        layout->addWidget(new QLabel("x:"));
        layout->addWidget(m_spbXDelegate);
        layout->addWidget(new QLabel("y:"));
        layout->addWidget(m_spbYDelegate);
        
        initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void PointParameter::updateValue()
{
    if(m_delegate != NULL)
    {
        m_value.setX(m_spbXDelegate->value());
        m_value.setY(m_spbYDelegate->value());
        
        Parameter::updateValue();
    }
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void PointParameter::initConnections()
{
    connect(m_spbXDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
    connect(m_spbYDelegate, SIGNAL(valueChanged(int)), this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
