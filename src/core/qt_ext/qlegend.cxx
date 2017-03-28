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

#include "qlegend.hxx"
#include "colortables.hxx"

#include <QFont>

/**
 * @file
 * @brief Implementation file for the QLegend class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * Default constructor of the QLegend class.
 *
 * \param parent The parent item of this graphics item.
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

/**
 * Second constructor of the QLegend class.
 * 
 * \param rect        The size of the Legend (= its bounding rect).
 * \param lower_val   The lowest value on the scale.
 * \param upper_val   The highest value on the scale.
 * \param ticks       The count of ticks between lower and upper value on the scale.
 * \param fixed_scale If true, the legend behaves as true to scale (see above).
 * \param parent      The parent item of QLegend item.
 */
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

/**
 * Third constructor of the QLegend class.
 * 
 * \param rect_left    The left position of the QLegend's rect.
 * \param rect_top     The top position of the QLegend's rect.
 * \param rect_width   The width of the QLegend's rect.
 * \param recht_height The height of the new rect.
 * \param lower_val    The lowest value on the scale.
 * \param upper_val    The highest value on the scale.
 * \param ticks        The count of ticks between lower and upper value on the scale.
 * \param fixed_scale  If true, the legend behaves as true to scale (see above).
 * \param parent       The parent item of QLegend item.
 */
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

/**
 * The paint procedure of the QLagend prints the scale and a caption below.
 * If the item is selected, the handles and outline are painted, too.
 *
 * \param painter The painter, which is used for drawing.
 * \param option  The style options used for drawing.
 * \param widget  The widget, where we paint on.
 */
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

/**
 * Const accessor to the caption of the legend.
 *
 * \return The caption of this QLegend.
 */
QString QLegend::caption() const
{
	return m_caption;
}

/**
 * Sets the caption of the legend to a new QString.
 *
 * \param caption The new caption of this QLegend.
 */
void QLegend::setCaption(const QString& caption)
{
	m_caption = caption;
}
	
/**
 * Const accessor to the digits of the scale labels of the legend.
 *
 * \return The digits precision of the scale labels of this QLegend.
 */
unsigned int QLegend::digits() const
{
    return m_digits;
}

/**
 * Sets the digits of the scale labels of the legend to a certain precision.
 *
 * \param digits The digits precision of the scale labels of this QLegend.
 */
void QLegend::setDigits(unsigned int digits)
{
    m_digits = digits;
}

/**
 * Const accessor to the count of ticks of the scale.
 *
 * \return The count of ticks of the scale.
 */
unsigned int QLegend::ticks() const
{
    return m_ticks;
}

/**
 * Sets the ticks of the scale to a given count.
 *
 * \param ticks The new tick division of the scale.
 */
void QLegend::setTicks(unsigned int ticks)
{
    m_ticks = ticks;
}
    
/**
 * Const accessor to find out if the "true to scale" mode is enabled.
 *
 * \return True, if the "true to scale" mode is enabled.
 */
bool QLegend::fixedScale() const
{
	return m_fixed_scale;
}

/**
 * Fixes the scale, such that it behaves like true to scale when resized.
 *
 * \param fix If true, the "true to scale" mode will we enabled.
 */
void QLegend::fixScale(bool fix)
{
	m_fixed_scale = fix;
}

/**
 * Const accessor to the minimum value of the scale.
 *
 * \return The minimum value of the scale.
 */
float QLegend::minValue() const
{
	return m_lower_val;
}

/**
 * Sets the minimum value of the scale.
 *
 * \param val The new minimum value of the scale.
 */
void QLegend::setMinValue(float val)
{
	m_lower_val = val;
    
    updateRect(rect());
}

/**
 * Const accessor to the maximum value of the scale.
 *
 * \return The maximum value of the scale.
 */
float QLegend::maxValue() const
{
	return m_upper_val;
}

/**
 * Sets the maximum value of the scale.
 *
 * \param val The new maximum value of the scale.
 */
void QLegend::setMaxValue(float val)
{
    m_upper_val = val;
    
    updateRect(rect());    
}

/**
 * Sets the range(minimum and maximum value) of the scale.
 *
 * \param min_val The new minimum value of the scale.
 * \param max_val The new maximum value of the scale.
 */
void QLegend::setValueRange(float min_val, float max_val)
{
	setMinValue(min_val);
	setMaxValue(max_val);
}

/**
 * Const accessor to the color table of the scale.
 *
 * \return The color table of the scale.
 */
QVector<QRgb> QLegend::colorTable() const
{
    return m_ct;
}

/**
 * Sets the color table (colors of minimum and maximum value) of the scale
 *
 * \param colorTable The new color table of the scale.
 */
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

/**
 * Overloaded function from QGraphicsResizableItem:
 * This function should be called after every computed change of the
 * rectangle, e.g. due to rescaling events. It checks against the 
 * scaling settings and rescales the rect if allowed.
 *
 * \param new_rect The new rect to be set.
 */
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

/**
 * Returns the width (in pixels), which is available for the labels of the
 * scale in between or under each tick.
 *
 * \return The maximum width for each scale label in pixels.
 */
inline float QLegend::text_interval_width() const
{
	return m_scale_rect.width()/m_ticks*0.8;
}

}//end of namespace graipe

/**
 * @}
 */
