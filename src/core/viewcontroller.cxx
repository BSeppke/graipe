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

#include "core/viewcontroller.hxx"
#include "core/workspace.hxx"

#include <algorithm>

#include <QPainter>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{ 
 *     @file
 *     @brief Implementation file for the ViewController class
 * @}
 */

ViewController::ViewController(Model * model)
:	m_model(model),
    m_name(new StringParameter("Name", "")),
    m_description(new LongStringParameter("Description:", "ViewController of model", 20, 6, NULL)),
    m_showAxis(new BoolParameter("Show axis?")),
    m_axisLineWidth(new FloatParameter("Line width:", 0,100,1,m_showAxis)),
    m_axisLineColor(new ColorParameter("Line color:", Qt::black, m_showAxis)),
    m_showAxisBackground(new BoolParameter("Show background?",true, m_showAxis)),
    m_axisBackgroundColor(new ColorParameter("Background color:",Qt::white,m_showAxisBackground)),
    m_axisSpacing(new PointParameter("Marker spacing:",QPoint(1,1),QPoint(1000,1000),QPoint(50,50),m_showAxis)),
    m_axisLabelSpacing(new PointParameter("Label spacing:",QPoint(1,1),QPoint(1000,1000),QPoint(100,100),m_showAxis)),
    m_axisFontSize(new FloatParameter("Label font size", 0,1000,10,m_showAxis)),
    m_axisGridStyle(NULL),
    m_parameters(new ParameterGroup("ViewController Properties"))
{
    using namespace ::std;
    
    //Instantiate penstyles for axisGridStyle
    QStringList penstyles;
		penstyles.append("none"); penstyles.append("-----------"); penstyles.append("- - - - - -");
		penstyles.append(". . . . . ."); penstyles.append("- . - . - ."); penstyles.append("- . . - . .");

    m_axisGridStyle = new EnumParameter("Draw grid:", penstyles, 0, m_showAxis);
	
    //Add the parameters to this view
    m_parameters->addParameter("name",m_name);
    m_parameters->addParameter("description",m_description);
    
    m_parameters->addParameter("showAx",m_showAxis);
    m_parameters->addParameter("axLineWidth", m_axisLineWidth);
    m_parameters->addParameter("axLineColor", m_axisLineColor);
    m_parameters->addParameter("showAxBG",m_showAxisBackground);
    m_parameters->addParameter("axBGColor", m_axisBackgroundColor);
    m_parameters->addParameter("axSpacing", m_axisSpacing);
    m_parameters->addParameter("axLabelSpacing", m_axisLabelSpacing);
    m_parameters->addParameter("axFontSize", m_axisFontSize);
    m_parameters->addParameter("axGridStyle", m_axisGridStyle);
    
	//connect other elements to update slot, too:
	connect(m_parameters, SIGNAL(valueChanged()), this,	SLOT(updateView()));
	connect(m_model,      SIGNAL(modelChanged()), this, SLOT(updateView()));
    
    //Add to global viewControllers list
    model->workspace()->viewControllers.push_back(this);
}

ViewController::~ViewController()
{
    //TODO: Check if neccessary..
    if(this->scene() != NULL)
    {
        this->scene()->removeItem(this);
    }
    
    delete m_parameters;
    
    //Remove from global viewControllers list
    model()->workspace()->viewControllers.erase(
        std::remove(model()->workspace()->viewControllers.begin(),
                    model()->workspace()->viewControllers.end(),
                    this),
        model()->workspace()->viewControllers.end());
}

QString ViewController::name() const
{
    return m_name->value();
}

void ViewController::setName(const QString& new_name)
{
    m_name->setValue(new_name);
    updateView();
}

QString ViewController::description() const
{
	return m_description->value();
}

void ViewController::setDescription(const QString & new_description)
{
    m_description->setValue(new_description);
    updateView();
}

Model * ViewController::model()
{
    return m_model;
}

ParameterGroup * ViewController::parameters()
{
    return m_parameters;
}

QRectF ViewController::boundingRect() const
{ 
	float offset = std::max(m_ticks_height*10, (float)m_axis_pen.widthF());
	
	if(m_axisFontSize->value() != 0)
	{
		QFont f("Arial");
		f.setPointSizeF(m_axisFontSize->value() );
		QFontMetricsF fm(f);
		QRectF	temp_rect = fm.boundingRect( QRect(0,0,0,0), Qt::AlignLeft|Qt::AlignBottom, "XXXX");
	
		offset += temp_rect.width();
	}
	
	//Always return rect in local coordinates -> global coords will be computed
	//by the scene by means of the sceneTransform of the item, which is handled
	//elsewhere.
	QRectF rect(QPointF(- offset,
                        - offset),
                QPointF(m_model->width()  + offset,
                        m_model->height() + offset));
	return rect;
}

QRectF ViewController::rect() const
{ 
	return QRectF(0, 0, m_model->width(), m_model->height());
}

void ViewController::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{ 
	paintBefore(painter, option, widget);
	paintAfter(painter, option, widget);
}

void ViewController::paintBefore(QPainter *painter, const QStyleOptionGraphicsItem * , QWidget * )
{ 
	if(m_showAxis->value())
	{
        painter->save();
        
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        painter->setPen(m_axis_pen);
        painter->setBrush(m_axis_brush);
        painter->drawRect(rect()); 
        
        painter->restore();
	}
}

