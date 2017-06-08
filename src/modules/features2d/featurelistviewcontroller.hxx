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

#ifndef GRAIPE_FEATURES2D_FEATURELISTVIEWCONTROLLER_HXX
#define GRAIPE_FEATURES2D_FEATURELISTVIEWCONTROLLER_HXX

#include "core/viewcontroller.hxx"
#include "core/qt_ext/qlegend.hxx"

#include "features2d/featurelist.hxx"
#include "features2d/featureliststatistics.hxx"
#include "features2d/config.hxx"

namespace graipe {

/**
 * A class for viewing a 2D point feature list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT PointFeatureList2DViewController
:	public ViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param features The point feature list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        PointFeatureList2DViewController(QGraphicsScene * scene, PointFeatureList2D* features, int z_value = 0);
        
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~PointFeatureList2DViewController();
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "PointFeatureList2DViewController"
         */
		QString typeName() const
        {
            return "PointFeatureList2DViewController";
        }
    
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
         * The bounding rect of the point feature list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
        
    protected:
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent (QGraphicsSceneMouseEvent * event);
    
        //Statistics
        PointFeatureList2DStatistics * m_stats;
    
        //Aditional parameters
        BoolParameter* m_showLabels;
        FloatParameter* m_fontSize;
        EnumParameter* m_mode;
        FloatParameter* m_radius;
        ColorParameter * m_color;
};

/**
 * A class for viewing a 2D weighted feature list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT WeightedPointFeatureList2DViewController
:	public ViewController
{
	public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param features The weighted point feature list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		WeightedPointFeatureList2DViewController(QGraphicsScene * scene, WeightedPointFeatureList2D * features, int z_value = 0);
		
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~WeightedPointFeatureList2DViewController();
	
        /**
         * The typename of this ViewController
         *
         * \return Always: "WeightedPointFeatureList2DViewController"
         */
		QString typeName() const
        {
            return "WeightedPointFeatureList2DViewController";
        }
    
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
         * The bounding rect of the weighted feature list.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
    
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
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent (QGraphicsSceneMouseEvent * event);
    
        //Statistics
        WeightedPointFeatureList2DStatistics * m_stats;
    
        //Additional parameters
        BoolParameter* m_showLabels;
        FloatParameter* m_fontSize;
        EnumParameter* m_mode;
        FloatParameter* m_radius;
        FloatParameter* m_minWeight;
        FloatParameter* m_maxWeight;
        ColorTableParameter* m_colorTable;
        BoolParameter * m_showWeightLegend;
        StringParameter* m_legendCaption;
        IntParameter* m_legendTicks;
        IntParameter* m_legendDigits;
    
        //Weight legend
        QLegend * m_weight_legend;
};

/**
 * A class for viewing a 2D edgel feature list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT EdgelFeatureList2DViewController 
:	public WeightedPointFeatureList2DViewController
{
	public:
        /**
         * Implementation/specialization of the WeightedPointFeatureList2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param features The edgel feature list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		EdgelFeatureList2DViewController(QGraphicsScene * scene, EdgelFeatureList2D* features, int z_value = 0);
		
        /**
         * Implementation/specialization of the WeightedPointFeatureList2DViewController's virtual
         * destructor.
         */
        ~EdgelFeatureList2DViewController();
		
        /**
         * The typename of this ViewController
         *
         * \return Always: "EdgelFeatureList2DViewController"
         */
		QString typeName() const
        {
            return "EdgelFeatureList2DViewController";
        }
    
        /**
         * Implementation/specialization of the WeightedPointFeatureList2DViewController's paint procedure. This is called
         * by the QGraphicsView on every re-draw request.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	protected:
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent (QGraphicsSceneMouseEvent * event);
        
        //Statistics
        EdgelFeatureList2DStatistics * m_stats;
};

/**
 * A class for viewing a 2D SIFT feature list on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_FEATURES2D_EXPORT SIFTFeatureList2DViewController 
:	public EdgelFeatureList2DViewController
{
	public:
        /**
         * Implementation/specialization of the EdgelFeatureList2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param features The SIFT feature list, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		SIFTFeatureList2DViewController(QGraphicsScene * scene, SIFTFeatureList2D* features, int z_value = 0);
		
        /**
         * Implementation/specialization of the EdgelFeatureList2DViewController's virtual
         * destructor.
         */
        ~SIFTFeatureList2DViewController();
		
        /**
         * The typename of this ViewController
         *
         * \return Always: "SIFTFeatureList2DViewController"
         */
		QString typeName() const
        {
            return "SIFTFeatureList2DViewController";
        }
    
        /**
         * Implementation/specialization of the EdgelFeatureList2DViewController's paint procedure. This is called
         * by the QGraphicsView on every re-draw request.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
	protected:
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
        
        //Statistics
        SIFTFeatureList2DStatistics * m_stats;
};
    
} //end of namespace graipe

#endif //GRAIPE_FEATURES2D_FEATURELISTVIEWCONTROLLER_HXX
