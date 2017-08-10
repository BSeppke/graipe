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

#ifndef GRAIPE_CORE_CONFIG_HXX
#define GRAIPE_CORE_CONFIG_HXX

#include <QString>
#include <QtGlobal>

namespace  graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the config of GRAIPE's core module
 */

#ifdef GRAIPE_VERSION_MAJOR
    static const unsigned int version_major = GRAIPE_VERSION_MAJOR;
#else
    static const unsigned int version_major = 0;
#endif

#ifdef GRAIPE_VERSION_MINOR
    static const  unsigned int version_minor = GRAIPE_VERSION_MINOR;
#else
    static const  unsigned int version_minor = 0;
#endif

#ifdef GRAIPE_VERSION_PATCH
    static const  unsigned int version_patch = GRAIPE_VERSION_PATCH;
#else
    static const  unsigned int version_patch = 0;
#endif

static const QString version_name = QString("%1.%2").arg(version_major).arg(version_minor);
static const QString full_version_name = QString("%1.%2.%3").arg(version_major).arg(version_minor).arg(version_patch);

#ifdef GRAIPE_GIT_VERSION
    static const QString git_version = GRAIPE_GIT_VERSION;
#else
    static const QString git_version = "n/a";
#endif

#ifdef Q_OS_OSX
    static const QString update_urlpattern = "https://kogs-www.informatik.uni-hamburg.de/~seppke/files/Graipe-v%1.dmg";
#elif defined Q_OS_WIN64
    static const QString update_urlpattern = "https://kogs-www.informatik.uni-hamburg.de/~seppke/files/GraipeSetup-v%1.exe";
#else
    static const QString update_urlpattern = "";
#endif

/**
 * This file holds global build definitions for this module. Since all
 * modules are considered to be shared libs, each module should provide
 * such a configuration. It also includes commonly needed math functions.
 */


#ifdef GRAIPE_CORE_BUILD
	#if (defined(QT_DLL) || defined(QT_SHARED)) && !defined(QT_PLUGIN)
		#define GRAIPE_CORE_EXPORT Q_DECL_EXPORT
	#else
		#define GRAIPE_CORE_EXPORT
	#endif
#else
	#define GRAIPE_CORE_EXPORT Q_DECL_IMPORT
#endif //GRAIPE_CORE_BUILD

/**
 * @}
 */

} //end of namepace graipe

#endif //GRAIPE_CORE_CONFIG_HXX
