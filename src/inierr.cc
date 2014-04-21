#include "inierr.h"

#include <sstream>

using namespace std;

static inline
string
build_parse_error(const char* src, int lnum, const string& __arg)
{
	stringstream sstr;
	sstr << src << ":" << lnum << ": " << __arg;
	return sstr.str();
}

parse_error::parse_error(const char* src, int lnum, const string& __arg) :
	runtime_error(build_parse_error(src, lnum, __arg))
{}

notfound_error::notfound_error(const string& __arg) :
	runtime_error(__arg)
{}
