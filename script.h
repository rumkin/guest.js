#ifndef SCRIPT_H
#define SCRIPT_H

#include <string>
#include "include/v8.h"

using namespace std;
using namespace v8;


struct ShellScript {
	char *filename;
	string source;
};

int exec(Isolate * isolate, ShellScript * script);


#endif