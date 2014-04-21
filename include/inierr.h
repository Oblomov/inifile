#ifndef _INIERR_H
#define _INIERR_H

#include <stdexcept>
#include <string>

class parse_error : public std::runtime_error
{
public:
	explicit parse_error(const char* src, int lnum, const std::string& __arg);
};


class notfound_error : public std::runtime_error
{
public:
	explicit notfound_error(const std::string& __arg);
};

#endif
