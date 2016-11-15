#include <vector>
#include "include/v8.h"
#include "plugins/ticker.h"
#include <sys/time.h>
#include <unistd.h>


typedef v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> PersistentTrait;
struct timer_s {
	uint32_t id;
	bool repeat;
	long long expires;
	uint32_t delay;
	PersistentTrait callback;
};

std::vector<timer_s> callbacks;

uint32_t id = 0;


long long getunixtime() {
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return (long long) tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

void pushTimer(timer_s timer) {
	if (callbacks.size() < 1) {
		callbacks.push_back(timer);
		return;
	}

	for (auto it = callbacks.begin(); it != callbacks.end(); ++it) {
		if (timer.expires < it->expires) {
			callbacks.insert(it, timer);
			return;
		}
	}

	callbacks.push_back(timer);
}

void SetTimeoutCallback(const v8::FunctionCallbackInfo<Value> &args) {
	v8::Isolate *isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[0]);
	PersistentTrait callback_trait(isolate, cb);
	uint32_t delay;

	if (args.Length() > 1) {
		delay = args[1]->Uint32Value();
	} else {
		delay = 1;
	}

	timer_s timer = {
		++id,
		false,
		getunixtime() + (long long) delay,
		delay,
		callback_trait
	};

	pushTimer(timer);
	args.GetReturnValue().Set(v8::Integer::New(isolate, timer.id));
}

void SetIntervalCallback(const v8::FunctionCallbackInfo<Value> &args) {
	v8::Isolate *isolate = args.GetIsolate();
	v8::HandleScope scope(isolate);

	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[0]);
	PersistentTrait callback_trait(isolate, cb);
	uint32_t delay;

	if (args.Length() > 1) {
		delay = args[1]->Uint32Value();
	} else {
		delay = 1;
	}

	timer_s timer = {
		++id,
		true,
		getunixtime() + (long long) delay,
		delay,
		callback_trait
	};

	pushTimer(timer);

	args.GetReturnValue().Set(v8::Integer::New(isolate, timer.id));
}

void ClearTimerCallback(const v8::FunctionCallbackInfo<Value> &args) {
	if (args.Length() < 1) return;

	uint32_t id = args[0]->Uint32Value();
	for (auto it = callbacks.begin(); it != callbacks.end(); it++) {
		if (it->id == id) {
			callbacks.erase(it);
			return;
		}
	}
}

void Ticker::OnInit() {
	v8::Isolate * isolate = _env->GetIsolate();

	isolate->GetCurrentContext()->Global()->Set(
		v8::String::NewFromUtf8(isolate, "setTimeout"),
		v8::FunctionTemplate::New(isolate, SetTimeoutCallback)->GetFunction()
	);

	isolate->GetCurrentContext()->Global()->Set(
		v8::String::NewFromUtf8(isolate, "setInterval"),
		v8::FunctionTemplate::New(isolate, SetIntervalCallback)->GetFunction()
	);

	isolate->GetCurrentContext()->Global()->Set(
		v8::String::NewFromUtf8(isolate, "clearTimeout"),
		v8::FunctionTemplate::New(isolate, ClearTimerCallback)->GetFunction()
	);

	isolate->GetCurrentContext()->Global()->Set(
		v8::String::NewFromUtf8(isolate, "clearInterval"),
		v8::FunctionTemplate::New(isolate, ClearTimerCallback)->GetFunction()
	);
}

void Ticker::OnTick() {
	if (callbacks.empty()) {
		Finish();
		return;
	}

	_is_finished = false;


	long long now = getunixtime();
	v8::Isolate * isolate = _env->GetIsolate();
	v8::TryCatch tryCatch(isolate);

	while(! callbacks.empty()){
		timer_s timer = callbacks.front();

		if (timer.expires > now) return;

		callbacks.erase(callbacks.begin());

		if (timer.repeat) {
			timer.expires = now + (long long) timer.delay;
			pushTimer(timer);
		}

		PersistentTrait pers = timer.callback;

		v8::Local<v8::Function> local = v8::Local<v8::Function>::New(isolate, pers);
		// FIXME Replace with binded values
		// v8::Local<v8::Value> argv[0] = {};
		local->Call(isolate->GetCurrentContext()->Global(), 0, {});

		pers.Reset();

		if (tryCatch.HasCaught()) {
			v8::String::Utf8Value error(tryCatch.StackTrace());
			callbacks.clear();
            Finish();
            _env->Error(* error);
            return;
		}
	}

	if (callbacks.size() > 0){
		_is_finished = false;
	} else {
		Finish();
	}
}

// typedef v8::Persistent<v8::Function, v8::CopyablePersistentTraits<v8::Function>> tPersistent;
// std::queue<tPersistent> callbacks;

// static void PersistentCallback(const v8::FunctionCallbackInfo<Value> &args) {
// 	Isolate *isolate = args.GetIsolate();
// 	HandleScope scope(isolate);
// 	v8::Local<v8::Function> cb = v8::Local<v8::Function>::Cast(args[0]);
// 	tPersistent value(isolate, cb);
// 	callbacks.push(value);
// }

// const unsigned argc = 2;
		
// while(!callbacks.empty()){
// 	tPersistent pers = callbacks.front();
// 	callbacks.pop();

// 	v8::Local<v8::Function> local = v8::Local<v8::Function>::New(isolate, pers);
// 	Local<Value> argv[argc] = { Null(isolate), String::NewFromUtf8(isolate, "success") };
// 	local->Call(isolate->GetCurrentContext()->Global(), argc, argv);

// 	if (try_catch.HasCaught()) {
// 		String::Utf8Value error(try_catch.StackTrace());
// 		printf("%s\n", *error);
// 		return 1;
// 	}

// 	pers.Reset();
// }