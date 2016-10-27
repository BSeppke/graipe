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

#define _USE_MATH_DEFINES
#include <cmath>

#include "features2d/featurelistviewcontroller.hxx"


#include <QMessageBox>
#include <QInputDialog>

namespace graipe {

/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param features The point feature list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
PointFeatureList2DViewController::PointFeatureList2DViewController(QGraphicsScene* scene, PointFeatureList2D* features, int z_order)
:	ViewController(scene, features, z_order),
    m_stats(new PointFeatureList2DStatistics(features)),
    m_showLabels(new BoolParameter("Show labels:", false)),
    m_fontSize(new FloatParameter("Label font size:", 1.0e-6f, 1.0e+6f, 10, m_showLabels)),
    m_mode(NULL),
    m_radius(new FloatParameter("Radius:", 1.0e-6f, 1.0e+6f, 2)),
    m_color(new ColorParameter("Color:", Qt::yellow))
{
    QStringList modes;
	modes.append("Select"); modes.append("Create"); modes.append("Delete");
    
    m_mode = new EnumParameter("mode:", modes);
    
    m_parameters->addParameter("showLabels", m_showLabels);
    m_parameters->addParameter("fontSize", m_fontSize);
    m_parameters->addParameter("mode", m_mode);
    m_parameters->addParameter("radius", m_radius);
    m_parameters->addParameter("color", m_color);
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
PointFeatureList2DViewController::~PointFeatureList2DViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    delete m_stats;
}

/**
 * Implementation/specialization of the ViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void PointFeatureList2DViewController::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget *widget)
{
	ViewController::paintBefore(painter, option, widget);
	
	PointFeatureList2D * features = static_cast<PointFeatureList2D*> (model());
    
    if(!features->isViewable())
        return;
    
	QBrush originalBrush = painter->brush();
	QPen   originalPen   = painter->pen();
	
	painter->setBrush(QBrush(m_color->value()));
	painter->setPen(Qt::NoPen);
	
	QFont f("Arial",1);
	QFontMetrics fm(f);
	
	f.setPointSizeF(m_radius->value()/fm.height());
	painter->setFont(f);
	
	for(unsigned int i=0; i<features->size(); ++i)
	{
        //Assuming  PointType==QPointF
        const PointFeatureList2D::PointType& p = features->position(i);
        
        painter->drawEllipse(p, m_radius->value(), m_radius->value());
		
		if(m_showLabels->value())
		{
			painter->setPen(Qt::black);
			painter->setFont(QFont("Arial",m_fontSize->value()));
			painter->drawText(p, QString("%1").arg(i));
			painter->setPen(Qt::NoPen);
		}
	}
	
	painter->setBrush(originalBrush);
	painter->setPen(originalPen);
	
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the point feature list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF PointFeatureList2DViewController::boundingRect() const
{
    float d  = 2*m_radius->value();
    
    //Always return rect in local coordinates -> global coords will be computed
    //by the scene by means of the sceneTransform of the item, which is handled
    //elsewhere.
    QRectF rect( QPointF( - d,
                         - d),
                QPointF(m_model->width()  + d,
                        m_model->height() + d));
    
    return rect.united(ViewController::boundingRect());
    
}

/**
 * The typename of this ViewController
 *
 * \return Always: "PointFeatureList2DViewController"
 */
QString PointFeatureList2DViewController::typeName() const
{
    return "PointFeatureList2DViewController";
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void PointFeatureList2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
    if(acceptHoverEvents())
    {
        PointFeatureList2D * features = static_cast<PointFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        
        if(		x >= 0 && x < features->width() 
            &&	y >= 0 && y < features->height())
        {
            QString features_in_reach;
            for(unsigned int i=0; i< features->size(); ++i)
            {
                const PointFeatureList2D::PointType& pos = features->position(i);
                QPointF dp = pos-mouse_pos;
                float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                {
                    features_in_reach = features_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> </tr>").arg(i).arg(pos.x()).arg(pos.y()).arg(sqrt(d2));
                }
            }
            if (features_in_reach.isEmpty()) 
            {
                features_in_reach = QString("<b>no points in reach</b>");
            }
            else
            {
                features_in_reach =		QString("<table align='center' border='1'><tr> <td>Idx</td> <td>x</td> <td>y</td> <td>dist</td> </tr>")
                +	features_in_reach
                +	QString("</table>");
            }
            
            emit updateStatusText(features->shortName() + QString("[%1,%2]").arg(x).arg(y));
            emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                         +	features->shortName()
                                         +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                                         +	features_in_reach);
             
        }
    }
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void PointFeatureList2DViewController::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
    if(acceptHoverEvents())
    {
        PointFeatureList2D * features = static_cast<PointFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        if(   x >= 0 && x < features->width()
           && y >= 0 && y < features->height())
        {                
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                    if ( QMessageBox::question(NULL, QString("Create feature?"), QString("Do you want to create a new feature at (%1, %2)?").arg(x).arg(y), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes )
                    {
                        features->addFeature(PointFeatureList2D::PointType(x,y));
                    }
                    break;
                case 2:
                    for(unsigned int i=0; i<features->size(); ++i)
                    {
                        const PointFeatureList2D::PointType& pos = features->position(i);
                        QPointF dp = pos-mouse_pos;
                        float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                        
                        if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                        {
                            QString delete_string = QString("Do you want to delete feature: %1 at (%2, %3)?").arg(i).arg(pos.x()).arg(pos.y());
                            if ( QMessageBox::question(NULL, QString("Delete feature?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                features->removeFeature(i);
                            }
                        }
                    }
            }
            updateParameters();
        }
	}
}




/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param features The weighted point feature list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
WeightedPointFeatureList2DViewController::WeightedPointFeatureList2DViewController(QGraphicsScene* scene, WeightedPointFeatureList2D * features, int z_order)
: ViewController(scene, features, z_order),
    m_stats(new WeightedPointFeatureList2DStatistics(features)),
    m_showLabels(new BoolParameter("Show labels:", false)),
    m_fontSize(new FloatParameter("Label font size:", 1.0e-6f, 1.0e+6f, 10, m_showLabels)),
    m_mode(NULL),
    m_radius(new FloatParameter("Radius:", 1.0e-6f, 1.0e+6f, 2)),
    m_minWeight(new FloatParameter("Min. weight:", 1.0e-6f, 1.0e+6f, 0)),
    m_maxWeight(new FloatParameter("Max. weight:", 1.0e-6f, 1.0e+6f, 1)),
    m_colorTable(new ColorTableParameter("Color:")),
    m_showWeightLegend(new BoolParameter("Show weight legend:", false)),
    m_legendCaption(new StringParameter("Legend Caption", "weights", 20, m_showWeightLegend)),
    m_legendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_legendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend))
{
    QStringList modes;
	modes.append("Select"); modes.append("Create"); modes.append("Delete");
    
    m_mode = new EnumParameter("mode:", modes);
    
    //update according to statistics:
    m_minWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max));
    m_minWeight->setValue(m_stats->weightStats().min);
    m_maxWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max));
    m_maxWeight->setValue(m_stats->weightStats().max);
    
    m_parameters->addParameter("showLabels", m_showLabels);
    m_parameters->addParameter("fontSize", m_fontSize);
    m_parameters->addParameter("mode", m_mode);
    m_parameters->addParameter("radius", m_radius);
    m_parameters->addParameter("minWeight", m_minWeight);
    m_parameters->addParameter("maxWeight", m_maxWeight);
    m_parameters->addParameter("colorTable", m_colorTable);
    m_parameters->addParameter("showWeightLegend", m_showWeightLegend);
    m_parameters->addParameter("legendCaption", m_legendCaption);
    m_parameters->addParameter("legendTicks", m_legendTicks);
    m_parameters->addParameter("legendDigits", m_legendDigits);
        
    
    //Create and show legend
    m_weight_legend = new QLegend(0, features->height()+5,
                                  150, 50,
                                  m_stats->weightStats().min, m_stats->weightStats().max,
                                  m_legendTicks->value(),
                                  false,
                                  this);
    
    m_weight_legend->setTransform(transform());
	m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_legendCaption->value());
	m_weight_legend->setDigits(m_legendDigits->value());
	m_weight_legend->setZValue(z_order); 
	
    //force controller ui update
    updateParameters(true);
	
	updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
