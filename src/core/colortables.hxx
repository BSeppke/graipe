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

#ifndef GRAIPE_CORE_COLORTABLES_HXX
#define GRAIPE_CORE_COLORTABLES_HXX

#include "core/config.hxx"

#include <QVector>
#include <QRgb>
#include <QColor>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the color table classes
 */

namespace detail
{
    //Static vector of all globally defined (non-extra) color tables:
    static QVector<QVector<QRgb> > colorTables;
}

/**
 * Helper function to create a color table as a gradient between three colors.
 * 
 * \param col1 First color (at start)
 * \param col2 Second color (at middle)
 * \param col3 Third color (at end)
 * \return 256-element color table by means of gradient w.r.t. three colors given.
 */
GRAIPE_CORE_EXPORT QVector<QRgb> createColorTableFrom3Colors(const QColor & col1, const QColor & col2, const QColor & col3);

/**
 * Accessor for the global color tables
 * \return The globally defined color tables
 */
GRAIPE_CORE_EXPORT QVector<QVector<QRgb> > colorTables();

/**
 * @}
 */

} //end of namespace graipe

#endif //GRAIPE_CORE_COLORTABLES_HXX
