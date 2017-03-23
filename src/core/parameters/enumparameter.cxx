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

#include "core/parameters/enumparameter.hxx"

#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the EnumParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param enum_names    All allowed enum labels of this parameter..
 * \param value         The initial (index, 0-starting) value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
EnumParameter::EnumParameter(const QString& name, const QStringList & enum_names, int value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_enum_names(enum_names),
    m_value(value),
    m_cmbDelegate(NULL)
{
}

/**
 * Destructor of the EnumParameter class
 */
EnumParameter::~EnumParameter()
{
    if(m_cmbDelegate != NULL)
        delete m_cmbDelegate;
}

/**
 * The (immutable) type name of this parameter class.
 *
 * \return "EnumParameter".
 */
QString  EnumParameter::typeName() const
{
	return "EnumParameter";
}

/** 
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter. (-1 on error)
 */
int EnumParameter::value() const
{
	return m_value;// m_cmbDelegate->currentIndex();
}
    
/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void EnumParameter::setValue(int value)
{
    m_value = value;
    
    if(m_cmbDelegate != NULL)
    {
    	m_cmbDelegate->setCurrentIndex(value);
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
QString EnumParameter::valueText() const
{
    if (isValid())
    	return m_enum_names[value()];
    else
        return "";
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void EnumParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + QString("%1").arg(value()), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool EnumParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    try
    {
        QString content(in.readLine().trimmed());
        
        unsigned int idx = content.toUInt();
        setValue(idx);
        
        return true;
    }
    catch (...)
    {
        qDebug("EnumParameter deserialize: enum value could not be imported from file");
    }
    
    return false;
}
    
/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
bool EnumParameter::isValid() const
{
    return value() > -1 && value() < m_enum_names.size();
}

/**
 * The delegate widget of this parameter. 
 * Each parameter generates such a widget on demand, which refers to the
 * first call of this function. This is needed due to the executability of
 * classes using parameters (like the Algorithm class) in different threads.
 *
 * \return The delegate widget to control the values of this parameter.
 */
QWidget*  EnumParameter::delegate()
{
    if(m_cmbDelegate == NULL)
    {
        m_cmbDelegate = new QComboBox;
        for(int v=0; v<m_enum_names.size(); ++v)
        {
            m_cmbDelegate->addItem(m_enum_names[v]);
        }
        m_cmbDelegate->setCurrentIndex(m_value);
    
        connect(m_cmbDelegate, SIGNAL(clicked()), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    return m_cmbDelegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void EnumParameter::updateValue()
{
    //Should not happen - otherwise, better safe than sorry:
    if(m_cmbDelegate != NULL)
    {
        m_value = m_cmbDelegate->currentIndex();
        Parameter::updateValue();
    }
}

} //end of namespace graipe
