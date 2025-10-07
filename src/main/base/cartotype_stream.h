/*
cartotype_stream.h
Copyright (C) 2004-2025 CartoType Ltd.
See www.cartotype.com for more information.
*/

#pragma once

#include <cartotype_types.h>
#include <cartotype_arithmetic.h>
#include <cartotype_errors.h>
#include <cartotype_string.h>
#include <string>
#include <list>
#include <mutex>
#include <stdio.h>

#if defined(__unix__) || defined(__APPLE__)
    #include <unistd.h> // to define _POSIX_VERSION
#endif

#ifdef _WIN32
    #include <io.h>
    #include <winapifamily.h>
    #if !(WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        #define CT_MEMORY_MAPPED_FILE_NOT_AVAILABLE
    #endif
#else
    #include <fcntl.h>
    #include <errno.h>
    #include <sys/mman.h>
#endif

#undef COLLECT_STATISTICS

namespace CartoTypeCore
{

// Forward declarations.
class MString;
class String;
class MapClient;
class BufferedFile;
class FileBuffer;
class MemoryMappedFile;
class Encryption;
class ArcFileBufferCache;

/**
The input stream base class.
Streams that do not support random access always throw exceptions for StreamSeek
and may throw exceptions for StreamPosition and StreamLength.
*/
class InputStream
    {
    private:
    /**
    Reads some data into a buffer owned by the InputStream object and returns
    a pointer to it in aPointer. Returns the number of bytes of data in aLength.
    This function will return at least one byte if there are bytes remaining in the
    stream. The pointer is valid until the next call to Read.
    */
    virtual void StreamRead(const uint8_t*& aPointer,size_t& aLength) = 0;
    /** Returns whether the end of the stream has been reached. */
    virtual bool StreamEnd() const = 0;
    /** Seeks to the specified position. */
    virtual void StreamSeek(int64_t aPosition) = 0;
    /** Returns the current position. */
    virtual int64_t StreamPosition() = 0;

    public:
    /** Virtual destructor. */
    virtual ~InputStream() { }
    /** Creates a copy of the stream. */
    virtual std::unique_ptr<InputStream> Copy() = 0;
    /** Returns the number of bytes in the stream. */
    virtual int64_t StreamLength() = 0;
    /** Returns the file name or URI associated with the stream if any. Return the empty string if there is no file name or URI. */
    virtual std::string StreamName() { return std::string(); }

