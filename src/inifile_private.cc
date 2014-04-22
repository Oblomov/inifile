#include "inierr.h"

#include "inifile_private.h"

using namespace std;

/*
 * Auxiliary text functions
 */

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


/*
 * IniFile::Private implementation
 */

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

			// TODO we might want to merge split sections together
			if (has_section(section))
				ERR("duplicate section: " + section);

			SAFE_add_section(section, comment);

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

		string skey = line.substr(0, eqpos);
		rtrim(skey); // no need to ltrim

		string key = dotjoin(section, skey);
		string val = line.substr(eqpos+1, string::npos);
		ltrim(val); // no need to rtrim

		_ssmap::const_iterator found(_data.find(key));
		if (found != _data.end())
			ERR("duplicate key " + key + " (previously defined as " + found->second + ")");

		_keylist[section].push_back(skey);
		_data[key] = val;
		_comments[key] = comment;
		comment.clear();
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
	_ssmap::const_iterator found(_data.find(key));
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

_svec const&
IniFile::Private::get_keys(string const& section) const
{
	_svmap::const_iterator found(_keylist.find(section));
	if (found == _keylist.end())
		throw notfound_error(section);

	return found->second;
}

string const&
IniFile::Private::get_comment(string const& ks) const
{
	_ssmap::const_iterator found(_comments.find(ks));
	if (found == _comments.end())
		throw notfound_error(ks);

	return found->second;
}

void
IniFile::Private::SAFE_add_section(string const& secname, string const& comment)
{
	_seclist.push_back(secname);
	_keylist[secname] = _svec();
	_comments[secname] = comment;
}

void
IniFile::Private::add_section(string const& secname, string const& _comment)
{
	if (has_section(secname))
		throw duplicate_error("section", secname);

	// TODO we might want to support whitespace in section names in the future
	if (secname.find_first_of(whitespace) != string::npos)
		throw invalid_argument("whitespace in section name");

	// now check that every comment line begins with a comment character
	// and add a terminating newline if necessary
	string comment, line;
	stringstream c(_comment);
	while (getline(c, line)) {
		// add comment sign if necessary
		if (!(line.empty() || line[0] == '#' || line[0] == ';'))
			comment += "; ";
		comment += line + "\n";
	}

	SAFE_add_section(secname, comment);
}

void
IniFile::Private::set(string const& sec, string const& skey, string const& value,
		bool create_missing)
{
	// first of all, check if sec exists
	if (!has_section(sec)) {
		if (create_missing)
			add_section(sec, "");
		else
			throw notfound_error(sec);
	}

	// full key
	string key = dotjoin(sec, skey);

	// does the key exist yet?
	_ssmap::const_iterator found(_data.find(key));

	// no, create it
	if (found == _data.end()) {
		_keylist[sec].push_back(skey);
		_comments[key] = "";
	}

	// (over)write the value
	_data[key] = value;
}

/*
 * Streaming of a IniFile::Private
 */

static ostream&
stream_secname(ostream &stream, string const& secname)
{
	size_t dotpos = secname.find('.');

	stream << "[" << secname.substr(0, dotpos);
	if (dotpos != string::npos)
		stream << " \"" << secname.substr(dotpos+1, string::npos) << "\"";
	return stream << "]";
}

ostream&
operator<<(ostream& out, IniFile::Private const& ip)
{
	_svec const& secs = ip.get_sections();
	_svec::const_iterator fs = secs.begin();
	_svec::const_iterator ls = secs.end();

	// for each section
	while (fs != ls) {
		// output comment
		out << ip.get_comment(*fs);

		// output section name
		stream_secname(out, *fs) << endl;

		const string prefix(*fs + ".");

		_svec const& keys(ip.get_keys(*fs));

		// for each key
		_svec::const_iterator fk = keys.begin();
		_svec::const_iterator lk = keys.end();
		while (fk != lk) {
			// section.key
			const string key(prefix + *fk);

			// output comment
			out << ip.get_comment(key);

			// output key = value
			out << *fk << " = " << ip.get(key) << endl;
			++fk;
		}

		++fs;
	}
	return out;
}
