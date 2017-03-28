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

#include "qgraphicsresizableitem.hxx"

#include <cmath>

#include <QCursor>
#include <QtDebug>

/**
 * @file
 * @brief Implementation file for the QGraphicsResizableItem class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {
    
//codes for the handles of the Resizable items
static const unsigned short top_c = 1, 
							right_c = 2, 
							bottom_c = 4, 
							left_c = 8, 
							forbidden_c = 16;
/**
 * Constructor of the resizable GraphicsItem class.
 * 
 * \param rect              The size of the resizable item (= its bounding rect).
 * \param handle_size       The size of the handles (in pixels)
 * \param protect_h_scaling If true, scaling the Item horizontally is forbidden.
 * \param protect_v_scaling If true, scaling the Item vertically is forbidden.
 * \param parent            The parent item of this graphics item.
 */
QGraphicsResizableItem::QGraphicsResizableItem(const QRectF& rect, unsigned int handle_size, bool protect_h_scaling, bool protect_v_scaling, QGraphicsItem * parent)
:	QGraphicsRectItem(rect, parent),
	m_handle_size(handle_size),
	m_protect_h_scaling(protect_h_scaling), 
	m_protect_v_scaling(protect_v_scaling)
{
    m_pen.setWidth(0);
    
    setFlags(ItemIsSelectable);
	setAcceptHoverEvents(true);
}

/**
 * The paint procedure of the resizable item prints the item's boundary by means of
 * a outlined rect with black line color and than paints the handles at each of the
 * eight positions.
 *
 * \param painter The painter, which is used for drawing.
 * \param option  The style options used for drawing.
 * \param widget  The widget, where we paint on.
 */
void QGraphicsResizableItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    painter->save();
    
	if(isSelected())
	{
		painter->setPen(m_pen);
        
		painter->setBrush(Qt::transparent);
		painter->drawRect(rect());
			
		painter->setBrush(Qt::black);		
		painter->drawRect(QRectF(rect().center().x() - m_handle_size/2.0,   rect().top(),                               m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().right() - m_handle_size,            rect().top(),                               m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().right() - m_handle_size,            rect().center().y() - m_handle_size/2.0,    m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().right() - m_handle_size,            rect().bottom() - m_handle_size,            m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().center().x() - m_handle_size/2.0,   rect().bottom() - m_handle_size,            m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().left(),                             rect().bottom() - m_handle_size,            m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().left(),                             rect().center().y() - m_handle_size/2.0,    m_handle_size, m_handle_size));
		painter->drawRect(QRectF(rect().left(),                             rect().top(),                               m_handle_size, m_handle_size));
	}
	painter->restore();
}

/**
 * Modifier to protect the horizontal scaling.
 *
 * \param protect Shall the horizontal scaling be protected?
 */
void QGraphicsResizableItem::protectHScaling(bool protect)
{
	m_protect_h_scaling = protect; 
}

/**
 * Modifier to protect the vertical scaling.
 *
 * \param protect Shall the vertical scaling be protected?
 */
void QGraphicsResizableItem::protectVScaling(bool protect)
{
	m_protect_v_scaling = protect; 
}

/**
 * Modifier to protect the horizontal and vertical scaling at once.
 *
 * \param protect_h Shall the horizontal scaling be protected?
 * \param protect_v Shall the vertical scaling be protected?
 */
void QGraphicsResizableItem::protectScaling(bool protect_h, bool protect_v)
{
	protectHScaling(protect_h);
	protectVScaling(protect_v);
}

/**
 * Change the Rect of the graphics item to a new one.
 *
 * \param rect The new rect.
 */
void QGraphicsResizableItem::setRect(const QRectF& rect)
{
    if(    rect.width()>=m_handle_size*2
       &&  rect.height()>=m_handle_size*2)
    {
        QGraphicsRectItem::setRect(rect);
    }
}

/**
 * Change the Rect of the graphics item to a new one.
 *
 * \param l The left position of the new rect.
 * \param t The top position of the new rect.
 * \param w The width of the new rect.
 * \param h The height of the new rect.
 */
void QGraphicsResizableItem::setRect(qreal l, qreal t, qreal w, qreal h)
{
    if(    w>=m_handle_size*2
       &&  h>=m_handle_size*2)
    {
        QGraphicsRectItem::setRect(l,t,w,h);
    }
}

