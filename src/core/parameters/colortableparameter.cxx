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

/**
 * @file
 * @brief Implementation file for the ColorTableParameter class
 *
 * @addtogroup core
 * @{
 */

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
    m_delegate(NULL)
{
    setValue(value);
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
    if(m_ct_idx >= 0 && m_ct_idx < colorTables().size())
    {
       return colorTables()[m_ct_idx];
    }
    else if(m_ct_idx - colorTables().size() < m_extra_tables.size())
    {
        return m_extra_tables[m_ct_idx - colorTables().size()];
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
    int ct_idx=-1;
    
    //search in std. color tables
    for(unsigned int i=0; i != colorTables().size(); ++i)
    {
        if(colorTables()[i] == ct)
        {
            ct_idx = i;
            break;
        }
    }
    
    if(ct_idx != -1)
    {
        return ct_idx;
    }
    
    //search in extra (user defined) color tables
    for(unsigned int i=0; i != m_extra_tables.size(); ++i)
    {
        if(m_extra_tables[i] == ct)
        {
            ct_idx = colorTables().size()+i;
            break;
        }
    }
    return ct_idx;
}

/**
 * Writing accessor of the current value of this parameter.
 *
 * \param value The new value of this parameter.
 */
void ColorTableParameter::setValue(const QVector<QRgb>& value)
{
    int ct_idx = colorTableIndex(value);
    
    if(ct_idx>=0)
    {
        m_ct_idx = ct_idx;
        
        if(m_delegate != NULL)
        {
            m_delegate->setCurrentIndex(ct_idx);
            Parameter::updateValue();
        }
    }
    else
    {
        qDebug("ColorTableParameter::setValue: Could not find value to set in colortables. Creating a new one");
        m_ct_idx = addCustomColorTable(value);
        if(m_delegate != NULL)
        {
            m_delegate->setCurrentIndex(ct_idx);
            Parameter::updateValue();
        }
        
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
        int ct_idx = colorTableIndex(ct);
        
        //Already known?
        if(ct_idx>=0)
        {
            return ct_idx;
        }
        else
        {
            m_extra_tables.push_back(ct);
            
            if(m_delegate != NULL)
            {
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
            }
            return colorTables().size() + m_extra_tables.size()-1;
        }
    }
    else
    {
        return -1;
    }
}
/**
 * The value converted to a QString. This returns a comma-separated list of all
 * Colors by means of #AARRGGBB values for each color.
 *
 * \return The value of the parameter converted to an QString.
 */
QString  ColorTableParameter::toString() const
{
    QVector<QRgb> ct = value();
    
    
    QString res;
    
    if(ct.size() != 0)
    {
        res= QColor(ct[0]).name(QColor::HexArgb);
    
        for(unsigned int i=0; i<ct.size(); ++i)
        {
            res += ", " + QColor(ct[i]).name(QColor::HexArgb);
        }
    }
    
    return res;
}

/**
 * Serialization of the parameter's state to a xml stream.
 * Writes the following XML code by default:
 * 
 * <ColorTableParameter>
 *     <Name>NAME</Name>
 *     <Colors>COLORCOUNT</Colors>
 *     <Color ID="0">#AARRGGBB</Color>
 *     ...
 *     <Color ID="COLORCOUNT-1">#AARRGGBB</Color>
 * </ColorTableParameter>
 *
 * with TYPENAME = typeName() and
 *    COLORCOUNT = ct.size().
 *
 * \param xmlWriter The QXMLStreamWriter, which we use serialize the 
 *                  parameter's type, name and value.
 */
void ColorTableParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    QVector<QRgb> ct = value();
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
        xmlWriter.writeTextElement("Name", name());
        xmlWriter.writeTextElement("Colors", QString::number(ct.size()));
    
    for(unsigned int i=0; i<ct.size(); ++i)
    {
        xmlWriter.writeStartElement("Color");
            xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeCharacters(QColor(ct[i]).name(QColor::HexArgb));
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

/**
 * Deserialization of a parameter's state from an xml file.
 *
 * \param xmlReader The QXmlStreamReader, where we read from.
 * \return True, if the deserialization was successful, else false.
 */
bool ColorTableParameter::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        if(     xmlReader.name() == typeName()
            &&  xmlReader.attributes().hasAttribute("ID"))
        {
            setID(xmlReader.attributes().value("ID").toString());
            
            QVector<QRgb> ct(256);
            
            while(xmlReader.readNextStartElement())
            {
                if(xmlReader.name() == "Name")
                {
                    setName(xmlReader.readElementText());
                }
                if(xmlReader.name() == "Colors")
                {
                    ct.resize(xmlReader.readElementText().toInt());
                }
                if(    xmlReader.name() == "Color"
                    && xmlReader.attributes().hasAttribute("ID"))
                {
                    int color_id = xmlReader.attributes().value("ID").toInt();
                    
                    QColor color(xmlReader.readElementText());
                    
                    ct[color_id] = color.rgb();
                }
            }
            setValue(ct);
            return true;
        }
        else
        {
            throw std::runtime_error("Did not find typeName() or id() in XML tree");
        }
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
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
        m_delegate = new QComboBox;
        
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
        setValue(value());
        
        connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
        Parameter::initConnections();
    }
    
    return m_delegate;
}

/**
 * This slot is called everytime, the delegate has changed. It has to synchronize
 * the internal value of the parameter with the current delegate's value
 */
void ColorTableParameter::updateValue()
{
    if(m_delegate->currentIndex() < m_delegate->count()-1)
    {
        m_ct_idx = m_delegate->currentIndex();
    }
    //Last index clicked -> add new ct!
    else if(m_delegate->currentIndex() == m_delegate->count()-1)
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
        
        ParameterSelection* custom_ct_selection = new ParameterSelection(NULL, custom_ct_params);
    
        if(custom_ct_selection->exec())
        {
            custom_ct_selection->hide();
            
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
            
            disconnect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
            int idx = addCustomColorTable(ct);
            
            if(idx >= 0)
            {
                setValue(ct);
            }
            connect(m_delegate, SIGNAL(currentIndexChanged(int)), this, SLOT(updateValue()));
        }
        delete custom_ct_params;
        delete custom_ct_selection;
    }
    Parameter::updateValue();
}


} //end of namespace graipe

/**
 * @}
 */
