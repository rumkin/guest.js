#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <istream>
#include <ostream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "include/v8.h"
#include "include/libplatform/libplatform.h"
#include "src/plugins/ticker.h"
#include "src/plugins/net.h"
// #include "socket.h"
#include "script.h"
#include "env.h"

using namespace v8;

const char* SHELL_NAME = "blank";
const char* SHELL_VERSION = "0.1.3";

// void VersionGetter(Local<String> property, const PropertyCallbackInfo<Value>& info) {
// 	info.GetReturnValue().Set(String::NewFromUtf8(info.GetIsolate(), SHELL_VERSION));
// }

// long long callbackId = 0;

// std::map<long long, Handle<Value>> callbacks;

// static void SetTimeoutCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
// 	if (args.Length() < 1) return; // TODO Trigger error
// 	HandleScope scope(args.GetIsolate());

// 	Handle<Value> value = args[0];

// 	if (! value->IsFunction()) {
// 		args.GetIsolate()->ThrowException(String::NewFromUtf8(args.GetIsolate(), "Argument #1 is not a function"));
// 		return;
// 	}

// 	long long delay;

// 	if (args.Length() > 1) {
// 		delay = args[1]->Uint32Value();
// 	} else {
// 		delay = 1;
// 	}
	
// 	callbackId++;
// 	callbacks[callbackId] = value;

// 	int id = setTimeout(callbackId, delay);

// 	args.GetReturnValue().Set(id);
// }

// static void ClearTimeoutCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
// 	if (args.Length() < 1) return;

// 	clearTimeout(args[0]->ToInt32().Value());
// }


// Persistent<Function> callback;
// struct Callback {
// 	Persistent<Function> callback;
// };

// typedef v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> tPersistent;
// std::queue<tPersistent> callbacks;

// static void PersistentCallback(const v8::FunctionCallbackInfo<Value> &args) {
// 	Isolate *isolate = args.GetIsolate();
// 	HandleScope scope(isolate);
// 	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[0]);
// 	tPersistent value(isolate, cb);
// 	callbacks.push(value);
// }
// static void PersistentCallback(const v8::FunctionCallbackInfo<Value> &args) {
// 	Isolate *isolate = args.GetIsolate();
// 	HandleScope scope(isolate);
// 	Persistent<Function> callback;
// 	callback.Reset(isolate, args[0].As<Function>());
// Local<Value> argv[argc] = { Null(isolate), String::NewFromUtf8(isolate, "success") };
// Local<Function>::New(isolate, callback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);
// callback.Reset();
// }

// static void PersistentCallback(const v8::FunctionCallbackInfo<Value> &args) {
// 	Isolate *isolate = Isolate::GetCurrent();
// 	HandleScope scope(isolate);
// 	Handle<Function> callback =
//         Local<Function>::Cast(args[1]->ToObject());
	
// 	Cb cb = {[args, callback](){
// 		const unsigned argc = 2;
// 		HandleScope scope(Isolate::GetCurrent());	
		
// 		v8::Persistent<v8::Function> function =
//             v8::Persistent<v8::Function>::New(args.GetIsolate(), callback);
//         // execute callback function
//         // causes the access violation
//         function->Call(function, 0, NULL);
//         function->Reset();
// 	}};

// 	callbacks.push(cb);
// }



static void ExitCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1) {
		exit(0);
	} else {
		exit(args[0]->IntegerValue());
	}
}

static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  HandleScope scope(args.GetIsolate());

  int length = args.Length();
  for (int i = 0; i < length; i++) {
	Handle<Value> arg = args[i];
	String::Utf8Value value(arg);
	fprintf(stdout, "%s", *value);
  }
  std::cout<<std::endl;
}

static void ErrorCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
  if (args.Length() < 1) return;
  HandleScope scope(args.GetIsolate());

  int length = args.Length();
  for (int i = 0; i < length; i++) {
	Handle<Value> arg = args[i];
	String::Utf8Value value(arg);
	fprintf(stderr, "%s", *value);
  }
  std::cerr<<std::endl;
}

// bool exec(Isolate* isolate, char * source_chr, char * fname) {
// 	TryCatch try_catch(isolate);

// 	Local<Context> context = isolate->GetCurrentContext();

// 	Local<Object> shell_instance = context->Global()->Get(String::NewFromUtf8(isolate, "shell"))->ToObject();

// 	Local<Value> prev_filename = shell_instance->Get(String::NewFromUtf8(isolate, "currentScript"));

// 	// Create a string containing the JavaScript source code.
// 	Local<String> source = String::NewFromUtf8(isolate, source_chr);

// 	// Create a string containing full filename.
// 	Local<String> filename = String::NewFromUtf8(isolate, fname);

