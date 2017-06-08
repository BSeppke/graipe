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

#ifndef GRAIPE_CORE_VIEWCONTROLLER_HXX
#define GRAIPE_CORE_VIEWCONTROLLER_HXX

#include "core/config.hxx"
#include "core/model.hxx"
#include "core/parameters.hxx"

#include <QGraphicsObject>
#include <QGraphicsScene>

/**
 * @file
 * @brief Header file for the ViewController class
 *
 * @addtogroup core
 * @{
 */

namespace graipe {

/**
 * This class defines the view of a (data) object, to which we refer as
 * models in the GRAIPE framework. Although it is generally possible to 
 * implement a model type without a view and to use it e.g. with some
 * algorithms, models become *really* useful, iff you can see them!
 *
 * We thus define the ViewController of a model by means of a specialized 2D
 * QGraphicsObject. The view's properties are defined by means of
 * parameters, as it is done for the models.
 *
 * The base implementation paints a rect with background and two axes
 * and a grid overlay. In own views you may either overwrite the 
 * complete paint() procedure and thus dismiss these drawings or (re)call
 * them by the paintBefore() and paintAfter() methods, which are inspired
 * by CLOS' aux class methods.
 *
 * Please not that, if you derive from view to define your own view, you
 * may also have to adjust the boundingRect() method to avoid incorrect
 * rendering or rendering artifacts.
 */
class GRAIPE_CORE_EXPORT ViewController
:	public QGraphicsObject,
    public Serializable
{
	Q_OBJECT
		
	public:
        /**
         * Default constructor of the ViewController class.
         *
         * \param scene  The QGraphicsScene, in which the view shall be placed at
         * \param model  The model, which shall be displayed by means of this view
         * \param z_oder The z-order of the new view
         */
		ViewController(QGraphicsScene *, Model*, int);
    
        /**
         * The default destructor of the view
         */
		virtual ~ViewController();
    
        /**
         * The (full) view name. This returns the complete name.
         *
         * \return The name of the model.
         */
        virtual QString name() const;

        /**
         * Set the view's name to a new QString.
         *
         * \param new_name The new name of the view.
         */
        virtual void setName(const QString & new_name);
	
        /**
         * Const accessor for the view's description QString.
         *
         * \return The description of the view.
         */
		virtual QString description() const;
    
        /**
         * Set the view#s description to a new QString.
         *
         * \param new_description The new description of the view.
         */
		virtual void setDescription(const QString & new_description);
    
        /**
         * Potentially non-const accessor to the model of this view
         *
         * \return Pointer to the model of this view
         */
        Model * model();
        
        /**
         * Potentially non-const accessor to the parameters of this view
         */
        ParameterGroup * parameters();
    
        /**
         * Reimplemented computation of the bounding rect of a view
         * in local coordinates.
         *
         * \return The bounding rectangle of this view.
         */
		QRectF boundingRect() const;
    
        /**
         * Each ViewController is 2D and thus has its assigned "inner rectangle, where it may 
         * present the data under some scales etc. This inner rectangle is defined
         * here. Don't confuse with boundingRect, which is the overall area of all 
         * drawing contents.
         *
         * \return The inner (data) rectangle of this view.
         */
 		QRectF rect() const;
		
        /**
         * This (reimplemented) method is called whenever the view has to be redrawn.
         * Since this base class does not provide any fancy data visualisation, it simply
         * calls two auxilary functions, namely paintBefore and paintAfter so draw the 
         * background and the scale of this view.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    
        /**
         * This auxilary method needs to be called before the data drawing is employed
         * by means of deriving sub-classes. It ensures that the background is drawn as
         * indicated by the parameters.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
        void paintBefore(QPainter *painter, const QStyleOptionGraphicsItem * , QWidget * );
 
        /**
         * This auxilary method needs to be called after the data drawing is employed
         * by means of deriving sub-classes. It ensures that the axis is drawn as
         * indicated by the parameters.
         *
         * \param painter Pointer to the painter, which is used for drawing.
         * \param option Further style options for this GraphicsItem's drawing.
         * \param widget The widget, where we will draw onto.
         */
 		void paintAfter(QPainter *painter, const QStyleOptionGraphicsItem * , QWidget * );

        /**
         * This (reimplemented) method is called everytime the mouse is moved over the ViewController,
         * since the View accepts hoverEvents. If you derive classes for specialized ViewControllers, please
         * make sure to implement a specialized version of this function, too. 
         * The function shall emit two signals:
         *  - updateStatusText(QString) for a brief status, and
         *  - updateStatusDescription(QString) for a detailed description.
         *
         * \param event The QGraphicsSceneHoverEvent, which is raised due to the mouse move.
         */
		void hoverMoveEvent(QGraphicsSceneHoverEvent * event);
    
        /**
         * Update the parameters of this ViewController according to the current
         * model's parameters. This is necessary, if something may have changed 
         * the model in meantime.
         * 
         * \param force_update If true, force every single parameter to update.
         */
        virtual void updateParameters(bool force_update=false);
    
        /**
         * The type of this ViewController (same for every instance).
         *
         * \return "ViewController"
         */
        QString typeName() const
        {
            return "ViewController";
        }
    
        /**
         * This function serializes a complete ViewController to an output device.
         * To do so, it serializes the typeName(), then the model denoted by the model's
         * filename and eventually the parameter set.
         *
         * \param out The output device, where we serialize to.
         */
        void serialize(QXmlStreamWriter& xmlWriter) const;
    
        /**
         * This function deserializes the ViewController by means of its parameter settings.
         * The model needs to be reconstructed elsewhere, espacially before the construction of
         * a ViewController. This will be handled inside the GUI of GRAIPE
         *
         * \param  in The input device, where we read the serialization of this ViewController class from.
         * \return True, if the parameters could be restored,
         */
        bool deserialize(QXmlStreamReader& xmlReader);
    
    public slots:
        /**
         * Update the view according to the current parameter settings
         */
        virtual void updateView();
    
	signals:
        //Brief status (usually send on hoverMouseEvents)
		void updateStatusText(QString);
        
        //Detailled status (usually send on hoverMouseEvents)
		void updateStatusDescription(QString);
		
	protected:
        //The model (not owned by the ViewController)
        Model * m_model;
        
        //The view's parameter
        StringParameter *m_name;
        LongStringParameter *m_description;
    
        BoolParameter	*m_showAxis;
        FloatParameter	*m_axisLineWidth;
        ColorParameter	*m_axisLineColor;
        BoolParameter	*m_showAxisBackground;
        ColorParameter	*m_axisBackgroundColor;
        PointParameter	*m_axisSpacing,
                        *m_axisLabelSpacing;
        FloatParameter	*m_axisFontSize;
        EnumParameter	*m_axisGridStyle;
    
        //A group to collect all the parameters
        ParameterGroup* m_parameters;
    
        //pens and brushes
		QPen   m_axis_pen, m_axis_grid_pen;
		QBrush m_axis_brush;
    
        //height of the scale ticks
		float m_ticks_height;
};

} //end of namespace graipe

/**
 * @}
 */

#endif //GRAIPE_CORE_SERIALIZABLE_HXX
