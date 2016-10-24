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

#include "images/imageviewcontroller.hxx"

#include <functional>

namespace graipe {
    
/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param img The image, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
template <class T>
ImageSingleBandViewController<T>::ImageSingleBandViewController(QGraphicsScene* scene, Image<T>* img, int z_order)
: ViewController(scene, img, z_order),
    m_stats(new ImageStatistics<T>(img)),
    m_minValue(new FloatParameter("Min. value:",-1e20f, 1e20f, 0)),
    m_transparentBelowMin(new BoolParameter("Transp. (< min):", false)),
    m_maxValue(new FloatParameter("Max. value:",-1e20f, 1e20f, 255)),
    m_transparentAboveMax(new BoolParameter("Transp. (> max):", false)),
    m_colorTable(new ColorTableParameter("Color:",colorTables()[2])),
    m_scalingFunction(NULL),
    m_bandId(new IntParameter("Show band:",0,img->numBands()-1,0)),
    m_showIntensityLegend(new BoolParameter("Show intensity legend:", false)),
    m_legendCaption(new StringParameter("Legend Caption", "intensity", 20, m_showIntensityLegend)),
    m_legendTicks(new IntParameter("Legend ticks", 0, 1000, 10, m_showIntensityLegend)),
    m_legendDigits(new IntParameter("Legend digits", 0, 10, 2, m_showIntensityLegend)),
    m_img(img)
{
    QStringList scalingFunctions;
		scalingFunctions.append("linear");
		scalingFunctions.append("10x log.");
		scalingFunctions.append("square root");
    m_scalingFunction = new EnumParameter("Scaling function:", scalingFunctions,0);
    
    m_parameters->addParameter("minValue", m_minValue);
    m_parameters->addParameter("transMinColor", m_transparentBelowMin);
    m_parameters->addParameter("maxValue", m_maxValue);
    m_parameters->addParameter("transMaxColor", m_transparentAboveMax);
    m_parameters->addParameter("colorTable", m_colorTable);
    m_parameters->addParameter("scaling", m_scalingFunction);
    
	this->setZValue(z_order);
	   
    m_parameters->addParameter("bandId", m_bandId);
    m_parameters->addParameter("showIntensityLegend", m_showIntensityLegend);
    m_parameters->addParameter("legendCaption", m_legendCaption);
    m_parameters->addParameter("legendTicks", m_legendTicks);
    m_parameters->addParameter("legendDigits", m_legendDigits);
    
    //Create and show legend
    m_intensity_legend = new QLegend(0, m_img->height()+5,
                                     150, 50,
                                     m_stats->intensityStats()[0].min, m_stats->intensityStats()[0].max,
                                     m_legendTicks->value(),
                                     false,
                                     this);
    
    m_intensity_legend->setTransform(transform());
    m_intensity_legend->setVisible(false);
    m_intensity_legend->setCaption(m_legendCaption->value());
    m_intensity_legend->setDigits(m_legendDigits->value());
    m_intensity_legend->setZValue(zValue());
    
    updateView();
	
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
template <class T>
ImageSingleBandViewController<T>::~ImageSingleBandViewController()
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
template <class T>
void ImageSingleBandViewController<T>::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter,option, widget);
	
    if(m_img->isViewable())
    {
        painter->drawImage(rect(), m_image);
    }
    
	ViewController::paintAfter(painter,option, widget);
}

/**
 * The typename of this ViewController
 *
 * \return Unique QString depending on the templated image type. Either:
 *         "ImageSingleBandViewController", "IntImageSingleBandViewController" or
 *         "ByteImageSingleBandViewController".
 */
template <class T>
QString ImageSingleBandViewController<T>::typeName() const
{
    Image<T> temp;
    
    return temp.typeName() + "SingleBandViewController";
}
/**
 * Specialization of the update of the view according to the current parameter settings.
 */