// 	// Compile the source code.
// 	Local<Script> script = Script::Compile(source, filename);

// 	if (script.IsEmpty()) {
// 		String::Utf8Value error(try_catch.StackTrace());
// 		printf("%s\n", *error);
// 		return false;
// 	}

// 	// TODO Set shell.curretScript name
// 	shell_instance->Set(String::NewFromUtf8(isolate, "currentScript"), filename);
// 	// Run the script to get the result.
// 	Local<Value> result = script->Run();
// 	// Restore previouse currentScript value.
// 	shell_instance->Set(String::NewFromUtf8(isolate, "currentScript"), prev_filename);

// 	if (result.IsEmpty()) {
// 		String::Utf8Value error(try_catch.StackTrace());
// 		printf("%s\n", *error);
// 		return false;
// 	}

// 	// Convert the result to an UTF8 string and print it.
// 	// String::Utf8Value utf8(result);
// 	return true;
// }

bool require(Isolate* isolate, char* path) {


	std::ifstream is (path, std::ifstream::binary);

	if (! is) return false;

	// std::cout << realpath(path, 0) << std::endl;
	
	// get length of file:
	is.seekg (0, is.end);
	int length = is.tellg();
	is.seekg (0, is.beg);

	char * buffer = new char [length + 1];

	// read data as a block:
	is.read (buffer,length);

	buffer[length] = '\0';

	if (! is)
	  std::cout << "error: only " << is.gcount() << " could be read";

	is.close();

	ShellScript script = {path, buffer};

	if (exec(isolate, &script) != 0) {
		return false;
	}

	delete[] buffer;

	return true;
}

static void RequireCallback(const FunctionCallbackInfo<v8::Value>& args) {
	if (args.Length() < 1) return; // TODO Throw error
	
	Isolate* isolate = args.GetIsolate();

	HandleScope scope(isolate);	

	Handle<Value> arg = args[0];
	String::Utf8Value value(arg);
  	
  	if ( ! require(isolate, (char*) std::string(* value).c_str())) {}
}

static void CwdCallback(const FunctionCallbackInfo<Value>& args) {
	args.GetReturnValue().Set(String::NewFromUtf8(args.GetIsolate(), get_current_dir_name()));
}

Local<ObjectTemplate> createShell(Isolate* isolate) {
	Local<ObjectTemplate> shell = ObjectTemplate::New(isolate);

	shell->Set(
		String::NewFromUtf8(isolate, "name"),
		String::NewFromUtf8(isolate, SHELL_NAME)
		);

	shell->Set(
		String::NewFromUtf8(isolate, "version"),
		String::NewFromUtf8(isolate, SHELL_VERSION)
		);

	shell->Set(
		String::NewFromUtf8(isolate, "require"),
		FunctionTemplate::New(isolate, RequireCallback)
		);


	shell->Set(
		String::NewFromUtf8(isolate, "exit"),
		FunctionTemplate::New(isolate, ExitCallback)
		);

	shell->Set(
		String::NewFromUtf8(isolate, "cwd"),
      	FunctionTemplate::New(isolate, CwdCallback)
		);

    // shell->Set(
    //   String::NewFromUtf8(isolate, "send"),
    //   FunctionTemplate::New(isolate, ResponseSendCallback)
    //   );

    shell->Set(
      String::NewFromUtf8(isolate, "readyState"),
      String::NewFromUtf8(isolate, "none")
      );
	
	return shell;
}

Local<ObjectTemplate> createConsole(Isolate* isolate) {
	Local<ObjectTemplate> console = ObjectTemplate::New(isolate);

	console->Set(
		String::NewFromUtf8(isolate, "log"),
		FunctionTemplate::New(isolate, LogCallback)
	);

	console->Set(
		String::NewFromUtf8(isolate, "error"),
		FunctionTemplate::New(isolate, ErrorCallback)
	);

	return console;
}

const int old_memory_size = 20000 * sizeof(void*)/4;

void GlobalGetter(Local<String> property, const PropertyCallbackInfo<Value>& info) {
	info.GetReturnValue().Set(info.GetIsolate()->GetCurrentContext()->Global());
}

