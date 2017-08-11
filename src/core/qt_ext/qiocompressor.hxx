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

#ifndef GRAIPE_CORE_QIOCOMPRESSOR_HXX
#define GRAIPE_CORE_QIOCOMPRESSOR_HXX

#include "core/config.hxx"

/****************************************************************************
**
** This file is part of a Qt Solutions component.
** 
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
** 
** Contact:  Qt Software Information (qt-info@nokia.com)
** 
** Commercial Usage  
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
** 
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
** 
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
** 
** GNU General Public License Usage 
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
** 
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
** 
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** 
****************************************************************************/

#include <QtCore/QIODevice>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *
 * @file
 * @brief Header file for the QIOCompressor class, see license terms above
 */
 
class QIOCompressorPrivate;

/*! \class QIOCompressor
    \brief The QIOCompressor class is a QIODevice&that compresses data streams.

    A QIOCompressor object is constructed with a pointer to an
    underlying QIODevice.  Data written to the QIOCompressor object
    will be compressed before it is written to the underlying
    QIODevice. Similary, if you read from the QIOCompressor object,
    the data will be read from the underlying device and then
    decompressed.

    QIOCompressor is a sequential device, which means that it does
    not support seeks or random access. Internally, QIOCompressor
    uses the zlib library to compress and uncompress data.

    Usage examples:
    Writing compressed data to a file:
    \code
        QFile file("foo");
        QIOCompressor compressor(&file);
        compressor.open(QIODevice::WriteOnly);
        compressor.write(QByteArray() << "The quick brown fox");
        compressor.close();
    \endcode

    Reading compressed data from a file:
    \code
        QFile file("foo");
        QIOCompressor compressor(&file);
        compressor.open(QIODevice::ReadOnly);
        const QByteArray text = compressor.readAll();
        compressor.close();
    \endcode

    QIOCompressor can also read and write compressed data in
    different compressed formats, ref. StreamFormat. Use
    setStreamFormat() before open() to select format.
*/
class GRAIPE_CORE_EXPORT QIOCompressor
:   public QIODevice
{
    Q_OBJECT
    
public:
    /*!
        \enum StreamFormat
        This enum specifies which stream format to use.

        ZlibFormat: This is the default and has the smallest overhead.

        GzipFormat: This format is compatible with the gzip file
        format, but has more overhead than ZlibFormat. Note: requires zlib
        version 1.2.x or higher at runtime.

        RawZipFormat: This is compatible with the most common
        compression method of the data blocks contained in ZIP
        archives. Note: ZIP file headers are not read or generated, so
        setting this format, by itself, does not let QIOCompressor read
        or write ZIP files. Ref. the ziplist example program.

        \sa setStreamFormat()
    */
    enum StreamFormat
    {
        ZlibFormat,
        GzipFormat,
        RawZipFormat
    };
    
    /*!
        Constructs a QIOCompressor using the given \a device as the underlying device.

        The allowed value range for \a compressionLevel is 0 to 9, where 0 means no compression
        and 9 means maximum compression. The default value is 6.

        \a bufferSize specifies the size of the internal buffer used when reading from and writing to the
        underlying device. The default value is 65KB. Using a larger value allows for faster compression and
        deompression at the expense of memory usage.
    */
    QIOCompressor(QIODevice *device, int compressionLevel = 6, int bufferSize = 65500);
    /*!
        Destroys the QIOCompressor, closing it if neccesary.
    */
    ~QIOCompressor();
    
    /*!
        Sets the format on the compressed stream to \a format.

        \sa QIOCompressor::StreamFormat
    */
    void setStreamFormat(StreamFormat format);
    
    /*!
        Returns the format set on the compressed stream.
        \sa QIOCompressor::StreamFormat
    */
    StreamFormat streamFormat() const;
    
    /*!
        Returns true if the zlib library in use supports the gzip format, false otherwise.
    */
    static bool isGzipSupported();
    
    /*!
        Is always true, since we compress linearly.
    */
    bool isSequential() const;
    
    /*!
        Opens the QIOCompressor in \a mode. Only ReadOnly and WriteOnly is supported.
        This functon will return false if you try to open in other modes.

        If the underlying device is not opened, this function will open it in a suitable mode. If this happens
        the device will also be closed when close() is called.

        If the underlying device is already opened, its openmode must be compatable with \a mode.

        Returns true on success, false on error.

        \sa close()
    */
    bool open(OpenMode mode);
    
    /*!
         Closes the QIOCompressor, and also the underlying device if it was opened by QIOCompressor.
        \sa open()
    */
    void close();
    
    /*!
        Flushes the internal buffer.

        Each time you call flush, all data written to the QIOCompressor is compressed and written to the
        underlying device. Calling this function can reduce the compression ratio. The underlying device
        is not flushed.

        Calling this function when QIOCompressor is in ReadOnly mode has no effect.
    */
    void flush();
    
    /*!
        Returns 1 if there might be data available for reading, or 0 if there is no data available.

        There is unfortunately no way of knowing how much data there is available when dealing with compressed streams.

        Also, since the remaining compressed data might be a part of the meta-data that ends the compressed stream (and
        therefore will yield no uncompressed data), you cannot assume that a read after getting a 1 from this function will return data.
    */
    qint64 bytesAvailable() const;
    
    /**
     * If nothing is written so far, this function returns 0, else 1.
     *
     * \return 0, if at the beginning of the stream, 1 else.
     */
    qint64 pos() const;
    
protected:
    /*!
        Reads and decompresses data from the underlying device.
        \param data    Byte pointer to the data.
        \param maxSize The max length to be read from data.
        \return the length of successfully read data.
    */
    qint64 readData(char * data, qint64 maxSize);

    /*!
        Compresses and writes data to the underlying device.
        \param data    Byte pointer to the data.
        \param maxSize The max length to be written from data.
        \return the length of successfully written data.
    */
    qint64 writeData(const char * data, qint64 maxSize);
    
private:
    /*
        \internal
        Checks if the run-time zlib version is 1.2.x or higher.
    */
    static bool checkGzipSupport(const char * const versionString);
    QIOCompressorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QIOCompressor)
    Q_DISABLE_COPY(QIOCompressor)
};

/**
 * @}
 */
    
}//end of namespace graipe

#endif
