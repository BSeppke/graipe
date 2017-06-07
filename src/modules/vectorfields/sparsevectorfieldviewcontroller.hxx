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

#ifndef GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDVIEWCONTROLLER_HXX
#define GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDVIEWCONTROLLER_HXX

#include "core/viewcontroller.hxx"

#include "vectorfields/vectordrawer.hxx"
#include "vectorfields/sparsevectorfield.hxx"
#include "vectorfields/sparsevectorfieldstatistics.hxx"
#include "vectorfields/config.hxx"

#include <QInputDialog>
#include <QMessageBox>

namespace graipe {

/**
 * A class for viewing of a sparse vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
template <class T>
class GRAIPE_VECTORFIELDS_EXPORT SparseVectorfield2DViewControllerBase
:	public ViewController
{
	public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The sparse vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		SparseVectorfield2DViewControllerBase(QGraphicsScene * scene, SparseVectorfield2DBase<T>* vf, int z_value=0)
        :	ViewController(scene, vf, z_value),
            m_lineWidth(new FloatParameter("Arrow width:", 0,100000,1)),
            m_headSize(new FloatParameter("Head size:",0,100000,0.3f)),
            m_minLength(new FloatParameter("Min. length (px.):",0,10000,0)),
            m_maxLength(new FloatParameter("Max. length (px.):",0,10000,0)),
            m_colorTable(new ColorTableParameter("Color:")),
            m_normalizeLength(new BoolParameter("Fix vector length?",false)),
            m_normalizedLength(new FloatParameter("Fixed length:", 1.0e-6f, 1.0e6f, 10, m_normalizeLength)),
            m_displayMotionMode(NULL),
            m_showVelocityLegend(new BoolParameter("Show velocity legend?",false)),
            m_velocityLegendCaption(new StringParameter("Legend caption:", "lengths", 20, m_showVelocityLegend)),
            m_velocityLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showVelocityLegend)),
            m_velocityLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showVelocityLegend)),
            m_mode(NULL),
            m_velocity_legend(NULL)
        {
            QStringList displayMotionModes;
                displayMotionModes.append("Complete motion");
                displayMotionModes.append("Local motion");
                displayMotionModes.append("Global motion");
            m_displayMotionMode = new EnumParameter("Display:", displayMotionModes);
            
            QStringList modes;
                modes.append("Select");
                modes.append("Create");
                modes.append("Delete");
            m_mode = new EnumParameter("Mode:", modes);
            
            m_parameters->addParameter("arrowWidth", m_lineWidth);
            m_parameters->addParameter("headSize", m_headSize);
            m_parameters->addParameter("minLength", m_minLength);
            m_parameters->addParameter("maxLength", m_maxLength);
            m_parameters->addParameter("colorTable", m_colorTable);
            m_parameters->addParameter("normalizeLength", m_normalizeLength);
            m_parameters->addParameter("normalizedLength", m_normalizedLength);
            m_parameters->addParameter("displayMotionMode", m_displayMotionMode);
            m_parameters->addParameter("showVelocityLegend", m_showVelocityLegend);
            m_parameters->addParameter("velocityLegendCaption", m_velocityLegendCaption);
            m_parameters->addParameter("velocityLegendTicks", m_velocityLegendTicks);
            m_parameters->addParameter("velocityLegendDigits", m_velocityLegendDigits);
            m_parameters->addParameter("mode", m_mode);
        }

                
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
		~SparseVectorfield2DViewControllerBase()
        {
            //The command "delete m_parameters;" inside the base class
            //will also delete all other (newly introduced) parameters
            delete m_velocity_legend;
        }
    
  
        /**
         * Specialization of the update of the view according to the current parameter settings.
         */
        void updateView()
        {
            ViewController::updateView();
            
            m_vector_drawer.setLineWidth(m_lineWidth->value());
            m_vector_drawer.setHeadSize(m_headSize->value());
            m_vector_drawer.setColorTable(m_colorTable->value());
            
            SparseVectorfield2DBase<T>* vf = static_cast<SparseVectorfield2DBase<T>*> (model());
            
            //Display arrows length scaled
            if(m_normalizeLength->value() && m_normalizeLength->value() != 0)
            {
                //if scaled, we need the color for vector-lengths to display an appropriate legend
                m_velocity_legend->fixScale(false);
                if(vf->scale() != 0)
                {
                    m_velocity_legend->setValueRange(m_minLength->value()*vf->scale(), m_maxLength->value()*vf->scale());
                }
                else
                {
                    m_velocity_legend->setValueRange(m_minLength->value(), m_maxLength->value());
                }
                m_velocity_legend->setColorTable(m_colorTable->value());
            } 
            //Display arrows at original length
            else
            {
                //if not scaled, we keep the size of the legend relative to real vector-size and set the 
                //color for vector-lengths to display an appropriate legend
                QRectF r_old = m_velocity_legend->rect();
                
                m_velocity_legend->fixScale(true);
                if(vf->scale() != 0)
                {
                    m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value()*vf->scale(), r_old.height()));
                    m_velocity_legend->setValueRange(0, m_maxLength->value()*vf->scale());
                }
                else
                {
                    m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value(), r_old.height()));
                    m_velocity_legend->setValueRange(0, m_maxLength->value());
                }
                m_velocity_legend->setColorTable(m_colorTable->value());
            }
            
            //set the caption of that legend
            m_velocity_legend->setCaption(m_velocityLegendCaption->value());
            m_velocity_legend->setTicks(m_velocityLegendTicks->value());
            m_velocity_legend->setDigits(m_velocityLegendDigits->value());
            
            m_velocity_legend->setVisible(m_showVelocityLegend->value());
        }
    
		//Statistics
		SparseVectorfield2DStatistics * m_stats;
    
        //Additional parameters:
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
        EnumParameter   * m_mode;
        
		//Length/velocity legend
		QLegend * m_velocity_legend;	
    
        //Drawing vectors
        VectorDrawer m_vector_drawer;
};


