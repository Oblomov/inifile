#ifndef _INIERR_H
#define _INIERR_H

#include <stdexcept>
#include <string>

// indicates an error while parsing a stream
class parse_error : public std::runtime_error
{
public:
	// source file name, source file line number, actual error text
	explicit parse_error(const char* src, int lnum, const std::string& __arg);
};

// indicates an error about a missing entry or file
class notfound_error : public std::invalid_argument
{
public:
	explicit notfound_error(const std::string& __arg);
};

// indicates an error about duplication of content
class duplicate_error : public std::invalid_argument
{
public:
	// type of duplicate content, name of duplicate content
	duplicate_error(const std::string& __type, const std::string& __arg);
};

#endif