int main(int argc, char * argv[]) {
	// Initialize V8.
	V8::InitializeICU();
	Platform* platform = platform::CreateDefaultPlatform();
	V8::InitializePlatform(platform);
	V8::Initialize();
	

	v8::Isolate::CreateParams params;
	// i::Heap::kMaxOldSpaceSizeLowMemoryDevice
	params.constraints.set_max_old_space_size(old_memory_size);

	int exit_code = 0;

	// Create a new Isolate and make it the current one.
	Isolate* isolate = Isolate::New(params);
	{
		Isolate::Scope isolate_scope(isolate);
		// Create a stack-allocated handle scope.
		HandleScope handle_scope(isolate);

		Local<ObjectTemplate> shell = createShell(isolate);
		Local<ObjectTemplate> console = createConsole(isolate);
		
		shell->Set(
			String::NewFromUtf8(isolate, "console"),
			console
			);

		shell->SetAccessor(String::NewFromUtf8(isolate, "shell"), GlobalGetter, NULL);







		// Create a new context.
		Local<Context> context = Context::New(isolate, NULL, shell);

		// Enter the context for compiling and running the hello world script.
		Context::Scope context_scope(context);

		Blank::Env env(isolate);

		int i;
		int priv = 0;
		bool crop = false;
		char * arg;
		std::queue<std::string> files;
	  	Local<Array> shell_argv = Array::New(isolate, 0);
		
		// Parse argv
		for (i = 1; i < argc; i++) {
			if (! crop) {
				std::string arg(argv[i]);

				if(arg.compare("--") == 0) {
					crop = true;
				} else if(arg.compare("-") == 0) {
					priv = files.size();
				} else if(arg[0] == '-' && arg[1] == '-') {
		            arg = arg.substr(2, arg.length());
		            
		            size_t pos = arg.find("=");
		            if(pos == std::string::npos) {
						env.config[arg] = "";
		            } else {
		            	env.config[arg.substr(0, pos)] = arg.substr(pos + 1);
		            }
		        } else {
		        	files.push(arg);
		        }

			} else {
				shell_argv->Set(i-crop-1, String::NewFromUtf8(isolate, argv[i])); // Remove ???
			}
		}

		
		Ticker * ticker = new Ticker(&env);
		env.AddPlugin("ticker", ticker);

		
		Blank::Net * net = NULL;
		if (env.config.find("port") != env.config.end()) {
			net = new Blank::Net(&env);
			env.AddPlugin("net", net);
			
			Blank::Net::ExtendShellTemplate(isolate, shell);
			net->SetPort(atoi(env.config.at("port").c_str()));

			printf("Port number: %i\n", net->GetPort());
		}
		// TODO Add plugin instance there
		env.Init();


		// Local<ObjectTemplate> global = ObjectTemplate::New(isolate);


  		// We will be creating temporary handles so we use a handle scope.
	  	// EscapableHandleScope handle_scope(isolate);



	  	// if (shell_argv.IsEmpty())
	  	// 	// TODO Report error
	  	// 	return 1;
	  	
	  	Local<Object> shell_instance = context->Global()->Get(String::NewFromUtf8(isolate, "shell"))->ToObject();

		shell_instance->Set(
			String::NewFromUtf8(isolate, "argv"),
			shell_argv
			);

		// handle_scope.Escape(array);

		TryCatch try_catch;

		shell->Set(
			String::NewFromUtf8(isolate, "readyState"),
			String::NewFromUtf8(isolate, "loading")
		);

		while(! files.empty()) {
			if((priv--) == 0) {
				// TODO Run unprivileged mode
				shell_instance->Delete(String::NewFromUtf8(isolate, "require"));	
			}

			std::string file = files.front();
			files.pop();

			if (! require(isolate, (char*) file.c_str())) {
				std::cerr << "error: could not read " << file << std::endl;
				return 1;
			}
		}
		
		shell->Set(
	     	String::NewFromUtf8(isolate, "readyState"),
	     	String::NewFromUtf8(isolate, "interactive")
	    	);

		if (! isatty(STDIN_FILENO)) {
			std::cin >> std::noskipws;

			std::istream_iterator<char> it(std::cin);
			std::istream_iterator<char> end;
			std::string results(it, end);

			ShellScript script;
			script.filename = (char*) "<stdin>";
			script.source = results;

		    if (exec(isolate, &script) != 0) {
		    	return 1;
		    }		    
		}

		shell->Set(
	      String::NewFromUtf8(isolate, "readyState"),
	      String::NewFromUtf8(isolate, "complete")
	      );

		// char addr[] = "/tmp/blank.sock";
		// listenSocket(isolate, (char *) addr);

		while (! env.IsStopped()) {
			env.Tick();
		}

		if (env.HasError()) {
			exit_code = 1;
			std::cerr << "Error: " << env.GetError() << std::endl;
		}

		env.Exit();
		delete ticker;
		
		if (net != NULL) {
			delete net;
		}
	}

	// Dispose the isolate and tear down V8.
	isolate->Dispose();
	V8::Dispose();
	V8::ShutdownPlatform();
	delete platform;
	return exit_code;
}