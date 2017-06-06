/************************************************************************/
/*                                                                      */
/*               Copyright 2008-2016 by Benjamin Seppke                 */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/************************************************************************/

#include "features2d/cubicsplinelistviewcontroller.hxx"

#define _USE_MATH_DEFINES
#include <cmath>

#include <QInputDialog>
#include <QMessageBox>

namespace graipe {
    

/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param splines The spline list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
CubicSplineList2DViewController::CubicSplineList2DViewController(QGraphicsScene* scene, CubicSplineList2D* splines, int z_order)
:	ViewController(scene, splines, z_order),
    m_stats(new CubicSplineList2DStatistics(splines)),
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
    
    refreshSplines();
    updateView();
}
    
/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
CubicSplineList2DViewController::~CubicSplineList2DViewController()
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
void CubicSplineList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ViewController::paintBefore(painter, option, widget);
    
    if(model()->isViewable())
    {
        QBrush originalBrush = painter->brush();
        QPen   originalPen   = painter->pen();
        
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
        
        
        for(const QPolygonF& poly: m_lines)
        {
            painter->drawPolyline(poly);
        }
        
        for(const QList<QPointF>& points: m_points)
        {
            for(const QPointF& point: points)
            {
                painter->drawEllipse(point, m_lineWidth->value(), m_lineWidth->value());
            }
        }
        
        if(m_showLabels->value())
        {
            unsigned int i = 0;
            for(const QPolygonF& poly: m_lines)
            {
                painter->drawText(poly[0], QString("%1").arg(i++));
            }
        }
        
        painter->setBrush(originalBrush);
        painter->setPen(originalPen);
    }
    
    ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the cubic spline list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF CubicSplineList2DViewController::boundingRect() const
{ 
    QRectF rect(-m_lineWidth->value(),
                -m_lineWidth->value(),
                m_model->width()  + 2*m_lineWidth->value(),
                m_model->height() + 2*m_lineWidth->value());
    
    return ViewController::boundingRect().united(rect);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "CubicSplineList2DViewController"
 */
QString CubicSplineList2DViewController::typeName() const
{
    return "CubicSplineList2DViewController";
}

/**
 * Snycronizes between the Model's data and the views reprsentation:
 * Re-creates the polylines and point sets defined by the model.
 */
void CubicSplineList2DViewController::refreshSplines()
{
    float m_sampling= 100.0;
    
    CubicSplineList2D * splines = static_cast<CubicSplineList2D*> (model());
    
    m_lines.clear();
    m_points.clear();
    
    for(unsigned int i = 0; i <  splines->size(); ++i)
    {
        CubicSpline2D spl = splines->spline(i);
        
        QList<QPointF> spl_points;
        for(unsigned int j = 0; j < spl.size(); ++j)
        {
            spl_points.push_back(spl.point(j));
        }
        m_points.push_back(spl_points);
        
        
        QPolygonF poly;
        
        if(m_sampling && spl.size()>1)
        {
            float step=1.0/m_sampling;
                
            for(float t = 0; t < 1+step; t+=step)
            {
                poly << QPointF(spl.interpolate(t));
            }
        }
        m_lines.push_back(poly);
    }
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void CubicSplineList2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(acceptHoverEvents())
    {
        CubicSplineList2D * splines = static_cast<CubicSplineList2D *> (model());
        
        if(!splines->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        if(x >= 0 && x < splines->width() && y >= 0 && y < splines->height())
        {
            switch (m_mode->value())
            {
                //Select
                case 0:
                    //controller->selectSpline(x,y);
                    break;
                case 1:
                    if ( QMessageBox::question(NULL, QString("Create feature?"), QString("Do you want to create a new feature at (%1, %2)?").arg(x).arg(y), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes )
                    {
                        if (splines->size()==0)
                        {
                            splines->addSpline(CubicSpline2D());
                        }
                        
                        CubicSpline2D spl = splines->spline(0);
                        spl.addPoint(x,y);
                        splines->setSpline(0,spl);
                        
                    }
                    break;
                case 2:
                    break;
            }
            
            refreshSplines();
            updateParameters();
            
            updateView();
        }
    }
}




/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param splines The weighted spline list, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
WeightedCubicSplineList2DViewController::WeightedCubicSplineList2DViewController(QGraphicsScene* scene, WeightedCubicSplineList2D * splines, int z_order)
:	ViewController(scene, splines, z_order),
    m_stats(new WeightedCubicSplineList2DStatistics(splines)),
    m_showLabels(new BoolParameter("Show labels:", false)),
    m_fontSize(new FloatParameter("Label font size:", 1.0e-6f, 1.0e+6f, 10, m_showLabels)),
    m_mode(NULL),
    m_lineWidth(new FloatParameter("Line width:", 1.0e-6f, 1.0e+6f, 2)),
    m_minWeight(new FloatParameter("Min. weight:", 1.0e-6f, 1.0e+6f, 0)),
    m_maxWeight(new FloatParameter("Max. weight:", 1.0e-6f, 1.0e+6f, 1)),
    m_colorTable(new ColorTableParameter("Color:")),
    m_showWeightLegend(new BoolParameter("Show weight legend:", false)),
    m_legendCaption(new StringParameter("Legend Caption", "weights", 20, m_showWeightLegend)),
    m_legendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showWeightLegend)),
    m_legendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showWeightLegend))
{ 
    //update weights according to statistics:
	m_minWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max+.5));
    m_minWeight->setValue(m_stats->weightStats().min);
    m_maxWeight->setRange(floor(m_stats->weightStats().min), ceil(m_stats->weightStats().max+.5));
    m_maxWeight->setValue(m_stats->weightStats().max);
    
    m_parameters->addParameter("minWeight", m_minWeight);
    m_parameters->addParameter("maxWeight", m_maxWeight);
    m_parameters->addParameter("colorTable", m_colorTable);
    m_parameters->addParameter("showWeightLegend", m_showWeightLegend);
    m_parameters->addParameter("legendCaption", m_legendCaption);
    m_parameters->addParameter("legendTicks", m_legendTicks);
    m_parameters->addParameter("legendDigits", m_legendDigits);
    
    //Create and show legend
    m_weight_legend = new QLegend(0, splines->height()+5,
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
    
    refreshSplines();
    
    updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
WeightedCubicSplineList2DViewController::~WeightedCubicSplineList2DViewController()
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
void WeightedCubicSplineList2DViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	ViewController::paintBefore(painter, option, widget);
    
    WeightedCubicSplineList2D * splines = static_cast<WeightedCubicSplineList2D*> (model());
    
    if(splines->isViewable())
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
        
        
        for(unsigned int i = 0; i <  splines->size(); ++i)
        {
            if(	(splines->weight(i) >= m_minWeight->value()) &&  (splines->weight(i) <= m_maxWeight->value()) )
            {
                float current_weight =		std::max(0.0f, std::min(1.0f,(splines->weight(i)-m_minWeight->value())/(m_maxWeight->value() - m_minWeight->value())));
                
                painter->setBrush(QColor(m_colorTable->value().at(current_weight*255)));
                
                painter->drawPolyline(m_lines[i]);
            }	
        }
        
        if(m_showLabels->value())
        {
            unsigned int i = 0;
            for(const QPolygonF& poly: m_lines)
            {
                painter->drawText(poly[0], QString("%1").arg(i++));
            }
        }
        
        painter->restore();
    }
    
	ViewController::paintAfter(painter, option, widget);
}