class GRAIPE_VECTORFIELDS_EXPORT SparseVectorfield2DViewController
:	public SparseVectorfield2DViewControllerBase<Vector2D>
{
    public:
        /**
         * Specialization of the SparseVectorfield2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The sparse weighted vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        SparseVectorfield2DViewController(QGraphicsScene * scene, SparseVectorfield2D* vf, int z_value=0);

        /**
         * The typename of this ViewController
         *
         * \return Always: "SparseVectorfield2DViewController"
         */
		QString typeName() const
        { 
            return "SparseVectorfield2DViewController";
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
         * The bounding rect of the sparse vectorfield.
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
    
        //Statistics of the view-controlled vectorfield
        SparseVectorfield2DStatistics m_stats;
    
};


/**
 * A class for viewing of a sparse weighted vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedVectorfield2DViewController 
:	public SparseVectorfield2DViewControllerBase<WeightedVector2D>
{
	public:
        /**
         * Specialization of the SparseWeightedVectorfield2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The sparse weighted vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		SparseWeightedVectorfield2DViewController(QGraphicsScene * scene, SparseWeightedVectorfield2D* vf, int z_value=0);
                
        /**
         * Specialization of the SparseVectorfield2DViewController's virtual
         * destructor.
         */
		~SparseWeightedVectorfield2DViewController();
				
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
         * \return Always: "SparseWeightedVectorfield2DViewController"
         */
		QString typeName() const
        {
            return "SparseWeightedVectorfield2DViewController";
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
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent (QGraphicsSceneMouseEvent * event);
    
        //Statistics of the view-controlled vectorfield
        SparseWeightedVectorfield2DStatistics m_stats;
    
        //Additional parameters:
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
 * A class for viewing of a sparse multi vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseMultiVectorfield2DViewController 
:	public SparseVectorfield2DViewControllerBase<MultiVector2D>
{
	
	public:
        /**
         * Specialization of the SparseVectorfield2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The sparse multi vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		SparseMultiVectorfield2DViewController(QGraphicsScene * scene, SparseMultiVectorfield2D* vf, int z_value=0);
                
        /**
         * Implementation/specialization of the SparseVectorfield2DViewController's virtual
         * destructor.
         */
		~SparseMultiVectorfield2DViewController();
		
        /**
         * The typename of this ViewController
         *
         * \return Always: "SparseMultiVectorfield2DViewController"
         */
		QString typeName() const
        {
            return "SparseMultiVectorfield2DViewController";
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
    
        //Statistics of the view-controlled vectorfield
        SparseMultiVectorfield2DStatistics m_stats;
    
        //Additional parameters
		IntParameter* m_showAlternative;
};




/**
 * A class for viewing of a sparse weighted multi vectorfield on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
class GRAIPE_VECTORFIELDS_EXPORT SparseWeightedMultiVectorfield2DViewController
:	public SparseVectorfield2DViewControllerBase<WeightedMultiVector2D>
{
	public:
        /**
         * Specialization of the SparseMultiVectorfield2DViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param vf The sparse weighted multi vectorfield, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
		SparseWeightedMultiVectorfield2DViewController(QGraphicsScene * scene, SparseWeightedMultiVectorfield2D* vf, int z_value=0);
                
        /**
         * Implementation/specialization of the SparseMultiVectorfield2DViewController's virtual
         * destructor.
         */
		~SparseWeightedMultiVectorfield2DViewController();
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "SparseWeightedMultiVectorfield2DViewController"
         */
		QString typeName() const
        {
            return "SparseWeightedMultiVectorfield2DViewController";
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
	
        //Statistics of the view-controlled vectorfield
        SparseWeightedMultiVectorfield2DStatistics m_stats;
    
        //Additional parameters
		IntParameter    * m_showAlternative;
        FloatParameter  * m_minWeight,
                        * m_maxWeight;
        BoolParameter   * m_showWeightLegend;
        StringParameter * m_weightLegendCaption;
        IntParameter    * m_weightLegendTicks;
        IntParameter    * m_weightLegendDigits;
        BoolParameter   * m_useColorForWeight;
        
		//Weight legend
		QLegend * m_weight_legend;
    
};
} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_SPARSEVECTORFIELDVIEWCONTROLLER_HXX
