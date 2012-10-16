#ifndef _libgeodecomp_io_ioexception_h_
#define _libgeodecomp_io_ioexception_h_

#include <cstring>
#include <stdexcept>

namespace LibGeoDecomp {

/**
 * exception class which is suitable for all I/O errors. More specific
 * error conditions can be represented as subclasses.
 */
class IOException : public std::runtime_error
{
public:
    /**
     * Initializes a new IOException object which means that an error described
     * by @a message occured.
     */
    IOException(std::string message) :
        std::runtime_error(message) 
    {}

    virtual ~IOException() throw ()
    {}
};


/**
 * More specific error class for errors during file opening.
 */
class FileOpenException : public IOException
{
public:
    FileOpenException(
        std::string file) : 
        IOException("Could not open file " + file) 
    {}
};


/**
 * More specific error class for errors during writing.
 */
class FileWriteException : public IOException
{
public:
    FileWriteException(
        std::string file) : 
        IOException("Could not write file " + file) 
    {}
};


/**
 * More specific error class for errors during reading.
 */
class FileReadException : public IOException
{
public:
    FileReadException(
        std::string file) : 
        IOException("Could not read file " + file) 
    {}
};

}

#endif
