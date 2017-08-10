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

#ifndef GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX
#define GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX

#include "vectorfields/config.hxx"
#include "core/qt_ext/qpointfx.hxx"
#include "core/colortables.hxx"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPolygonF>

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *
 * @file
 * @brief Header file for drawing single vectors
 */

class GRAIPE_VECTORFIELDS_EXPORT VectorDrawer
{
public:
    /**
     * Creates a new vector drawer, which can be used draw vectors by means of arrows 
     * on arbitrary positions (see the paint function).
     *
     * \param line_width line width of the arrows (in pixels)
     * \param head_size  size of the arrow head (in pixels)
     * \param colorTable the color table used for the color selection of drawing
     */
    VectorDrawer(float line_width=0, float head_size=1, QVector<QRgb> colorTable = colorTables()[0]);
    
    
    /**
     * Re-sets the line width of the vectors to be drawn
     *
     * \param new_line_width the new line width
     */
    void setLineWidth(float new_line_width);
    
    /**
     * Returns the currently set line width of the drawer.
     *
     * \return the current line width
     */
    float lineWidth() const;
    
    /**
     * Re-sets the head size of the vectors to be drawn
     *
     * \param new_head_size the new head size
     */
    void setHeadSize(float new_head_size);
    /**
     * Returns the currently set head size of the drawer.
     *
     * \return the current head size
     */
    float headSize() const;
    
    /**
     * Re-sets the color table of the vectors to be drawn
     *
     * \param colorTable the new color table
     */
    void setColorTable(QVector<QRgb> colorTable);
    
    /**
     * Returns the currently setcolor table of the drawer.
     *
     * \return the current color table
     */
    QVector<QRgb> colorTable() const;
    
    /**
     * Paints a vector using a painter from given position to a target using a normalized weight
     * in the range of {0.0, ..., 1.0}.
     *
     * \param painter the painter which carries out the drawing
     * \param origin the starting position of the vector
     * \param target the final point of the vector
     * \param normalized_weight a normalized weight in the range of {0.0, ..., 1.0}
     */
    void paint(QPainter * painter, const QPointFX& origin, const QPointFX& target, float normalized_weight);
    
private:
    /**
     * Updates the unrotated variant of the arrow head. This will be neccessary, if
     * the head size is changed.
     */
    void updateHeadTriangle();
    
    /** The head size **/
    float m_head_size;
    
    /** copy of the used pen for painting the line **/
    QPen   m_line_pen;
    
    /** copy of the used brush for painting the triangle**/
    QBrush m_arrow_brush;
    
    /** copy of the used triange/head for painting **/
    QPolygonF m_triangle;
    
    /** copy of the used colorTable for painting **/
    QVector<QRgb>  m_colorTable;
};

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX
