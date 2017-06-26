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

#include "vectorfields/densevectorfieldviewcontroller.hxx"
#include <vigra/convolution.hxx>
#include "vigra/transformimage.hxx"
#include "vigra/functorexpression.hxx"

#include <QInputDialog>
#include <QMessageBox>

namespace graipe {

/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The dense vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
DenseVectorfield2DViewController::DenseVectorfield2DViewController(DenseVectorfield2D * vf)
:	ViewController(vf),
	m_stats(new DenseVectorfield2DStatistics(vf)),
    m_resolution(new PointParameter("Resolution of vectors:",QPoint(1,1), QPoint(vf->width(),vf->height()), QPoint(vf->width()/50,vf->width()/50))),
    m_lineWidth(new FloatParameter("Line width:", 0,100000,1)),
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
    m_velocity_legend(NULL)
{    
	QStringList displayMotionModes;
		displayMotionModes.append("Complete motion");
		displayMotionModes.append("Local motion");
		displayMotionModes.append("Global motion");
    m_displayMotionMode = new EnumParameter("Display:", displayMotionModes);
    
    m_headSize->setValue(m_stats->lengthStats().mean*0.3);
    
    //update according to statistics:
    m_minLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_minLength->setValue(m_stats->lengthStats().min);
    m_maxLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_maxLength->setValue(m_stats->lengthStats().max);
    
    m_parameters->addParameter("resolution", m_resolution);
    m_parameters->addParameter("lineWidth", m_lineWidth);
    m_parameters->addParameter("headSize", m_headSize);
    m_parameters->addParameter("minLength", m_minLength);
    m_parameters->addParameter("maxLength", m_maxLength);
    m_parameters->addParameter("colorTable", m_colorTable);
    m_parameters->addParameter("normalizeLength", m_normalizeLength);
    m_parameters->addParameter("normalizedLength", m_normalizedLength);
    m_parameters->addParameter("displayMotionMode", m_displayMotionMode);
    m_parameters->addParameter("showVelocityLegend", m_showVelocityLegend);
    m_parameters->addParameter("velocityLegendCaption", m_velocityLegendCaption);

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
	m_velocity_legend->setZValue(zValue());
	
	updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
DenseVectorfield2DViewController::~DenseVectorfield2DViewController()
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
void DenseVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	DenseVectorfield2D * vf = static_cast<DenseVectorfield2D *> (model());
    
    if(vf->isViewable())
    {
        painter->save();
        
        int step_y = vf->height()/m_resolution->value().y(),
            step_x = vf->width()/m_resolution->value().x();
        
        QPointFX origin, direction, target;
        
        for(unsigned int y=step_y/2; y < vf->height(); y+=step_x)
        {
            for(unsigned int x=step_x/2; x < vf->width(); x+=step_y)
            {
                float current_length = vf->length(x,y);
                
                if(current_length!=0 && (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()))
                {
                    origin.setX(x);
                    origin.setY(y);
                    
                    switch( m_displayMotionMode->value() )
                    {
                        case GlobalMotion:
                            direction = vf->globalDirection(x,y);
                            break;
                            
                        case LocalMotion:
                            direction = vf->localDirection(x,y);
                            break;
                            
                        case CompleteMotion:
                        default:
                            direction = vf->direction(x,y);
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
                    
                        m_vector_drawer.paint(painter, origin, target, normalized_weight);
                    }
                }
            }
        }
        
        painter->restore();
    }
    
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the dense vectorfield.
 *
 * \return The bounding rectangle of this view.
 */
QRectF DenseVectorfield2DViewController::boundingRect() const
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
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void DenseVectorfield2DViewController::updateParameters(bool force_update)
{
    ViewController::updateParameters(force_update);

    DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>(model());
    
    DenseVectorfield2DStatistics* old_stats = static_cast<DenseVectorfield2DStatistics*>(m_stats);
    DenseVectorfield2DStatistics* new_stats = new DenseVectorfield2DStatistics(vf);
	
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
void DenseVectorfield2DViewController::updateView()
{
    ViewController::updateView();
    
	DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>(model());
    
    m_vector_drawer.setLineWidth(m_lineWidth->value());
    m_vector_drawer.setHeadSize(m_headSize->value());
    m_vector_drawer.setColorTable(m_colorTable->value());
    
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

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void DenseVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);

	if(acceptHoverEvents())
    {
        DenseVectorfield2D * vf = static_cast<DenseVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {
            float length = vf->length(x,y);
            float angle = vf->angle(x,y);
            DenseVectorfield2D::PointType dir = vf->direction(x,y),
                                        g_dir = vf->globalDirection(x,y),
                                        l_dir = vf->localDirection(x,y);
            
            emit updateStatusText(vf->shortName() + QString::fromUtf8("[%1,%2] = angle:%3°, length:%4px").arg(x).arg(y).arg(angle).arg(length));
            
            QString message("<b>Mouse moved over Object: </b><br/><i>");
                message +=	vf->shortName()
                        +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                        +	QString::fromUtf8("<br/> <b>angle: %1°</b>").arg(angle)
                        +	QString("<br/> <b>length: %1px</b>").arg(length)
                        +	QString("<br/> <b>direction vector: (%1,%2)px</b>").arg(dir.x()).arg(dir.y());
            
            if( g_dir.length() !=0)
            {
                message +=	QString("<br/> <b>&nbsp;&nbsp;&nbsp;global part: (%1,%2)px</b>").arg(g_dir.x()).arg(g_dir.y())
                        +	QString("<br/> <b>&nbsp;&nbsp;&nbsp;local part: (%1,%2)px</b>").arg(l_dir.x()).arg(l_dir.y());
            }
            
            emit updateStatusDescription(message);
            
            event->accept();
        }
    }
}





/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The dense vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
DenseVectorfield2DParticleViewController::DenseVectorfield2DParticleViewController(DenseVectorfield2D * vf)
:	ViewController(vf),
	m_stats(new DenseVectorfield2DStatistics(vf)),
    m_particles(new IntParameter("Particles:",1,1000000,1000)),
    m_particleRadius(new FloatParameter("Particle size:", 0,100000,1)),
    m_particleLifetime(new IntParameter("Particle lifetime (ticks):", 1,1000,50)),
    m_timerInterval(new IntParameter("Timer interval (ms):", 1,1000,50)),
    m_slowDown(new FloatParameter("Slow down factor:",0,10000,1)),
    m_minLength(new FloatParameter("Min. length (px.):",0,10000,0)),
    m_maxLength(new FloatParameter("Max. length (px.):",0,10000,0)),
    m_colorTable(new ColorTableParameter("Color:")),
    m_displayMotionMode(NULL),
    m_showVelocityLegend(new BoolParameter("Show velocity legend?",false)),
    m_velocityLegendCaption(new StringParameter("Legend caption:","weights", 20, m_showVelocityLegend)),
    m_velocityLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showVelocityLegend)),
    m_velocityLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showVelocityLegend)),
    m_velocity_legend(NULL),
    m_dense_model(vf)
{
    QStringList displayMotionModes;
		displayMotionModes.append("Complete motion");
		displayMotionModes.append("Local motion");
		displayMotionModes.append("Global motion");
    m_displayMotionMode = new EnumParameter("Display:", displayMotionModes);
    
	m_parameters->addParameter("particles", m_particles);
    m_parameters->addParameter("radius", m_particleRadius);
    m_parameters->addParameter("lifetime", m_particleLifetime);
    m_parameters->addParameter("interval", m_timerInterval);
    m_parameters->addParameter("slowDown", m_slowDown);
    m_parameters->addParameter("minLength", m_minLength);
    m_parameters->addParameter("maxLength", m_maxLength);
    m_parameters->addParameter("colorTable", m_colorTable);
    m_parameters->addParameter("displayMotionMode", m_displayMotionMode);
    m_parameters->addParameter("showVelocityLegend", m_showVelocityLegend);
    m_parameters->addParameter("velocityLegendCaption", m_velocityLegendCaption);
    m_parameters->addParameter("velocityLegendTicks", m_velocityLegendTicks);
    m_parameters->addParameter("velocityLegendDigits", m_velocityLegendDigits);
    
	if(vf->scale() == 0)
		m_velocityLegendCaption->setValue("velocity");
	else
		m_velocityLegendCaption->setValue("velocity (cm/s)");
    
    //update according to statistics:
    m_minLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_minLength->setValue(m_stats->lengthStats().min);
    m_maxLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_maxLength->setValue(m_stats->lengthStats().max);
	
	//create and position Legend:	
	QPointF legend_xy(0, vf->height());
	
	if (vf->scale() !=0)
	{
		m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats->lengthStats().min*vf->scale(), m_stats->lengthStats().max*vf->scale(),
                                        m_velocityLegendTicks->value(),
                                        false,
                                        this);
	}
	else{
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
    
	m_velocity_legend->setZValue(zValue());
    
    updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
DenseVectorfield2DParticleViewController::~DenseVectorfield2DParticleViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    
	if(m_timer_id != -1)
		killTimer(m_timer_id);
    
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
void DenseVectorfield2DParticleViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
    if(m_dense_model->isViewable())
    {
        painter->save();

        QPen   dotPen;
            
        for(unsigned int i=0 ; i < (unsigned int)m_positions.size(); i++)
        {	
            float current_length = m_dense_model->length(m_positions[i].x(),m_positions[i].y());
            
            float normalized_weight = std::min(1.0f,std::max(0.0f,(current_length - m_minLength->value())/(m_maxLength->value() - m_minLength->value())));

            dotPen.setColor(QColor(m_colorTable->value().at(normalized_weight*255)));
            painter->setPen(dotPen);
            painter->setBrush(dotPen.color());
            
            
            if(m_lifetimes[i] && ViewController::rect().contains(m_positions[i]) )
            {
                painter->drawEllipse(m_positions[i],m_particleRadius->value(),m_particleRadius->value());
            }
        }
        painter->restore();
    }
    
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the dense vectorfield animation.
 *
 * \return The bounding rectangle of this view.
 */
QRectF DenseVectorfield2DParticleViewController::boundingRect() const
{ 
	float d = 2*m_particleRadius->value();
	
	//Always return rect in local coordinates -> global coords will be computed
	//by the scene by means of the sceneTransform of the item, which is handled
	//elsewhere.	
	QRectF rect(QPointF( - d,
						 - d),
				QPointF(m_dense_model->width()  + d,
						m_dense_model->height() + d));
	return rect.united(ViewController::boundingRect());
}

/**
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void DenseVectorfield2DParticleViewController::updateParameters(bool force_update)
{
    ViewController::updateParameters(force_update);

    DenseVectorfield2D* vf = static_cast<DenseVectorfield2D*>(model());
    
    DenseVectorfield2DStatistics* old_stats = static_cast<DenseVectorfield2DStatistics*>(m_stats);
    DenseVectorfield2DStatistics* new_stats = new DenseVectorfield2DStatistics(vf);
	
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
void DenseVectorfield2DParticleViewController::updateView()
{
	ViewController::updateView();
	
	//Underly colorful gradient of velocity to legend
	if(m_dense_model->scale() != 0)
	{
        m_velocity_legend->setValueRange(m_minLength->value()*m_dense_model->scale(), m_maxLength->value()*m_dense_model->scale());
    }
	else
	{
        m_velocity_legend->setValueRange(m_minLength->value(), m_maxLength->value());
    }
    
    m_velocity_legend->setColorTable(m_colorTable->value());
    
    //set the caption of that legend
    m_velocity_legend->setCaption(m_velocityLegendCaption->value());
    m_velocity_legend->setTicks(m_velocityLegendTicks->value());
    m_velocity_legend->setDigits(m_velocityLegendDigits->value());
    
    m_velocity_legend->setVisible(m_showVelocityLegend->value());
    
	if(m_particles->value() !=  m_positions.size())
	{
        unsigned int new_particle_count = m_particles->value();
        
        m_positions.resize(new_particle_count);
        m_lifetimes.resize(new_particle_count);
	
        int w = model()->width(),
            h = model()->height();
        
        for(unsigned int i=0; i < new_particle_count; i++)
        {		
            m_positions[i] = QPointF(rand() % w, rand() % h);
            m_lifetimes[i] = m_particleLifetime->value();
        }
	}
    
	if(m_timerInterval->value() != m_timing)
	{
        if (m_timer_id != -1)
            killTimer(m_timer_id);
        
        m_timing = m_timerInterval->value();
        
        m_timer_id = QObject::startTimer(m_timing);
    }
}

/**
 * Slot for handling the update of the timer
 *
 * \param event The timer event which triggered this function.
 */
void DenseVectorfield2DParticleViewController::timerEvent(QTimerEvent *event)
{
	
	if(	!this->isVisible() || event->timerId() != m_timer_id)
		return;
	
    if(!m_dense_model->isViewable())
        return;
	
	int x,y;
	float current_length;
    
    DenseVectorfield2D::PointType dir;
	
	for(unsigned int i=0; i < (unsigned int)m_positions.size(); i++)
	{
		x=(int)(m_positions[i].x()+0.5);
		y=(int)(m_positions[i].y()+0.5);
        dir = m_dense_model->direction(x,y);
        
		current_length = m_dense_model->length(x,y);
        
		switch( m_displayMotionMode->value() )
		{
			case GlobalMotion:
				dir = m_dense_model->globalDirection(x,y);
				break;
				
			case LocalMotion:
				dir = m_dense_model->localDirection(x,y);
				break;
			case CompleteMotion:
            default:
				dir = m_dense_model->direction(x,y);
                break;
		}
		
		if (	m_lifetimes[i] > 0
			&&	x >= 0 && x < (int)m_dense_model->width()
			&&	y >= 0 && y < (int)m_dense_model->height()
			&&	current_length >= m_minLength->value() && current_length <= m_maxLength->value() )
		{		
			m_positions[i].rx() += dir.x()/m_slowDown->value();
			m_positions[i].ry() += dir.y()/m_slowDown->value();
			
			m_lifetimes[i]--;
		}
		else
		{
			m_positions[i] = QPointF(rand() % (int)m_dense_model->width(),rand() % (int)m_dense_model->height());
			m_lifetimes[i] = m_particleLifetime->value();
		}
	}
	update();
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void DenseVectorfield2DParticleViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
	if(acceptHoverEvents())
    {
        DenseVectorfield2D * vf = static_cast<DenseVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {

            float length = vf->length(x,y);
            float angle = vf->angle(x,y);
            DenseVectorfield2D::PointType dir = vf->direction(x,y),
                                        g_dir = vf->globalDirection(x,y),
                                        l_dir = vf->localDirection(x,y);
            
            emit updateStatusText(vf->shortName() + QString::fromUtf8("[%1,%2] = angle:%3°, length:%4px").arg(x).arg(y).arg(angle).arg(length));
            
            QString message("<b>Mouse moved over Object: </b><br/><i>");
                message +=	vf->shortName()
                        +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                        +	QString::fromUtf8("<br/> <b>angle: %1°</b>").arg(angle)
                        +	QString("<br/> <b>length: %1px</b>").arg(length)
                        +	QString("<br/> <b>direction vector: (%1,%2)px</b>").arg(dir.x()).arg(dir.y());
            
            if( g_dir.length() !=0)
            {
                message +=	QString("<br/> <b>&nbsp;&nbsp;&nbsp;global part: (%1,%2)px</b>").arg(g_dir.x()).arg(g_dir.y())
                       +	QString("<br/> <b>&nbsp;&nbsp;&nbsp;local part: (%1,%2)px</b>").arg(l_dir.x()).arg(l_dir.y());
            }
            
            emit updateStatusDescription(message);
            
            event->accept();
        }
    }
}




/**
 * specialization of the DenseVectorfield2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The dense weighted vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
DenseWeightedVectorfield2DViewController::DenseWeightedVectorfield2DViewController(DenseWeightedVectorfield2D * vf)
: DenseVectorfield2DViewController(vf),
    m_minWeight(new FloatParameter("Min. weight:",-1.0e10,1.0e10,0)),
    m_maxWeight(new FloatParameter("Max. weight:",-1.0e10,1.0e10,1)),
    m_showWeightLegend(new BoolParameter("Show weight legend?", false)),
    m_weightLegendCaption(new StringParameter("Legend caption:","weights", 20, m_showWeightLegend)),
    m_weightLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_weightLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend)),
    m_useColorForWeight(new BoolParameter("Use colors for weights?",false)),
    m_weight_legend(NULL)
{	
	//create statistics
	delete m_stats;
	DenseWeightedVectorfield2DStatistics* stats = new DenseWeightedVectorfield2DStatistics(vf);
	m_stats = stats;
    
    //update according to weight statistics:
    m_minWeight->setRange(floor(stats->weightStats().min), ceil(stats->weightStats().max));
    m_minWeight->setValue(stats->weightStats().min);
    m_maxWeight->setRange(floor(stats->weightStats().min), ceil(stats->weightStats().max));
    m_maxWeight->setValue(stats->weightStats().max);
    
    m_parameters->addParameter("minWeight",m_minWeight);
    m_parameters->addParameter("maxWeight",m_maxWeight);
    m_parameters->addParameter("useColorForWeight",m_useColorForWeight);
    m_parameters->addParameter("showWeightLegend",m_showWeightLegend);
    m_parameters->addParameter("weightLegendCaption",m_weightLegendCaption);
    
    //create and position Legend:
    m_weight_legend = new QLegend(m_velocity_legend->rect().right()+5, m_velocity_legend->rect().top(),
                                  150, 50,
                                  stats->weightStats().min,stats->weightStats().max,
                                  m_weightLegendTicks->value(),
                                  false,
                                  this);
    
    m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_weightLegendCaption->value());
    m_weight_legend->setTicks(m_weightLegendTicks->value());
    m_weight_legend->setDigits(m_weightLegendDigits->value());
    m_weight_legend->setZValue(zValue());
    
    updateView();
}

/**
 * Specialization of the DenseVectorfield2DViewController's virtual
 * destructor.
 */
DenseWeightedVectorfield2DViewController::~DenseWeightedVectorfield2DViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    delete m_weight_legend;
}

