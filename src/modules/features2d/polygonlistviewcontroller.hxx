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

#ifndef GRAIPE_FEATURES2D_POLYGONLISTVIEWCONTROLLER_HXX
#define GRAIPE_FEATURES2D_POLYGONLISTVIEWCONTROLLER_HXX

#include "core/viewcontroller.hxx"
#include "core/qt_ext/qlegend.hxx"

#include "features2d/polygonlist.hxx"
#include "features2d/polygonliststatistics.hxx"
#include "features2d/config.hxx"

namespace graipe {
    

/**
 * A class for viewing a 2D polygon list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT PolygonList2DViewController
:	public ViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param polygons The polygon list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        PolygonList2DViewController(QGraphicsScene * scene, PolygonList2D* polygons, int z_value = 0);
    
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~PolygonList2DViewController();
        
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
         * The bounding rect of the polygon list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "PolygonList2DViewController"
         */
        QString typeName() const
        {
            return "PolygonList2DViewController";
        }
        
    protected:
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent(QGraphicsSceneMouseEvent * event);
    
    
        //Pointer to the polygons (to avaid casts)
        PolygonList2D* m_polygons;
    
        //Statistics
        PolygonList2DStatistics * m_stats;
    
        //Additional parameters
        BoolParameter* m_showLabels;
        FloatParameter* m_fontSize;
        EnumParameter* m_mode;
        FloatParameter* m_lineWidth;
        ColorParameter *m_color;
};
   
/**
 * A class for viewing a 2D weighted polygon list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPolygonList2DViewController
:	public ViewController
{
        
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param polygons The weighted polygon list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        WeightedPolygonList2DViewController(QGraphicsScene * scene, WeightedPolygonList2D* polygons, int z_value = 0);
    
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~WeightedPolygonList2DViewController();
    
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
         * The bounding rect of the weighted polygon list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "WeightedPolygonList2DViewController"
         */
        QString typeName() const
        {
            return "WeightedPolygonList2DViewController";
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
        //Pointer to the weighted polygons (to avaid casts)
        WeightedPolygonList2D* m_polygons;
    
        //Statistics
        WeightedPolygonList2DStatistics * m_stats;
    
        //Additional parameters
        BoolParameter* m_showLabels;
        FloatParameter* m_fontSize;
        EnumParameter* m_mode;
        FloatParameter* m_lineWidth;
	    FloatParameter* m_minWeight;
        FloatParameter* m_maxWeight;
        ColorTableParameter * m_colorTable;
        BoolParameter * m_showWeightLegend;
        StringParameter* m_legendCaption;
        IntParameter* m_legendTicks;
        IntParameter* m_legendDigits;
    
        //Weight legend
        QLegend * m_weight_legend;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_POLYGONLISTVIEW_HXX
