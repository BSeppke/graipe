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

#include "core/parameters/colorparameter.hxx"
#include "core/parameters/boolparameter.hxx"
#include "core/parameterselection.hxx"

#include <QColorDialog>
#include <QtDebug>
#include <QObject>

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
    m_delegate(new QComboBox)
{
    unsigned int w=256, h=12;
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
    setValue(value);
    
    connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
    Parameter::initConnections();
}

/**
 * The destructor of the ColorTableParameter class.
 */
ColorTableParameter::~ColorTableParameter()
{
    delete m_delegate;
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
QVector<QRgb> ColorTableParameter::value() const
{
    int idx = m_delegate->currentIndex();

    if(idx >= 0 && idx < colorTables().size())
    {
       return colorTables()[idx];
    }
    else if(idx >= colorTables().size() && idx < m_delegate->count()-1)
    {
        return m_extra_tables[idx-colorTables().size()];
    }
    else
    {
        return QVector<QRgb>(256);
    }
}

/**
 * Gives information if a colortable is already knwon or not,
 * either in the system-wide colorTables or in the extraTables
 *
 * \param ct The color table to be checked for.
 * \return Positive index, if the color table is either in colorTables() or in m_extra_tables.
 *         The index corresponds to the comboBox index. Else, -1
 */
int ColorTableParameter::colorTableIndex(const QVector<QRgb> & ct) const
{
    int colorTable_id=-1;
    
    //search in std. color tables
    for(unsigned int i=0; i != colorTables().size(); ++i)
    {
        if(colorTables()[i] == ct)
        {
            colorTable_id = i;
            break;
        }
    }
    
    //search in extra (user defined) color tables
    for(unsigned int i=0; i != m_extra_tables.size(); ++i)
    {
        if(m_extra_tables[i] == ct)
        {
            colorTable_id = colorTables().size()+i;
            break;
        }
    }
    
    return colorTable_id;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void ColorTableParameter::setValue(const QVector<QRgb>& value)
{
    int colorTable_id = colorTableIndex(value);
    
    if(colorTable_id>=0)
    {
        m_delegate->setCurrentIndex(colorTable_id);
        Parameter::updateValue();
    }
    else
    {
        qDebug("ColorTableParameter::setValue: Could not find value to set in colortables");
    }
}

/**
 * Add another (user defined) color table to this parameter.
 *
 * \param ct The new user defined ct of this parameter.
 */
int ColorTableParameter::addCustomColorTable(const QVector<QRgb>& ct)
{
    if(ct.size()==256)
    {
        int colorTable_id = colorTableIndex(ct);
        
        //Already known?
        if(colorTable_id>=0)
        {
            return colorTable_id;
        }
        else
        {
            m_extra_tables.push_back(ct);
            
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
            m_delegate->insertItem(m_delegate->count()-1, QPixmap::fromImage(img),"");
            
            return m_delegate->count()-2;
        }
    }
    else
    {
        return -1;
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
    QVector<QRgb> ct = value();
    
    QString str = QString::number(ct[0]);
    for(unsigned int i=1; i<ct.size(); ++i)
    {
        str += ", " + QString::number(ct[i]);
    }
    
    return str;
}

/**
 * Serialization of the parameter's state to an output device.
 * Basically, just: "ColorTableParameter, " + valueText()
 *
 * \param out The output device on which we serialize the parameter's state.
 */
void ColorTableParameter::serialize(QIODevice& out) const
{
    Parameter::serialize(out);
    write_on_device(", " + valueText(), out);
}

/**
 * Deserialization of a parameter's state from an input device.
 *
 * \param in the input device.
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
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ColorTableParameter::updateValue()
{
    if(m_delegate->currentIndex() == m_delegate->count()-1)
    {
        ParameterGroup* custom_ct_params = new ParameterGroup("Custom color table");

        //Prepare parameter selection for individual ColorTable creations:
        //Select up to three color parameters (color, start, end)
        ColorParameter* param_start_color = new ColorParameter("Start color", Qt::yellow);
        
        BoolParameter*  param_use_mid_color= new BoolParameter("Use mid color");
        ColorParameter* param_mid_color = new ColorParameter("Mid color", Qt::green, param_use_mid_color);
        
        BoolParameter*  param_use_end_color= new BoolParameter("Use end color");
        ColorParameter* param_end_color = new ColorParameter("End color", Qt::red, param_use_end_color);
        
        //Note: Ownership gows to m_custom_color_params on addition:
        custom_ct_params->addParameter("start_color",   param_start_color);
        custom_ct_params->addParameter("use_mid_color", param_use_mid_color);
        custom_ct_params->addParameter("mid_color",     param_mid_color);
        custom_ct_params->addParameter("use_end_color", param_use_end_color);
        custom_ct_params->addParameter("end_color",     param_end_color);
        
        ParameterSelection custom_ct_selection(NULL, custom_ct_params);
    
        if(custom_ct_selection.exec())
        {
            //Get results
            QColor start_color = param_start_color->value();
            QColor mid_color = param_mid_color->value();
            QColor end_color = param_end_color->value();
            
            bool use_mid_color = param_use_mid_color->value();
            bool use_end_color = param_use_end_color->value();
            
            //Generate 0, 0.5 and 1 color assignment
            QColor col1 = start_color,
                   col2 = start_color,
                   col3 = start_color;
            
            if(use_end_color)
            {
                col3 = end_color;
                
                if(use_mid_color)
                {
                    col2 = mid_color;
                }
                else
                {
                    col2 = QColor(  (col1.red()  +col3.red())/2,
                                    (col1.green()+col3.green())/2,
                                    (col1.blue() +col3.blue())/2);
                }
            }
            else if(use_mid_color)
            {
                col2 = col3 =mid_color;
            }
            QVector<QRgb> ct = createColorTableFrom3Colors(col1, col2, col3);
            int idx = addCustomColorTable(ct);
            
            if(idx >= 0)
            {
                m_delegate->setCurrentIndex(idx);
            }
        }
        delete custom_ct_params;
    }
    Parameter::updateValue();
}


} //end of namespace graipe
