#include "inifile_err.h"

#include "inifile_private.h"

#include <fstream>

#include <cstring>

using namespace std;

/*
 * IniFile implementation
 */

IniFile::IniFile() : _private(new IniFile::Private())
{}

IniFile::~IniFile()
{ delete _private;}

IniFile::IniFile(IniFile const& o) :
	_private(new IniFile::Private(*(o._private)))
{}

IniFile&
IniFile::operator=(IniFile const& o)
{
	*_private = *o._private;
	return *this;
}

IniFile
IniFile::load(const char *fname)
{
	ifstream inifile(fname);
	if (!inifile)
		throw notfound_error(fname);
	return parse(inifile, fname);

}

IniFile
IniFile::parse(istream &inifile, const char *_fname)
{
	const char *fname = _fname ? _fname : "<>";

	IniFile ret;
	ret._private->parse(inifile, fname);

	return ret;
}

string const&
IniFile::get(string const& key) const
{
	return _private->get(key);
}

string const&
IniFile::get(string const& key, string const& missing) const
{
	try {
		return this->get(key);
	} catch (notfound_error &) {
		return missing;
	}
}

static const char* trueties[] = {
	"yes", "true", "on", "1", NULL
};

static const char* falsies[] = {
	"no", "false", "off", "0", NULL
};

template<>
bool
IniFile::get<bool>(string const& key) const
{
	const char* val(this->get(key).c_str());

	const char** test = trueties;
	while (*test) {
		if (!strcmp(*test++, val))
			return true;
	}
	test = falsies;
	while (*test) {
		if (!strcmp(*test++, val))
			return false;
	}

	stringstream error;
	error << "cannot determine truth value of " << key << ": " << val;
	throw runtime_error(error.str());
}

template<typename T> T
IniFile::get(string const& key) const
{
	T val;

	stringstream str(this->get(key));
	str >> val;
	return val;
}

template<typename T> T
IniFile::get(string const& key, T missing) const
{
	try {
		return this->get<T>(key);
	} catch (notfound_error &) {
		return missing;
	}
}

template bool
IniFile::get(string const&, bool) const;

template double
IniFile::get(string const&, double) const;

template float
IniFile::get(string const&, float) const;

_svec const&
IniFile::get_sections() const
{ return _private->get_sections(); }

_svec const&
IniFile::get_keys(string const& section) const
{ return _private->get_keys(section); }

string const&
IniFile::get_comment(string const& ks) const
{ return _private->get_comment(ks); }

void
IniFile::add_section(string const& name, string const& comment)
{ _private->add_section(name, comment); }

/* Value setters */

void
IniFile::set(string const& section, string const& key,
	string const& value)
{
	_private->set(section, key, value, false);
}

void
IniFile::set_p(string const& section, string const& key,
	string const& value)
{
	_private->set(section, key, value, true);
}

void
IniFile::set(string const& dotted, string const& value)
{
	size_t lastdot = dotted.rfind('.');
	if (lastdot == string::npos)
		throw parse_error("<key.value>", 0, "missing dot");

	_private->set(dotted.substr(0, lastdot),
		dotted.substr(lastdot+1, string::npos),
		value, false);
}

void
IniFile::set_p(string const& dotted, string const& value)
{
	size_t lastdot = dotted.rfind('.');
	if (lastdot == string::npos)
		throw parse_error("<key.value>", 0, "missing dot");

	_private->set(dotted.substr(0, lastdot),
		dotted.substr(lastdot+1, string::npos),
		value, true);
}

/* Streaming functions */

ostream&
operator<<(ostream& out, IniFile const& ini)
{
	out << *(ini._private);
	return out;
}

istream&
operator>>(istream& in, IniFile &ini)
{
	ini = IniFile::parse(in);
	return in;
}
