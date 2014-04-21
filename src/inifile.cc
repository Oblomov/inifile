#include "inifile.h"
#include "inierr.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>

#include <cstring>
#include <stdexcept>

using namespace std;

static const char whitespace[] = " \n\t\v";

// trim whitespace at the end of a string
static inline
string& rtrim(string &str)
{
	size_t nws = str.find_last_not_of(whitespace);
	if (nws == string::npos)
		str.clear();
	else
		str.erase(++nws, string::npos);
	return str;
}

// trim whitespace at the beginning of a string
static inline
string& ltrim(string &str)
{
	size_t nws = str.find_first_not_of(whitespace);
	if (nws == string::npos)
		str.clear();
	else
		str.erase(0, nws);
	return str;
}

// trim whitespace on both sides
static inline
string& trim(string &str)
{ return ltrim(rtrim(str)); }

class IniFile::Private
{
	// a datum is a comment (string) followed by a (content) string
	typedef pair<string, string> _datum;
	typedef map<string, _datum> _data_type;

	vector<_datum> _seclist; // list of sections
	_data_type _data;
public:
	void parse(istream &stream, const char *fname);

	string const& get(string const&) const;
};

void
IniFile::Private::parse(istream &stream, const char *fname)
{
	string line;
	string section;
	string comment;
	int lnum = 0;
	stringstream errmsg;

#define ERR(stuff) \
	throw parse_error(fname, lnum, stuff);

	while (getline(stream, line)) {
		++lnum;

		// trim whitespace
		trim(line);

		// skip empty lines/comment lines.
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			comment += line + "\n";
			continue;
		}

		// check section begin
		if (line[0] == '[') {
			if (line[line.size()-1] != ']')
				ERR("missing ] at end of section");
			section = line.substr(1, line.size()-2);
			size_t spc = section.find(' ');
			if (spc != string::npos) {
				// 'named' sections such as [section "name"]
				// are renamed into section.name
				string rest = section.substr(spc+1,string::npos);
				section.erase(spc,string::npos);
				// check if rest is quoted
				if (rest[0] == '"') {
					// we want to bomb also on [section "], where rest is exactly the
					// double-quote character only
					if (rest.size() < 2 || rest[rest.size()-1] != '"')
						ERR("missing \" at end of quoted section name");

					rest = rest.substr(1, rest.size()-2);
				}
				section += ".";
				section += rest;
			}

			_seclist.push_back(make_pair(comment, section));
			comment.clear();

			continue;
		}

		// key/value pairs are only accepted in a section
		if (section.empty())
			ERR("non-comment line before first section");

		// key-value: spit at =, trim whitespace and go
		size_t eqpos = line.find('=');
		if (eqpos == string::npos)
			ERR("missing = sign");

		string key = dotjoin(section, line.substr(0, eqpos));
		string val = line.substr(eqpos+1, string::npos);

		// trim key and value
		rtrim(key); // no need to ltrim
		ltrim(val); // no need to rtrim

		_data_type::const_iterator found(_data.find(key));
		if (found != _data.end())
			ERR("duplicate key " + key + " (previously defined as " + found->second.second + ")");
		_data[key] = make_pair(comment, val);
	}
}

// provide a fallback key, obtained replacing the text
// between two dots with a *. returns an empty string
// if two dots are not found in key
static string
fallback_key(string const& key)
{
	string ret;
	size_t dots[2];

	dots[0] = key.find('.');
	if (dots[0] == string::npos)
		return ret;

	dots[1] = key.find('.', ++dots[0]);
	if (dots[1] == string::npos)
		return ret;

	ret += key.substr(0,dots[0]) + "*" +
		key.substr(dots[1], string::npos);
	return ret;
}

string const&
IniFile::Private::get(string const& key) const
{
	_data_type::const_iterator found(_data.find(key));
	if (found == _data.end()) {
		// not found, look for fallbacks
		string fallback(fallback_key(key));
		if (!fallback.empty())
			found = _data.find(fallback);
	}
	if (found == _data.end())
		throw notfound_error(key);

	return found->second.second;
}

/* IniFile proper */

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
	delete _private;
	_private = new IniFile::Private(*(o._private));
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
IniFile::get(std::string const& key) const
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
