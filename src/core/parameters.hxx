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

#ifndef GRAIPE_CORE_PARAMETERS_HXX
#define GRAIPE_CORE_PARAMETERS_HXX
/**
 * @file
 * @brief This file contains a collection of different parameters. The general use of a
 * parameter to encapsulate a value of various types and to provide an
 * editing possibility with respect to a QWidget. This allows us to
 * define something on a higher parameter level and to get the editing 
 * possibility for free. 
 *
 * Inside the basic GRAIPE functionality we use parameters for definition
 * of:
 *    - (Model) properties
 *    - (ViewController) properties
 *    - Algorithm parameters
 *
 * Each parameter implements also a serialization/deserialization method
 * to support the serializability of a ParameterGroup and this the storage
 * and restoring of ViewControllers.
 *
 * This file provides the following parameter classes, which all derive
 * the general Parameter base class:
 *
 *    Name                    Type         Editing widget
 *    -------------------------------------------------------------------
 *    BoolParameter           bool            QCheckBox
 *    IntParameter            int             QSpinBox
 *    PointParameter          QPoint          QSpinBox, QSpinBox
 *    FloatParameter          float           QDoubleSpinBox
 *    PointFParameter         QPointF         QDoubleSpinBox,QDoubleSpinBox
 *    DoubleParameter         double          QDoubleSpinBox
 *    EnumParameter           int             QComboBox
 *    StringParameter         QString          QLineEdit
 *    LongStringParameter     QString          QPlainTextEdit
 *    DateTimeParameter       QDateTime       QDateTimeEdit
 *    FilenameParameter       QString          QLineEdit, QPushButton
 *    ColorParameter          QColor          QPushButton
 *    TransformParameter      QTransform      3x3 QLineEdits
 *    ModelParameter          Model*          QComboBox
 *    MultiModelParameter     vector<Model*>  QListWidget
 *
 * You are, of course, able to extend these Parameters by your own
 * specialized class, as it is done inside the image-Module to select just
 * one band of an image.
 *
 * @addtogroup core
 * @{
 */

#include "core/parameters/boolparameter.hxx"
#include "core/parameters/colorparameter.hxx"
#include "core/parameters/colortableparameter.hxx"
#include "core/parameters/datetimeparameter.hxx"
#include "core/parameters/doubleparameter.hxx"
#include "core/parameters/enumparameter.hxx"
#include "core/parameters/filenameparameter.hxx"
#include "core/parameters/floatparameter.hxx"
#include "core/parameters/intparameter.hxx"
#include "core/parameters/longstringparameter.hxx"
#include "core/parameters/modelparameter.hxx"
#include "core/parameters/multimodelparameter.hxx"
#include "core/parameters/pointfparameter.hxx"
#include "core/parameters/pointparameter.hxx"
#include "core/parameters/stringparameter.hxx"
#include "core/parameters/transformparameter.hxx"
#include "core/parameters/parametergroup.hxx"

/**
 * @}
 */

#endif //GRAIPE_CORE_PARAMETERS_HXX
