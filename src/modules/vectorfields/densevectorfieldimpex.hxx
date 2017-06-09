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

#ifndef GRAIPE_VECTORFIELDS_DENSEVECTORFIELDIMPEX_HXX
#define GRAIPE_VECTORFIELDS_DENSEVECTORFIELDIMPEX_HXX

#include "vectorfields/densevectorfield.hxx"
#include "vectorfields/config.hxx"

namespace graipe {
    
/**
 * The ImageImpex class is just a frame for two static template functions, which
 * are using GDAL/OGR to import or export images into the graipe-format.
 */
class GRAIPE_VECTORFIELDS_EXPORT DenseVectorfieldImpex
{
    public:

        /**
         * Imports a dense vectorfield from the .flo-Format into the graipe format.
         *
         * \param filename The filename of the .flo file to be loaded.
         * \param vf the dense vectorfield, which we fill using the data on harddisk.
         * \return true, if the import was successful, else otherwise.
         */
        static bool importVectorfield(const QString & filename, DenseVectorfield2D & vf);

        /**
         * Exports a dense vectorfield from the graipe format as a .flo-file onto harddisk. 
         *
         * \param vf the dense vectorfield, which we want so store as .flo-file on harddisk
         * \param filename The filename of the image.
         * \return true, if the export was successful, else otherwise.
         */
        static bool exportVectorfield(const DenseVectorfield2D & vf, const QString& filename);
};

    
/**
 * Dense vectorfield import implemented by means of algorithm.
 * This class implements an algorithm to import images.
 */
class DenseVectorfieldImporter
:   public Algorithm
{
    public:
        /**
         * Default constructor for the dense vectorfield importer class.
         */
        DenseVectorfieldImporter();
    
        QString typeName() const
        {
            return "DenseVectorfieldImporter";
        }
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run();
    
};

    
/**
 * Image export implemented by means of algorithm.
 * This class implements an algorithm to export images.
 */
class DenseVectorfieldExporter
:   public Algorithm
{
    public:
        /**
         * Default constructor for the dense vectorfield exporter class.
         */
        DenseVectorfieldExporter();
    
        QString typeName() const
        {
            return "DenseVectorfieldExporter";
        }
    
        /**
         * Specialization of the running phase of this algorithm.
         */
        void run();
};

} //end of namespace graipe

#endif //GRAIPE_VECTORFIELDS_DENSEVECTORFIELDIMPEX_HXX
