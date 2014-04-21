#include "notfound.h"

notfound_error::notfound_error(const std::string& __arg)
	: std::runtime_error(__arg)
{}
