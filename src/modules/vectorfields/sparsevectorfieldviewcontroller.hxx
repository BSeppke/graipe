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
           //TODO m_stats(new SparseVectorfield2DStatistics(vf)),
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
            
           /*TODO: m_headSize->setValue(m_stats->lengthStats().mean*0.3);
            
            //update according to statistics:
            m_minLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
            m_minLength->setValue(m_stats->lengthStats().min);
            m_maxLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
            m_maxLength->setValue(m_stats->lengthStats().max);
            */
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
            
            //create and position Legend:	
            QPointF legend_xy(0, vf->height());
            
            if(vf->scale()!=0)
            {
                m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                                150, 50,
                                                m_stats->lengthStats().min*vf->scale(), m_stats->lengthStats().max*vf->scale(),
                                                m_velocityLegendTicks->value(),
                                                true,
                                                this);
            }
            else
            {
                m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                                150, 50,
                                                m_stats->lengthStats().min, m_stats->lengthStats().max,
                                                m_velocityLegendTicks->value(),
                                                false,
                                                this);
            }
            
            m_velocity_legend->setTransform(transform());
            m_velocity_legend->setVisible(false);
            m_velocity_legend->setCaption(m_velocityLegendCaption->value());
            m_velocity_legend->setTicks(m_velocityLegendTicks->value());
            m_velocity_legend->setDigits(m_velocityLegendDigits->value());
            m_velocity_legend->setZValue(z_value);
            
            updateView();
        }

                
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
		~SparseVectorfield2DViewControllerBase()
        {
            //The command "delete m_parameters;" inside the base class
            //will also delete all other (newly introduced) parameters
            delete m_stats;
            delete m_velocity_legend;
        }


        /**
         * Implementation/specialization of the ViewController's paint procedure. This is called
         * by the QGraphicsView on every re-draw request.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
        { 
            ViewController::paintBefore(painter, option, widget);
            
            SparseVectorfield2D * vf = static_cast<SparseVectorfield2D *>(model());
            
            if(vf->isViewable())
            {
                painter->save();
                
                QPointFX origin, direction, target;
                QColor current_color;
                
                for(const Vector2D& v : *vf)
                {
                    float current_length = v.direction.length();
                    
                    if(current_length!=0 && (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()))
                    {
                        origin = v.origin;
                        
                        switch( m_displayMotionMode->value() )
                        {/*
                            case GlobalMotion:
                                direction = vf->globalDirection(i);
                                break;
                                
                            case LocalMotion:
                                direction = vf->localDirection(i);
                                break;
                           TODO*/
                            case CompleteMotion:
                            default:
                                direction = v.direction;
                                break;
                        }
                        
                        float len = direction.length();
                        
                        if(len!=0)
                        {
                            if(m_normalizeLength->value() && m_normalizedLength->value()!= 0)
                            {
                                direction=direction/len*m_normalizedLength->value();
                            }
                            
                            target = origin + direction;
                            
                            float normalized_weight = std::min(1.0f,std::max(0.0f,(current_length - m_minLength->value())/(m_maxLength->value() - m_minLength->value())));
                            m_vector_drawer.paint(painter, origin, target,normalized_weight);
                        }
                    }
                }
                
                painter->restore();
            }
            
            ViewController::paintAfter(painter, option, widget);
        }
    
        /**
         * The bounding rect of the sparse vectorfield.
         *
         * \return The bounding rectangle of this view.
         */
        QRectF boundingRect() const
        {
            qreal maxLength = m_stats->lengthStats().max;
            if(m_normalizeLength->value() && m_normalizedLength->value() != 0)
            {
                maxLength = m_normalizedLength->value();
            }
            //Always return rect in local coordinates -> global coords will be computed
            //by the scene by means of the sceneTransform of the item, which is handled
            //elsewhere.
            QRectF rect(QPointF( - maxLength,
                                 - maxLength),
                          QPointF(m_model->width()  + maxLength,
                                  m_model->height() + maxLength));

            return rect.united(ViewController::boundingRect());
        }
    
        /**
         * The typename of this ViewController
         *
         * \return Always: "SparseVectorfield2DViewController"
         */
		virtual QString typeName() const
        { 
            return "SparseVectorfield2DViewController";
        }
    
        /**
         * Specialization of the update of  the parameters of this ViewController according to the current
         * model's parameters. This is necessary, if something may have changed 
         * the model in meantime.
         * 
         * \param force_update If true, force every single parameter to update.
         */
        void updateParameters(bool force_update=false)
        {
            ViewController::updateParameters(force_update);

            SparseVectorfield2D* vf = static_cast<SparseVectorfield2D*>(model());
            
            SparseVectorfield2DStatistics* old_stats = static_cast<SparseVectorfield2DStatistics*>(m_stats);
            SparseVectorfield2DStatistics* new_stats = new SparseVectorfield2DStatistics(vf);
            
            //Check if min-max-statistics have changed:
            if( old_stats && new_stats &&
               (   new_stats->lengthStats().min != old_stats->lengthStats().min
                || new_stats->lengthStats().max != old_stats->lengthStats().max
                || force_update) )
            {
                m_stats = new_stats;
                delete old_stats;
                
                m_minLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
                m_maxLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
            }
            else
            {
                delete new_stats;
            }
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
            
            SparseVectorfield2D * vf = static_cast<SparseVectorfield2D*> (model());
            
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
    
    protected:
        /**
         * Implementation/specialization of the handling of a mouse-move event
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent(QGraphicsSceneHoverEvent * event)
        {	
            QGraphicsItem::hoverMoveEvent(event);
            
            if(acceptHoverEvents())
            {
                SparseVectorfield2D * vf = static_cast<SparseVectorfield2D *> (model());
                
                if(!vf->isViewable())
                    return;
                
                QPointF p = event->pos();
                float	x = p.x(),
                        y = p.y();
                SparseVectorfield2D::PointType mouse_pos(x,y);
                
                unsigned int i=0;
                
                if(		x >= 0 && x < vf->width() 
                   &&	y >= 0 && y < vf->height())
                {
                    QString vectors_in_reach;
                    for(const Vector2D& v: *vf)
                    {
                        SparseVectorfield2D::PointType ori = v.origin;
                        float d2 = QPointFX(ori-mouse_pos).squaredLength();
                        
                        if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                        {
                            vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(v.direction.length()).arg(v.direction.angle()).arg(sqrt(d2));
                        }
                        ++i;
                    }
                    if (vectors_in_reach.isEmpty()) 
                    {
                        vectors_in_reach = QString("<b>no vectors in reach</b>");
                    }
                    else
                    {
                        vectors_in_reach =		QString("<table><tr> <th>Idx</th> <th>x</th> <th>y</th> <th>length</th> <th>angle</th> <th>dist</th> </tr>")
                        +	vectors_in_reach
                        +	QString("</table>");
                    }
                    
                    emit updateStatusText(vf->shortName() + QString("[%1,%2]").arg(x).arg(y));
                    emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                                 +	vf->shortName()
                                                 +	QString("</i><br/>at position [%1,%2]:<br/>").arg(x).arg(y)
                                                 +	vectors_in_reach);
                    
                    event->accept();
                }
            }
        }
    
        /**
         * Implementation/specialization of the handling of a mouse-pressed event
         *
         * \param event The mouse event which triggered this function.
         */
        void mousePressEvent (QGraphicsSceneMouseEvent * event)
        {
            QGraphicsItem::mousePressEvent(event);
            
            if(acceptHoverEvents())
            {
                SparseVectorfield2D * vf = static_cast<SparseVectorfield2D *> (model());
                
                if(!vf->isViewable())
                    return;
                
                QPointF p = event->pos();
                float	x = p.x(),
                        y = p.y();
                SparseVectorfield2D::PointType mouse_pos(x,y);
                
                if(   x >= 0 && x < vf->width()
                   && y >= 0 && y < vf->height())
                {                
                    switch (m_mode->value())
                    {
                        case 0:
                            break;
                        case 1:
                        {
                            bool ok;
                            double dir_x = QInputDialog::getDouble(NULL, QString("Values for new vector at (%1, %2) -> (?, dir_y)").arg(x).arg(y), QString("x-direction (px) of new vector"), 0.0, -999999.99, 999999.99, 2, &ok);
                            if(ok)
                            {
                                double dir_y = QInputDialog::getDouble(NULL, QString("Values for new vector at (%1, %2) -> (%3, dir_y)").arg(x).arg(y).arg(dir_x), QString("y-direction (px) of new vector"), 0.0, -999999.99, 999999.99, 2, &ok);
                                if(ok)
                                {
                                    Vector2D new_v;
                                        new_v.origin=mouse_pos;
                                        new_v.direction=SparseVectorfield2D::PointType(dir_x, dir_y);
                                    vf->append(new_v);
                                }
                            }
                        }
                            break;
                        case 2:
                            for(unsigned int i=0; i< vf->size(); ++i)
                            {
                                SparseVectorfield2D::PointType ori = vf->item(i).origin;
                                SparseVectorfield2D::PointType dir = vf->item(i).direction;
                                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                                
                                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                                {
                                    QString delete_string = QString("Do you want to delete vector: %1 at (%2, %3) -> (%4, %5)?").arg(i).arg(ori.x()).arg(ori.y()).arg(dir.x()).arg(dir.y());
                                    if ( QMessageBox::question(NULL, QString("Delete vector?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                                    {
                                        vf->remove(i);
                                    }
                                }
                            }
                            break;
                    }
                    updateParameters();
                }
            }
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
        SparseVectorfield2DViewController(QGraphicsScene * scene, SparseVectorfield2D* vf, int z_value=0)
        :	SparseVectorfield2DViewControllerBase(scene, vf, z_value)
        {
        }
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
         * specialization of the SparseVectorfield2DViewController's
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
		virtual QString typeName() const;
	
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
         * \return Always: "SparseMultiVectorfield2DViewController"
         */
		virtual QString typeName() const;
		
	
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
		//TODO SparseMultiVectorfield2DStatistics * m_stats;
    
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
         * \return Always: "SparseWeightedMultiVectorfield2DViewController"
         */
		virtual QString typeName() const;
	
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
		//TODO SparseWeightedMultiVectorfield2DStatistics * m_stats;
    
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
