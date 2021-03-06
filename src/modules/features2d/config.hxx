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

#ifndef GRAIPE_FEATURES2D_CONFIG_HXX
#define GRAIPE_FEATURES2D_CONFIG_HXX

/**
 * @addtogroup graipe_features2d
 * @{
 *
 * @file
 * @brief Configuration file to the features2d module
 */

#include <QtCore/QtGlobal>

#ifdef GRAIPE_FEATURES2D_BUILD
	#if (defined(QT_DLL) || defined(QT_SHARED)) && !defined(QT_PLUGIN)
        /** Set GRAIPE_FEATURES2D_EXPORT to export functions **/
		#define GRAIPE_FEATURES2D_EXPORT Q_DECL_EXPORT
	#else
        /** Unset GRAIPE_FEATURES2D_EXPORT **/
		#define GRAIPE_FEATURES2D_EXPORT
	#endif
#else
    /** Set GRAIPE_FEATURES2D_EXPORT to import functions **/
	#define GRAIPE_FEATURES2D_EXPORT Q_DECL_IMPORT
#endif

/**
 * @}
 */

#endif //GRAIPE_FEATURES2D_CONFIG_HXX
