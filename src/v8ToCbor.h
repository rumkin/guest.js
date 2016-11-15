#include "include/v8.h"
#include <string>

namespace Blank {
	v8::Handle<v8::Value> cborToV8Value(v8::Isolate *, const char * data, size_t size);
	std::string v8ValueToCbor(v8::Handle<v8::Value>);
}