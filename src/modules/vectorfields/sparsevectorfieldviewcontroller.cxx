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

#include "vectorfields/sparsevectorfieldviewcontroller.hxx"

namespace graipe {

/**
 * Specialization of the SparseVectorfield2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The sparse weighted vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
SparseVectorfield2DViewController::SparseVectorfield2DViewController(QGraphicsScene * scene, SparseVectorfield2D* vf, int z_value)
:	SparseVectorfield2DViewControllerBase(scene, vf, z_value),
    m_stats(vf)
{
    //update parameters according to statistics:
    m_headSize->setValue(m_stats.lengthStats().mean*0.3);
   
    m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_minLength->setValue(m_stats.lengthStats().min);
    m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_maxLength->setValue(m_stats.lengthStats().max);
    
    //create and position Legend:	
    QPointF legend_xy(0, vf->height());
    
    if(vf->scale()!=0)
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min*vf->scale(), m_stats.lengthStats().max*vf->scale(),
                                        m_velocityLegendTicks->value(),
                                        true,
                                        this);
    }
    else
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min, m_stats.lengthStats().max,
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
QRectF SparseVectorfield2DViewController::boundingRect() const
{
    qreal maxLength = m_stats.lengthStats().max;
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
    
    SparseVectorfield2DStatistics new_stats = SparseVectorfield2DStatistics(vf);
    
    //Check if min-max-statistics have changed:
    if(    new_stats.lengthStats().min != m_stats.lengthStats().min
        || new_stats.lengthStats().max != m_stats.lengthStats().max
        || force_update)
    {
        m_stats = new_stats;
        
        m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
        m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    }
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
void SparseVectorfield2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
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








/**
 * specialization of the SparseWeightedVectorfield2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The sparse weighted vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
SparseWeightedVectorfield2DViewController::SparseWeightedVectorfield2DViewController(QGraphicsScene* scene, SparseWeightedVectorfield2D * vf, int z_value)
 :	SparseVectorfield2DViewControllerBase(scene, vf, z_value),
    m_stats(SparseWeightedVectorfield2DStatistics(vf)),
    m_minWeight(new FloatParameter("Min. weight:",-1.0e10,1.0e10,0)),
    m_maxWeight(new FloatParameter("Max. weight:",-1.0e10,1.0e10,1)),
    m_showWeightLegend(new BoolParameter("Show weight legend?", false)),
    m_weightLegendCaption(new StringParameter("Legend caption:","weights", 20, m_showWeightLegend)),
    m_weightLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_weightLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend)),
    m_useColorForWeight(new BoolParameter("Use colors for weights?",false)),
    m_weight_legend(NULL)
{
    //update parameters according to statistics:
    m_headSize->setValue(m_stats.lengthStats().mean*0.3);
   
    m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_minLength->setValue(m_stats.lengthStats().min);
    m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_maxLength->setValue(m_stats.lengthStats().max);
    
    //create and position Legend:	
    QPointF legend_xy(0, vf->height());
    
    if(vf->scale()!=0)
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min*vf->scale(), m_stats.lengthStats().max*vf->scale(),
                                        m_velocityLegendTicks->value(),
                                        true,
                                        this);
    }
    else
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min, m_stats.lengthStats().max,
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
    
    //update according to weight statistics:
    m_minWeight->setRange(floor(m_stats.weightStats().min), ceil(m_stats.weightStats().max));
    m_minWeight->setValue(m_stats.weightStats().min);
    m_maxWeight->setRange(floor(m_stats.weightStats().min), ceil(m_stats.weightStats().max));
    m_maxWeight->setValue(m_stats.weightStats().max);
    
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
                                  m_stats.weightStats().min,m_stats.weightStats().max,
                                  m_weightLegendTicks->value(),
                                  false,
                                  this);
	
	m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_weightLegendCaption->value());
    m_weight_legend->setTicks(m_weightLegendTicks->value());
    m_weight_legend->setDigits(m_weightLegendDigits->value());
	m_weight_legend->setZValue(z_value);
	
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
        
        for(const WeightedVector2D& v : *vf)
        {
            float current_weight = v.weight;
            float current_length = v.direction.length();
            
            if(current_length!=0	&& (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()) 
                                    && (current_weight>= m_minWeight->value()) && (current_weight <= m_maxWeight->value()))
            {
                origin = v.origin;
                
                switch( m_displayMotionMode->value() )
                {
                /*
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
    SparseVectorfield2DViewControllerBase::updateParameters(force_update);
    
    SparseWeightedVectorfield2D* vf = static_cast<SparseWeightedVectorfield2D*>(model());
    
    SparseWeightedVectorfield2DStatistics new_stats(vf);
	
    //Check if min-max-statistics have changed:
    if(    m_stats.lengthStats().min != new_stats.lengthStats().min
        || m_stats.lengthStats().max != new_stats.lengthStats().max
        || m_stats.weightStats().min != new_stats.weightStats().min
        || m_stats.weightStats().max != new_stats.weightStats().max
        || force_update)
    {
        m_stats = new_stats;
        
        m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
        m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
        
        m_minWeight->setRange(floor(new_stats.weightStats().min), ceil(new_stats.weightStats().max));
        m_maxWeight->setRange(floor(new_stats.weightStats().min), ceil(new_stats.weightStats().max));
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void SparseWeightedVectorfield2DViewController::updateView()
{
	SparseVectorfield2DViewControllerBase::updateView();
	
    SparseWeightedVectorfield2D* vf	= static_cast<SparseWeightedVectorfield2D*> (model());
	
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
        SparseWeightedVectorfield2D* vf = static_cast<SparseWeightedVectorfield2D *> (model());
	
        if(!vf->isViewable())
            return;
        
        QPointF p = event->pos();
            
        float	x = p.x(),
                y = p.y();
        SparseWeightedVectorfield2D::PointType mouse_pos(x,y);
	
        unsigned int i=0;
    
        if(		x >= 0 && x < vf->width()
           &&	y >= 0 && y < vf->height())
        {
            QString vectors_in_reach;
            for(const WeightedVector2D& v : *vf)
            {
                SparseWeightedVectorfield2D::PointType ori = v.origin;
                //SparseWeightedVectorfield2D::PointType dir = v.direction;
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> <td>%7</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(v.direction.length()).arg(v.direction.angle()).arg(v.weight).arg(sqrt(d2));
                }
                ++i;
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
                                WeightedVector2D new_v;
                                    new_v.origin = mouse_pos;
                                    new_v.direction = SparseWeightedVectorfield2D::PointType(dir_x, dir_y);
                                    new_v.weight = weight;
                                vf->append(new_v);
                            }
                        }
                    }
                }
                    break;
                case 2:
                    for(unsigned int i=0; i< vf->size(); ++i)
                    {
                        SparseWeightedVectorfield2D::PointType ori = vf->item(i).origin;
                        SparseWeightedVectorfield2D::PointType dir = vf->item(i).direction;
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
            }
            updateParameters();
        }
	}
}




/**
 * Specialization of the SparseVectorfield2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The sparse multi vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
SparseMultiVectorfield2DViewController::SparseMultiVectorfield2DViewController(QGraphicsScene* scene, SparseMultiVectorfield2D * vf, int z_value)
:	SparseVectorfield2DViewControllerBase(scene, vf, z_value),
    m_stats(vf),
    m_showAlternative(new IntParameter("Show alt. (0=best):",0,0,0))
{
    //update parameters according to statistics:
    m_headSize->setValue(m_stats.lengthStats().mean*0.3);
   
    m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_minLength->setValue(m_stats.lengthStats().min);
    m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_maxLength->setValue(m_stats.lengthStats().max);
    
    //create and position Legend:	
    QPointF legend_xy(0, vf->height());
    
    if(vf->scale()!=0)
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min*vf->scale(), m_stats.lengthStats().max*vf->scale(),
                                        m_velocityLegendTicks->value(),
                                        true,
                                        this);
    }
    else
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min, m_stats.lengthStats().max,
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
    
    m_parameters->addParameter("alt", m_showAlternative);
    
	m_showAlternative->setRange(0,m_stats.altDirectionStats().size());

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
        
        for(const MultiVector2D & v: *vf)
        {		
            float current_length = alt>0 ? v.altDirections[alt-1].length() : v.direction.length();
            
            if(current_length!=0)
            {
                origin = v.origin;
                
                switch( m_displayMotionMode->value() )
                {/*
                    case GlobalMotion:
                        direction = alt>0 ? vf->altGlobalDirection(i, alt-1) : vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = alt>0 ? vf->altLocalDirection(i, alt-1) : vf->localDirection(i);
                        break;
                       TODO*/
                    case CompleteMotion:
                    default:
                        direction = alt>0 ? v.altDirections[alt-1] : v.direction;
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
    
