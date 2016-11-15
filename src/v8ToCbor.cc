#include <string>
#include <vector>
#include "cbor.h"
#include "include/v8.h"

namespace Blank {
	v8::Handle<v8::Value> cborToV8Value(v8::Isolate *, const char * data, size_t size){
		v8::Handle<v8::Value> value;

		return value;
	}
	
	std::string v8ValueToCbor(v8::Handle<v8::Value> value) {
		// std::vector<V8::Local<v8::Value>> nest = {value};
		cbor::output_dynamic output;
		cbor::encoder encoder(output);

	    // do {
	    // 	value = nest.front();
	    // 	if (value->IsArray()) {
	    // 		//
	    // 		nest.insert(value);
	    // 	} else if (value->IsNull()) {
    	// 		output.write_null();
    	// 	} else if (value->IsError()) {

	    // 	} else if (value->IsObject()) {

	    // 	} else if (value->IsNumber()) {
	    // 		output.write_int(value->Int32Value());
	    // 	} else if (value->IsBoolean()) {
	    // 	
	    // 	} else if (value->IsDate()) {
	    // 	
	    // 	} else if (value->IsUndefined()) {

	    // 	}
	    // } while (! nest.empty());
	    
	    v8::String::Utf8Value message(value);
	    encoder.write_string(* message);

	    return std::string((const char *) output.data(), output.size());
	}
}