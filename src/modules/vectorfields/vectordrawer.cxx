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

#include "vectorfields/vectordrawer.hxx"

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *
 * @file
 * @brief Implementation file for drawing single vectors
 */
 
/**
 * Creates a new vector drawer, which can be used draw vectors by means of arrows 
 * on arbitrary positions (see the paint function).
 *
 * \param line_width line width of the arrows (in pixels)
 * \param head_size  size of the arrow head (in pixels)
 * \param colorTable the color table used for the color selection of drawing (needs to have 256 entries)
 */
VectorDrawer::VectorDrawer(float line_width, float head_size, QVector<QRgb> colorTable)
: m_arrow_brush(colorTable[0])
{
    setLineWidth(line_width);
    setHeadSize(head_size);
    setColorTable(colorTable);
}

/**
 * Re-sets the line width of the vectors to be drawn
 *
 * \param new_line_width the new line width
 */
void VectorDrawer::setLineWidth(float new_line_width)
{
    m_line_pen.setWidthF(new_line_width);
}

/**
 * Returns the currently set line width of the drawer.
 *
 * \return the current line width
 */
float VectorDrawer::lineWidth() const
{
    return m_line_pen.widthF();
}

/**
 * Re-sets the head size of the vectors to be drawn
 *
 * \param new_head_size the new head size
 */
void VectorDrawer::setHeadSize(float new_head_size)
{
    m_head_size = new_head_size;
    updateHeadTriangle();
}

/**
 * Returns the currently set head size of the drawer.
 *
 * \return the current head size
 */
float VectorDrawer::headSize() const
{
    return m_head_size;
}

/**
 * Re-sets the color table of the vectors to be drawn
 *
 * \param colorTable the new color table (needs to have 256 entries)
 */
void VectorDrawer::setColorTable(QVector<QRgb> colorTable)
{
    m_colorTable = colorTable;
}

/**
 * Returns the currently setcolor table of the drawer.
 *
 * \return the current color table
 */
QVector<QRgb> VectorDrawer::colorTable() const
{
    return m_colorTable;
}

/**
 * Paints a vector using a painter from given position to a target using a normalized weight
 * in the range of {0.0, ..., 1.0}.
 *
 * \param painter the painter which carries out the drawing
 * \param origin the starting position of the vector
 * \param target the final point of the vector
 * \param normalized_weight a normalized weight in the range of {0.0, ..., 1.0}
 */
void VectorDrawer::paint(QPainter * painter, const QPointFX& origin, const QPointFX& target, float normalized_weight)
{
    QColor current_color =  QColor(m_colorTable[normalized_weight*255]);
    
    painter->setPen(QPen());
    painter->setBrush(QBrush());
    
    m_line_pen.setColor(current_color);
    m_arrow_brush.setColor(current_color);
    
    QPointFX direction = target-origin;
    
    QTransform t;
    t.translate(target.x(), target.y());
    t.rotate(direction.angle());
            
    float line_length = direction.length() - 2*m_head_size;
                    
    if(line_length > 0)
    {
        painter->setPen(m_line_pen);
        painter->setBrush(QBrush());
        painter->drawLine(origin, origin + direction/direction.length()*line_length);
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(m_arrow_brush);
    painter->drawConvexPolygon(t.map(m_triangle));
}

/**
 * Updates the unrotated variant of the arrow head. This will be neccessary, if
 * the head size is changed.
 */
void VectorDrawer::updateHeadTriangle()
{
   QPolygonF new_polygon;
        
   new_polygon << QPointF( 0,  0)
                 << QPointF(-2*m_head_size, -m_head_size*0.6)
                 << QPointF(-2*m_head_size, m_head_size*0.6)
                 << QPointF( 0,  0);
    
    m_triangle = new_polygon;
}

/**
 * @}
 */

} //end of namespace graipe
