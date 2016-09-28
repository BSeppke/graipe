/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/************************************************************************/
#include "features2d/polygonlistviewcontroller.hxx"

#include <cmath>

#include <QMessageBox>

namespace graipe {
    
/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param polygons The polygon list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
PolygonList2DViewController::PolygonList2DViewController(QGraphicsScene* scene, PolygonList2D* polygons, int z_order)
:	ViewController(scene, polygons, z_order),
    m_polygons(polygons),
    m_stats(new PolygonList2DStatistics(polygons)),
    m_showLabels(new BoolParameter("Show labels:", false)),
    m_fontSize(new FloatParameter("Label font size:", 1.0e-6f, 1.0e+6f, 10, m_showLabels)),
    m_mode(NULL),
    m_lineWidth(new FloatParameter("Line width:", 1.0e-6f, 1.0e+6f, 2)),
    m_color(new ColorParameter("Color:", Qt::yellow))
{
    QStringList modes;
	modes.append("Select"); modes.append("Create"); modes.append("Delete");

    m_mode = new EnumParameter("mode:", modes);
    
    m_parameters->addParameter("showLabels", m_showLabels);
    m_parameters->addParameter("fontSize", m_fontSize);
    m_parameters->addParameter("mode", m_mode);
    m_parameters->addParameter("lineWidth", m_lineWidth);
    m_parameters->addParameter("color", m_color);
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
PolygonList2DViewController::~PolygonList2DViewController()
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
void PolygonList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ViewController::paintBefore(painter, option, widget);
    
    if(model()->isViewable())
    {
        painter->save();
        
        painter->setBrush(m_color->value());
        
        QPen new_pen;
        
        if(m_lineWidth > 0)
        {
            new_pen.setColor(Qt::black);
            new_pen.setWidthF(m_lineWidth->value());
        }
        else
        {
            new_pen = Qt::NoPen;
        }
        
        painter->setPen(new_pen);
        painter->setFont(QFont("Arial",m_fontSize->value()));
        
        for(unsigned int i=0; i<m_polygons->size(); i++)
        {
            painter->drawPolygon(m_polygons->polygon(i));
        }	
        
        if(m_showLabels->value())
        {
            for(unsigned int i=0; i<m_polygons->size(); i++)
            {
                painter->drawText(m_polygons->polygon(i)[0], QString("%1").arg(i));
            }
        }
        
        painter->restore();
    }
    
    ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the polygon list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF PolygonList2DViewController::boundingRect() const
{ 
	QRectF rect (-m_lineWidth->value(), -m_lineWidth->value(),
				  m_model->width()+2*m_lineWidth->value(),m_model->height()+2*m_lineWidth->value());
    
    return ViewController::boundingRect().united(rect);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "PolygonList2DViewController"
 */
QString PolygonList2DViewController::typeName() const
{
	return "PolygonList2DViewController";
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void PolygonList2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    QGraphicsItem::mousePressEvent(event);
    
    if(acceptHoverEvents())
    {
        if(!m_polygons->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        if(   x >= 0 && x < m_polygons->width()
           && y >= 0 && y < m_polygons->height())
        {
            switch (m_mode->value())
            {
                case 0:
                    break;
                case 1:
                    if ( QMessageBox::question(NULL, QString("Create feature?"), QString("Do you want to create a new feature at (%1, %2)?").arg(x).arg(y), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes )
                    {
                        if (m_polygons->size()==0)
                        {
                            m_polygons->addPolygon(Polygon2D());
                        }
                        
                        Polygon2D poly = m_polygons->polygon(0);
                        poly.addPoint(Polygon2D::PointType(x,y));
                        m_polygons->setPolygon(0,poly);
                    }
                    break;
                case 2:
                    break;
            }
            updateParameters();
            update();
        }
    }
}




/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param polygons The weighted polygon list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
WeightedPolygonList2DViewController::WeightedPolygonList2DViewController(QGraphicsScene* scene, WeightedPolygonList2D * polygons, int z_order)
:	ViewController(scene, polygons, z_order),
    m_stats(new WeightedPolygonList2DStatistics(polygons)),
    m_showLabels(new BoolParameter("Show labels:", false)),
    m_fontSize(new FloatParameter("Label font size:", 1.0e-6f, 1.0e+6f, 10, m_showLabels)),
    m_mode(NULL),
    m_lineWidth(new FloatParameter("Line width:", 1.0e-6f, 1.0e+6f, 2)),
    m_minWeight(new FloatParameter("Min. weight:", 1.0e-6f, 1.0e+6f, 0)),
    m_minColor(new ColorParameter("Color of min.:", Qt::yellow)),
    m_maxWeight(new FloatParameter("Max. weight:", 1.0e-6f, 1.0e+6f, 1)),
    m_maxColor(new ColorParameter("Color of max.:", Qt::red)),
    m_showWeightLegend(new BoolParameter("Show weight legend:", false)),
    m_legendCaption(new StringParameter("Legend Caption", "weights", 20, m_showWeightLegend)),
    m_legendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_legendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend))
{
    QStringList modes;
	modes.append("Select"); modes.append("Create"); modes.append("Delete");

    m_mode = new EnumParameter("mode:", modes);
    
    m_parameters->addParameter("showLabels", m_showLabels);
    m_parameters->addParameter("fontSize", m_fontSize);
    m_parameters->addParameter("mode", m_mode);
    m_parameters->addParameter("lineWidth", m_lineWidth);
    
    //update weights according to statistics:
    m_minWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max));
    m_minWeight->setValue(m_stats->weightStats().min);
    m_maxWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max));
    m_maxWeight->setValue(m_stats->weightStats().max);
    
    m_parameters->addParameter("minWeight", m_minWeight);
    m_parameters->addParameter("minColor", m_minColor);
    m_parameters->addParameter("maxWeight", m_maxWeight);
    m_parameters->addParameter("maxColor", m_maxColor);
    m_parameters->addParameter("showWeightLegend", m_showWeightLegend);
    m_parameters->addParameter("legendCaption", m_legendCaption);
    m_parameters->addParameter("legendTicks", m_legendTicks);
    m_parameters->addParameter("legendDigits", m_legendDigits);
    
    //Create and show legend
    m_weight_legend = new QLegend(0, polygons->height()+5,
                                  150, 50,
                                  m_stats->weightStats().min, m_stats->weightStats().max,
                                  m_legendTicks->value(),
                                  false,
                                  this);
    
    m_weight_legend->setTransform(transform());
    m_weight_legend->setVisible(false);
    m_weight_legend->setCaption(m_legendCaption->value());
    m_weight_legend->setDigits(m_legendDigits->value());
    m_weight_legend->setZValue(zValue());
    
    //force controller ui update
    updateParameters(true);
    
    updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
