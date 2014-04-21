#ifndef _INIFILE_H
#define _INIFILE_H

#include <string>
#include <istream>
#include <ostream>
#include <sstream>
#include <map>
#include <vector>

class IniFile
{
public:
	class Private;

private:
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

	// get available sections
	std::vector<std::string> const&
	get_sections() const;

	// get available keys in the given section
	std::vector<std::string> const&
	get_keys(std::string const&) const;

	// get comment for section or key
	std::string const&
	get_comment(std::string const&) const;

	friend std::ostream& operator<<(std::ostream&, IniFile const&);
	friend std::istream& operator>>(std::istream&, IniFile &);
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