/**
 * Implementation/specialization of the ViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void DenseWeightedVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 	
	ViewController::paintBefore(painter, option, widget);
	
	DenseWeightedVectorfield2D * vf = static_cast<DenseWeightedVectorfield2D *> (model());
    
    if(vf->isViewable())
    {
        painter->save();
        
        QPointFX origin, direction, target;
        
        int step_y = vf->height()/m_resolution->value().y(),
            step_x = vf->width()/m_resolution->value().x();
        
        for(unsigned int y=step_y/2; y < vf->height(); y+=step_x)
        {
            for(unsigned int x=step_x/2; x < vf->width(); x+=step_y)
            {
                float current_length = vf->length(x,y);
                float current_weight = vf->weight(x,y);
                
                if(     current_length!=0
                    && (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value())
                    && (current_weight>= m_minWeight->value()) && (current_weight <= m_maxWeight->value()))
                {
                    origin.setX(x);
                    origin.setY(y);

                    switch( m_displayMotionMode->value() )
                    {
                        case GlobalMotion:
                            direction = vf->globalDirection(x,y);
                            break;
                            
                        case LocalMotion:
                            direction = vf->localDirection(x,y);
                            break;
                            
                        case CompleteMotion:
                        default:
                            direction = vf->direction(x,y);
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
                    
                        if (m_useColorForWeight->value() )
                        {
                            normalized_weight = (current_weight - m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value());
                        }
                        
                        m_vector_drawer.paint(painter, origin, target, normalized_weight);
                    }
                }
            }
        }
        
        painter->restore();
    }
	ViewController::paintAfter(painter, option, widget);
}

/**
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void DenseWeightedVectorfield2DViewController::updateParameters(bool force_update)
{
    DenseVectorfield2DViewController::updateParameters(force_update);
    
    DenseWeightedVectorfield2D* vf = static_cast<DenseWeightedVectorfield2D*>(model());
    
    DenseWeightedVectorfield2DStatistics* old_stats = static_cast<DenseWeightedVectorfield2DStatistics*>(m_stats);
    DenseWeightedVectorfield2DStatistics* new_stats = new DenseWeightedVectorfield2DStatistics(vf);
	
    //Check if min-max-statistics have changed:
    if( old_stats && new_stats &&
       (   new_stats->weightStats().min != old_stats->weightStats().min
        || new_stats->weightStats().max != old_stats->weightStats().max
        || force_update) )
    {
        m_stats = new_stats;
        delete old_stats;
        
        m_minWeight->setRange(floor(new_stats->weightStats().min), ceil(new_stats->weightStats().max));
        m_maxWeight->setRange(floor(new_stats->weightStats().min), ceil(new_stats->weightStats().max));
    }
    else
    {
        delete new_stats;
    }
}
        
/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void DenseWeightedVectorfield2DViewController::updateView()
{
	DenseVectorfield2DViewController::updateView();
	
	DenseWeightedVectorfield2D * vf	= static_cast<DenseWeightedVectorfield2D*> (model());
	
    //If the colors shall be used for weight coding
    if(m_useColorForWeight->value())
    {
        m_weight_legend->setColorTable(m_colorTable->value());
        m_weight_legend->setValueRange(m_minWeight->value(), m_maxWeight->value());
        m_weight_legend->setCaption(m_weightLegendCaption->value());
        m_weight_legend->setTicks(m_weightLegendTicks->value());
        m_weight_legend->setDigits(m_weightLegendDigits->value());
        
        m_weight_legend->setVisible(m_showWeightLegend->value());
        
        if(m_normalizeLength->value() && m_normalizedLength->value() != 0)
        {
            //we cannot enable a velocity legend then...
            m_velocity_legend->setVisible(false);
        }
        else
        {
            //and we will display it if needed
            if(	m_showVelocityLegend->value())
            {
                m_velocity_legend->fixScale(true);
                
                QRectF r_old = m_velocity_legend->rect();
                
                if(vf->scale() != 0)
                {
                    m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value()/vf->scale(), r_old.height()));
                }
                else
                {
                    m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value(), r_old.height()));
                }
                
                m_velocity_legend->setValueRange(0, m_maxLength->value());
                m_velocity_legend->setColorTable(colorTables()[0]);
                m_velocity_legend->setVisible(true);
            }			
        }
    }
    else
    {
        //if no colors are used for weights, we cannot show a legend for the weights...
        m_weight_legend->setVisible(false);
        
        //but we can show a legend of the lengths
        m_velocity_legend->setVisible(m_showVelocityLegend->value());
    }
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void DenseWeightedVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
	if(acceptHoverEvents())
    {
        DenseWeightedVectorfield2D * vf = static_cast<DenseWeightedVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
            
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {
            float length = vf->length(x,y);
            float angle = vf->angle(x,y);
            float weight = vf->weight(x,y);
            
            DenseVectorfield2D::PointType dir = vf->direction(x,y),
                                        g_dir = vf->globalDirection(x,y),
                                        l_dir = vf->localDirection(x,y);
            
            emit updateStatusText(vf->shortName() + QString::fromUtf8("[%1,%2] = angle:%3°, length:%4px").arg(x).arg(y).arg(angle).arg(length));
            
            QString message("<b>Mouse moved over Object: </b><br/><i>");
                message +=	vf->shortName()
                        +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                        +	QString::fromUtf8("<br/> <b>angle: %1°</b>").arg(angle)
                        +	QString("<br/> <b>length: %1px</b>").arg(length)
                        +	QString("<br/> <b>weight: %1</b>").arg(weight)
                        +	QString("<br/> <b>direction vector: (%1,%2)px</b>").arg(dir.x()).arg(dir.y());
            
            if( g_dir.length() !=0)
            {
                message +=	QString("<br/> <b>&nbsp;&nbsp;&nbsp;global part: (%1,%2)px</b>").arg(g_dir.x()).arg(g_dir.y())
                        +	QString("<br/> <b>&nbsp;&nbsp;&nbsp;local part: (%1,%2)px</b>").arg(l_dir.x()).arg(l_dir.y());
            }
            
            emit updateStatusDescription(message);
            
            event->accept();
        }
    }
}

/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The dense weighted vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
DenseWeightedVectorfield2DParticleViewController::DenseWeightedVectorfield2DParticleViewController(DenseWeightedVectorfield2D * vf)
:	DenseVectorfield2DParticleViewController(vf),
    m_minWeight(new FloatParameter("Min. weight:",-1.0e10,1.0e10,0)),
    m_maxWeight(new FloatParameter("Max. weight:",-1.0e10,1.0e10,1)),
    m_showWeightLegend(new BoolParameter("Show weight legend?", false)),
    m_weightLegendCaption(new StringParameter("Legend caption:","weights", 20, m_showWeightLegend)),
    m_weightLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_weightLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend)),
    m_useColorForWeight(new BoolParameter("Use colors for weights?",false)),
    m_weight_legend(NULL),
    m_dense_weighted_model(vf)
{	
	//create statistics
	delete m_stats;
	DenseWeightedVectorfield2DStatistics* stats = new DenseWeightedVectorfield2DStatistics(vf);
	m_stats = stats;
    
    //update according to weight statistics:
    m_minWeight->setRange(floor(stats->weightStats().min), ceil(stats->weightStats().max));
    m_minWeight->setValue(stats->weightStats().min);
    m_maxWeight->setRange(floor(stats->weightStats().min), ceil(stats->weightStats().max));
    m_maxWeight->setValue(stats->weightStats().max);
    
    m_parameters->addParameter("minWeight",m_minWeight);
    m_parameters->addParameter("maxWeight",m_maxWeight);
    m_parameters->addParameter("useColorForWeight",m_useColorForWeight);
    m_parameters->addParameter("showWeightLegend",m_showWeightLegend);
    m_parameters->addParameter("weightLegendCaption",m_weightLegendCaption);
    m_parameters->addParameter("weightLegendTicks",m_weightLegendTicks);
    m_parameters->addParameter("weightLegendDigits",m_weightLegendDigits);
    
    //create and position Legend:
    m_weight_legend = new QLegend(m_velocity_legend->rect().right()+5, m_velocity_legend->rect().top(),
                                  150, 50,
                                  stats->weightStats().min,stats->weightStats().max,
                                  m_weightLegendTicks->value(),
                                  false,
                                  this);
    
    m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_weightLegendCaption->value());
    m_weight_legend->setTicks(m_weightLegendTicks->value());
    m_weight_legend->setDigits(m_weightLegendDigits->value());
    m_weight_legend->setZValue(zValue());
	
	updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
DenseWeightedVectorfield2DParticleViewController::~DenseWeightedVectorfield2DParticleViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    delete m_weight_legend;
}

/**
 * Implementation/specialization of the ViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void DenseWeightedVectorfield2DParticleViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
    if(m_dense_weighted_model->isViewable())
    {
        painter->save();
        
        QPen   dotPen;
        
        if(m_useColorForWeight->value())
        {
            
            for(unsigned int i=0 ; i < (unsigned int)m_positions.size(); i++)
            {	
                float current_weight = m_dense_weighted_model->length(m_positions[i].x(),m_positions[i].y());
                float normalized_weight = std::min(1.0f,std::max(0.0f,(current_weight - m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
                
                dotPen.setColor(QColor(m_colorTable->value().at(normalized_weight*255)));
                painter->setPen(dotPen);
                painter->setBrush(dotPen.color());
                
                
                if(m_lifetimes[i] && rect().contains(m_positions[i]) )
                {
                    painter->drawEllipse(m_positions[i],m_particleRadius->value(),m_particleRadius->value());
                }
            }
        }
        else
        {
            for(unsigned int i=0 ; i < (unsigned int)m_positions.size(); i++)
            {	
                float current_length = m_dense_weighted_model->length(m_positions[i].x(),m_positions[i].y());
                float normalized_length = std::min(1.0f,std::max(0.0f,(current_length - m_minLength->value())/(m_maxLength->value() - m_minLength->value())));
                
                dotPen.setColor(QColor(m_colorTable->value().at(normalized_length*255)));
                painter->setPen(dotPen);
                painter->setBrush(dotPen.color());
                
                
                if(m_lifetimes[i] && rect().contains(m_positions[i]) )
                {
                    painter->drawEllipse(m_positions[i],m_particleRadius->value(),m_particleRadius->value());
                }
            }
        }
        painter->restore();
    }
    
	ViewController::paintAfter(painter, option, widget);
}

/**
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void DenseWeightedVectorfield2DParticleViewController::updateParameters(bool force_update)
{
    DenseVectorfield2DParticleViewController::updateParameters(force_update);
    
    DenseWeightedVectorfield2D* vf = static_cast<DenseWeightedVectorfield2D*>(model());
    
    DenseWeightedVectorfield2DStatistics* old_stats = static_cast<DenseWeightedVectorfield2DStatistics*>(m_stats);
    DenseWeightedVectorfield2DStatistics* new_stats = new DenseWeightedVectorfield2DStatistics(vf);
	
    //Check if min-max-statistics have changed:
    if( old_stats && new_stats &&
       (   new_stats->weightStats().min != old_stats->weightStats().min
        || new_stats->weightStats().max != old_stats->weightStats().max
        || force_update) )
    {
        m_stats = new_stats;
        delete old_stats;
        
        m_minWeight->setRange(floor(new_stats->weightStats().min), ceil(new_stats->weightStats().max));
        m_maxWeight->setRange(floor(new_stats->weightStats().min), ceil(new_stats->weightStats().max));
    }
    else
    {
        delete new_stats;
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void DenseWeightedVectorfield2DParticleViewController::updateView()
{
	
	DenseVectorfield2DParticleViewController::updateView();
    
    //If the colors shall be used for weight coding
    if(m_useColorForWeight->value())
    {
        m_weight_legend->setColorTable(m_colorTable->value());
        m_weight_legend->setValueRange(m_minWeight->value(), m_maxWeight->value());
        m_weight_legend->setCaption(m_weightLegendCaption->value());
        m_weight_legend->setTicks(m_weightLegendTicks->value());
        m_weight_legend->setDigits(m_weightLegendDigits->value());
        
        m_weight_legend->setVisible(m_showWeightLegend->value());
        
		m_velocity_legend->setVisible(false);
	}
	else
	{
		//if no colors are used for weights, we cannot show a legend for the weights...
		m_weight_legend->setVisible(false);
		
        //but we can show a legend of the lengths
		m_velocity_legend->setVisible(m_showVelocityLegend->value());
	}	
}

/**
 * Slot for handling the update of the timer
 *
 * \param event The timer event which triggered this function.
 */
