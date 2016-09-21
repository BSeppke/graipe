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

#ifndef GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX
#define GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX

#include "vectorfields/config.hxx"
#include "core/qt_ext/qpointfx.hxx"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPolygonF>

namespace graipe {

class GRAIPE_VECTORFIELDS_EXPORT VectorDrawer
{
public:
    VectorDrawer(float line_width=0, float head_size=1, QColor min_color=Qt::white, QColor max_color = Qt::black);

    void setLineWidth(float new_line_width);
    float lineWidth() const;
    
    void setHeadSize(float new_head_size);
    float headSize() const;
    
    void setMinColor(const QColor& new_min_color);
    const QColor& minColor() const;
    
    void setMaxColor(const QColor& new_max_color);
    const QColor& maxColor() const;
    
    
    void paint(QPainter * painter, const QPointFX& origin, const QPointFX& target, float normalized_weight);
    
private:
    void updateHeadTriangle();
    
    float m_head_size;
    
    QPen   m_line_pen;
    QBrush m_arrow_brush;
    
    QPolygonF m_triangle;
    
    QColor m_min_color;
    QColor m_max_color;
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_VECTORFIELDDRAWER_HXX