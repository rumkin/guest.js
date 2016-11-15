#include "script.h"
#include "include/v8.h"

using namespace v8;

int exec(Isolate * isolate, ShellScript * script) {
	TryCatch try_catch(isolate);

	Local<Context> context = isolate->GetCurrentContext();

	Local<Object> shell_instance = context->Global()->Get(String::NewFromUtf8(isolate, "shell"))->ToObject();

	Local<Value> prev_filename = shell_instance->Get(String::NewFromUtf8(isolate, "currentScript"));

	// Create a string containing the JavaScript source code.
	Local<String> source = String::NewFromUtf8(isolate, script->source.c_str());

	// Create a string containing full filename.
	Local<String> filename = String::NewFromUtf8(isolate, script->filename);

	// Compile the source code.
	Local<v8::Script> v8Script = Script::Compile(source, filename);

	if (v8Script.IsEmpty()) {
		String::Utf8Value error(try_catch.StackTrace());
		printf("%s\n", *error);
		return 1;
	}

	// TODO Set shell.curretScript name
	shell_instance->Set(String::NewFromUtf8(isolate, "currentScript"), filename);
	// Run the script to get the result.
	Local<Value> result = v8Script->Run();
	// Restore previouse currentScript value.
	shell_instance->Set(String::NewFromUtf8(isolate, "currentScript"), prev_filename);

	if (result.IsEmpty()) {
		String::Utf8Value error(try_catch.StackTrace());
		printf("%s\n", *error);
		return 1;
	}

	// Convert the result to an UTF8 string and print it.
	// String::Utf8Value utf8(result);
	return 0;
}