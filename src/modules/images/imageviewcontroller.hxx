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

#ifndef GRAIPE_IMAGES_IMAGEVIEWCONTROLLER_HXX
#define GRAIPE_IMAGES_IMAGEVIEWCONTROLLER_HXX

#include "core/core.h"
#include "images/image.hxx"
#include "images/imagestatistics.hxx"
#include "images/config.hxx"

namespace graipe {

/**
 * @addtogroup graipe_images
 * @{
 *
 * @file
 * @brief Header file for the viewing of images
 */

/**
 * A class for viewing one band of an image on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
template <class T>
class GRAIPE_IMAGES_EXPORT ImageSingleBandViewController
:   public ViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param img The image, which we want to show.
         */
        ImageSingleBandViewController(Image<T>* img);
    
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~ImageSingleBandViewController();
    
        /**
         * The typename of this ViewController
         *
         * \return Unique QString depending on the templated image type. Either:
         *         "ImageSingleBandViewController", "IntImageSingleBandViewController" or
         *         "ByteImageSingleBandViewController".
         */
        QString typeName() const
        {
            return m_img->typeName() + "SingleBandViewController";
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
         * Specialization of the update of the view according to the current parameter settings.
         */
        void updateView();
        
    protected:
        /**
         * Implementation/specialization of the handling of a mouse-hover event.
         * Will send pixel info using signals while hovering over the image.
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent (QGraphicsSceneHoverEvent * event);
        
    private:
        //Statistics
        ImageStatistics<T>* m_stats;
    
        //Additional parameters
        FloatParameter*  m_minValue;
        BoolParameter*   m_transparentBelowMin;
        FloatParameter*  m_maxValue;
        BoolParameter*   m_transparentAboveMax;
        ColorTableParameter*  m_colorTable;
        IntParameter*    m_bandId;
        BoolParameter*   m_showIntensityLegend;
        StringParameter* m_legendCaption;
        IntParameter*    m_legendTicks;
        IntParameter*    m_legendDigits;
    
        //Legend stuff
        QLegend* m_intensity_legend;
        QLinearGradient* m_gradient;
    
        //Pointer to image (to avoid casts)
        Image<T>* m_img;
    
        //Qt image representation and used color table
        QImage m_image;
        QVector<QRgb> m_ct;
};




/**
 * A class for viewing three bands of an image by means of RGB assignment on a QGraphicsScene/View and
 * controlling the view using different parameters.
 */
template <class T>
class GRAIPE_IMAGES_EXPORT ImageRGBViewController
:   public ViewController
{
    public:
        /**
         * Implementation/specialization of the ViewController's
         * constructor.
         *
         * \param scene The scene, where this View shall be carried out.
         * \param img The image, which we want to show.
         * \param z_value The layer (z-coordinate) of our view. Defaults to zero.
         */
        ImageRGBViewController(Image<T>* image);
        
        /**
         * Implementation/specialization of the ViewController's virtual
         * destructor.
         */
        ~ImageRGBViewController();
    
        /**
         * The typename of this ViewController
         *
         * \return Unique QString depending on the templated image type. Either:
         *         "ImageRGBViewController", "IntImageRGBViewController" or
         *         "ByteImageRGBViewController".
         */
        QString typeName() const
        {
            return m_img->typeName() + "RGBViewController";
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
         * Specialization of the update of the view according to the current parameter settings.
         */
        void updateView();
        
    protected:
        /**
         * Implementation/specialization of the handling of a mouse-hover event.
         * Will send pixel info using signals while hovering over the image.
         *
         * \param event The mouse event which triggered this function.
         */
        void hoverMoveEvent (QGraphicsSceneHoverEvent * event);
        
    private:
        //Aditional parameters
        FloatParameter* m_minValue;
        BoolParameter*  m_transparentBelowMin;
        FloatParameter* m_maxValue;
        BoolParameter*  m_transparentAboveMax;
        IntParameter*   m_redBandId;
        IntParameter*   m_greenBandId;
        IntParameter*   m_blueBandId;
    
        //Pointer to the image (to avoid casts)
        Image<T> * m_img;
        
        //Qt image representation
        QImage m_image;
};

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_IMAGES_IMAGEVIEWCONTROLLER_HXX