/**
 * The bounding rect of the cubic spline list.
 *
 * \return The bounding rectangle of this view.
 */
QRectF WeightedCubicSplineList2DViewController::boundingRect() const
{ 
    QRectF rect(-m_lineWidth->value(),
                -m_lineWidth->value(),
                m_model->width()  + 2*m_lineWidth->value(),
                m_model->height() + 2*m_lineWidth->value());
    
    return ViewController::boundingRect().united(rect);
}

/**
 * The typename of this ViewController
 *
 * \return Always: "WeightedCubicSplineList2DViewController"
 */
QString WeightedCubicSplineList2DViewController::typeName() const
{
	return "WeightedCubicSplineList2DViewController";
}

/**
 * Specialization of the update of  the parameters of this ViewController according to the current
 * model's parameters. This is necessary, if something may have changed 
 * the model in meantime.
 * 
 * \param force_update If true, force every single parameter to update.
 */
void WeightedCubicSplineList2DViewController::updateParameters(bool force_update)
{
    WeightedCubicSplineList2D * splines = static_cast<WeightedCubicSplineList2D*> (model());
    
    WeightedCubicSplineList2DStatistics* old_stats = static_cast<WeightedCubicSplineList2DStatistics*>(m_stats);
    WeightedCubicSplineList2DStatistics* new_stats = new WeightedCubicSplineList2DStatistics(splines);
    
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
void WeightedCubicSplineList2DViewController::updateView()
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
 * Snycronizes between the Model's data and the views reprsentation:
 * Re-creates the polylines and point sets defined by the model.
 */
void WeightedCubicSplineList2DViewController::refreshSplines()
{
    float m_sampling= 100.0;
    
    WeightedCubicSplineList2D * splines = static_cast<WeightedCubicSplineList2D*> (model());
    
    m_lines.clear();
    m_points.clear();
    
    for(unsigned int i = 0; i <  splines->size(); ++i)
    {
        CubicSpline2D spl = splines->spline(i);
        
        QList<QPointF> spl_points;
        for(unsigned int j = 0; j < spl.size(); ++j)
        {
            spl_points.push_back(spl.point(j));
        }
        m_points.push_back(spl_points);
        
        
        QPolygonF poly;
        
        if(m_sampling && spl.size()>1)
        {
            float step=1.0/m_sampling;
                
            for(float t = 0; t < 1+step; t+=step)
            {
                poly << spl.interpolate(t);
            }
        }
        m_lines.push_back(poly);
    }
}

/**
 * Implementation/specialization of the handling of a mouse-pressed event
 *
 * \param event The mouse event which triggered this function.
 */
void WeightedCubicSplineList2DViewController::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    if(acceptHoverEvents())
    {
        WeightedCubicSplineList2D * splines = static_cast<WeightedCubicSplineList2D *> (model());
        
        if(!splines->isViewable())
            return;
        
        QPointF p = event->pos();
        float	x = p.x(),
                y = p.y();
        
        if(x >= 0 && x < splines->width() && y >= 0 && y < splines->height())
        {
            switch (m_mode->value())
            {
                //Select
                case 0:
                    //controller->selectSpline(x,y);
                    break;
                case 1:
                    if ( QMessageBox::question(NULL, QString("Create feature?"), QString("Do you want to create a new feature at (%1, %2)?").arg(x).arg(y), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes )
                    {
                       CubicSpline2D spl = splines->spline(0);
                       spl.addPoint(x,y);
                       splines->setSpline(0,spl);

                        bool ok;
                        double weight = QInputDialog::getDouble(NULL, QString("Values for new feature at (%1, %2)").arg(x).arg(y), QString("Weight of new feature"), 0.0, 0.0, 999999.99, 2, &ok);
                        
                        if(ok)
                        {
                            if (splines->size()==0)
                            {
                                splines->addSpline(CubicSpline2D());
                            }
                           splines->setWeight(0, weight);
                        }
                    }
                    break;
                case 2:
                    break;
            }
            
            refreshSplines();
            updateParameters();
            
            updateView();
        }
    }
}

} //End of namespace graipe
