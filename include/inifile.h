#ifndef _INIFILE_H
#define _INIFILE_H

#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <map>
#include <vector>

#include "inifile_api.h"

class INIFILE_API IniFile
{
public:
	class Private;

private:
	Private *_private;

public:
	/* Constructors/destructors */

	IniFile();
	~IniFile();

	IniFile(IniFile const&);
	IniFile& operator=(IniFile const&);

	/* Static parsers/loaders */

	// parse an input stream, optionally associate
	// a filename to it (to improve error messages)
	static IniFile
	parse(std::istream &stream, const char *fname=NULL);

	static IniFile
	parse(std::string const& str, const char *fname=NULL)
	{
		std::stringstream sstr(str);
		return parse(sstr, fname);
	}

	// load a (named) file
	static IniFile
	load(const char *fname);

	// std::string version
	static inline IniFile
	load(std::string const& fname)
	{ return load(fname.c_str()); }

	/* Read-only accessors */

	// if key not found, return missing
	template<typename T> T
	get(std::string const& key, T missing) const;
	// if key not found, throw
	template<typename T> T
	get(std::string const& key) const;

	// if key not found, return missing
	std::string const&
	get(std::string const& key, std::string const& missing) const;

	// if key not found, throw
	std::string const&
	get(std::string const& key) const;

	// get available sections
	std::vector<std::string> const&
	get_sections() const;

	// get available keys in the given section
	std::vector<std::string> const&
	get_keys(std::string const&) const;

	// get comment for section or key
	std::string const&
	get_comment(std::string const&) const;

	/* Write-only accessors */

	// add a section with an optional comment. throws if section
	// exists, sanitizes comment
	void add_section(std::string const& name, std::string const& comment="");

	// add a key in a given section. throws if section doesn't exist
	void set(std::string const& section, std::string const& key,
		std::string const& value);

	template<typename T> inline void
	set(std::string const& section, std::string const& key, T _v)
	{
		std::stringstream value; value << _v;
		this->set(section, key, value.str());
	}

	// add a key in a given section, creates section if necessary
	void set_p(std::string const& section, std::string const& key,
		std::string const& value);

	template<typename T> inline void
	set_p(std::string const& section, std::string const& key, T _v)
	{
		std::stringstream value; value << _v;
		this->set_p(section, key, value.str());
	}

	// add a key (dotted notation). throws if section doesn't exist
	void set(std::string const& dotted, std::string const& value);

	template<typename T> inline void
	set(std::string const& dotted, T _v)
	{
		std::stringstream value; value << _v;
		this->set(dotted, value.str());
	}

	// add a key (dotted notation), creates section if necessary
	void set_p(std::string const& dotted, std::string const& value);

	template<typename T> inline void
	set_p(std::string const& dotted, T _v)
	{
		std::stringstream value; value << _v;
		this->set_p(dotted, value.str());
	}

	/* Streaming functions */

	friend INIFILE_API std::ostream& operator<<(std::ostream&, IniFile const&);
	friend INIFILE_API std::istream& operator>>(std::istream&, IniFile &);
};

/// dotjoin: joins two or three strings with dots

inline
std::string
dotjoin(std::string const& a,
	std::string const& b)
{ return a + "." + b; }

inline
std::string
dotjoin(std::string const& a,
	std::string const& b,
	std::string const& c)
{ return a + "." + b + "." + c; }


#endif