WeightedPointFeatureList2DViewController::~WeightedPointFeatureList2DViewController()
{
    //The command "delete m_parameters;" inside the base class
    //will also delete all other (newly introduced) parameters
    delete m_stats;
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
void WeightedPointFeatureList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	WeightedPointFeatureList2D * features = static_cast<WeightedPointFeatureList2D*> (model());
    
    if(!features->isViewable())
        return;
    
	painter->save();
	
	painter->setPen(Qt::NoPen);
	
	QFont f("Arial",1);
	QFontMetrics fm(f);
	
	f.setPointSizeF(m_radius->value()/fm.height());
	painter->setFont(f);
	
	for(unsigned int i=0; i<features->size(); ++i)
	{
        if(	(features->weight(i) >= m_minWeight->value()) &&  (features->weight(i) <= m_maxWeight->value()) )
		{
			float current_weight =		std::max(0.0f, std::min(1.0f,(features->weight(i)-m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
			
			painter->setBrush(QColor(m_colorTable->value().at(current_weight*255)));
            
            //Assuming PointType==QPointF
            const PointFeatureList2D::PointType& pos = features->position(i);
            
			painter->drawEllipse(pos, m_radius->value(), m_radius->value());
			
			if(m_showLabels->value())
			{
				painter->setPen(Qt::black);
				painter->setFont(QFont("Arial",m_fontSize->value()));
				painter->drawText(pos, QString("%1").arg(i));
				painter->setPen(Qt::NoPen);
			}
		}
	}
	
	painter->restore();
	
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the weighted feature list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF WeightedPointFeatureList2DViewController::boundingRect() const
{
    float d  = 2*m_radius->value();
    
    //Always return rect in local coordinates -> global coords will be computed
    //by the scene by means of the sceneTransform of the item, which is handled
    //elsewhere.
    QRectF rect( QPointF( - d,
                         - d),
                QPointF(m_model->width()  + d,
                        m_model->height() + d));
    
    return rect.united(ViewController::boundingRect());
    
}

/**
 * The typename of this ViewController
 *
 * \return Always: "WeightedPointFeatureList2DViewController"
 */
QString WeightedPointFeatureList2DViewController::typeName() const
{
	return "WeightedPointFeatureList2DViewController";
}

/**
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void WeightedPointFeatureList2DViewController::updateParameters(bool force_update)
{
	ViewController::updateParameters(force_update);
    
    WeightedPointFeatureList2D* featurelist = static_cast<WeightedPointFeatureList2D*>(model());
    
    WeightedPointFeatureList2DStatistics* old_stats = static_cast<WeightedPointFeatureList2DStatistics*>(m_stats);
    WeightedPointFeatureList2DStatistics* new_stats = new WeightedPointFeatureList2DStatistics(featurelist);
    
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
void WeightedPointFeatureList2DViewController::updateView()
{
	ViewController::updateView();
    
	//Underly colorful gradient of velocity to legend
    m_weight_legend->setColorTable(m_colorTable->value());
    m_weight_legend->setValueRange(m_minWeight->value(), m_maxWeight->value());
    m_weight_legend->setCaption(m_legendCaption->value());
    m_weight_legend->setTicks(m_legendTicks->value());
    m_weight_legend->setDigits(m_legendDigits->value());
	
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void WeightedPointFeatureList2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
    if(acceptHoverEvents())
    {
        WeightedPointFeatureList2D * features = static_cast<WeightedPointFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        if(		x >= 0 && x < features->width() 
           &&	y >= 0 && y < features->height())
        {
            QString features_in_reach;
            for(unsigned int i=0; i<features->size(); ++i)
            {
                const PointFeatureList2D::PointType& pos = features->position(i);
                QPointF dp = pos-mouse_pos;
                float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                {
                    features_in_reach = features_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> </tr>").arg(i).arg(pos.x()).arg(pos.y()).arg(features->weight(i)).arg(sqrt(d2));
                }
            }
            if (features_in_reach.isEmpty()) 
            {
                features_in_reach = QString("<b>no points in reach</b>");
            }
            else
            {
                features_in_reach =		QString("<table align='center' border='1'><tr> <td>Idx</td> <td>x</td> <td>y</td> <td>weight</td> <td>dist</td> </tr>")
                +	features_in_reach
                +	QString("</table>");
            }
            
            emit updateStatusText(features->shortName() + QString("[%1,%2]").arg(x).arg(y));
            emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                         +	features->shortName()
                                         +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                                         +	features_in_reach);
             
        }
    }
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void WeightedPointFeatureList2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
    if(acceptHoverEvents())
    {
        WeightedPointFeatureList2D * features = static_cast<WeightedPointFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        if(   x >= 0 && x < features->width()
           && y >= 0 && y < features->height())
        {                
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                    {
                        bool ok;
                        double weight = QInputDialog::getDouble(NULL, QString("Values for new feature at (%1, %2)").arg(x).arg(y), QString("Weight of new feature"), 0.0, 0.0, 999999.99, 2, &ok);
                        if(ok)
                        {
                            features->addFeature(PointFeatureList2D::PointType(x,y),weight);
                        }
                    }
                    break;
                case 2:
                    for(unsigned int i=0; i<features->size(); ++i)
                    {
                        const PointFeatureList2D::PointType& pos = features->position(i);
                        QPointF dp = pos-mouse_pos;
                        float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                        if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                        {
                            QString delete_string = QString("Do you want to delete feature: %1 at (%2, %3) w: %4?").arg(i).arg(pos.x()).arg(pos.y()).arg(features->weight(i));
                            if ( QMessageBox::question(NULL, QString("Delete feature?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                features->removeFeature(i);
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
 * Implementation/specialization of the WeightedPointFeatureList2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param features The edgel feature list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
EdgelFeatureList2DViewController::EdgelFeatureList2DViewController(QGraphicsScene* scene, EdgelFeatureList2D* features, int z_order)
:	WeightedPointFeatureList2DViewController(scene, features, z_order),
    m_stats(new EdgelFeatureList2DStatistics(features))
{
}

/**
 * Implementation/specialization of the WeightedPointFeatureList2DViewController's virtual
 * destructor.
 */
EdgelFeatureList2DViewController::~EdgelFeatureList2DViewController()
{
    delete m_stats;
}

/**
 * Implementation/specialization of the WeightedPointFeatureList2DViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void EdgelFeatureList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	EdgelFeatureList2D * features = static_cast<EdgelFeatureList2D*> (model());
    
    if(!features->isViewable())
        return;
    
	painter->save();
	
	painter->setPen(Qt::NoPen);
	
	QFont f("Arial",1);
	QFontMetrics fm(f);
	
	f.setPointSizeF(m_radius->value()/fm.height());
	painter->setFont(f);
	
    QPolygonF triangle;
    
    triangle << QPointF( m_radius->value(),  0)
             << QPointF(-m_radius->value(), -m_radius->value()*0.6)
             << QPointF(-m_radius->value(),  m_radius->value()*0.6)
             << QPointF( m_radius->value(),  0);
    
	for(unsigned int i=0; i<features->size(); ++i)
	{
		if(	(features->weight(i) >= m_minWeight->value()) &&  (features->weight(i) <= m_maxWeight->value()) )
		{
			float current_weight = std::max(0.0f, std::min(1.0f,(features->weight(i)-m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
			
			painter->setBrush(QColor(m_colorTable->value().at(current_weight*255)));
            
            const PointFeatureList2D::PointType& pos = features->position(i);
            
            QTransform t;
            t.translate(pos.x(), pos.y());
            t.rotate(features->angle(i));
            
			painter->drawConvexPolygon(t.map(triangle));
			
			if(m_showLabels->value())
			{
				painter->setPen(Qt::black);
				painter->setFont(QFont("Arial",m_fontSize->value()));
				painter->drawText(pos, QString("%1").arg(i));
				painter->setPen(Qt::NoPen);
			}
		}
	}
	
	painter->restore();
	
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "EdgelList2DViewController"
 */
QString EdgelFeatureList2DViewController::typeName() const
{
	return "EdgelFeatureList2DViewController";
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void EdgelFeatureList2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
    if(acceptHoverEvents())
    {
        EdgelFeatureList2D * features = static_cast<EdgelFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        
        if(		x >= 0 && x < features->width() 
           &&	y >= 0 && y < features->height())
        {
            QString features_in_reach;
            for(unsigned int i=0; i<features->size() ; ++i)
            {
                const PointFeatureList2D::PointType& pos = features->position(i);
                QPointF dp = pos-mouse_pos;
                float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                {
                    features_in_reach = features_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> </tr>").arg(i).arg(pos.x()).arg(pos.y()).arg(features->weight(i)).arg(features->angle(i)).arg(sqrt(d2));
                }
            }
            if (features_in_reach.isEmpty()) 
            {
                features_in_reach = QString("<b>no points in reach</b>");
            }
            else
            {
                features_in_reach =		QString("<table align='center' border='1'><tr> <td>Idx</td> <td>x</td> <td>y</td> <td>weight</td> <td>angle</td> <td>dist</td> </tr>")
                +	features_in_reach
                +	QString("</table>");
            }
            
            emit updateStatusText(features->shortName() + QString("[%1,%2]").arg(x).arg(y));
            emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                         +	features->shortName()
                                         +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                                         +	features_in_reach);
        }
    }
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void EdgelFeatureList2DViewController::mousePressEvent (QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
	
	if(acceptHoverEvents())
    {
        EdgelFeatureList2D * features = static_cast<EdgelFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        if(   x >= 0 && x < features->width()
           && y >= 0 && y < features->height())
        {                
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                    {
                        bool ok;
                        double weight = QInputDialog::getDouble(NULL, QString("Values for new feature at (%1, %2)").arg(x).arg(y), QString("Weight of new feature"), 0.0, 0.0, 999999.99, 2, &ok);
                        if(ok)
                        {
                            double orientation = QInputDialog::getDouble(NULL,  QString("Values for new feature at (%1, %2) w: %3").arg(x).arg(y).arg(weight), QString("orientation of new feature [0..2pi]"), 0.0, 0.0, 2*M_PI,3, &ok);
                            if(ok)
                            { 
                                features->addFeature(PointFeatureList2D::PointType(x,y), weight, orientation);
                            }
                        }
                    }
                    break;
                case 2:
                    for(unsigned int i=0; i<features->size(); ++i)
                    {
                        const PointFeatureList2D::PointType& pos = features->position(i);
                        QPointF dp = pos-mouse_pos;
                        float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                        if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                        {
                            QString delete_string = QString("Do you want to delete feature: %1 at (%2, %3) w: %4, a: %5?").arg(i).arg(pos.x()).arg(pos.y()).arg(features->weight(i)).arg(features->angle(i));
                            if ( QMessageBox::question(NULL, QString("Delete feature?"), delete_string, QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
                            {
                                features->removeFeature(i);
                            }
                        }
                    }
            }
            updateParameters();
        }
    }
}




/**
 * Implementation/specialization of the EdgelFeatureList2DViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param features The SIFT feature list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
SIFTFeatureList2DViewController::SIFTFeatureList2DViewController(QGraphicsScene* scene, SIFTFeatureList2D* features, int z_order)
:	EdgelFeatureList2DViewController(scene, features, z_order),
    m_stats(new SIFTFeatureList2DStatistics(features))
{
}

/**
 * Implementation/specialization of the EdgelFeatureList2DViewController's virtual
 * destructor.
 */
SIFTFeatureList2DViewController::~SIFTFeatureList2DViewController()
{
    delete m_stats;
}

/**
 * Implementation/specialization of the EdgelFeatureList2DViewController's paint procedure. This is called
 * by the QGraphicsView on every re-draw request.
 *
 * \param painter Pointer to the painter, which is used for drawing.
 * \param option Further style options for this GraphicsItem's drawing.
 * \param widget The widget, where we will draw onto.
 */
void SIFTFeatureList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter, option, widget);
	
	SIFTFeatureList2D * features = static_cast<SIFTFeatureList2D*> (model());
	
    if(!features->isViewable())
        return;
    
	painter->save();
	
	painter->setBrush(Qt::NoBrush);
	
	float left = -0.5, right=0.5, top=-0.5, bottom = 0.5;
	QRectF rectangle(left, top,right-left, bottom-top);
	QLineF line(0.0,0.0, right, 0.0);
	
	QTransform trans = painter->transform();
	
    PointFeatureList2D::PointType last_pos;
    
	int str_idx=0;
	
	QColor strokes[4];
	strokes[0] = Qt::red;
	strokes[1] = Qt::green;
	strokes[2] = Qt::blue;
	strokes[3] = Qt::yellow;
	
	for(unsigned int i = 0; i<features->size(); ++i)
	{
		if(	(features->weight(i) >= m_minWeight->value()) &&  (features->weight(i) <= m_maxWeight->value()) )
		{
			float current_weight =		std::max(0.0f, std::min(1.0f,(features->weight(i)-m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
			QColor col(m_colorTable->value().at(current_weight*255));
            col.setAlpha(128);
			painter->setBrush(col);
            
            const PointFeatureList2D::PointType& pos = features->position(i);
			
			//if keypoint position is equal to last pos -> angle alternative was detected (mark colorful)
			if(pos == last_pos)
			{
				str_idx++;
				str_idx = str_idx % 4;
			}
			else
			{
				last_pos=pos;
				str_idx =0;
			}
			
			QTransform transform;
			transform.translate(pos.x(), pos.y());
			transform.rotate(features->angle(i));
			transform.scale(features->scale(i),features->scale(i));
			
			painter->setTransform(transform*trans);
			
			painter->setPen(QPen(strokes[str_idx],0));//m_radius->value()/features->scale(i)));
			painter->drawRect(rectangle);
			painter->drawLine(line);
			
			if(m_showLabels->value())
			{
				painter->setPen(Qt::black);
				painter->setFont(QFont("Arial",m_fontSize->value()));
				painter->drawText(rectangle.center(), QString("%1").arg(i));
				painter->setPen(Qt::NoPen);
			}
		}
	}
	painter->setTransform(trans);
	painter->restore();
	
	
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "SIFTFeatureList2DViewController"
 */
QString SIFTFeatureList2DViewController::typeName() const
{
	return "SIFTFeatureList2DViewController"; 
}

/**
 * Implementation/specialization of the handling of a mouse-move event
 *
 * \param event The mouse event which triggered this function.
 */
void SIFTFeatureList2DViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
    if(acceptHoverEvents())
    {
        SIFTFeatureList2D * features = static_cast<SIFTFeatureList2D *> (model());
        
        if(!features->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        PointFeatureList2D::PointType mouse_pos(x,y);
        
        
        if(		x >= 0 && x < features->width() 
           &&	y >= 0 && y < features->height())
        {
            QString features_in_reach;
            for(unsigned int i=0; i<features->size() ; ++i)
            {
                const PointFeatureList2D::PointType& pos = features->position(i);
                QPointF dp = pos-mouse_pos;
                float d2 = dp.x()*dp.x() + dp.y()*dp.y();
                
                if( d2 <= std::max(2.0f, m_radius->value()*m_radius->value()) )
                {
                    features_in_reach = features_in_reach + QString("<tr> <td>%1</td> <td>%2</td> <td>%3</td> <td>%4</td> <td>%5</td> <td>%6</td> <td>%7</td> </tr>").arg(i).arg(pos.x()).arg(pos.y()).arg(features->weight(i)).arg(features->angle(i)).arg(features->scale(i)).arg(sqrt(d2));
                }
            }
            if (features_in_reach.isEmpty()) 
            {
                features_in_reach = QString("<b>no points in reach</b>");
            }
            else
            {
                features_in_reach =		QString("<table align='center' border='1'><tr> <td>Idx</td> <td>x</td> <td>y</td> <td>weight</td> <td>angle</td>  <td>scale</td> <td>dist</td> </tr>")
                +	features_in_reach
                +	QString("</table>");
            }
            
            emit updateStatusText(features->shortName() + QString("[%1,%2]").arg(x).arg(y));
            emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                         +	features->shortName()
                                         +	QString("</i><br/>at position [%1,%2]").arg(x).arg(y)
                                         +	features_in_reach);
        }
    }
}
    
} //End of namespace graipe