    void Seek(int64_t aPosition);
    /** Returns the current position as a byte offset from the start of the stream. */
    int64_t Position() const { return m_data_position + (int64_t)(m_data - m_data_start); }
    /** Returns true if this stream is at the end of the data. */
    bool EndOfData() const { return !m_data_bytes && StreamEnd(); }
    /** Reads an 8-bit unsigned integer. */
    uint8_t ReadUint8()
        {
        if (m_data_bytes >= 1)
            {
            m_data_bytes--;
            return *m_data++;
            }
        return ReadUint8Helper();
        }
    uint16_t ReadUint16();
    uint16_t ReadUint16LE();
    uint32_t ReadUint32();
    uint32_t ReadUint32LE();
    /** Reads a 40-bit unsigned integer in big-endian form. */
    uint64_t ReadUint40()
        {
        if (m_data_bytes >= 5)
            {
            m_data_bytes -= 5;
            m_data += 5;
            return uint64_t((uint64_t)m_data[-5] << 32 | (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint40Helper();
        }
    /** Reads a file position: that is, an unsigned integer stored in the number of bytes returned by FilePosBytes. */
    int64_t ReadFilePos() { return m_file_pos_bytes == 4 ? ReadUint32() : ReadUint40(); }
    /** Reads a file position combined with a degree square code: that is, an unsigned integer stored in two more bytes than FilePosBytes. */
    int64_t ReadFilePosWithDegreeSquare() { return  m_file_pos_bytes == 4 ? ReadUint48() : ReadUint56(); }
    /** Returns the number of bytes storing a file position; by default 4. The number is always 4 or 5. */
    int32_t FilePosBytes() const { return m_file_pos_bytes; }
    /** Sets the number of bytes used to store a file position: must be 4 or 5. */
    void SetFilePosBytes(int32_t aBytes)
        {
        if (aBytes != 4 && aBytes != 5)
            throw KErrorInvalidArgument;
        m_file_pos_bytes = aBytes;
        }

    uint32_t ReadUintOfSize(int32_t aSize);
    uint32_t ReadUintOfSizeLE(int32_t aSize);
    uint64_t ReadUint();
    int64_t ReadInt();
    uint32_t ReadUintMax32();
    int32_t ReadIntMax32();
    float ReadFloat();
    float ReadFloatLE();
    double ReadDouble();
    double ReadDoubleLE();
    void ReadLine(uint8_t* aBuffer,size_t aMaxBytes,size_t& aActualBytes);
    void ReadBytes(uint8_t* aBuffer,size_t aMaxBytes,size_t& aActualBytes);
    void Read(const uint8_t*& aPointer,size_t& aLength);
    String ReadNullTerminatedString();
    String ReadUtf8StringWithLength();
    std::string ReadUtf8StringWithLengthToStdString();
    std::string ReadUtf8StringToStdString();
    String ReadString(size_t* aBytesRead = nullptr);
    void SkipUtf8String(size_t* aBytesRead = nullptr);
    void Skip(int64_t aBytes);

    /**
    Reads the next aBytes bytes, returning a pointer to them, or return nullptr if
    fewer than that number of bytes is cached.
    */
    const uint8_t* Read(size_t aBytes)
        {
        if (m_data_bytes == 0)
            ReadData();
        if (m_data_bytes >= aBytes)
            {
            m_data += aBytes;
            m_data_bytes -= aBytes;
            return m_data - aBytes;
            }
        return nullptr;
        }

    /** Reads an unsigned big-endian 48-bit number. */
    uint64_t ReadUint48()
        {
        if (m_data_bytes >= 6)
            {
            m_data_bytes -= 6;
            m_data += 6;
            return uint64_t((uint64_t)m_data[-6] << 40 | (uint64_t)m_data[-5] << 32 | (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint48Helper();
        }
    /** Reads an unsigned big-endian 56-bit number. */
    uint64_t ReadUint56()
        {
        if (m_data_bytes >= 7)
            {
            m_data_bytes -= 7;
            m_data += 7;
            return uint64_t((uint64_t)m_data[-7] << 48 | (uint64_t)m_data[-6] << 40 | (uint64_t)m_data[-5] << 32 |
                            (uint64_t)m_data[-4] << 24 | (uint64_t)m_data[-3] << 16 | (uint64_t)m_data[-2] << 8 | m_data[-1]);
            }
        return ReadUint56Helper();
        }

    private:
    uint8_t ReadUint8Helper();
    uint64_t ReadUint40Helper();
    uint64_t ReadUint48Helper();
    uint64_t ReadUint56Helper();
    void ReadAdditionalBytes(size_t aBytesRequired);
    void ReadData()
        {
        m_data_position = StreamPosition();
        StreamRead(m_data_start,m_data_bytes);
        m_data = m_data_start;
        }

    /**
    Internal buffer for reading ints and floats, and for holding incomplete UTF
    sequences
    */
    uint8_t m_buffer[8] = { };
    /** The current data pointer. */
    const uint8_t* m_data = nullptr;
    /** The number of data bytes remaining. */
    size_t m_data_bytes = 0;
    /** The position of m_data_start within the whole of the data. */
    int64_t m_data_position = 0;
    /** Start of data returned by the last call to StreamRead. */
    const uint8_t* m_data_start = nullptr;
    /** Number of bytes in which a file position is stored. */
    int32_t m_file_pos_bytes = 4;
    };

/**
The output stream base class. The byte order for integers, floats and doubles is big-endian unless functions suffixed LE are used.
Implementations must define a single function, Write.
*/
class OutputStream
    {
    public:
    /** Virtual destructor. */
    virtual ~OutputStream() { }
    /** Writes aBytes bytes from aBuffer to the stream. */
    virtual void Write(const uint8_t* aBuffer,size_t aBytes) = 0;

    /** Writes a null-terminated string to the stream. Does not write the final null. */
    void WriteString(const char* aString) { Write((const uint8_t*)aString,strlen(aString)); }
    /** Writes a std::string to the stream. */
    void WriteString(const std::string& aString) { Write((const uint8_t*)aString.c_str(),aString.size()); }
    void WriteString(const MString& aString);
    void WriteXmlText(const MString& aString);
    void WriteUint8(uint8_t aValue);
    void WriteUint16(uint16_t aValue);
    void WriteUint16LE(uint16_t aValue);
    void WriteUint32(uint32_t aValue);
    void WriteUint32LE(uint32_t aValue);
    void WriteUint(uint32_t aValue,int32_t aSize);
    void WriteUintLE(uint32_t aValue,int32_t aSize);
    void WriteUint(uint64_t aValue);
    void WriteInt(int64_t aValue);
    void WriteFloat(float aValue);
    void WriteFloatLE(float aValue);
    void WriteDouble(double aValue);
    void WriteDoubleLE(double aValue);
    void WriteUtf8StringWithLength(const MString& aString);
    void WriteUtf8StringWithLength(const std::string& aString);
    void WriteNullTerminatedString(const MString& aString);
    };

/** An input stream for a contiguous piece of memory. */
class MemoryInputStream: public InputStream
    {
    public:
    /** Creates a memory input stream to read from data of aLength bytes starting at aData. */
    MemoryInputStream(const uint8_t* aData,size_t aLength):
        iData(aData),
        iLength(aLength)
        {
        }

    /** Creates a memory input stream to read from a std::string. */
    explicit MemoryInputStream(const std::string& aString):
        iData((const uint8_t*)aString.data()),
        iLength(aString.length())
        {
        }

    // from InputStream
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override { return iPosition >= iLength; }
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override
        {
        return iPosition;
        }
    int64_t StreamLength() override
        {
        return iLength;
        }
    std::unique_ptr<InputStream> Copy() override;

    private:
    const uint8_t* iData = nullptr;
    int64_t iLength = 0;
    int64_t iPosition = 0;
    };

/** A file input class for reading binary data from file which may be greater than 4Gb in size. */
class BinaryInputFile
    {
    public:
    BinaryInputFile() = default;

    /** Returns the file descriptor. */
    int Fd() const { return m_fd; }

    /** Opens a file. */
    void Open(const char* aFileName);

    /** Opens the file, taking ownership of an already-open file descriptor. */
    void Open(int aFd);

    /** Opens standard input. */
    void OpenStandardInput()
        {
        m_fd = 0;
        }

    ~BinaryInputFile()
        {
        if (m_fd != -1)
#if (defined(_MSC_VER))
            _close(m_fd);
#else
            close(m_fd);
#endif
        }

    /** Seeks to a byte position aOffset in the file; aOrigin is the same as for fseek(). */
    void Seek(int64_t aOffset,int aOrigin)
        {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        int64_t pos = _lseeki64(m_fd,aOffset,aOrigin);
#elif defined(__APPLE__)
        int64_t pos = lseek(m_fd,aOffset,aOrigin);
#elif (defined(_POSIX_VERSION) || defined(__MINGW32__))
        int64_t pos = lseek64(m_fd,aOffset,aOrigin);
#else
        int64_t pos = -1;
        if (aOffset >= INT32_MIN && aOffset <= INT32_MAX)
            pos = lseek(m_fd,long(aOffset),aOrigin);
#endif
        if (pos < 0)
            throw KErrorSystem | (errno & 0xFFFF);
        }

    /** Returns the current byte position in the file. */
    int64_t Tell() const
        {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
        return _telli64(m_fd);
#elif defined (__APPLE__)
        return lseek(m_fd,0,SEEK_CUR);
#elif (defined(_POSIX_VERSION) || defined(__MINGW32__))
        return lseek64(m_fd,0,SEEK_CUR);
#else
        return lseek(m_fd,0,SEEK_CUR);
#endif
        }

    /** Reads up to aBufferSize bytes into aBuffer and returns the number of bytes actually read. */
    size_t Read(uint8_t* aBuffer,size_t aBufferSize)
        {
#if (defined(_MSC_VER))
        return _read(m_fd,aBuffer,(unsigned int)aBufferSize);
#else
        return read(m_fd,aBuffer,aBufferSize);
#endif
        }

    BinaryInputFile(const BinaryInputFile&) = delete;
    BinaryInputFile(BinaryInputFile&&) = delete;
    void operator=(const BinaryInputFile&) = delete;
    void operator=(BinaryInputFile&&) = delete;

    private:
    int m_fd = -1;
    };

/** A cache for buffers (blocks of data) read from a file. */
class FileBufferCache
    {
    public:
    FileBufferCache();
    ~FileBufferCache();
    std::shared_ptr<FileBuffer> Buffer(BinaryInputFile& aFile,int64_t aFilePos,int64_t aFileSize,const Encryption* aEncryption);
    void DeleteBuffersForFile(int aFd);

    private:
    std::mutex m_mutex;
    std::unique_ptr<ArcFileBufferCache> m_arc_cache;
    };

/** A buffered file input stream. */
class BufferedFileInputStream: public InputStream
    {
    public:
    BufferedFileInputStream(std::shared_ptr<FileBufferCache> aSharedCache,const std::string& aFileName,const std::string* aEncryptionKey = nullptr);
    /** Returns a copy of this InputStream. */
    std::unique_ptr<InputStream> Copy() override;

    private:
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override;
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override;
    int64_t StreamLength() override;
    std::string StreamName() override;

    const std::string m_name;
    std::shared_ptr<BufferedFile> m_buffered_file;
    std::shared_ptr<FileBuffer> m_buffer;
    int64_t m_position_in_file = 0;
    const int64_t m_length = 0;
    };


#ifndef CT_MEMORY_MAPPED_FILE_NOT_AVAILABLE

/** A memory-mapped file input stream. */
class MemoryMappedFileInputStream: public InputStream
    {
    public:
    MemoryMappedFileInputStream(const std::string& aFileName);
    std::unique_ptr<InputStream> Copy() override;

    private:
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override;
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override;
    int64_t StreamLength() override;
    std::string StreamName() override;

    const std::string m_name;
    std::shared_ptr<MemoryMappedFile> m_handle;
    int64_t m_position_in_file = 0;
    uint8_t const * const m_data = nullptr;
    const int64_t m_length = 0;
    };

#endif // !CT_MEMORY_MAPPED_FILE_NOT_AVAILABLE

/**
A simple file input stream that does not use seek when reading sequentially.
If the first part of the filename, before any extensions, is '-', it reads from standard input.
*/
class SimpleFileInputStream: public InputStream
    {
    public:
    /** Creates a SimpleFileInputStream to read from the file aFileName. Returns the result in aError. */
    static std::unique_ptr<SimpleFileInputStream> New(Result& aError,const std::string& aFileName,size_t aBufferSize = 64 * 1024);
    /** Creates a SimpleFileInputStream to read from the file aFileName. Throws an exception if the file cannot be opened. */
    SimpleFileInputStream(const std::string& aFileName,size_t aBufferSize = 64 * 1024);

    std::unique_ptr<InputStream> Copy() override;

    private:
    void StreamRead(const uint8_t*& aPointer,size_t& aLength) override;
    bool StreamEnd() const override;
    void StreamSeek(int64_t aPosition) override;
    int64_t StreamPosition() override;
    int64_t StreamLength() override;
    std::string StreamName() override { return iName; }

    BinaryInputFile iFile;
    std::vector<uint8_t> iBuffer;
    std::string iName;
    int64_t iLength = -1;
    bool iStandardInput = false;
    bool iEndOfStream = false;
    };

/** The standard file input stream is the simple one, so create an alias for it.*/
using FileInputStream = SimpleFileInputStream;

/**
An output stream to write to a file that is already open for writing.
The destructor does not close the file.
*/
class OpenFileOutputStream: public OutputStream
    {
    public:
    /**
    Creates a file output stream from a file pointer (the value returned by fopen).
    The file must already have been opened for writing.
    */
    OpenFileOutputStream(void* aFile): iFD(aFile) { }
    void Write(const uint8_t* aBuffer,size_t aBytes) override;
    /** Returns the current position in the file as a byte offset relative to the start of the file. */
    int64_t Position();

    OpenFileOutputStream(const OpenFileOutputStream&) = delete;
    OpenFileOutputStream(OpenFileOutputStream&&) = delete;
    void operator=(const OpenFileOutputStream&) = delete;
    void operator=(OpenFileOutputStream&&) = delete;

    protected:
    OpenFileOutputStream(): iFD(nullptr) { }
    /** The file pointer. The actual type is FILE*. */
    void* iFD;
    };

/**
An output stream to write to a file. The New function opens the file and
the destructor closes it.
*/
class FileOutputStream: public OpenFileOutputStream
    {
    public:
    /** Creates a FileOutputStream to write to the file aFileName. Returns the result in aError. */
    static std::unique_ptr<FileOutputStream> New(Result& aError,const std::string& aFileName);
    /** Creates a FileOutputStream to write to the file aFileName. Throws an exception if the file cannot be opened. */
    FileOutputStream(const std::string& aFileName);
    ~FileOutputStream();
    };

/**
Output stream for a buffer in memory. The caller specifies the initial size of the buffer,
which is automatically enlarged when necessary.
*/
class MemoryOutputStream: public OutputStream
    {
    public:
    /** Creates a MemoryOutputStream object to write to a buffer owned by it, optionally specifying an initial buffer size in bytes. */
    MemoryOutputStream(size_t aInitialBufferSize = 0) { iBuffer.reserve(aInitialBufferSize); }
    void Write(const uint8_t* aBuffer,size_t aBytes) override;

    /** Return a pointer to the memory buffer. */
    const uint8_t* Data() const { return iBuffer.data(); }
    /** Take ownership of the data. */
    std::vector<uint8_t> RemoveData() { std::vector<uint8_t> a; std::swap(a,iBuffer); return a; }
    /** Return the number of bytes written. */
    size_t Length() const { return iBuffer.size(); }

    private:
    std::vector<uint8_t> iBuffer;
    };

/**
An fseek-compatible function for moving to a position in a file, specifying
it using a 64-bit signed integer.
*/
inline int FileSeek(FILE* aFile,int64_t aOffset,int aOrigin)
    {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
    return _fseeki64(aFile,aOffset,aOrigin);
#elif defined(__APPLE__)
    return fseeko(aFile,aOffset,aOrigin);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
    return fseeko64(aFile,aOffset,aOrigin);
#else
    if (aOffset < INT32_MIN)
        return -1;
    else if (aOffset > INT32_MAX)
        return -1;
    return fseek(aFile,long(aOffset),aOrigin);
#endif
    }

/**
An ftell-compatible function for getting the current position in a file,
returning a 64-bit signed integer.
*/
inline int64_t FileTell(FILE* aFile)
    {
#if (defined(_MSC_VER) && !defined(_WIN32_WCE))
    return _ftelli64(aFile);
#elif defined (__APPLE__)
    return ftello(aFile);
#elif ((defined(_POSIX_VERSION) || defined(__MINGW32__)) && !defined(ANDROID) && !defined(__ANDROID__))
    return ftello64(aFile);
#else
    return ftell(aFile);
#endif
    }

bool FileExists(const char* aFileName);
int64_t FileSize(const char* aFileName);
int64_t FileSize(int aFd);

/** Policies used to load and cache files. */
enum class FilePolicy
    {
    /** The default policy. */
    Default = 0,
    /** Each file has a cache that is shared among threads. */
    Cache = 1,
    /** A single cache is used, shared among all files and all threads. */
    SharedCache = 2,
    /** A memory map is created for the file. */
    MemoryMap = 3,
    /** Each file has a single 64K buffer. */
    Simple = 4
    };

std::unique_ptr<InputStream> NewInputStream(FilePolicy aPolicy,std::shared_ptr<FileBufferCache> aSharedCache,const String& aFilename,const std::string* aEncryptionKey);

} // namespace CartoTypeCore

