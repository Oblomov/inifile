#ifndef _NOTFOUND_H
#define _NOTFOUND_H

#include <stdexcept>
#include <string>

class notfound_error : public std::runtime_error
{
public:
	explicit notfound_error(const std::string& __arg);
};

#endif
