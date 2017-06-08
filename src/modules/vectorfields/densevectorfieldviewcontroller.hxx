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

#ifndef GRAIPE_VECTORFIELDS_DENSEVECTORFIELDVIEWCONTROLLER_HXX
#define GRAIPE_VECTORFIELDS_DENSEVECTORFIELDVIEWCONTROLLER_HXX

#include "core/viewcontroller.hxx"

#include "vectorfields/vectordrawer.hxx"
#include "vectorfields/densevectorfield.hxx"
#include "vectorfields/densevectorfieldstatistics.hxx"
#include "vectorfields/config.hxx"

namespace graipe {

/**
 * A class for viewing of a dense vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseVectorfield2DViewController 
: public ViewController
{		
	public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The dense vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		DenseVectorfield2DViewController(QGraphicsScene * scene, DenseVectorfield2D* vf, int z_value=0);
                
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
		~DenseVectorfield2DViewController();

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
         * The bounding rect of the sparse vectorfield.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
            
        /**
         * The typename of this ViewController
         *
         * \return Always: "DenseVectorfield2DViewController"
         */
		QString typeName() const
        {
            return "DenseVectorfield2DViewController";
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
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);

	
		//Statistics
		DenseVectorfield2DStatistics* m_stats;
    
        //Additional parameters:
        PointParameter  * m_resolution;
        FloatParameter  * m_lineWidth,
                        * m_headSize;
        FloatParameter  * m_minLength;
        FloatParameter  * m_maxLength;
        ColorTableParameter  * m_colorTable;
        BoolParameter   * m_normalizeLength;
        FloatParameter  * m_normalizedLength;
        EnumParameter   * m_displayMotionMode;
        BoolParameter   * m_showVelocityLegend;
        StringParameter * m_velocityLegendCaption;
        IntParameter    * m_velocityLegendTicks;
        IntParameter    * m_velocityLegendDigits;
        
		//Length/velocity legend
		QLegend * m_velocity_legend;
	
        //Drawing vectors
        VectorDrawer m_vector_drawer;
};




/**
 * A class for viewing of a dense vectorfield by means of a particle flow
 * simulation on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseVectorfield2DParticleViewController
: public ViewController
{
	public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The dense vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		DenseVectorfield2DParticleViewController(QGraphicsScene * scene, DenseVectorfield2D* vf, int z_value=0);
                
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
		~DenseVectorfield2DParticleViewController();
    
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
         * The bounding rect of the dense vectorfield animation.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const;
            
        /**
         * The typename of this ViewController
         *
         * \return Always: "DenseVectorfield2DParticleViewController"
         */
		QString typeName() const
        {
            return "DenseVectorfield2DParticleViewController";
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
         * Slot for handling the update of the timer
         *
         * \param event The timer event which triggered this function.
         */
		void timerEvent(QTimerEvent *event);
    
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
	
        //Statistics
        DenseVectorfield2DStatistics* m_stats;

        //Controller's additional parameters:
        IntParameter    * m_particles;
        FloatParameter  * m_particleRadius;
        IntParameter    * m_particleLifetime,
                        * m_timerInterval;
        FloatParameter  * m_slowDown;
        FloatParameter  * m_minLength;
        FloatParameter  * m_maxLength;
        ColorTableParameter  * m_colorTable;
        EnumParameter   * m_displayMotionMode;
        BoolParameter   * m_showVelocityLegend;
        StringParameter * m_velocityLegendCaption;
        IntParameter    * m_velocityLegendTicks;
        IntParameter    * m_velocityLegendDigits;

        //Legend that is displayed if necessary
        QLegend * m_velocity_legend;
    
        DenseVectorfield2D* m_dense_model;
        
		int m_timing, m_timer_id;
	
		QVector<QPointF>      m_positions;
		QVector<unsigned int> m_lifetimes;
	
};




/**
 * A class for viewing of a dense weighted vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseWeightedVectorfield2DViewController 
:	public DenseVectorfield2DViewController
{
    public:
        /**
         * specialization of the DenseVectorfield2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The dense weighted vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		DenseWeightedVectorfield2DViewController(QGraphicsScene * scene, DenseWeightedVectorfield2D* vf, int z_value=0);
                
        /**
         * Specialization of the DenseVectorfield2DViewController's virtual
         * destructor.
         */
	   ~DenseWeightedVectorfield2DViewController();
				
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
         * The typename of this ViewController
         *
         * \return Always: "DenseWeightedVectorfield2DViewController"
         */
		QString typeName() const
        {
            return "DenseWeightedVectorfield2DViewController";
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
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        //Controller's additional parameters:
        FloatParameter  * m_minWeight,
                        * m_maxWeight;
        BoolParameter   * m_showWeightLegend;
        StringParameter * m_weightLegendCaption;
        IntParameter    * m_weightLegendTicks;
        IntParameter    * m_weightLegendDigits;
        BoolParameter   * m_useColorForWeight;
    
		//Legend that is displayed if necessary
		QLegend * m_weight_legend;
    
};




/**
 * A class for viewing of a dense weighted vectorfield by means of a particle flow
 * simulation on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseWeightedVectorfield2DParticleViewController
: public DenseVectorfield2DParticleViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The dense weighted vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		DenseWeightedVectorfield2DParticleViewController(QGraphicsScene * scene, DenseWeightedVectorfield2D* vf, int z_value=0);
                
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
		~DenseWeightedVectorfield2DParticleViewController();
    
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
         * The typename of this ViewController
         *
         * \return Always: "DenseWeightedVectorfield2DParticleViewController"
         */
		QString typeName() const
        {
            return "DenseWeightedVectorfield2DParticleViewController";
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
         * Slot for handling the update of the timer
         *
         * \param event The timer event which triggered this function.
         */
		void timerEvent(QTimerEvent *event);
    
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        //Controller's additional parameters:
        FloatParameter  * m_minWeight,
                        * m_maxWeight;
        BoolParameter   * m_showWeightLegend;
        StringParameter * m_weightLegendCaption;
        IntParameter    * m_weightLegendTicks;
        IntParameter    * m_weightLegendDigits;
        BoolParameter   * m_useColorForWeight;
    
		//Legend that is displayed if necessary
		QLegend * m_weight_legend;
    
        //To avoid casting
        DenseWeightedVectorfield2D* m_dense_weighted_model;
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_DENSEVECTORFIELDVIEWCONTROLLER_HXX