template <class T>
void ImageSingleBandViewController<T>::updateView()
{
    ViewController::updateView();
    
    //Check if image is viewable
    if(!m_img->isViewable())
        return;
    
    m_minValue->setRange(floor(m_stats->intensityStats()[m_bandId->value()].min), ceil(m_stats->intensityStats()[m_bandId->value()].max));
    m_maxValue->setRange(floor(m_stats->intensityStats()[m_bandId->value()].min), ceil(m_stats->intensityStats()[m_bandId->value()].max));
    
    m_ct = m_colorTable->value();
    
    if(m_transparentBelowMin->value())
    {
        m_ct[0] = Qt::transparent;
    }
    if(m_transparentAboveMax->value())
    {
        m_ct[255] = Qt::transparent;
    }
    
    const vigra::MultiArray<2,T>& band = m_img->band(m_bandId->value());

    int w = m_img->width();
    int h = m_img->height();
    
    //Function, which we need to rescale:
    std::function<float (float)> func = [](float x){return x;};
    
    switch(m_scalingFunction->value())
    {
         case 1:
            func   = [](float x){return 10*log(std::max(x, 1e-10f));};
            break;
         case 2:
            func   = [](float x){return sqrt(x);};
            break;
    }
    
    
    //Underly colorful gradient of velocity to legend
    m_intensity_legend->setColorTable(m_colorTable->value());
    m_intensity_legend->setValueRange(func(m_minValue->value()), func(m_maxValue->value()));
    m_intensity_legend->setCaption(m_legendCaption->value());
    m_intensity_legend->setTicks(m_legendTicks->value());
    m_intensity_legend->setDigits(m_legendDigits->value());
    
    m_intensity_legend->setVisible(m_showIntensityLegend->value());
    
    
    float offset = -func(m_minValue->value()),
          scale  = (func(m_minValue->value()) == func(m_maxValue->value())) ? 1.0 : 255.0 / (func(m_maxValue->value()) - func(m_minValue->value()));
    
    //Generate final conversion function
    func   = [offset, scale](float x){return scale*(x+offset);};
    
    float val;
    m_image = QImage(w,h,QImage::Format_Indexed8);
    
    for (int y = 0; y < h; y++)
    {
        unsigned char * p = (unsigned char*) m_image.scanLine(y);
        for (int x = 0; x < w; x++)
        {
            val = std::max(std::min(func(band(x,y)),255.0f),0.0f);
            *p = (unsigned char) val;
            p++;
        }
    }
    m_image.setColorTable(m_ct);
    
    update();
}

/**
 * Implementation/specialization of the handling of a mouse-hover event.
 * Will send pixel info using signals while hovering over the image.
 *
 * \param event The mouse event which triggered this function.
 */
template <class T>
void ImageSingleBandViewController<T>::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
    
    //Check if image is viewable
    if(!m_img->isViewable())
        return;

    QPointF p = event->pos();
    float	x = p.x(),
            y = p.y();
    
    if(m_img->band(m_bandId->value()).isInside(vigra::Shape2(x,y)))
    {
        QRgb col = m_image.pixel(x,y);
        float val = m_img->band(m_bandId->value())(x,y);
        
        //Function, which we need to rescale:
        std::function<float (float)> func = [](float x){return x;};
    
        switch(m_scalingFunction->value())
        {
            case 1:
                func   = [](float x){return 10*log(std::max(x, 1e-10f));};
                break;
            case 2:
                func   = [](float x){return sqrt(x);};
                break;
        }
        
        float converted_val = func(val);
        
        emit updateStatusText(m_img->shortName() + QString("[%1,%2] = %3").arg(x).arg(y).arg(val));
        emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                     +	m_img->shortName()
                                     +	QString("</i><br/> at position [%1,%2]").arg(x).arg(y)
                                     +	QString("<br/> <b>Data value: %1</b>").arg(val)
                                     +	QString("<br/> <b>Converted value: %1</b>").arg(converted_val)
                                     +	QString("<br/> <b>Displayed color value: (%1,%2,%3)</b>").arg(qRed(col)).arg(qGreen(col)).arg(qBlue(col))
                                     +	QString("<br/> <b>transparency: %1%</b>").arg((255-qAlpha(col))/2.55));
    }
}





