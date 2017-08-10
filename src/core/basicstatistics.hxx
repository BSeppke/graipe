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

#ifndef GRAIPE_CORE_BASICSTATISTICS_HXX
#define GRAIPE_CORE_BASICSTATISTICS_HXX

#include <cmath>

#include <QtDebug>

namespace graipe  {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the BasicStatistics class
 */

/**
 * This file just contains a very basic data structure for the storage
 * of a simple statistic, w.r.t. minimum, maximum, mean and standard
 * deviation of the data.
 *
 * There exists a print method, which uses qDebug to output the 
 * statistics on the terminal.
 *
 * Since this a header only file, we need no export definitions here!
 */
template <typename T>
struct BasicStatistics
{   
    public:
        /**
         * Comparison of Basic statistics.
         * \param other other statistic to compare with
         * \return true if both have equal min, max, mean and std. dev.
         */
        bool operator==(const BasicStatistics<T>&  other) const
        {
            return min == other.min && max == other.max && mean == other.mean && stddev == other.stddev;
        };
    
        /** minimum of the data **/
        T min;
        /** maximum of the data **/
        T max;
        /** mean of the data **/
        T mean;
        /** standard deviation of the data **/
        T stddev;
};

/**
 * Helper function to log BasicStatistics to the Debug log
 * \param stats Statistics to be logged.
 */
template <typename T>
void printStats(const BasicStatistics<T>& stats)
{
	qDebug().nospace() << "min: " << stats.min << ", max: " << stats.max << ", mean" <<  stats.mean << ", std.dev.: " << stats.stddev << "\n";
}

/**
 * @}
 */
    
}//end of namespace graipe

#endif //GRAIPE_CORE_BASICSTATISTICS_HXX
