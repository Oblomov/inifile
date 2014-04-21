#ifndef _INIFILE_H
#define _INIFILE_H

#include <string>
#include <istream>
#include <sstream>
#include <map>

class IniFile
{
	class Private;
	Private *_private;

public:
	IniFile();
	~IniFile();

	IniFile(IniFile const&);
	IniFile& operator=(IniFile const&);

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