    SparseMultiVectorfield2DStatistics new_stats(vf);
	
    //Check if min-max-statistics have changed:
    if(    new_stats.combinedLengthStats().min != m_stats.combinedLengthStats().min
        || new_stats.combinedLengthStats().max != m_stats.combinedLengthStats().max
        || force_update)
    {
        m_stats = new_stats;
        
        m_minLength->setRange(floor(m_stats.combinedLengthStats().min), ceil(m_stats.combinedLengthStats().max));
        m_maxLength->setRange(floor(m_stats.combinedLengthStats().min), ceil(m_stats.combinedLengthStats().max));
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void SparseMultiVectorfield2DViewController::updateView()
{
	SparseVectorfield2DViewControllerBase::updateView();
    
    m_vector_drawer.setLineWidth(m_lineWidth->value());
    m_vector_drawer.setHeadSize(m_headSize->value());
    m_vector_drawer.setColorTable(m_colorTable->value());
    
	SparseMultiVectorfield2D * vf = static_cast<SparseMultiVectorfield2D*> (model());
    
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
            unsigned int i=0;
            QString vectors_in_reach;
            
            for(const MultiVector2D& v : *vf)
            {
                SparseMultiVectorfield2D::PointType ori = v.origin;
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(alt>0 ? v.altDirections[alt-1].length() : v.direction.length()).arg(alt>0 ? v.altDirections[alt-1].angle() : v.direction.angle()).arg(sqrt(d2));
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
                        SparseMultiVectorfield2D::PointType ori = vf->item(i).origin;
                        SparseMultiVectorfield2D::PointType dir = vf->item(i).direction;
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
            }
            updateParameters();
        }
	}
}