void DenseWeightedVectorfield2DParticleViewController::timerEvent(QTimerEvent *event)
{
	
	if(	!this->isVisible() || event->timerId() != m_timer_id)
		return;
	
    if(!m_dense_weighted_model->isViewable())
        return;
	
    int x,y;
	float current_length, current_weight;
    
    DenseVectorfield2D::PointType dir;
    
	for(unsigned int i=0; i < (unsigned int)m_positions.size(); i++)
	{
		x=(int)(m_positions[i].x()+0.5);
        y=(int)(m_positions[i].y()+0.5);
        
		current_length = m_dense_weighted_model->length(x,y);
		current_weight = m_dense_weighted_model->weight(x,y);
		
        switch( m_displayMotionMode->value() )
		{
			case GlobalMotion:
				dir = m_dense_model->globalDirection(x,y);
				break;
				
			case LocalMotion:
				dir = m_dense_model->localDirection(x,y);
				break;
			case CompleteMotion:
            default:
				dir = m_dense_model->direction(x,y);
                break;
		}
        
		if (	m_lifetimes[i] > 0
			&&	x >= 0 && x < (int)m_dense_weighted_model->width()
			&&	y >= 0 && y < (int)m_dense_weighted_model->height()
			&&	current_length >= m_minLength->value() && current_length <= m_maxLength->value() 
			&&	current_weight >= m_minWeight->value() && current_weight <= m_maxWeight->value() )
		{		
			m_positions[i].rx() += dir.x()/m_slowDown->value();
			m_positions[i].ry() += dir.y()/m_slowDown->value();
			
			m_lifetimes[i]--;
		}
		else
		{
			m_positions[i] = QPointF(rand() % (int)m_dense_weighted_model->width(),rand() % (int)m_dense_weighted_model->height());
			m_lifetimes[i] = m_particleLifetime->value();
		}
	}
	update();
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void DenseWeightedVectorfield2DParticleViewController::hoverMoveEvent ( QGraphicsSceneHoverEvent * event )
{	
	QGraphicsItem::hoverMoveEvent(event);
	QPointF p = event->pos();
	
	DenseWeightedVectorfield2D * vf = static_cast<DenseWeightedVectorfield2D *> (model());
    
    if(!vf->isViewable())
        return;
    
	float	x = p.x(),
			y = p.y();
	
	if(		x >= 0 && x < vf->width() 
	   &&	y >= 0 && y < vf->height())
	{
		float length = vf->length(x,y);
		float angle = vf->angle(x,y);
        float weight = vf->weight(x,y);
        
        DenseVectorfield2D::PointType dir = vf->direction(x,y),
                                    g_dir = vf->globalDirection(x,y),
                                    l_dir = vf->localDirection(x,y);
        
        emit updateStatusText(vf->shortName() + QString::fromUtf8("[%1,%2] = angle:%3°, length:%4px").arg(x).arg(y).arg(angle).arg(length));
        
        QString message("<b>Mouse moved over Object: </b><br/><i>");
            message +=	vf->shortName()
                    +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                    +	QString::fromUtf8("<br/> <b>angle: %1°</b>").arg(angle)
                    +	QString("<br/> <b>length: %1px</b>").arg(length)
                    +	QString("<br/> <b>weight: %1</b>").arg(weight)
                    +	QString("<br/> <b>direction vector: (%1,%2)px</b>").arg(dir.x()).arg(dir.y());
        
        if( g_dir.length() !=0)
        {
            message +=	QString("<br/> <b>&nbsp;&nbsp;&nbsp;global part: (%1,%2)px</b>").arg(g_dir.x()).arg(g_dir.y())
                   +	QString("<br/> <b>&nbsp;&nbsp;&nbsp;local part: (%1,%2)px</b>").arg(l_dir.x()).arg(l_dir.y());
        }
        
        emit updateStatusDescription(message);
        
        event->accept();
	}
}

} //end of namespace graipe
