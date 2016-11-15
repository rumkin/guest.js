#ifndef ENV_H
#define ENV_H

#include <map>
#include <vector>
#include <string>
#include <functional>
#include "include/v8.h"
#include "script.h"

namespace Blank {
	class Plugin;
	class Env;

	typedef std::map<std::string,Plugin*> Plugins;
	typedef std::function<int(Env*)> EventCallback;
	typedef std::map<std::string,std::vector<EventCallback>> EventCallbacks;
	typedef std::map<std::string,std::string> Config;

	class Env {
	protected:
		v8::Isolate* isolate;
		
		EventCallbacks events = EventCallbacks{};
		Plugins plugins;

		bool isStopped = false;
		int exitCode;
		bool hasError = false;
		std::string error;
	public:
		Env(v8::Isolate* isolate) {
			this->isolate = isolate;
		};
		v8::Isolate* GetIsolate() {
			return isolate;
		};

		Config config = {};
		
		void Stop(int);
		void Stop();
		bool IsStopped();
		void Error(std::string);
		bool HasError();
		std::string GetError();
		void AddPlugin(std::string, Plugin * plugin);
		void AddPlugin(std::string, std::function<Plugin * (Env *)>);
		bool HasPlugin(std::string);
		void Init();
		void Tick();
		void Exit();
		// static std::map<std::string,std::function<void(Env*)>> plugins;
		// static void AddPlugin(std::string, std::function<void(Env*)>);
		// void InitPlugin(std::string);
		size_t On(std::string, EventCallback);
		void Off(std::string, size_t);
		void Off(std::string);
		void Fire(std::string);
		int execScript(ShellScript*);
	};
}

#endif