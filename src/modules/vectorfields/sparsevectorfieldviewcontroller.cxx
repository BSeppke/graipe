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

#include "vectorfields/sparsevectorfieldviewcontroller.hxx"

#include <QInputDialog>
#include <QMessageBox>

namespace graipe {

/**
 * @addtogroup graipe_vectorfields
 * @{
 *
 * @file
 * @brief Implementation file for sparse vectorfield viewing
 */
 
/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param vf The sparse vectorfield, which we want to show.
 */
SparseVectorfield2DViewController::SparseVectorfield2DViewController(SparseVectorfield2D * vf)
:	ViewController(vf),
	m_stats(new SparseVectorfield2DStatistics(vf)),
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
    
	m_headSize->setValue(m_stats->lengthStats().mean*0.3);
    
    //update according to statistics:
    m_minLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_minLength->setValue(m_stats->lengthStats().min);
    m_maxLength->setRange(floor(m_stats->lengthStats().min), ceil(m_stats->lengthStats().max));
    m_maxLength->setValue(m_stats->lengthStats().max);
    
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
	m_velocity_legend->setZValue(zValue());
	
	updateView();
}
                
/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
SparseVectorfield2DViewController::~SparseVectorfield2DViewController()
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
void SparseVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	SparseVectorfield2D * vf = static_cast<SparseVectorfield2D *>(model());
	
    if(vf->isViewable())
    {
        painter->save();
        
        QPointFX origin, direction, target;
        QColor current_color;
        
        for(unsigned int i=0; i<vf->size(); ++i)
        {
            float current_length = vf->length(i);
            
            if(current_length!=0 && (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()))
            {
                origin = vf->origin(i);
                
                switch( m_displayMotionMode->value() )
                {
                    case GlobalMotion:
                        direction = vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = vf->localDirection(i);
                        break;
                        
                    case CompleteMotion:
                    default:
                        direction = vf->direction(i);
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
QRectF SparseVectorfield2DViewController::boundingRect () const
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
void SparseVectorfield2DViewController::updateParameters(bool force_update)
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
void SparseVectorfield2DViewController::updateView()
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

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void SparseVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
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
        
        
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {
            QString vectors_in_reach;
            for(unsigned int i=0; i< vf->size(); ++i)
            {
                SparseVectorfield2D::PointType ori = vf->origin(i);
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(vf->length(i)).arg(vf->angle(i)).arg(sqrt(d2));
                }
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
void SparseVectorfield2DViewController::mousePressEvent (QGraphicsSceneMouseEvent * event)
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
                            vf->addVector(mouse_pos, SparseVectorfield2D::PointType(dir_x, dir_y));
                        }
                    }
                }
                    break;
                case 2:
                    for(unsigned int i=0; i< vf->size(); ++i)
                    {
                        SparseVectorfield2D::PointType ori = vf->origin(i);
                        SparseVectorfield2D::PointType dir = vf->direction(i);
                        float d2 = QPointFX(ori-mouse_pos).squaredLength();
                        
                        if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                        {
                            QString delete_string = QString("Do you want to delete vector: %1 at (%2, %3) -> (%4, %5)?").arg(i).arg(ori.x()).arg(ori.y()).arg(dir.x()).arg(dir.y());
                            if ( QMessageBox::question(NULL, QString("Delete vector?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                vf->removeVector(i);   
                            }
                        }
                    }
                    break;
            }
            updateParameters();
        }
	}
}




/**
 * specialization of the SparseVectorfield2DViewController's
 * constructor.
 *
 * \param vf The sparse weighted vectorfield, which we want to show.
 */
SparseWeightedVectorfield2DViewController::SparseWeightedVectorfield2DViewController(SparseWeightedVectorfield2D * vf)
 :	SparseVectorfield2DViewController(vf),
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
	SparseWeightedVectorfield2DStatistics* stats = new SparseWeightedVectorfield2DStatistics(vf);
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
    m_parameters->addParameter("weightLegendTicks", m_weightLegendTicks);
    m_parameters->addParameter("weightLegendDigits", m_weightLegendDigits);
	
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
 * Specialization of the SparseVectorfield2DViewController's virtual
 * destructor.
 */
