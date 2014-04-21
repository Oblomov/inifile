#include "inifile.h"
#include "notfound.h"

#include <vector>

#include <fstream>
#include <sstream>
#include <iostream>

#include <cstring>
#include <stdexcept>

using namespace std;

// trim whitespace at the end of a string
static inline
string& rtrim(string &str)
{
	// iterate from the end of the string to find
	// the first non-space character
	string::iterator end = str.end();
	string::iterator start = str.begin();
	string::iterator it(end);

	// empty body, just stepping it backwards while it points to whitespace
	while (it != start && isspace(*(--it)));

	// note the ++it, becase it is currently pointing
	// at the last nonspace character
	if (++it != end)
		str.erase(it, end);
	return str;
}

// trim whitespace at the beginning of a string
static inline
string& ltrim(string &str)
{
	// iterate from the beginning of the string to find
	// the first non-space character
	string::iterator end = str.end();
	string::iterator start = str.begin();
	string::iterator it(start);

	// empty body, just stepping forward while it points to whitespace
	while (it != end && isspace(*(it++)))

	if (it != start)
		str.erase(start, it);
	return str;
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
IniFile::parse(std::istream &inifile, const char *_fname)
{
	string line;
	string section;
	int lnum = 0;
	const char *fname = _fname ? _fname : "<>";

	IniFile ret;
	stringstream errmsg;

#define ERR(stuff) do { \
		errmsg << fname << ":" << lnum << ": " << stuff; \
		throw runtime_error(errmsg.str()); \
	} while (0)

	while (getline(inifile, line)) {
		++lnum;

		// skip empty lines
		if (line.empty())
			continue;

		// trim EOL whitespace
		rtrim(line);

		// skip comment lines
		// TODO if we want to be able to save, we want to preserve
		// comments somehow
		if (line[0] == '#' || line[0] == ';')
			continue;

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
		ltrim(key);
		rtrim(key);
		ltrim(val); // no need to rtrim

		if (ret._data.find(key) != ret._data.end())
			ERR("duplicate key " << key << " (previously defined as " << ret._data.find(key)->second << ")");
		ret._data[key] = val;
	}
	return ret;
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
IniFile::get(string const& key)
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

	return found->second;
}

string const&
IniFile::get(string const& key, string const& missing)
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
IniFile::get<bool>(string const& key)
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
IniFile::get(std::string const& key)
{
	T val;

	stringstream str(this->get(key));
	str >> val;
	return val;
}

template<typename T> T
IniFile::get(string const& key, T missing)
{
	try {
		return this->get<T>(key);
	} catch (notfound_error &) {
		return missing;
	}
}

template bool
IniFile::get(string const&, bool);

template double
IniFile::get(string const&, double);

template float
IniFile::get(string const&, float);
