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

#ifndef GRAIPE_CORE_QGRAPHICSRESIZABLEITEM_HXX
#define GRAIPE_CORE_QGRAPHICSRESIZABLEITEM_HXX

#include "core/config.hxx"

#include <QGraphicsRectItem>
#include <QBrush>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

/**
 * @file
 * @brief Header file for the QGraphicsResizableItem class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This is a 2D QGraphicsItem, which may be reseized
 * interactively by the user. This generally does not apply to the ViewController
 * of Models, which are indeed fixed in their geometry and thus size.
 *
 * But for annotations and legends, this class is highly helpful. Thus
 * it is used as a base class for the QLegend, to display data legends 
 * on demand for each model. 
 *
 * Moreover, the resizablity of the item may be restricted to either
 * horizontal or vertical scaling. Additionally, a resizable item may
 * also be dragged around by the user.
 */
class GRAIPE_CORE_EXPORT QGraphicsResizableItem
:	public QGraphicsRectItem
{
	public:
        /**
         * Constructor of the resizable GraphicsItem class.
         * 
         * \param rect              The size of the resizable item (= its bounding rect).
         * \param handle_size       The size of the handles (in pixels)
         * \param protect_h_scaling If true, scaling the Item horizontally is forbidden.
         * \param protect_v_scaling If true, scaling the Item vertically is forbidden.
         * \param parent            The parent item of this graphics item.
         */
		QGraphicsResizableItem(const QRectF& rect, unsigned int handle_size=5, bool protect_h_scaling=false, bool protect_v_scaling=false, QGraphicsItem * parent = NULL);
		
        /**
         * The paint procedure of the resizable item prints the item's boundary by means of
         * a outlined rect with black line color and than paints the handles at each of the
         * eight positions if the item is selected.
         *
         * \param painter The painter, which is used for drawing.
         * \param option  The style options used for drawing.
         * \param widget  The widget, where we paint on.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);	
		
        /**
         * Modifier to protect the horizontal scaling.
         *
         * \param protect Shall the horizontal scaling be protected?
         */
		void protectHScaling(bool protect);
    
        /**
         * Modifier to protect the vertical scaling.
         *
         * \param protect Shall the vertical scaling be protected?
         */
		void protectVScaling(bool protect);
    
        /**
         * Modifier to protect the horizontal and vertical scaling at once.
         *
         * \param protect_h Shall the horizontal scaling be protected?
         * \param protect_v Shall the vertical scaling be protected?
         */
		void protectScaling(bool protect_h, bool protect_v);
    
        /**
         * Change the Rect of the graphics item to a new one.
         *
         * \param rect The new rect.
         */
        void setRect(const QRectF& rect);
    
        /**
         * Change the Rect of the graphics item to a new one.
         *
         * \param l The left position of the new rect.
         * \param t The top position of the new rect.
         * \param w The width of the new rect.
         * \param h The height of the new rect.
         */
        void setRect(qreal l, qreal t, qreal w, qreal h);
    
        /**
         * This function should be called after every computed change of the 
         * rectangle, e.g. due to rescaling events. It checks against the 
         * scaling settings and rescales the rect if allowed.
         *
         * \param new_rect The new rect to be set.
         */
		virtual void updateRect(const QRectF& new_rect);
		
	protected:
        /**
         * This function is called whenever the mouse is pressed on this item.
         *
         * \param event The event, with more information about mouse position etc.
         */
		void mousePressEvent(QGraphicsSceneMouseEvent * event);
		
        /**
         * This function is called whenever the mouse is released on this item.
         *
         * \param event The event, with more information about mouse position etc.
         */
        void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
		
        /**
         * This function is called whenever the mouse is moved on this item.
         *
         * \param event The event, with more information about mouse position etc.
         */
         void mouseMoveEvent(QGraphicsSceneMouseEvent  * event);
	
        /** Holds the currently active handle **/
		unsigned short m_handle;
    
        /** The position, where the drag started **/
		QPointF m_dragStart;
    
        /** The drawing pen **/
        QPen m_pen;
		
        /** The size of each handle in pixels **/
		unsigned int m_handle_size;
		
        /** The protection settings for horiontal scaling **/
		bool m_protect_h_scaling;
        /** The protection settings for vertical scaling **/
        bool m_protect_v_scaling;
};
    
}//end of namespace graipe

/**
 * @}
 */

#endif
