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

#include "core/parameters/colortableparameter.hxx"

#include <QColorDialog>
#include <QtDebug>

namespace graipe {

/**
 * Default constructor of the ColorTableParameter class with a setting of the
 * most important values directly.
 *
 * \param name          The name (label) of this parameter.
 * \param value         The initial value of this parameter.
 * \param parent        If given (!= NULL), this parameter has a parent and will
 *                      be enabled/disabled, if the parent is a BoolParameter.
 * \param invert_parent If true, the enables/disabled dependency to the parent will be swapped.
 */
ColorTableParameter::ColorTableParameter(const QString& name, QVector<QRgb> value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_delegate(NULL),
    m_value(value),
    m_colorTable_id(0)
{
}

/**
 * The destructor of the ColorTableParameter class.
 */
ColorTableParameter::~ColorTableParameter()
{
    if(m_delegate != NULL)
    {
        delete m_delegate;
        m_delegate=NULL;
    }
}
    
/**
 * The (immutable) type name of this parameter class.
 *
 * \return "ColorTableParameter".
 */
QString ColorTableParameter::typeName() const
{
    return "ColorTableParameter";
}

/**
 * The current value of this parameter in the correct, most special type.
 *
 * \return The value of this parameter.
 */
const QVector<QRgb>& ColorTableParameter::value() const
{
    return m_value;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void ColorTableParameter::setValue(const QVector<QRgb>& value)
{
    bool found = false;
    
    //search in std. color tables
    for(unsigned int ct=0; ct != colorTables().size(); ++ct)
    {
        if(colorTables()[ct] == value)
        {
            found=true;
            m_value = value;
            m_colorTable_id = ct;
            break;
        }
    }
    
    //search in extra (user defined) color tables
    for(unsigned int extra_ct=0; extra_ct != m_extra_tables.size(); ++extra_ct)
    {
        if(m_extra_tables[extra_ct] == value)
        {
            found=true;
            m_value = value;
            m_colorTable_id = colorTables().size()+extra_ct;
            break;
        }
    }
    
    //if not found, add a new one
    if(!found)
    {
        addCustomColorTable(value);
        m_colorTable_id = colorTables().size() + m_extra_tables.size() - 1;
    }
    
    //if delegate is available, update index
    if(m_delegate!= NULL)
    {
        m_delegate->setCurrentIndex(m_colorTable_id);
    }
}

/**
 * Add another (user defined) color table to this parameter.
 *
 * \param ct The new user defined ct of this parameter.
 */
void ColorTableParameter::addCustomColorTable(const QVector<QRgb>& ct)
{
    if(ct.size()==256)
    {
        m_extra_tables.push_back(ct);
        m_value = ct;
        m_colorTable_id = colorTables().size() + m_extra_tables.size() - 1;
        
        if(m_delegate != NULL)
        {
            //Remove last entry (Create custom ct)
            m_delegate->removeItem(m_delegate->count()-1);
            
            //Add this (new) color table
            unsigned int w=256, h=16;
            m_delegate->setIconSize(QSize(w, h));
        
            QImage img(w, h, QImage::Format_Indexed8);
            img.setColorCount(256);
        
            for(unsigned int y=0; y<h; ++y)
            {
                for(unsigned int x=0; x<w; ++x)
                {
                    img.setPixel(x,y, w/256.0*x);
                }
            }
            img.setColorTable(ct);
            m_delegate->addItem(QPixmap::fromImage(img),"");
            
            //Finally add the custom entry again
            m_delegate->addItem("Add new");
            
            m_delegate->setCurrentIndex(m_colorTable_id);
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
QString  ColorTableParameter::valueText() const
{
    QString str = QString::number(m_value[0]);
    for(unsigned int i=1; i<m_value.size(); ++i)
    {
        str += ", " + QString::number(m_value[i]);
    }
    
    return str;
}

/**
 * Serialization of the parameter's state to a QString. Please note, that this can 
 * vary from the valueText() result, which also returns a QString. This is due to the fact,
 * that serialize also may perform encoding of QStrings to avoid special chars.
 *
 * \return The serialization of the parameter's state.
 */
void ColorTableParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + valueText(), out);
}

/**
 * Deserialization of a parameter's state from a QString.
 *
 * \param str The serialization of this parameter's state.
 * \return True, if the deserialization was successful, else false.
 */
bool ColorTableParameter::deserialize(QIODevice& in)
{
    if(!Parameter::deserialize(in))
    {
        return false;
    }
    
    QString content(in.readLine().trimmed());
    QStringList content_list = split_string(content, ", ");
    try
    {
        if(content_list.size() != 256)
        {
            throw std::runtime_error("Error: Did not find 256 entries");
        }
        
        QVector<QRgb> ct(256);
        for(unsigned int i=0; i<ct.size(); ++i)
        {
            ct[i] = content_list[i].toUInt();
        }
        setValue(ct);
        
        return true;
    }
    catch (...)
    {
        qDebug() << "ColorTableParameter deserialize: value has to be an integer list in file, but found: " << content;
    }
    return false;
}

/**
 * This function indicates whether the value of a parameter is valid or not.
 *
 * \return True, if the parameter's value is valid.
 */
 bool ColorTableParameter::isValid() const
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
QWidget*  ColorTableParameter::delegate()
{
    if(m_delegate == NULL)
    {
        m_delegate = new QComboBox();
        
        unsigned int w=256, h=16;
        m_delegate->setIconSize(QSize(w, h));
        
        QImage img(w, h, QImage::Format_Indexed8);
        img.setColorCount(256);
        
        for(unsigned int y=0; y<h; ++y)
        {
            for(unsigned int x=0; x<w; ++x)
            {
                img.setPixel(x,y, w/256.0*x);
            }
        }
        
        for( QVector<QRgb> ct : colorTables())
        {
            img.setColorTable(ct);
            m_delegate->addItem(QPixmap::fromImage(img),"");
        
        }
        
        for( QVector<QRgb> ct : m_extra_tables)
        {
            img.setColorTable(ct);
            m_delegate->addItem(QPixmap::fromImage(img),"");
        
        }
        m_delegate->addItem("Add new");
        setValue(m_value);
        initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ColorTableParameter::updateValue()
{
    if(m_delegate)
    {
        int idx = m_delegate->currentIndex();
    
        if(idx >= 0 && idx < colorTables().size())
        {
            m_value = (colorTables()[idx]);
            m_colorTable_id = idx;
        }
        else if(idx >= colorTables().size() && idx < m_delegate->count()-1)
        {
            m_value = m_extra_tables[idx-colorTables().size()];
            m_colorTable_id = idx;
        }
        else if(idx == m_delegate->count()-1)
        {
            QColor col = QColorDialog::getColor(Qt::red, m_delegate, name());
        
            if(col.isValid())
            {
                QVector<QRgb> ct(256, qRgb(col.red(), col.green(), col.blue()));
                setValue(ct);
            }
        }
    }
    Parameter::updateValue();
}

/**
 * Initializes the connections (signal<->slot) between the parameter class and
 * the delegate widget. This will be done after the first call of the delegate()
 * function, since the delegate is NULL until then.
 */
void ColorTableParameter::initConnections()
{
    connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
    Parameter::initConnections();
}

} //end of namespace graipe
