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

	// add a section with a comment. doesn't check for section existence,
	// doesn't check or normalize comment
	void SAFE_add_section(string const& name, string const& comment);

public:
	void parse(istream &stream, const char *fname);

	/* Read-only accessors */

	// get a key
	string const& get(string const&) const;

	// get avalable sections
	inline _svec const& get_sections() const
	{ return _seclist; }

	// do we have the given section?
	inline bool has_section(string const& secname) const
	{ return _keylist.find(secname) != _keylist.end(); }

	// get available keys in the given section
	_svec const& get_keys(string const&) const;

	// get comment for section or key
	string const& get_comment(string const&) const;

	/* Write-only accessors */

	// add a section with an (optional) comment. throws if section
	// exists, sanitizes comment
	void add_section(string const& name, string const& comment);

	// set a key value under the given section. If the section sec does not
	// exist, an error will be thrown, unless create_missing is true, in which
	// case a new section will be created
	void set(string const& sec, string const& key, string const& value,
		bool create_missing);
};

ostream& operator<<(ostream& out, IniFile::Private const& ip);

#endif

