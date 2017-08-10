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

#include "qlegend.hxx"
#include "colortables.hxx"

#include <QFont>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the QLegend class
 * @}
 */

QLegend::QLegend(QGraphicsItem* parent)
:	QGraphicsResizableItem(QRectF(0,0,10,10)),
	m_lower_val(0.0),
	m_upper_val(1.0), 
	m_ticks(10), 
	m_digits(1), 
	m_fixed_scale(false),
	m_ct(colorTables()[0]),
    m_bg_pixmap(NULL)
{
    if (parent != NULL)
    {
        setParentItem(parent);
    }
    updateRect(rect());
    setValueRange(m_lower_val,m_upper_val);
}

QLegend::QLegend(QRectF rect, float lower_val, float upper_val, int ticks, bool fixed_scale, QGraphicsItem* parent)
:	QGraphicsResizableItem(rect),
	m_lower_val(lower_val),
	m_upper_val(upper_val), 
	m_ticks(ticks),
	m_digits(1), 
	m_fixed_scale(fixed_scale),
	m_ct(colorTables()[0]),
    m_bg_pixmap(NULL)
{
    if (parent != NULL)
    {
        setParentItem(parent);
    }
    updateRect(QGraphicsResizableItem::rect());
    setValueRange(lower_val,upper_val);
}

QLegend::QLegend(float rect_left, float rect_top, float rect_width, float rect_height, float lower_val, float upper_val, int ticks, bool fixed_scale, QGraphicsItem* parent)
:   QGraphicsResizableItem(QRectF(rect_left,rect_top,rect_width,rect_height)),
	m_lower_val(lower_val),
	m_upper_val(upper_val), 
	m_ticks(ticks), 
	m_digits(1), 
	m_fixed_scale(fixed_scale),
	m_ct(colorTables()[0]),
    m_bg_pixmap(NULL)
{
    if (parent != NULL)
    {
        setParentItem(parent);
    }
    updateRect(rect());
    setValueRange(lower_val,upper_val);
}

void QLegend::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
    
    painter->setPen(m_pen);
	painter->setFont(m_scale_font);
    
	//First draw rectangle
    if(m_bg_pixmap != NULL)
    {
        painter->drawPixmap(m_scale_rect.left(), m_scale_rect.top(),
                            m_bg_pixmap->scaled(m_scale_rect.width()+1, m_scale_rect.height()+1));
	}
    painter->drawRect(m_scale_rect);
    
    painter->setFont(m_scale_font);
    QFontMetrics fm_scale(m_scale_font);
    
	//Then draw ticks and labels:
	for(unsigned int i=0; i<=m_ticks; ++i)
	{
		
		float current_x = m_scale_rect.left() + (float)i/m_ticks*m_scale_rect.width();
		
		painter->drawLine(QLineF(current_x, m_scale_rect.top(),
                                 current_x, m_scale_rect.bottom()));
        
		QString scale_text = QString::number(m_lower_val+ (float)i/m_ticks*(m_upper_val-m_lower_val), 'f', m_digits);
		
		if(m_digits==0 )
			scale_text = QString::number(m_lower_val+ (float)i/m_ticks*(m_upper_val-m_lower_val)+0.5, 'f', m_digits);
        
		painter->drawText(QRectF(current_x-text_interval_width()/2.0,
                                 rect().top()+rect().height()/2.7,
                                 text_interval_width(),
                                 rect().height()/5.0),
                          Qt::AlignCenter,
                          scale_text);
	}
	
	if(!m_caption.isEmpty())
	{
		painter->setFont(m_caption_font);
		painter->drawText(QRectF(rect().left(),
                                 rect().top()+4.0*rect().height()/7.0,
                                 rect().width(),
                                 rect().height()/3.0),
                          Qt::AlignCenter,
                          m_caption);
	}
	
	painter->restore();
	
	QGraphicsResizableItem::paint(painter, option, widget);
}

QString QLegend::caption() const
{
	return m_caption;
}

void QLegend::setCaption(const QString& caption)
{
	m_caption = caption;
}

unsigned int QLegend::digits() const
{
    return m_digits;
}

void QLegend::setDigits(unsigned int digits)
{
    m_digits = digits;
}

unsigned int QLegend::ticks() const
{
    return m_ticks;
}

void QLegend::setTicks(unsigned int ticks)
{
    m_ticks = ticks;
}

bool QLegend::fixedScale() const
{
	return m_fixed_scale;
}

void QLegend::fixScale(bool fix)
{
	m_fixed_scale = fix;
}

float QLegend::minValue() const
{
	return m_lower_val;
}

void QLegend::setMinValue(float val)
{
	m_lower_val = val;
    
    updateRect(rect());
}

float QLegend::maxValue() const
{
	return m_upper_val;
}

void QLegend::setMaxValue(float val)
{
    m_upper_val = val;
    
    updateRect(rect());    
}

void QLegend::setValueRange(float min_val, float max_val)
{
	setMinValue(min_val);
	setMaxValue(max_val);
}

QVector<QRgb> QLegend::colorTable() const
{
    return m_ct;
}

void QLegend::setColorTable(QVector<QRgb> colorTable)
{
    m_ct = colorTable;

    unsigned int w=256, h=1;
    
    QImage background_img = QImage(w, h, QImage::Format_Indexed8);
    background_img.setColorCount(256);
    
    for(unsigned int y=0; y<h; ++y)
    {
        for(unsigned int x=0; x<w; ++x)
        {
            background_img.setPixel(x,y, w/256.0*x);
        }
    }
    background_img.setColorTable(m_ct);
    
    if(m_bg_pixmap)
    {
        delete m_bg_pixmap;
        m_bg_pixmap = NULL;
    }
    m_bg_pixmap = new QPixmap;
    m_bg_pixmap->convertFromImage(background_img);

}

void QLegend::updateRect(const QRectF& new_rect)
{
    QGraphicsResizableItem::updateRect(new_rect);
    
    float zoom_factor = (new_rect.width()-20)/m_scale_rect.width();
    if(fixedScale())
    {
        float range = m_upper_val - m_lower_val;
        m_upper_val = m_lower_val + range*zoom_factor;
    }
    
    m_scale_rect.setLeft(rect().left()+10);
    m_scale_rect.setWidth(rect().width()-20);
    m_scale_rect.setTop(rect().top());
    m_scale_rect.setHeight(rect().height()/3.0);
    
    QFontMetrics fm_scale(QFont(QString("Arial"), 1.0));
    float scale_w = std::max(fm_scale.width(QString::number(m_lower_val, 'f', m_digits)),
                             fm_scale.width(QString::number(m_upper_val, 'f', m_digits)));
    float scale_h = fm_scale.height();
    
    float scale_pts = 0.9*std::min(text_interval_width()/scale_w, float(rect().height()/5.0)/scale_h);
    m_scale_font = QFont(QString("Arial"), scale_pts);
    
    QFontMetrics fm_caption(QFont(QString("Arial"), 1.0));
    float caption_w = fm_caption.width(caption());
    float caption_h = fm_caption.height();
    
    float caption_pts = 0.9*std::min(float(rect().width()/caption_w), float(rect().height()/3.0)/caption_h);
    m_caption_font = QFont(QString("Arial"), caption_pts);
    
}

inline float QLegend::text_interval_width() const
{
	return m_scale_rect.width()/m_ticks*0.8;
}

}//end of namespace graipe