WeightedPolygonList2DViewController::~WeightedPolygonList2DViewController()
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
void WeightedPolygonList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
    ViewController::paintBefore(painter, option, widget);
	
    if(m_polygons->isViewable())
    {
        painter->save();
        
        QPen new_pen;
        
        if(m_lineWidth > 0)
        {
            new_pen.setColor(Qt::black);
            new_pen.setWidthF(m_lineWidth->value());
        }
        else
        {
            new_pen = Qt::NoPen;	
        }
        
        painter->setPen(new_pen);	
        painter->setFont(QFont("Arial",m_fontSize->value()));
        
        
        for(unsigned int i=0; i<m_polygons->size(); ++i)
        {
            if(	(m_polygons->weight(i) >= m_minWeight->value()) &&  (m_polygons->weight(i) <= m_maxWeight->value()) )
            {
                float current_weight = std::max(0.0f, std::min(1.0f,(m_polygons->weight(i)-m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
                
                painter->setBrush(QColor(m_minColor->value().red()  *(1.0-current_weight) + m_maxColor->value().red()  *current_weight,
                                         m_minColor->value().green()*(1.0-current_weight) + m_maxColor->value().green()*current_weight,
                                         m_minColor->value().blue() *(1.0-current_weight) + m_maxColor->value().blue() *current_weight));
                
                painter->drawPolygon(m_polygons->polygon(i));
            }	
        }
        
        if(m_showLabels->value())
        {
            for(unsigned int i=0; i<m_polygons->size(); ++i)
            {
                painter->drawText(m_polygons->polygon(i)[0], QString("%1").arg(i));
            }
        }
        
        painter->restore();
    }
    
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the weighted polygon list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF WeightedPolygonList2DViewController::boundingRect() const
{ 
	QRectF rect (-m_lineWidth->value(), -m_lineWidth->value(),
				  m_model->width()+2*m_lineWidth->value(),m_model->height()+2*m_lineWidth->value());
    
    return ViewController::boundingRect().united(rect);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "WeightedPolygonList2DViewController"
 */
QString WeightedPolygonList2DViewController::typeName() const
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
void WeightedPolygonList2DViewController::updateParameters(bool force_update)
{
    ViewController::updateParameters(force_update);
    
    WeightedPolygonList2D* featurelist = static_cast<WeightedPolygonList2D*>(model());
    
    WeightedPolygonList2DStatistics* old_stats = static_cast<WeightedPolygonList2DStatistics*>(m_stats);
    WeightedPolygonList2DStatistics* new_stats = new WeightedPolygonList2DStatistics(featurelist);
    
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
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
void WeightedPolygonList2DViewController::updateView()
{
    ViewController::updateView();
    
    //Underly colorful gradient of velocity to legend
    m_weight_legend->setColorRange(m_minColor->value(), m_maxColor->value());
    
    m_weight_legend->setValueRange(m_minWeight->value(), m_maxWeight->value());
    m_weight_legend->setCaption(m_legendCaption->value());
    m_weight_legend->setTicks(m_legendTicks->value());
    m_weight_legend->setDigits(m_legendDigits->value());
}
    
} //End of namespace graipe
