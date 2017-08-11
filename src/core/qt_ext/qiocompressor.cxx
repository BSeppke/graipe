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

#include "qiocompressor.hxx"
#include "zlib.h"
#include <QtCore/QDebug>

namespace graipe {

/**
 * @addtogroup graipe_core
 * @{
 *     @file
 *     @brief Implementation file for the QIOCompressor class, see license terms above
 * @}
 */

/** internal type for buffered reading: Bytef **/
typedef Bytef ZlibByte;
/** internal type for sizes: uInt **/
typedef uInt ZlibSize;


/**
 * Pirvate/Hidden implementation of the QIOCompressor's internal magic
 */
class QIOCompressorPrivate
{
    QIOCompressor *q_ptr;
    Q_DECLARE_PUBLIC(QIOCompressor)
public:
    /** internal states **/
    enum State {
        // Read state
        NotReadFirstByte,
        InStream,
        EndOfStream,
        // Write state
        NoBytesWritten,
        BytesWritten,
        // Common
        Closed,
        Error
    };

    /**
     * Constructor of the QIOCompressorPrivate class.
     *
     * \param q_ptr Pointer to the QIOCompressor
     * \param device Pointer to the input/output device
     * \param compressionLevel the compression level
     * \param bufferSize the size of the buffer
     */
    QIOCompressorPrivate(QIOCompressor *q_ptr, QIODevice *device, int compressionLevel, int bufferSize);
    
    /**
     * Destructor of the QIOCompressorPrivate class.
     */
    ~QIOCompressorPrivate();
    
    /**
     * Flushing the stream using the underlying zlib library
     *
     * \param flushMode The mode for the flush.
     */
    void flushZlib(int flushMode);
    
    /**
     * Write bytes on the stream using the underlying zlib library
     *
     * \param buffer The buffer, which will be written (compressed)
     * \param outputSize The size before writing
     * \return True, if successful, else false.
     */
    bool writeBytes(ZlibByte *buffer, ZlibSize outputSize);
    
    /**
     * Sets the error QString to errorMessage + zlib error QString for zlibErrorCode
     *
     * \param errorMessage The error message
     * \param zlibErrorCode The zlib Error Code
     */
    void setZlibError(const QString &errorMessage, int zlibErrorCode);

