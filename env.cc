#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include "include/v8.h"
#include "env.h"
#include "plugin.h"


namespace Blank {
	// std::map<std::string,std::function<void(Env*)>> Env::plugins = std::map<std::string,std::function<void(Env*)>>{};

	void Env::Stop() {
		exitCode = 0;
		isStopped = true;
	}

	void Env::Stop(int code = 0) {
		exitCode = code;
		isStopped = true;
	}

	bool Env::IsStopped() {
		return isStopped;
	};

	void Env::Error(std::string message) {
		error = message;
		hasError = true;
		this->Stop(1);

		for (auto it = plugins.begin(); it != plugins.end(); it++) {
			it->second->OnError(message);
		}

		this->Fire("error");
	}

	bool Env::HasError() {
		return hasError;
	};

	std::string Env::GetError() {
		return error;
	}

	void Env::AddPlugin(std::string name, Plugin * plugin) {
		// TODO Something remove, trigger an error?
		// if (plugins.find(name) != plugins.end()) {
		// }

		plugins[name] = plugin;
	}

	void Env::AddPlugin(std::string name, std::function<Plugin* (Env *)> factory) {
		// TODO Something remove, trigger an error?
		// if (plugins.find(name) != plugins.end()) {
		// }

		plugins[name] = factory(this);
	}

	bool Env::HasPlugin(std::string name) {
		return plugins.find(name) != plugins.end();
	}

	void Env::Init() {
		for (auto it = plugins.begin(); it != plugins.end(); it++) {
			it->second->OnInit();
		}
		
		this->Fire("init");
	}

	void Env::Tick() {
		int isFinished = 0;
		for (auto it = plugins.begin(); it != plugins.end(); it++) {
			it->second->OnTick();
			if (it->second->IsFinished()) {
				isFinished++;
			}
		}

		this->Fire("tick");
		
		// Everythink is done
		if (isFinished == plugins.size()) {
			Stop(0);
		}
	}

	void Env::Exit() {
		for (auto it = plugins.begin(); it != plugins.end(); it++) {
			it->second->OnExit(this->exitCode);
		}

		this->Fire("exit");
	}

	// void Env::AddPlugin(std::string name, std::function<void(Env*)> fn) {
	// 	Blank::Env::plugins[name] = fn;
	// }


	// void Env::InitPlugin(std::string name) {
	// 	// TODO Throw an Exception!
	// 	if (plugins.count(name) < 1) return;

	// 	plugins[name](this);
	// }


	// Add event listener
	size_t Env::On(std::string event, std::function<int(Env*)> callback) {

		if (events.count(event) == 0) {
			std::vector<std::function<int(Blank::Env*)>> q;
			events[event] = q;
		}

		events[event].push_back(callback);
		return events[event].size() - 1;
	}

	// Remove event listener at specified position
	void Env::Off(std::string event, size_t i) {
		if (events.count(event) != 0 && events[event].size() > i) {
			events[event].erase(events[event].begin()+i);
		}
	}
	
	// Remove all event listeners for specified event
	void Env::Off(std::string event) {
		if (events.count(event) != 0) {
			events[event].erase(events[event].begin(), events[event].end());
		}
	}

	// Fire event by name
	void Env::Fire(std::string event) {
		if (events.count(event) < 1) return;

		std::vector<std::function<int(Blank::Env*)>> callbacks = events[event];

		int result;

		for (int i = 0; i < callbacks.size(); i++) {
			result = callbacks[i](this);

			if (result != 0) {
				return;
			}
		}
	}

	int Env::execScript(ShellScript* script) {
		return exec(isolate, script);
	}
}