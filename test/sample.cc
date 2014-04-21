#include "inifile.h"
#include "inierr.h"

#include <sstream>
#include <iostream>

using namespace std;

// TODO turn this into a proper set of unit tests

int main(int argc, char *argv[])
{
	IniFile ini;
	int test = 0;

#define TEST_STRING(text) \
	try { \
		++test; \
		ini = IniFile::parse(text); \
		throw runtime_error("parsed correctly"); \
	} catch (runtime_error &e) { \
		cerr << "error test #" << test << ": " << e.what() << endl; \
	}

	TEST_STRING(" just a comment\n[section]\nkey=value");
	TEST_STRING("# just a comment\n[section\nkey=value");
	TEST_STRING("# just a comment\n[section \"]\nkey=value");
	TEST_STRING("# just a comment\n[section \"open]\nkey=value");
	TEST_STRING("# just a comment\n[section]\nkey value");
	TEST_STRING("# just a comment\n[section]\nkey=value\n[override *]\nkey=valuestar\n[override spec]\nkeyspec=valuespec");

#define TEST_VALUE(key, expected) \
	try { \
		++test; \
		string got = ini.get(key); \
		if (got != expected) { \
			stringstream error; \
			error << "expected " << expected ", got " << got; \
			throw runtime_error(error.str()); \
		} \
	} catch (notfound_error &e) { \
		cerr << "error test #" << test << ": not found: " << e.what() << endl; \
	} catch (runtime_error &e) { \
		cerr << "error test #" << test << ": " << e.what() << endl; \
	}

	TEST_VALUE("section", "fail");
	TEST_VALUE("section.key", "should be value");
	TEST_VALUE("override.spec.key", "should be valuestar");
	TEST_VALUE("override.spec.keyspec", "should be valuespec");


	try {
		++test;
		ini = IniFile::load("this file doesn't exist");
	} catch (notfound_error &e) {
		cerr << "error test #" << test << ": not found (file): " << e.what() << endl;
	}

	if (argc > 1) {
		ini = IniFile::load(argv[1]);
		string writer(ini.get("core.writer"));

		bool norms = ini.get<bool>(dotjoin("writer", writer, "normals"));
		bool engy = ini.get<bool>(dotjoin("writer", writer, "energy"));
		cout << norms << " " << engy << endl;
		cout << ini.get<double>("problem.slope") << endl;
	}
}