void ViewController::paintAfter(QPainter *painter, const QStyleOptionGraphicsItem * , QWidget * )
{ 
	if(m_showAxis->value())
    {
        painter->save();
        
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        
		if(m_axisFontSize->value() != 0)
		{
			QFont f("Arial");
			f.setPointSizeF(m_axisFontSize->value());
			QFontMetricsF fm(f);
		
			painter->setBrush(m_axis_brush);
			painter->setPen(m_axis_pen);
			painter->setFont(f);
			
			
			for (unsigned int x=0; x<m_model->width(); x+=m_axisSpacing->value().x())
			{
				painter->drawLine(QPointF(x, m_model->height()),
								  QPointF(x, m_model->height()+m_ticks_height));
				
				painter->setPen(m_axis_grid_pen);
				painter->drawLine(QPointF(x, 0),
								  QPointF(x, m_model->height()));
				painter->setPen(m_axis_pen);
				
				if(x%m_axisLabelSpacing->value().x() == 0)
				{
					QString label = QString("%1").arg(x);
					QRectF	rect = fm.boundingRect( QRect(0,0,0,0), Qt::AlignLeft|Qt::AlignBottom, label);
					rect = rect.translated(x - rect.width()/2, 
										   m_model->height()+1.25*m_ticks_height+rect.height());
					painter->drawText(rect, label, QTextOption(Qt::AlignCenter));
				}
			}
			
			for (unsigned int y=0; y<m_model->height(); y+=m_axisSpacing->value().y())
			{
				painter->drawLine(QPointF(0 ,y),
								  QPointF(-m_ticks_height,y));
				
				painter->setPen(m_axis_grid_pen);
				painter->drawLine(QPointF(0, y),
								  QPointF(m_model->width(), y));
				painter->setPen(m_axis_pen);
				
				if(y%m_axisLabelSpacing->value().y() == 0)
				{
					QString label = QString("%1").arg(y);
					QRectF	rect = fm.boundingRect( QRect(0,0,0,0), Qt::AlignLeft|Qt::AlignBottom, label);
					rect = rect.translated(-rect.width() - 1.25*m_ticks_height,
										   y + rect.height()/2);
					painter->drawText(rect, label, QTextOption(Qt::AlignCenter));
				}
			}
            painter->restore();
		}
	}
}

void ViewController::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	QPointF p = event->pos();
	float	x = p.x(),
			y = p.y();
	
	emit updateStatusText(m_model->name() + QString("[%1,%2]").arg(x).arg(y));
	emit updateStatusDescription(QString("<b>Mouse moved over object: </b><br/><i>") + m_model->name() + QString("</i><br/> at position [%1,%2]").arg(x).arg(y));
    
    event->accept();
}

void ViewController::updateView()
{
    //Update view parameters to the newly changed model
    updateParameters();

    prepareGeometryChange();
    
    QFont f("Arial");
    f.setPointSizeF(m_axisFontSize->value());
    QFontMetricsF fm(f);
    m_ticks_height = fm.xHeight();
    
	m_axis_pen = QPen(m_axisLineColor->value(), m_axisLineWidth->value());
	
	if(m_showAxisBackground->value())
	{
		m_axis_brush = QBrush(m_axisBackgroundColor->value());
	}
	else
	{
		m_axis_brush =  QBrush(Qt::NoBrush);
	}
	
	m_axis_grid_pen = QPen(m_axisLineColor->value(), m_axisLineWidth->value(),(Qt::PenStyle)m_axisGridStyle->value());
}

void ViewController::updateParameters(bool /*force_update*/)
{
}

void ViewController::serialize(QXmlStreamWriter& xmlWriter) const
{
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(4);
        
    bool fullFile = (xmlWriter.device()->pos() == 0);
    
    if (fullFile)
    {
        xmlWriter.writeStartDocument();
     }
        xmlWriter.writeStartElement(typeName());
        xmlWriter.writeAttribute("ID", id());
        xmlWriter.writeAttribute("ModelID", m_model->id());
        xmlWriter.writeAttribute("ZOrder", QString::number(zValue()));
    
        if(isVisible())
        {
            xmlWriter.writeAttribute("visible", "true");
        }
    
            m_parameters->serialize(xmlWriter);
        xmlWriter.writeEndElement();
    
    if (fullFile)
    {
        xmlWriter.writeEndDocument();
     }
}

bool ViewController::deserialize(QXmlStreamReader& xmlReader)
{
    try
    {
        //Assume, that the deserialized has already read the start node:
        //if (xmlReader.readNextStartElement())
        //{
            if(     xmlReader.name() == typeName()
                &&  xmlReader.attributes().hasAttribute("ID"))
            {
                setID(xmlReader.attributes().value("ID").toString());
                
                if(xmlReader.attributes().hasAttribute("visible"))
                {
                    setVisible(xmlReader.attributes().value("visible").toString() == "true");
                }
                else
                {
                    setVisible(false);
                }
                
                
                bool success = m_parameters->deserialize(xmlReader);
                 
                while(true)
                {
                    if(xmlReader.tokenType()==QXmlStreamReader::EndElement && xmlReader.name() == typeName())
                    {
                        return true;
                    }
                    if(!xmlReader.readNext())
                    {
                        throw std::runtime_error("End of XML file reached before closing tag for ViewController.");
                    }
                }
                
                return success;
            }
            else
            {
                throw std::runtime_error("Did not find typeName() or id() in XML tree");
            }
        //}
        //else
        //{
        //  throw std::runtime_error("Did not find any start element in XML tree");
        //}
    }
    catch(std::runtime_error & e)
    {
        qCritical() << "Parameter::deserialize failed! Was looking for typeName(): " << typeName() << "Error: " << e.what();
        return false;
    }
    return  false;
}

} //end of namespace graipe