SparseWeightedVectorfield2DViewController::~SparseWeightedVectorfield2DViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    //m_stats will also be deleted by the mother class
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
void SparseWeightedVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 	
	ViewController::paintBefore(painter, option, widget);
	
	SparseWeightedVectorfield2D * vf = static_cast<SparseWeightedVectorfield2D *> (model());
	
    if(vf->isViewable())
    {
        painter->save();
        
        QPointFX origin, direction, target;
        
        for(unsigned int i=0; i<vf->size(); ++i)
        {
            float current_weight = vf->weight(i);
            float current_length = vf->length(i);
            
            if(current_length!=0	&& (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()) 
                                    && (current_weight>= m_minWeight->value()) && (current_weight <= m_maxWeight->value()))
            {
                origin = vf->origin(i);
                
                switch( m_displayMotionMode->value() )
                {
                    case GlobalMotion:
                        direction = vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = vf->localDirection(i);
                        break;
                        
                    case CompleteMotion:
                    default:
                        direction = vf->direction(i);
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
                
                    if(m_useColorForWeight->value())
                    {
                        normalized_weight = std::min(1.0f,std::max(0.0f,(current_weight - m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
                    }
                    m_vector_drawer.paint(painter, origin, target, normalized_weight);
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
void SparseWeightedVectorfield2DViewController::updateParameters(bool force_update)
{
    SparseVectorfield2DViewController::updateParameters(force_update);
    
    SparseWeightedVectorfield2D* vf = static_cast<SparseWeightedVectorfield2D*>(model());
    
    SparseWeightedVectorfield2DStatistics* old_stats = static_cast<SparseWeightedVectorfield2DStatistics*>(m_stats);
    SparseWeightedVectorfield2DStatistics* new_stats = new SparseWeightedVectorfield2DStatistics(vf);
	
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
void SparseWeightedVectorfield2DViewController::updateView()
{
	SparseVectorfield2DViewController::updateView();
	
    SparseWeightedVectorfield2D *		vf		= static_cast<SparseWeightedVectorfield2D*> (model());
	
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
void SparseWeightedVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
	if(acceptHoverEvents())
    {
        SparseWeightedVectorfield2D * vf = static_cast<SparseWeightedVectorfield2D *> (model());
	
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
            
        float	x = p.x(),
                y = p.y();
        SparseWeightedVectorfield2D::PointType mouse_pos(x,y);
	
	
        if(		x >= 0 && x < vf->width()
           &&	y >= 0 && y < vf->height())
        {
            QString vectors_in_reach;
            for(unsigned int i=0; i< vf->size() ; ++i)
            {
                SparseWeightedVectorfield2D::PointType ori = vf->origin(i);
                //SparseWeightedVectorfield2D::PointType dir = vf->direction(i);
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> <td>%7</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(vf->length(i)).arg(vf->angle(i)).arg(vf->weight(i)).arg(sqrt(d2));
                }
            }
            if (vectors_in_reach.isEmpty()) 
            {
                vectors_in_reach = QString("<b>no vectors in reach</b>");
            }
            else
            {
                vectors_in_reach =		QString("<table><tr> <th>Idx</th> <th>x</th> <th>y</th> <th>length</th> <th>angle</th> <th>weight</th> <th>dist</th> </tr>")
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
void SparseWeightedVectorfield2DViewController::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
    if(acceptHoverEvents())
    {
        SparseWeightedVectorfield2D * vf = static_cast<SparseWeightedVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        SparseWeightedVectorfield2D::PointType mouse_pos(x,y);
        
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
                    double dir_x = QInputDialog::getDouble(NULL, QString("Values for new vector at (%1, %2) -> (?, dir_y) w: weight").arg(x).arg(y), QString("x-direction (px) of new vector"), 0.0, -999999.99, 999999.99, 2, &ok);
                    if(ok)
                    {
                        double dir_y = QInputDialog::getDouble(NULL, QString("Values for new vector at (%1, %2) -> (%3, dir_y) w: weight").arg(x).arg(y).arg(dir_x), QString("y-direction (px) of new vector"), 0.0, -999999.99, 999999.99, 2, &ok);
                        if(ok)
                        { 
                            double weight = QInputDialog::getDouble(NULL, QString("Values for new vector at (%1, %2) -> (%3, %4) w: ?").arg(x).arg(y).arg(dir_x).arg(dir_y), QString("weight of new vector"), 0.0, 0, 999999.99, 2, &ok);
                            if(ok)
                            { 
                                vf->addVector(mouse_pos, SparseWeightedVectorfield2D::PointType(dir_x, dir_y), weight);
                            }
                        }
                    }
                }
                    break;
                case 2:
                    for(unsigned int i=0; i< vf->size(); ++i)
                    {
                        SparseWeightedVectorfield2D::PointType ori = vf->origin(i);
                        SparseWeightedVectorfield2D::PointType dir = vf->direction(i);
                        float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                        if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                        {
                            QString delete_string = QString("Do you want to delete vector: %1 at (%2, %3) -> (%4, %5)?").arg(i).arg(ori.x()).arg(ori.y()).arg(dir.x()).arg(dir.y());
                            if ( QMessageBox::question(NULL, QString("Delete vector?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                vf->removeVector(i);   
                            }
                        }
                    }
            }
            updateParameters();
        }
	}
}




/**
 * Specialization of the SparseVectorfield2DViewController's
 * constructor.
 *
 * \param vf The sparse multi vectorfield, which we want to show.
 */
SparseMultiVectorfield2DViewController::SparseMultiVectorfield2DViewController(SparseMultiVectorfield2D * vf)
:	SparseVectorfield2DViewController(vf),
    m_showAlternative(new IntParameter("Show alt. (0=best):",0,0,0))
{

    //create statistics
	delete m_stats;
	SparseMultiVectorfield2DStatistics* stats = new SparseMultiVectorfield2DStatistics(vf);
	m_stats = stats;
    
    m_parameters->addParameter("alt", m_showAlternative);
    
	int directionCount = 0;
	if(vf->size())
		directionCount = vf->alternatives()+1;
	
	m_showAlternative->setRange(0,directionCount);

	updateView();
}

/**
 * Implementation/specialization of the SparseVectorfield2DViewController's virtual
 * destructor.
 */
SparseMultiVectorfield2DViewController::~SparseMultiVectorfield2DViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    //No need to do anything here
}

/**
 * Implementation/specialization of the ViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void SparseMultiVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
		
	SparseMultiVectorfield2D * vf = static_cast<SparseMultiVectorfield2D *> (model());
	
    if(vf->isViewable())
    {
        painter->save();

        QPointFX origin, direction, target;
        
        unsigned int alt = m_showAlternative->value();
        
        for(unsigned int i=0; i<vf->size(); ++i)
        {		
            float current_length = alt>0 ? vf->altLength(i,alt-1) : vf->length(i);
            
            if(current_length!=0)
            {
                origin = vf->origin(i);
                
                switch( m_displayMotionMode->value() )
                {
                    case GlobalMotion:
                        direction = alt>0 ? vf->altGlobalDirection(i, alt-1) : vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = alt>0 ? vf->altLocalDirection(i, alt-1) : vf->localDirection(i);
                        break;
                        
                    case CompleteMotion:
                    default:
                        direction = alt>0 ? vf->altDirection(i, alt-1) : vf->direction(i);
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
void SparseMultiVectorfield2DViewController::updateParameters(bool force_update)
{
    ViewController::updateParameters(force_update);

    SparseMultiVectorfield2D* vf = static_cast<SparseMultiVectorfield2D*>(model());
    
    SparseMultiVectorfield2DStatistics* old_stats = static_cast<SparseMultiVectorfield2DStatistics*>(m_stats);
    SparseMultiVectorfield2DStatistics* new_stats = new SparseMultiVectorfield2DStatistics(vf);
	
    //Check if min-max-statistics have changed:
    if( old_stats && new_stats &&
       (   new_stats->combinedLengthStats().min != old_stats->combinedLengthStats().min
        || new_stats->combinedLengthStats().max != old_stats->combinedLengthStats().max
        || force_update) )
    {
        m_stats = new_stats;
        delete old_stats;
        
        m_minLength->setRange(floor(m_stats->combinedLengthStats().min), ceil(m_stats->combinedLengthStats().max));
        m_maxLength->setRange(floor(m_stats->combinedLengthStats().min), ceil(m_stats->combinedLengthStats().max));
    }
    else
    {
        delete new_stats;
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void SparseMultiVectorfield2DViewController::updateView()
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

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void SparseMultiVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
	if(acceptHoverEvents())
    {
        SparseMultiVectorfield2D * vf = static_cast<SparseMultiVectorfield2D *> (model());
        
        QPointF p = event->pos();
        
        float	x = p.x(),
                y = p.y();
        SparseMultiVectorfield2D::PointType mouse_pos(x,y);
        
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {
            
        unsigned int alt = m_showAlternative->value();
        
            QString vectors_in_reach;
            for(unsigned int i=0; i< vf->size(); ++i)
            {
                SparseMultiVectorfield2D::PointType ori = vf->origin(i);
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(alt>0 ? vf->altLength(i,alt-1) : vf->length(i)).arg(alt>0 ? vf->altAngle(i, alt-1) : vf->angle(i)).arg(sqrt(d2));
                }
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
void SparseMultiVectorfield2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
    if(acceptHoverEvents())
    {
        
        SparseMultiVectorfield2D * vf = static_cast<SparseMultiVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        SparseMultiVectorfield2D::PointType mouse_pos(x,y);
        
        if(		x >= 0 && x < vf->width()
             && y >= 0 && y < vf->height())
        {                
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                {
                    //do not support creation of new multi-vectors.. 
                }
                    break;
                case 2:
                    for(unsigned int i=0; i< vf->size(); ++i)
                    {
                        SparseMultiVectorfield2D::PointType ori = vf->origin(i);
                        SparseMultiVectorfield2D::PointType dir = vf->direction(i);
                        float d2 = QPointFX(ori-mouse_pos).squaredLength();
                        
                        if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                        {
                            QString delete_string = QString("Do you want to delete vector: %1 at (%2, %3) -> (%4, %5)?").arg(i).arg(ori.x()).arg(ori.y()).arg(dir.x()).arg(dir.y());
                            if ( QMessageBox::question(NULL, QString("Delete vector?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                vf->removeVector(i);   
                            }
                        }
                    }
            }
            updateParameters();
        }
	}
}








/**
 * Specialization of the SparseMultiVectorfield2DViewController's
 * constructor.
 *
 * \param vf The sparse weighted multi vectorfield, which we want to show.
 */
SparseWeightedMultiVectorfield2DViewController::SparseWeightedMultiVectorfield2DViewController(SparseWeightedMultiVectorfield2D * vf)
 :	SparseMultiVectorfield2DViewController(vf),
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
	SparseWeightedMultiVectorfield2DStatistics* stats = new SparseWeightedMultiVectorfield2DStatistics(vf);
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
    m_parameters->addParameter("weightLegendTicks", m_weightLegendTicks);
    m_parameters->addParameter("weightLegendDigits", m_weightLegendDigits);
	
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
 * Implementation/specialization of the SparseMultiVectorfield2DViewController's virtual
 * destructor.
 */
SparseWeightedMultiVectorfield2DViewController::~SparseWeightedMultiVectorfield2DViewController()
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
void SparseWeightedMultiVectorfield2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	SparseWeightedMultiVectorfield2D * vf = static_cast<SparseWeightedMultiVectorfield2D *> (model());
	
    if(vf->isViewable())
    {
        painter->save();
        
        QPointFX origin, direction, target;
        
        unsigned int alt = m_showAlternative->value();
        
        for(unsigned int i=0; i<vf->size(); ++i)
        {
            float current_weight = alt>0 ? vf->altWeight(i,alt-1) : vf->weight(i);
            float current_length = alt>0 ? vf->altLength(i,alt-1) : vf->length(i);
            
            if(current_length!=0	&& (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()) 
                                    && (current_weight>= m_minWeight->value()) && (current_weight <= m_maxWeight->value()))
            {
                origin = vf->origin(i);
                
                switch( m_displayMotionMode->value() )
                {
                    case GlobalMotion:
                        direction = alt>0 ? vf->altGlobalDirection(i, alt-1) : vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = alt>0 ? vf->altLocalDirection(i, alt-1) : vf->localDirection(i);
                        break;
                        
                    case CompleteMotion:
                    default:
                        direction = alt>0 ? vf->altDirection(i, alt-1) : vf->direction(i);
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
                    
                    if(m_useColorForWeight->value())
                    {
                       normalized_weight = std::min(1.0f,std::max(0.0f,(current_weight - m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
                    }
                    
                    m_vector_drawer.paint(painter, origin, target, normalized_weight);
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
void SparseWeightedMultiVectorfield2DViewController::updateParameters(bool force_update)
{
    SparseMultiVectorfield2DViewController::updateParameters(force_update);
    
    SparseWeightedMultiVectorfield2D* vf = static_cast<SparseWeightedMultiVectorfield2D*>(model());
    
    SparseWeightedMultiVectorfield2DStatistics* old_stats = static_cast<SparseWeightedMultiVectorfield2DStatistics*>(m_stats);
    SparseWeightedMultiVectorfield2DStatistics* new_stats = new SparseWeightedMultiVectorfield2DStatistics(vf);
	
    //Check if min-max-statistics have changed:
    if( old_stats && new_stats &&
       (   new_stats->combinedWeightStats().min != old_stats->combinedWeightStats().min
        || new_stats->combinedWeightStats().max != old_stats->combinedWeightStats().max
        || force_update) )
    {
        m_stats = new_stats;
        delete old_stats;
        
        m_minWeight->setRange(floor(new_stats->combinedWeightStats().min), ceil(new_stats->combinedWeightStats().max));
        m_maxWeight->setRange(floor(new_stats->combinedWeightStats().min), ceil(new_stats->combinedWeightStats().max));
    }
    else
    {
        delete new_stats;
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void SparseWeightedMultiVectorfield2DViewController::updateView()
{
	SparseMultiVectorfield2DViewController::updateView();
	
	SparseWeightedMultiVectorfield2D *	vf = static_cast<SparseWeightedMultiVectorfield2D*> (model());
	
	
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
                    m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value()*vf->scale(), r_old.height()));
					m_velocity_legend->setValueRange(0, m_maxLength->value()*vf->scale());
				}
				else
				{
					m_velocity_legend->setRect(QRectF(r_old.left(), r_old.top(), m_maxLength->value(), r_old.height()));
					m_velocity_legend->setValueRange(0, m_maxLength->value());
				}
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
void SparseWeightedMultiVectorfield2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
	if(acceptHoverEvents())
    {
        SparseWeightedMultiVectorfield2D * vf = static_cast<SparseWeightedMultiVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        SparseMultiVectorfield2D::PointType mouse_pos(x,y);
        
        if(		x >= 0 && x < vf->width() 
           &&	y >= 0 && y < vf->height())
        {
            unsigned int alt = vf->alternatives();
            
            QString vectors_in_reach;
            for(unsigned int i=0; i< vf->size(); ++i)
            {
                SparseMultiVectorfield2D::PointType ori = vf->origin(i);
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> <td>%7</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(alt>0 ? vf->altLength(i,alt-1) : vf->length(i)).arg(alt>0 ? vf->altAngle(i, alt-1) : vf->angle(i)).arg(alt>0 ? vf->altWeight(i,alt-1) : vf->weight(i)).arg(sqrt(d2));
                }
            }
            if (vectors_in_reach.isEmpty()) 
            {
                vectors_in_reach = QString("<b>no vectors in reach</b>");
            }
            else
            {
                vectors_in_reach =		QString("<table><tr> <th>Idx</th> <th>x</th> <th>y</th> <th>length</th> <th>angle</th> <th>weight</th> <th>dist</th> </tr>")
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
void SparseWeightedMultiVectorfield2DViewController::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
    if(acceptHoverEvents())
    {
        
        SparseWeightedMultiVectorfield2D * vf = static_cast<SparseWeightedMultiVectorfield2D *> (model());
        
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        SparseMultiVectorfield2D::PointType mouse_pos(x,y);
        
        if(	  x >= 0 && x < vf->width()
           && y >= 0 && y < vf->height())
        {                
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                {
                    //do not support creation of new multi-vectors.. 
                }
                    break;
                case 2:
                    for(unsigned int i=0; i< vf->size(); ++i)
                    {
                        SparseMultiVectorfield2D::PointType ori = vf->origin(i);
                        SparseMultiVectorfield2D::PointType dir = vf->direction(i);
                        float d2 = QPointFX(ori-mouse_pos).squaredLength();
                        
                        if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                        {
                            QString delete_string = QString("Do you want to delete vector: %1 at (%2, %3) -> (%4, %5)?").arg(i).arg(ori.x()).arg(ori.y()).arg(dir.x()).arg(dir.y());
                            if ( QMessageBox::question(NULL, QString("Delete vector?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                vf->removeVector(i);   
                            }
                        }
                    }
            }
            updateParameters();
        }
	}
}

/**
 * @}
 */

} //end of namespace graipe
