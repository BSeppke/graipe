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

#include "core/parameters/colortableparameter.hxx"
#include "core/parameters/colorparameter.hxx"
#include "core/parameters/boolparameter.hxx"
#include "core/parameterselection.hxx"

#include <QColorDialog>
#include <QtDebug>
#include <QObject>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the ColorTableParameter class
 * @}
 */

ColorTableParameter::ColorTableParameter(const QString& name, QVector<QRgb> value, Parameter* parent, bool invert_parent)
:	Parameter(name, parent, invert_parent),
    m_delegate(NULL)
{
    setValue(value);
}

ColorTableParameter::~ColorTableParameter()
{
    delete m_delegate;
}

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

QString  ColorTableParameter::toString() const
{
    QVector<QRgb> ct = value();
    
    QString res;
    
    if(ct.size() != 0)
    {
        res= QColor(ct[0]).name(QColor::HexArgb);
    
        for(int i=0; i<ct.size(); ++i)
        {
            res += ", " + QColor(ct[i]).name(QColor::HexArgb);
        }
    }
    
    return res;
}

void ColorTableParameter::serialize(QXmlStreamWriter& xmlWriter) const
{
    QVector<QRgb> ct = value();
    
    xmlWriter.setAutoFormatting(true);
    
    xmlWriter.writeStartElement(typeName());
    xmlWriter.writeAttribute("ID", id());
        xmlWriter.writeTextElement("Name", name());
        xmlWriter.writeTextElement("Colors", QString::number(ct.size()));
    
    for(int i=0; i<ct.size(); ++i)
    {
        xmlWriter.writeStartElement("Color");
            xmlWriter.writeAttribute("ID", QString::number(i));
            xmlWriter.writeCharacters(QColor(ct[i]).name(QColor::HexArgb));
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

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

 bool ColorTableParameter::isValid() const
{
    return true;
}

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