/**
 * This function should be called after every computed change of the 
 * rectangle, e.g. due to rescaling events. It checks against the 
 * scaling settings and rescales the rect if allowed.
 *
 * \param new_rect The new rect to be set.
 */
void QGraphicsResizableItem::updateRect(const QRectF & new_rect)
{
	if(m_protect_h_scaling && new_rect.width()!=rect().width())
	{
		//forbidden
	}
	else if(m_protect_v_scaling && new_rect.height()!=rect().height())
	{
		//also forbidden
	}
	else
    {
        prepareGeometryChange();
        setRect(new_rect);
        update();
	}
}

/**
 * This function is called whenever the mouse is pressed on this item.
 *
 * \param event The event, with more information about mouse position etc.
 */
void QGraphicsResizableItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    m_dragStart=event->pos();
    
    float	delta_l = std::abs(m_dragStart.x()-rect().left()),
			delta_t = std::abs(m_dragStart.y()-rect().top()),
			delta_r = std::abs(m_dragStart.x()-rect().right()),
			delta_b = std::abs(m_dragStart.y()-rect().bottom());
	
	m_handle=0;
	
	//look left:
	if (delta_l < m_handle_size)
	{
		if( m_protect_h_scaling )
		{
			setCursor(Qt::ForbiddenCursor);
			m_handle = forbidden_c;
		}
		else
		{
			m_handle = m_handle | left_c ;
			setCursor(Qt::SizeHorCursor);
		}
	}
	
	if (delta_r < m_handle_size)
	{
		if( m_protect_h_scaling )
		{
			setCursor(Qt::ForbiddenCursor);
			m_handle =  forbidden_c;
		}
		else
		{
			m_handle = m_handle | right_c;
			setCursor(Qt::SizeHorCursor);
		}
	}
	
	if (delta_t < m_handle_size)
	{
		if( m_protect_v_scaling )
		{
			setCursor(Qt::ForbiddenCursor);
			m_handle = forbidden_c;
		}
		else
		{
			m_handle = m_handle | top_c;
			
			if(m_handle & right_c)
			{
            	setCursor(Qt::SizeBDiagCursor);
			}
            else if(m_handle & left_c)
			{
            	setCursor(Qt::SizeFDiagCursor);
			}
            else
			{
            	setCursor(Qt::SizeVerCursor);
            }
        }
	}
	
	if (delta_b < m_handle_size)
	{
		if( m_protect_v_scaling )
		{
			setCursor(Qt::ForbiddenCursor);
			m_handle = forbidden_c;
		}
		else
		{
			m_handle = m_handle | bottom_c;
			
			if(m_handle & right_c)
			{
            	setCursor(Qt::SizeFDiagCursor);
			}
            else if(m_handle & left_c)
			{
            	setCursor(Qt::SizeBDiagCursor);
			}
            else
			{
            	setCursor(Qt::SizeVerCursor);
            }
        }
	}
	
	if(!m_handle)
	{
    	setCursor(Qt::SizeAllCursor);
    }
    
	QGraphicsItem::mousePressEvent(event);	
}

/**
 * This function is called whenever the mouse is released on this item.
 *
 * \param event The event, with more information about mouse position etc.
 */
void QGraphicsResizableItem::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
	setCursor(Qt::ArrowCursor);
	QGraphicsItem::mouseReleaseEvent(event);
}

/**
 * This function is called whenever the mouse is moved on this item.
 *
 * \param event The event, with more information about mouse position etc.
 */
void QGraphicsResizableItem::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{		
	if (m_handle != forbidden_c)
	{
        QRectF new_rect(rect());
	
        if( m_handle & left_c)
        {
            new_rect.setLeft(event->pos().x());
        }
        if( m_handle & top_c)
        {
            new_rect.setTop(event->pos().y());
        }
        if( m_handle & right_c)
        {
            new_rect.setRight(event->pos().x());
        }
        if( m_handle & bottom_c)
        {
            new_rect.setBottom(event->pos().y());
        }
        if( m_handle != 0)
        {
            updateRect(new_rect);
        }
        else
        {
            QPointF delta = event->pos() - m_dragStart;
			prepareGeometryChange();
            moveBy(delta.x(), delta.y());
        }
        event->accept();
    }
	else
    {
		QGraphicsItem::mouseMoveEvent(event);
	}
}

}//end of namespace graipe

/**
 * @}
 */