/**
 * Implementation/specialization of the ViewController's
 * constructor.
 *
 * \param scene The scene, where this View shall be carried out.
 * \param img The image, which we want to show.
 * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
 */
template <class T>
ImageRGBViewController<T>::ImageRGBViewController(QGraphicsScene* scene, Image<T>* img, int z_order)
:   ViewController(scene, img, z_order),
    m_minValue(new FloatParameter("Min. value:", -1e20f, 1e20f, 0)),
    m_transparentBelowMin(new BoolParameter("Transp. (< min):", false)),
    m_maxValue(new FloatParameter("Max. value:", -1e20f, 1e20f, 255)),
    m_transparentAboveMax(new BoolParameter("Transp. (> max):", false)),
    m_scalingFunction(NULL),
    m_redBandId(new IntParameter("Red band:",0,img->numBands()-1,0)),
    m_greenBandId(new IntParameter("Green band:",0,img->numBands()-1,(img->numBands()-1)/2)),
    m_blueBandId(new IntParameter("Blue band:",0,img->numBands()-1,img->numBands()-1)),
    m_img(img)
{
    QStringList scalingFunctions;
		scalingFunctions.append("linear");
		scalingFunctions.append("10x log.");
		scalingFunctions.append("square root");
    m_scalingFunction = new EnumParameter("Scaling function:", scalingFunctions,0);
    
    m_parameters->addParameter("minValue", m_minValue);
    m_parameters->addParameter("transMinColor", m_transparentBelowMin);
    m_parameters->addParameter("maxValue", m_maxValue);
    m_parameters->addParameter("transMaxColor", m_transparentAboveMax);
    m_parameters->addParameter("scaling", m_scalingFunction);
    
	this->setZValue(z_order);
    
    m_parameters->addParameter("redBandId", m_redBandId);
    m_parameters->addParameter("greenBandId", m_greenBandId);
    m_parameters->addParameter("blueBandId", m_blueBandId);
    
    updateView();
}

/**
 * Implementation/specialization of the ViewController's virtual
 * destructor.
 */
template <class T>
ImageRGBViewController<T>::~ImageRGBViewController()
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
template <class T>
void ImageRGBViewController<T>::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{ 
	ViewController::paintBefore(painter,option, widget);
    
    //Check if image is viewable
    if(m_img->isViewable())
    {
        painter->drawImage(rect(), m_image);
    }
    
	ViewController::paintAfter(painter,option, widget);
}

/**
 * The typename of this ViewController
 *
 * \return Unique QString depending on the templated image type. Either:
 *         "ImageRGBViewController", "IntImageRGBViewController" or
 *         "ByteImageRGBViewController".
 */
template <class T>
QString ImageRGBViewController<T>::typeName() const
{
    Image<T> temp;
    
    return temp.typeName() + "RGBViewController";
}

/**
 * Specialization of the update of the view according to the current parameter settings.
 */
template <class T>
void ImageRGBViewController<T>::updateView()
{
    ViewController::updateView();
    
    if(!m_img->isViewable())
        return;

    const vigra::MultiArray<2,T>& r = m_img->band(m_redBandId->value());
    const vigra::MultiArray<2,T>& g = m_img->band(m_greenBandId->value());
    const vigra::MultiArray<2,T>& b = m_img->band(m_blueBandId->value());
    
    int w = m_img->width();
    int h = m_img->height();
    
    //Function, which we need to rescale:
    std::function<float (float)> func = [](float x){return x;};
    
    switch(m_scalingFunction->value())
    {
         case 1:
            func   = [](float x){return 10*log(std::max(x, 1e-10f));};
            break;
         case 2:
            func   = [](float x){return sqrt(x);};
            break;
    }
    
    float offset = -func(m_minValue->value()),
          scale  = (func(m_minValue->value()) == func(m_maxValue->value())) ? 1.0 : 255.0 / (func(m_maxValue->value()) - func(m_minValue->value()));
    
    //Generate final conversion function
    func   = [offset, scale](float x){return scale*(x+offset);};
    
    float r_val, g_val, b_val;
    m_image = QImage(w,h,QImage::Format_ARGB32);
    
    for (int y = 0; y < h; y++)
    {
        QRgb * p = (QRgb*) m_image.scanLine(y);
        
        for (int x = 0; x < w; x++)
        {
        
            r_val = func(r(x,y));
            g_val = func(g(x,y));
            b_val = func(b(x,y));
            
            if( m_transparentAboveMax->value() && (r_val > 255 || g_val > 255 || b_val > 255))
                *p = 0;
            else if( m_transparentBelowMin->value() && (r_val < 0 || g_val < 0 || b_val < 0))
                *p = 0;
            else
                *p = qRgb(std::max(std::min(r_val,255.0f),0.0f),
                          std::max(std::min(g_val,255.0f),0.0f),
                          std::max(std::min(b_val,255.0f),0.0f));
            p++;
        }
    }
    update();
}
    
