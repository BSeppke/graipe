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

#ifndef GRAIPE_FEATURES2D_CUBICSPLINELISTVIEWCONTROLLER_HXX
#define GRAIPE_FEATURES2D_CUBICSPLINELISTVIEWCONTROLLER_HXX

#include "core/viewcontroller.hxx"
#include "core/qt_ext/qlegend.hxx"

#include "features2d/cubicsplinelist.hxx"
#include "features2d/cubicsplineliststatistics.hxx"
#include "features2d/config.hxx"

namespace graipe {
    
/**
 * A class for viewing a 2D cubic spline list on a QGraphicsScene/View and 
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT CubicSplineList2DViewController
:	public ViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param splines The spline list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        CubicSplineList2DViewController(QGraphicsScene * scene, CubicSplineList2D* splines, int z_value = 0);
    
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~CubicSplineList2DViewController();
    
        /**
         * Implementation/specialization of the ViewController's paint procedure. This is called
         * by the QGraphicsView on every re-draw request.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
        /**
         * The bounding rect of the cubic spline list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "CubicSplineList2DViewController"
         */
        QString typeName() const
        {
            return "CubicSplineList2DViewController";
        }
        
    protected:
        /**
         * Snycronizes between the Model's data and the views reprsentation:
         * Re-creates the polylines and point sets defined by the model.
         */
        void refreshSplines();
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent(QGraphicsSceneMouseEvent * event);
    
    
        //Polygons/Polylines are used to draw the splines
        QVector<QPolygonF> m_lines;
    
        //...overlaid with supporting points.
        QVector<QList<QPointF> > m_points;
    
        //Spline list statistics.
        CubicSplineList2DStatistics * m_stats;
    
        //Additional parameters:
        BoolParameter*  m_showLabels;
        FloatParameter* m_fontSize;
        EnumParameter*  m_mode;
        FloatParameter* m_lineWidth;
        ColorParameter* m_color;
};
    
    

    
/**
 * A class for viewing a weighted 2D cubic spline list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedCubicSplineList2DViewController
:	public ViewController
{
        
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param splines The weighted spline list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        WeightedCubicSplineList2DViewController(QGraphicsScene * scene, WeightedCubicSplineList2D* splines, int z_value = 0);
        
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~WeightedCubicSplineList2DViewController();
    
        /**
         * Implementation/specialization of the ViewController's paint procedure. This is called
         * by the QGraphicsView on every re-draw request.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
        /**
         * The bounding rect of the cubic spline list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "WeightedCubicSplineList2DViewController"
         */
        QString typeName() const
        {
            return "WeightedCubicSplineList2DViewController";
        }
    
        /**
         * Specialization of the update of  the parameters of this ViewController according to the current
         * model's parameters. This is necessary, if something may have changed 
         * the model in meantime.
         * 
         * \param force_update If true, force every single parameter to update.
         */
        void updateParameters(bool force_update=false);
    
        /**
         * Specialization of the update of the view according to the current parameter settings.
         */
        void updateView();
        
    protected:
        /**
         * Snycronizes between the Model's data and the views reprsentation:
         * Re-creates the polylines and point sets defined by the model.
         */
        void refreshSplines();
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent(QGraphicsSceneMouseEvent * event);
        
        //Polygons/Polylines are used to draw the splines
        QVector<QPolygonF> m_lines;
    
        //...overlaid with supporting points.
        QVector<QList<QPointF> > m_points;
    
        //Statistics mainly for the weight of the splines
        WeightedCubicSplineList2DStatistics* m_stats;
    
        //Additional parameters
        BoolParameter*   m_showLabels;
        FloatParameter*  m_fontSize;
        EnumParameter*   m_mode;
        FloatParameter*  m_lineWidth;
        FloatParameter*  m_minWeight;
        FloatParameter*  m_maxWeight;
        ColorTableParameter*  m_colorTable;
        BoolParameter*   m_showWeightLegend;
        StringParameter* m_legendCaption;
        IntParameter*    m_legendTicks;
        IntParameter*    m_legendDigits;
    
        //Weight legend
        QLegend* m_weight_legend;
};
    
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_CUBICSPLINELISTVIEWCONTROLLER_HXX
