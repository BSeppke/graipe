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
    m_delegate(NULL),
    m_dsbXDelegate(NULL),
    m_dsbYDelegate(NULL),
    m_value(value),
    m_min(low),
    m_max(upp)
{
}

/**
 * Destructor of the PointFParameter class.
 */
PointFParameter::~PointFParameter()
{
    if(m_delegate != NULL)
    {
        //Also deletes other widget, since they are owned
        //by the assigned layout.
        delete m_delegate;
        m_delegate=NULL;
    }
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
const QPointF& PointFParameter::lowerBound() const
{
    return m_min;
}

/**
 * Writing accessor of the minimum value of this parameter.
 *
 * \param value The new minimum value of this parameter.
 */
void PointFParameter::setLowerBound(const QPointF& value)
{
    m_min = value;
    
    if(m_delegate != NULL)
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
const QPointF& PointFParameter::upperBound() const
{
    return m_max;
}

/**
 * Writing accessor of the maximum value of this parameter.
 *
 * \param value The new maximum value of this parameter.
 */
void PointFParameter::setUpperBound(const QPointF& value)
{
    m_max = value;
    
    if(m_delegate != NULL)
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
const QPointF& PointFParameter::value() const
{
    return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void PointFParameter::setValue(const QPointF& value)
{
    if(   (value.x() >= lowerBound().x() && value.y() >= lowerBound().y())
       && (value.x() <  upperBound().x() && value.y() <  upperBound().y()))
    {
        m_value = value;
        
        if(m_delegate != NULL)
        {
            m_dsbXDelegate->setValue(value.x());
            m_dsbYDelegate->setValue(value.y());
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
QString  PointFParameter::valueText() const
{
    return QString("(") + QString::number(m_value.x(),'g', 10) + "x" + QString::number(m_value.y(),'g', 10) + ")";
}
/**
 * Serialization of the parameter's state to a QString. Please note, that this can
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void PointFParameter::serialize(QIODevice& out) const
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
QWidget*  PointFParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QWidget;
        m_dsbXDelegate = new QDoubleSpinBox;
        m_dsbYDelegate = new QDoubleSpinBox;
        
        m_dsbXDelegate->setMinimum(m_min.x());
        m_dsbXDelegate->setMaximum(m_max.x());
        m_dsbXDelegate->setValue(m_value.x());
        m_dsbXDelegate->setMaximumSize(9999,9999);
        m_dsbXDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        m_dsbYDelegate->setMinimum(m_min.y());
        m_dsbYDelegate->setMaximum(m_max.y());
        m_dsbYDelegate->setValue(m_value.y());
        m_dsbYDelegate->setMaximumSize(9999,9999);
        m_dsbYDelegate->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        
        QHBoxLayout * layout = new QHBoxLayout(m_delegate);
        
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(new QLabel("x:"));
        layout->addWidget(m_dsbXDelegate);
        layout->addWidget(new QLabel("y:"));
        layout->addWidget(m_dsbYDelegate);
        
        initConnections();
    }
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void PointFParameter::updateValue()
{
    if(m_delegate != NULL)
    {
        m_value.setX(m_dsbXDelegate->value());
        m_value.setY(m_dsbYDelegate->value());
        
        Parameter::updateValue();
    }
}
/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void PointFParameter::initConnections()
{
    connect(m_dsbXDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
    connect(m_dsbYDelegate, SIGNAL(valueChanged(double)), this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