/**
 * Specialization of the SparseMultiVectorfield2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param vf The sparse weighted multi vectorfield, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
SparseWeightedMultiVectorfield2DViewController::SparseWeightedMultiVectorfield2DViewController(QGraphicsScene* scene, SparseWeightedMultiVectorfield2D * vf, int z_value)
 :	SparseVectorfield2DViewControllerBase(scene, vf, z_value),
    m_stats(vf),
    m_minWeight(new FloatParameter("Min. weight:",-1.0e10,1.0e10,0)),
    m_maxWeight(new FloatParameter("Max. weight:",-1.0e10,1.0e10,1)),
    m_showWeightLegend(new BoolParameter("Show weight legend?", false)),
    m_weightLegendCaption(new StringParameter("Legend caption:","weights", 20, m_showWeightLegend)),
    m_weightLegendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_weightLegendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend)),
    m_useColorForWeight(new BoolParameter("Use colors for weights?",false)),
    m_weight_legend(NULL)
{
    //update parameters according to statistics:
    m_headSize->setValue(m_stats.lengthStats().mean*0.3);
   
    m_minLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_minLength->setValue(m_stats.lengthStats().min);
    m_maxLength->setRange(floor(m_stats.lengthStats().min), ceil(m_stats.lengthStats().max));
    m_maxLength->setValue(m_stats.lengthStats().max);
    
    //create and position Legend:	
    QPointF legend_xy(0, vf->height());
    
    if(vf->scale()!=0)
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min*vf->scale(), m_stats.lengthStats().max*vf->scale(),
                                        m_velocityLegendTicks->value(),
                                        true,
                                        this);
    }
    else
    {
        m_velocity_legend = new QLegend(legend_xy.x(), legend_xy.y()+5,
                                        150, 50,
                                        m_stats.lengthStats().min, m_stats.lengthStats().max,
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
    
    m_parameters->addParameter("alt", m_showAlternative);
    
	m_showAlternative->setRange(0,m_stats.altDirectionStats().size());
    
    //update according to weight statistics:
    m_minWeight->setRange(floor(m_stats.weightStats().min), ceil(m_stats.weightStats().max));
    m_minWeight->setValue(m_stats.weightStats().min);
    m_maxWeight->setRange(floor(m_stats.weightStats().min), ceil(m_stats.weightStats().max));
    m_maxWeight->setValue(m_stats.weightStats().max);

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
                                  m_stats.weightStats().min, m_stats.weightStats().max,
                                  m_weightLegendTicks->value(),
                                  false,
                                  this);
    
	m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_weightLegendCaption->value());
    m_weight_legend->setTicks(m_weightLegendTicks->value());
    m_weight_legend->setDigits(m_weightLegendDigits->value());
	m_weight_legend->setZValue(z_value);
	
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
        
        for(const WeightedMultiVector2D& v: *vf)
        {
            float current_weight = alt>0 ? v.altWeights[alt-1] : v.weight;
            float current_length = alt>0 ? v.altDirections[alt-1].length() : v.direction.length();
            
            if(current_length!=0	&& (current_length>= m_minLength->value()) && (current_length <= m_maxLength->value()) 
                                    && (current_weight>= m_minWeight->value()) && (current_weight <= m_maxWeight->value()))
            {
                origin = v.origin;
                
                switch( m_displayMotionMode->value() )
                {
                /*
                    case GlobalMotion:
                        direction = alt>0 ? vf->altGlobalDirection(i, alt-1) : vf->globalDirection(i);
                        break;
                        
                    case LocalMotion:
                        direction = alt>0 ? vf->altLocalDirection(i, alt-1) : vf->localDirection(i);
                        break;
                   TODO     */
                    case CompleteMotion:
                    default:
                        direction = alt>0 ? v.altDirections[alt-1] : v.direction;
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
    SparseVectorfield2DViewControllerBase::updateParameters(force_update);
    
    SparseWeightedMultiVectorfield2D* vf = static_cast<SparseWeightedMultiVectorfield2D*>(model());
    
    SparseWeightedMultiVectorfield2DStatistics new_stats(vf);
	
    //Check if min-max-statistics have changed:
    if(    new_stats.combinedLengthStats().min != m_stats.combinedLengthStats().min
        || new_stats.combinedLengthStats().max != m_stats.combinedLengthStats().max
        || new_stats.combinedWeightStats().min != m_stats.combinedWeightStats().min
        || new_stats.combinedWeightStats().max != m_stats.combinedWeightStats().max
        || force_update)
    {
        m_stats = new_stats;
        
        m_minLength->setRange(floor(m_stats.combinedLengthStats().min), ceil(m_stats.combinedLengthStats().max));
        m_maxLength->setRange(floor(m_stats.combinedLengthStats().min), ceil(m_stats.combinedLengthStats().max));
        
        m_minWeight->setRange(floor(m_stats.combinedWeightStats().min), ceil(m_stats.combinedWeightStats().max));
        m_maxWeight->setRange(floor(m_stats.combinedWeightStats().min), ceil(m_stats.combinedWeightStats().max));
    }
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void SparseWeightedMultiVectorfield2DViewController::updateView()
{
	SparseVectorfield2DViewControllerBase::updateView();
	
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
        
            unsigned int alt = m_showAlternative->value();
            QString vectors_in_reach;
            unsigned int i=0;
            
            for(const WeightedMultiVector2D & v: *vf)
            {
                SparseMultiVectorfield2D::PointType ori = v.origin;
                float d2 = QPointFX(ori-mouse_pos).squaredLength();
                
                if( d2 <= std::max(2.0f, m_lineWidth->value()*m_lineWidth->value()) )
                {
                    vectors_in_reach = vectors_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> <td>%7</td> </tr>").arg(i).arg(ori.x()).arg(ori.y()).arg(alt>0 ? v.altDirections[alt-1].length() : v.direction.length()).arg(alt>0 ? v.altDirections[alt-1].angle() : v.direction.angle()).arg(alt>0 ? v.altWeights[alt-1] : v.weight).arg(sqrt(d2));
                }
                ++i;
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
                        SparseMultiVectorfield2D::PointType ori = vf->item(i).origin;
                        SparseMultiVectorfield2D::PointType dir = vf->item(i).direction;
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
            }
            updateParameters();
        }
	}
}

} //end of namespace graipe
