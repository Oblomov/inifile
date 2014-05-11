#include "inifile.h"
#include "inifile_err.h"

#include <sstream>
#include <iostream>

using namespace std;

// TODO turn this into a proper set of unit tests

int main(int argc, char *argv[])
{
	IniFile ini;
	int test = 0;

#define EXPECT_FAILURE(testcode, type) do { \
	try { \
		++test; \
		testcode; \
		throw; \
	} catch (type &e) { \
		cerr << "[OK] (l." << __LINE__ << ") #" << test << ": " << #type << ": " << e.what() << endl; \
	} catch (...) { \
		cerr << "[FAIL] (l." << __LINE__ << ") #" << test << endl; \
	} } while (0)

#define EXPECT_SUCCESS(testcode) do { \
	try { \
		++test; \
		testcode; \
		cerr << "[OK] (l." << __LINE__ << ") #" << test << endl; \
	} catch (runtime_error &e) { \
		cerr << "[FAIL] (l." << __LINE__ << ") #" << test << ": " << e.what() << endl; \
	} catch (...) { \
		cerr << "[FAIL] (l." << __LINE__ << ") #" << test << endl; \
	} } while (0)

#define TEST_PARSE_FAIL(text) \
	EXPECT_FAILURE(ini = IniFile::parse(text), parse_error)
#define TEST_PARSE_OK(text) \
	EXPECT_SUCCESS(ini = IniFile::parse(text))

	TEST_PARSE_OK(""); // empty string
	TEST_PARSE_OK("\n"); // empty line
	TEST_PARSE_OK(" \n"); // empty line
	TEST_PARSE_FAIL(" just a comment\n[section]\nkey=value");
	TEST_PARSE_FAIL("# just a comment\n[section\nkey=value");
	TEST_PARSE_FAIL("# just a comment\n[section \"]\nkey=value");
	TEST_PARSE_FAIL("# just a comment\n[section \"open]\nkey=value");
	TEST_PARSE_FAIL("# just a comment\n[section]\nkey value");
	TEST_PARSE_OK("# just a comment\n[section]\nkey=value\n\n[override *]\nkey=valuestar\n[override spec]\nkeyspec=valuespec");

#define TEST_VALUE_FAIL(key) \
	EXPECT_FAILURE(ini.get(key), notfound_error)

#define TEST_RETURNS(action, expected) \
	EXPECT_SUCCESS( \
		string got = action; \
		if (got != expected) { \
			stringstream error; \
			error << "expected " << expected ", got " << got; \
			throw runtime_error(error.str()); \
		} \
		)

#define TEST_VALUE_IS(key, expected) \
	TEST_RETURNS(ini.get(key), expected)

	TEST_VALUE_FAIL("section");
	TEST_VALUE_IS("section.key", "value");
	TEST_VALUE_IS("override.spec.key", "valuestar");
	TEST_VALUE_IS("override.spec.keyspec", "valuespec");

	TEST_RETURNS(ini.get_keys("section").front(), "key");

	cout << ini << endl;

	EXPECT_FAILURE(ini = IniFile::load("this file doesn't exist"), notfound_error);

	stringstream sometext;
	sometext << "[section]\n; comment\nkey = value";
	EXPECT_SUCCESS(sometext >> ini);
	TEST_RETURNS(ini.get_comment("section.key"), "; comment\n");

	EXPECT_FAILURE(ini.add_section("section", "comment"), duplicate_error);
	EXPECT_SUCCESS(ini.add_section("section2", "comment"));
	TEST_RETURNS(ini.get_comment("section2"), "; comment\n");

	EXPECT_FAILURE(ini.set("section3", "key", "value"), notfound_error);
	EXPECT_SUCCESS(ini.set_p("section3", "key", "value"));
	TEST_RETURNS(ini.get("section3.key"), "value");
	TEST_RETURNS(ini.get_comment("section3.key"), "");

	cout << ini << endl;

	if (argc > 1) {
		ini = IniFile::load(argv[1]);
		string writer(ini.get("core.writer"));

		bool norms = ini.get<bool>(dotjoin("writer", writer, "normals"));
		bool engy = ini.get<bool>(dotjoin("writer", writer, "energy"));
		cout << norms << " " << engy << endl;
		cout << ini.get<double>("problem.slope") << endl;

		cout << ini << endl;
	}
}