    /**
     * @{ 
     * Member variables for the internal class
     */
    QIODevice* device;
    bool manageDevice;
    z_stream zlibStream;
    const int compressionLevel;
    const ZlibSize bufferSize;
    ZlibByte *buffer;
    State state;
    QIOCompressor::StreamFormat streamFormat;
    /**
     * @}
     */
};

QIOCompressorPrivate::QIOCompressorPrivate(QIOCompressor *q_ptr, QIODevice *device, int compressionLevel, int bufferSize)
:q_ptr(q_ptr)
,device(device)
,compressionLevel(compressionLevel)
,bufferSize(bufferSize)
,buffer(new ZlibByte[bufferSize])
,state(Closed)
,streamFormat(QIOCompressor::ZlibFormat)
{
    // Use default zlib memory management.
    zlibStream.zalloc = Z_NULL;
    zlibStream.zfree = Z_NULL;
    zlibStream.opaque = Z_NULL;
}

QIOCompressorPrivate::~QIOCompressorPrivate()
{
    delete[] buffer;
}

void QIOCompressorPrivate::flushZlib(int flushMode)
{
    // No input.
    zlibStream.next_in = 0;
    zlibStream.avail_in = 0;
    int status;
    do {
        zlibStream.next_out = buffer;
        zlibStream.avail_out = bufferSize;
        status = deflate(&zlibStream, flushMode);
        if (status != Z_OK && status != Z_STREAM_END) {
            state = QIOCompressorPrivate::Error;
            setZlibError(QT_TRANSLATE_NOOP("QIOCompressor", "Internal zlib error when compressing: "), status);
            return;
        }

        ZlibSize outputSize = bufferSize - zlibStream.avail_out;

        // Try to write data from the buffer to to the underlying device, return on failure.
        if (!writeBytes(buffer, outputSize))
            return;

    // If the mode is Z_FNISH we must loop until we get Z_STREAM_END,
    // else we loop as long as zlib is able to fill the output buffer.
    } while ((flushMode == Z_FINISH && status != Z_STREAM_END) || (flushMode != Z_FINISH && zlibStream.avail_out == 0));

    if (flushMode == Z_FINISH)
        Q_ASSERT(status == Z_STREAM_END);
    else
        Q_ASSERT(status == Z_OK);
}

bool QIOCompressorPrivate::writeBytes(ZlibByte *buffer, ZlibSize outputSize)
{
    Q_Q(QIOCompressor);
    ZlibSize totalBytesWritten = 0;
    // Loop until all bytes are written to the underlying device.
    do {
        const qint64 bytesWritten = device->write(reinterpret_cast<char *>(buffer), outputSize);
        if (bytesWritten == -1) {
            q->setErrorString(QT_TRANSLATE_NOOP("QIOCompressor", "Error writing to underlying device: ") + device->errorString());
            return false;
        }
        totalBytesWritten += bytesWritten;
    } while (totalBytesWritten != outputSize);

    // put up a flag so that the device will be flushed on close.
    state = BytesWritten;
    return true;
}

void QIOCompressorPrivate::setZlibError(const QString &errorMessage, int zlibErrorCode)
{
    Q_Q(QIOCompressor);
    // Watch out, zlibErrorString may be null.
    const char * const zlibErrorString = zError(zlibErrorCode);
    QString errorString;
    if (zlibErrorString)
        errorString = errorMessage + zlibErrorString;
    else
        errorString = errorMessage  + " Unknown error, code " + QString::number(zlibErrorCode);

    q->setErrorString(errorString);
}










QIOCompressor::QIOCompressor(QIODevice* device, int compressionLevel, int bufferSize)
:d_ptr(new QIOCompressorPrivate(this, device, compressionLevel, bufferSize))
{}


QIOCompressor::~QIOCompressor()
{
    Q_D(QIOCompressor);
    close();
    delete d;
}

void QIOCompressor::setStreamFormat(StreamFormat format)
{
    Q_D(QIOCompressor);

    // Print a waning if the compile-time version of zlib does not support gzip.
    if (format == GzipFormat && checkGzipSupport(ZLIB_VERSION) == false)
        qWarning("QIOCompressor::setStreamFormat: zlib 1.2.x or higher is "
                 "required to use the gzip format. Current version is: %s",
                 ZLIB_VERSION);

    d->streamFormat = format;
}

QIOCompressor::StreamFormat QIOCompressor::streamFormat() const
{
    Q_D(const QIOCompressor);
    return d->streamFormat;
}

bool QIOCompressor::isGzipSupported()
{
    return checkGzipSupport(zlibVersion());
}


bool QIOCompressor::isSequential() const
{
    return true;
}

bool QIOCompressor::open(OpenMode mode)
{
    Q_D(QIOCompressor);
    if (isOpen()) {
        qWarning("QIOCompressor::open: device already open");
        return false;
    }

    // Check for correct mode: ReadOnly xor WriteOnly
    const bool read = (bool)(mode & ReadOnly);
    const bool write = (bool)(mode & WriteOnly);
    const bool both = (read && write);
    const bool neither = !(read || write);
    if (both || neither) {
        qWarning("QIOCompressor::open: QIOCompressor can only be opened in the ReadOnly or WriteOnly modes");
        return false;
    }

    // If the underlying device is open, check that is it opened in a compatible mode.
    if (d->device->isOpen()) {
        d->manageDevice = false;
        const OpenMode deviceMode = d->device->openMode();
        if (read && !(deviceMode & ReadOnly)) {
            qWarning("QIOCompressor::open: underlying device must be open in one of the ReadOnly or WriteOnly modes");
            return false;
        } else if (write && !(deviceMode & WriteOnly)) {
            qWarning("QIOCompressor::open: underlying device must be open in one of the ReadOnly or WriteOnly modes");
            return false;
        }

    // If the underlying device is closed, open it.
    } else {
        d->manageDevice = true;
        if (d->device->open(mode) == false) {
            setErrorString(QT_TRANSLATE_NOOP("QIOCompressor", "Error opening underlying device: ") + d->device->errorString());
            return false;
        }
    }

    // Initialize zlib for deflating or inflating.

    // The second argument to inflate/deflateInit2 is the windowBits parameter,
    // which also controls what kind of compression stream headers to use.
    // The default value for this is 15. Passing a value greater than 15
    // enables gzip headers and then subtracts 16 form the windowBits value.
    // (So passing 31 gives gzip headers and 15 windowBits). Passing a negative
    // value selects no headers hand then negates the windowBits argument.
    int windowBits;
    switch (d->streamFormat) {
    case QIOCompressor::GzipFormat:
        windowBits = 31;
        break;
    case QIOCompressor::RawZipFormat:
        windowBits = -15;
        break;
    default:
        windowBits = 15;
    }

    int status;
    if (read) {
        d->state = QIOCompressorPrivate::NotReadFirstByte;
        d->zlibStream.avail_in = 0;
        d->zlibStream.next_in = 0;
        if (d->streamFormat == QIOCompressor::ZlibFormat) {
            status = inflateInit(&d->zlibStream);
        } else {
            if (checkGzipSupport(zlibVersion()) == false) {
                setErrorString(QT_TRANSLATE_NOOP("QIOCompressor::open", "The gzip format not supported in this version of zlib."));
                return false;
            }

            status = inflateInit2(&d->zlibStream, windowBits);
        }
    } else {
        d->state = QIOCompressorPrivate::NoBytesWritten;
        if (d->streamFormat == QIOCompressor::ZlibFormat)
            status = deflateInit(&d->zlibStream, d->compressionLevel);
        else
            status = deflateInit2(&d->zlibStream, d->compressionLevel, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY);
    }

    // Handle error.
    if (status != Z_OK) {
        d->setZlibError(QT_TRANSLATE_NOOP("QIOCompressor::open", "Internal zlib error: "), status);
        return false;
    }
    return QIODevice::open(mode);
}

void QIOCompressor::close()
{
    Q_D(QIOCompressor);
    if (isOpen() == false)
        return;

    // Flush and close the zlib stream.
    if (openMode() & ReadOnly) {
        d->state = QIOCompressorPrivate::NotReadFirstByte;
        inflateEnd(&d->zlibStream);
    } else {
        if (d->state == QIOCompressorPrivate::BytesWritten) { // Only flush if we have written anything.
            d->state = QIOCompressorPrivate::NoBytesWritten;
            d->flushZlib(Z_FINISH);
        }
        deflateEnd(&d->zlibStream);
    }

    // Close the underlying device if we are managing it.
    if (d->manageDevice)
        d->device->close();

    QIODevice::close();
}


void QIOCompressor::flush()
{
    Q_D(QIOCompressor);
    if (isOpen() == false || openMode() & ReadOnly)
        return;

    d->flushZlib(Z_SYNC_FLUSH);
}

qint64 QIOCompressor::bytesAvailable() const
{
    Q_D(const QIOCompressor);
    if ((openMode() & ReadOnly) == false)
        return 0;

    int numBytes = 0;

    switch (d->state) {
        case QIOCompressorPrivate::NotReadFirstByte:
            numBytes = d->device->bytesAvailable();
        break;
        case QIOCompressorPrivate::InStream:
            numBytes = 1;
        break;
        case QIOCompressorPrivate::EndOfStream:
        case QIOCompressorPrivate::Error:
        default:
            numBytes = 0;
        break;
    };

    numBytes += QIODevice::bytesAvailable();

    if (numBytes > 0)
        return 1;
    else
        return 0;
}

qint64 QIOCompressor::readData(char *data, qint64 maxSize)
{
    Q_D(QIOCompressor);

    if (d->state == QIOCompressorPrivate::EndOfStream)
        return 0;

    if (d->state == QIOCompressorPrivate::Error)
        return -1;

    // We are ging to try to fill the data buffer
    d->zlibStream.next_out = reinterpret_cast<ZlibByte *>(data);
    d->zlibStream.avail_out = maxSize;

    int status;
    do {
        // Read data if if the input buffer is empty. There could be data in the buffer
        // from a previous readData call.
        if (d->zlibStream.avail_in == 0) {
            qint64 bytesAvalible = d->device->read(reinterpret_cast<char *>(d->buffer), d->bufferSize);
            d->zlibStream.next_in = d->buffer;
            d->zlibStream.avail_in = bytesAvalible;

            if (bytesAvalible == -1) {
                d->state = QIOCompressorPrivate::Error;
                setErrorString(QT_TRANSLATE_NOOP("QIOCompressor", "Error reading data from underlying device: ") + d->device->errorString());
                return -1;
            }

            if (d->state != QIOCompressorPrivate::InStream) {
                // If we are not in a stream and get 0 bytes, we are probably trying to read from an empty device.
                if(bytesAvalible == 0)
                    return 0;
                else if (bytesAvalible > 0)
                    d->state = QIOCompressorPrivate::InStream;
            }
        }

        // Decompress.
        status = inflate(&d->zlibStream, Z_SYNC_FLUSH);
        switch (status) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                d->state = QIOCompressorPrivate::Error;
                d->setZlibError(QT_TRANSLATE_NOOP("QIOCompressor", "Internal zlib error when decompressing: "), status);
                return -1;
            case Z_BUF_ERROR: // No more input and zlib can not privide more output - Not an error, we can try to read again when we have more input.
                return 0;
            break;
        }
    // Loop util data buffer is full or we reach the end of the input stream.
    } while (d->zlibStream.avail_out != 0 && status != Z_STREAM_END);

