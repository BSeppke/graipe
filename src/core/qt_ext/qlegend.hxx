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

#ifndef GRAIPE_CORE_QLEGEND_HXX
#define GRAIPE_CORE_QLEGEND_HXX

#include "core/config.hxx"
#include "core/qt_ext/qgraphicsresizableitem.hxx"

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the QLegend class
 */

/**
 * Based upon the QGraphicsResizableItem, this class defines a legend for
 * various applications. The scale of legend can be set from min to max
 * value, and a color gradient can be given, too. The class then renders
 * this information to show a scale/legend.
 *
 * This class supports two different resizing modes:
 *
 *    1. True to scale: If the width is increased, the max value increases
 *       accordingly to fit to the new width.
 *    2. Not true to scale: The max value remains the same, the legend
 *       simply gets "stretched".
 */
class GRAIPE_CORE_EXPORT QLegend
:	public QGraphicsResizableItem
{
	public:
        /**
         * Default constructor of the QLegend class.
         *
         * \param parent The parent item of this graphics item.
         */
		QLegend(QGraphicsItem * parent = NULL);
    
        /**
         * Second constructor of the QLegend class.
         * 
         * \param rect        The size of the Legend (= its bounding rect).
         * \param lower_val   The lowest value on the scale.
         * \param upper_val   The highest value on the scale.
         * \param ticks       The count of ticks between lower and upper value on the scale.
         * \param fixed_scale If true, the legend behaves as true to scale (see above).
         * \param parent      The parent item of QLegend item.
         */
		QLegend(QRectF rect, float lower_val, float upper_val, int ticks, bool fixed_scale=false, QGraphicsItem * parent = NULL) ;
    
        /**
         * Third constructor of the QLegend class.
         * 
         * \param rect_left    The left position of the QLegend's rect.
         * \param rect_top     The top position of the QLegend's rect.
         * \param rect_width   The width of the QLegend's rect.
         * \param recht_height The height of the new rect.
         * \param lower_val    The lowest value on the scale.
         * \param upper_val    The highest value on the scale.
         * \param ticks        The count of ticks between lower and upper value on the scale.
         * \param fixed_scale  If true, the legend behaves as true to scale (see above).
         * \param parent       The parent item of QLegend item.
         */
		QLegend(float rect_left, float rect_top, float rect_width, float rect_height, float lower_val, float upper_val, int ticks, bool fixed_scale=false, QGraphicsItem * parent = NULL) ;
		
        /**
         * The paint procedure of the QLagend prints the scale and a caption below.
         * If the item is selected, the handles and outline are painted, too.
         *
         * \param painter The painter, which is used for drawing.
         * \param option  The style options used for drawing.
         * \param widget  The widget, where we paint on.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);	
			
        /**
         * Const accessor to the caption of the legend.
         *
         * \return The caption of this QLegend.
         */
		QString caption() const;

        /**
         * Sets the caption of the legend to a new QString.
         *
         * \param caption The new caption of this QLegend.
         */
		void setCaption(const QString& caption);
    
        /**
         * Const accessor to the digits of the scale labels of the legend.
         *
         * \return The digits precision of the scale labels of this QLegend.
         */
        unsigned int digits() const;
    
        /**
         * Sets the digits of the scale labels of the legend to a certain precision.
         *
         * \param digits The digits precision of the scale labels of this QLegend.
         */
        void setDigits(unsigned int digits);
    
        /**
         * Const accessor to the count of ticks of the scale.
         *
         * \return The count of ticks of the scale.
         */
        unsigned int ticks() const;
        
        /**
         * Sets the ticks of the scale to a given count.
         *
         * \param ticks The new tick division of the scale.
         */
        void setTicks(unsigned int ticks);
    
        /**
         * Const accessor to find out if the "true to scale" mode is enabled.
         *
         * \return True, if the "true to scale" mode is enabled.
         */
		bool fixedScale() const;
    
        /**
         * Fixes the scale, such that it behaves like true to scale when resized.
         *
         * \param fix If true, the "true to scale" mode will we enabled.
         */
		void fixScale(bool fix);
    
        /**
         * Const accessor to the minimum value of the scale.
         *
         * \return The minimum value of the scale.
         */
		float minValue() const;
	
        /**
         * Sets the minimum value of the scale.
         *
         * \param val The new minimum value of the scale.
         */
		void setMinValue(float val);
        
        /**
         * Const accessor to the maximum value of the scale.
         *
         * \return The maximum value of the scale.
         */
		float maxValue() const;
	
        /**
         * Sets the maximum value of the scale.
         *
         * \param val The new maximum value of the scale.
         */
		void setMaxValue(float val);
	
        /**
         * Sets the range(minimum and maximum value) of the scale.
         *
         * \param min_val The new minimum value of the scale.
         * \param max_val The new maximum value of the scale.
         */
		void setValueRange(float min_val, float max_val);
		
        /**
         * Const accessor to the color table of the scale.
         *
         * \return The color table of the scale.
         */
        QVector<QRgb> colorTable() const;
        
        /**
         * Sets the color table (colors of minimum and maximum value) of the scale
         *
         * \param colorTable The new color table of the scale.
         */
        void setColorTable(QVector<QRgb> colorTable);
    
    
        /**
         * Overloaded function from QGraphicsResizableItem:
         * This function should be called after every computed change of the
         * rectangle, e.g. due to rescaling events. It checks against the 
         * scaling settings and rescales the rect if allowed.
         *
         * \param new_rect The new rect to be set.
         */
		void updateRect(const QRectF& new_rect);
		
	private:
        /**
         * Returns the width (in pixels), which is available for the labels of the
         * scale in between or under each tick.
         *
         * \return The maximum width for each scale label in pixels.
         */
		inline float text_interval_width() const;
		
    
        //The value range of the legend
		float m_lower_val, m_upper_val;
    
        //The caption of the legend
		QString m_caption;
    
        //The count of ticks between min and max value
		unsigned int m_ticks;
	
        //The accuracy in digits after "." for the scale labels
		unsigned int m_digits;
    
        //Holds the "true to scale" mode
		bool m_fixed_scale;

        //The rect of the scale
        QRectF m_scale_rect;
    
        //The font for the scale labels
        QFont m_scale_font;
    
        //The font for the caption
		QFont m_caption_font;
		
        //Min and max color
		QVector<QRgb> m_ct;
    
        //The background image of the scale
		QPixmap* m_bg_pixmap;
};

/**
 * @}
 */
    
}//end of namespace graipe

#endif //GRAIPE_CORE_QLEGEND_HXX