/**
 * Implementation/specialization of the handling of a mouse-hover event.
 * Will send pixel info using signals while hovering over the image.
 *
 * \param event The mouse event which triggered this function.
 */
template <class T>
void ImageRGBViewController<T>::hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{	
	QGraphicsItem::hoverMoveEvent(event);
	
    if(!m_img->isViewable())
        return;

    QPointF p = event->pos();
    float	x = p.x(),
            y = p.y();
    
    if(m_img->band(m_redBandId->value()).isInside(vigra::Shape2(x,y)))
    {
        QRgb col = m_image.pixel(x,y);
        float val_red = m_img->band(m_redBandId->value())(x,y);
        float val_green = m_img->band(m_greenBandId->value())(x,y);
        float val_blue = m_img->band(m_blueBandId->value())(x,y);
    
        //Function, which we need to rescale:
        std::function<float (float)> func = [](float x){return x;};
    
        switch(m_scalingFunction->value())
        {
            case 1:
                func   = [](float x){return 10*log(std::max(x, 1e-10f));};
                break;
            case 2:
                func   = [](float x){return sqrt(x);};
                break;
        }
    
        float converted_val_red = func(val_red);
        float converted_val_green = func(val_green);
        float converted_val_blue = func(val_blue);
        
        emit updateStatusText(m_img->shortName() + QString("[%1,%2] = (R: %3, G: %4, B: %5)").arg(x).arg(y).arg(val_red).arg(val_green).arg(val_blue));
        emit updateStatusDescription(	QString("<b>Mouse moved over Object: </b><br/><i>") 
                                     +	m_img->shortName()
                                     +	QString("</i><br/> at position [%1,%2]").arg(x).arg(y)
                                     +	QString("<br/> <b>Data value: (B%3: %4, B%5: %6, B%7: %8)</b>").arg(m_redBandId->value()).arg(val_red).arg(m_greenBandId->value()).arg(val_green).arg(m_blueBandId->value()).arg(val_blue)
                                     +	QString("<br/> <b>Converted data value: (B%3: %4, B%5: %6, B%7: %8)</b>").arg(m_redBandId->value()).arg(converted_val_red).arg(m_greenBandId->value()).arg(converted_val_green).arg(m_blueBandId->value()).arg(converted_val_blue)
                                     +	QString("<br/> <b>Displayed color value: (%1,%2,%3)</b>").arg(qRed(col)).arg(qGreen(col)).arg(qBlue(col))
                                     +	QString("<br/> <b>transparency: %1%</b>").arg((255-qAlpha(col))/2.55));
    }
}

//Promote ImageSingleBandViewControllers for all three promoted image classes
template class ImageSingleBandViewController<float>;
template class ImageSingleBandViewController<int>;
template class ImageSingleBandViewController<unsigned char>;

//Promote ImageRGBViewControllers for all three promoted image classes
template class ImageRGBViewController<float>;
template class ImageRGBViewController<int>;
template class ImageRGBViewController<unsigned char>;
    
} //end of namespace graipe
