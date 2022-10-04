#include "stdafx.h"

#include "ms_getenv.h"

using namespace std;

string ms_getenv(string envName)
{
	char * pValue;
	size_t len;
	errno_t err = _dupenv_s(&pValue, &len, envName.c_str());
	if (err != 0) {
		return "";
	}
	if (len == 0) {
		return "";
	}
	string value = pValue;
	delete[] pValue;
	return value;
}