    if (status == Z_STREAM_END) {
        d->state = QIOCompressorPrivate::EndOfStream;

        // Unget any data left in the read buffer.
        for (int i = d->zlibStream.avail_in;  i >= 0; --i)
            d->device->ungetChar(*reinterpret_cast<char *>(d->zlibStream.next_in + i));
    }

    const ZlibSize outputSize = maxSize - d->zlibStream.avail_out;
    return outputSize;
}

qint64 QIOCompressor::writeData(const char *data, qint64 maxSize)
{
    if (maxSize < 1)
        return 0;
    Q_D(QIOCompressor);
    d->zlibStream.next_in = reinterpret_cast<ZlibByte *>(const_cast<char *>(data));
    d->zlibStream.avail_in = maxSize;

    if (d->state == QIOCompressorPrivate::Error)
        return -1;

    do {
        d->zlibStream.next_out = d->buffer;
        d->zlibStream.avail_out = d->bufferSize;
        const int status = deflate(&d->zlibStream, Z_NO_FLUSH);
        if (status != Z_OK) {
            d->state = QIOCompressorPrivate::Error;
            d->setZlibError(QT_TRANSLATE_NOOP("QIOCompressor", "Internal zlib error when compressing: "), status);
            return -1;
        }

        ZlibSize outputSize = d->bufferSize - d->zlibStream.avail_out;

        // Try to write data from the buffer to to the underlying device, return -1 on failure.
        if (d->writeBytes(d->buffer, outputSize) == false)
            return -1;

    } while (d->zlibStream.avail_out == 0); // run until output is not full.
    Q_ASSERT(d->zlibStream.avail_in == 0);

    return maxSize;
}

bool QIOCompressor::checkGzipSupport(const char * const versionString)
{
    if (strlen(versionString) < 3)
        return false;

    if (versionString[0] == '0' || (versionString[0] == '1' && (versionString[2] == '0' || versionString[2]  == '1' )))
        return false;

    return true;
}
qint64 QIOCompressor::pos() const
{
    Q_D(const QIOCompressor);
    if(d->state == QIOCompressorPrivate::NoBytesWritten)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

}//end of namespace graipe
