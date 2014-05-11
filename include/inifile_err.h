#ifndef _INIERR_H
#define _INIERR_H

#include <stdexcept>
#include <string>

#include "inifile_api.h"

// indicates an error while parsing a stream
class INIFILE_API parse_error : public std::runtime_error
{
public:
	// source file name, source file line number, actual error text
	explicit parse_error(const char* src, int lnum, const std::string& __arg);
	virtual ~parse_error() throw();
};

// indicates an error about a missing entry or file
class INIFILE_API notfound_error : public std::invalid_argument
{
public:
	explicit notfound_error(const std::string& __arg);
	virtual ~notfound_error() throw();
};

// indicates an error about duplication of content
class INIFILE_API duplicate_error : public std::invalid_argument
{
public:
	// type of duplicate content, name of duplicate content
	duplicate_error(const std::string& __type, const std::string& __arg);
	virtual ~duplicate_error() throw();
};

#endif
