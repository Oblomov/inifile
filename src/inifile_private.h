#ifndef _INIFILE_PRIVATE_H
#define _INIFILE_PRIVATE

#include "inifile.h"

using namespace std;

typedef vector<string> _svec;
typedef map<string, string> _ssmap;
typedef map<string, _svec > _svmap;

class IniFile::Private
{
	_svec _seclist; // list of sections
	_svmap _keylist; // list of keys per section
	_ssmap _data; // maps section.key to value
	_ssmap _comments; // map section and section.key to comments

public:
	void parse(istream &stream, const char *fname);

	string const& get(string const&) const;

	// get avalable sections
	inline _svec const& get_sections() const
	{ return _seclist; }

	// get available keys in the given section
	_svec const& get_keys(string const&) const;

	// get comment for section or key
	string const& get_comment(string const&) const;
};

ostream& operator<<(ostream& out, IniFile::Private const& ip);

#endif

