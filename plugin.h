#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>
#include "env.h"

namespace Blank {
	class Plugin {
	protected:
		Env * _env;
		bool _is_finished = false;
		void Finish() {
			_is_finished = true;
		}
		// TODO Add GetShell method
	public:
		Plugin();
		Plugin(Env * env) {
			_env = env;
		};
		
		virtual bool IsFinished() {
			return _is_finished;
		};

		virtual ~Plugin() {};
		virtual void OnInit() {};
		virtual void OnTick() {};
		virtual void OnError(std::string message) {};
		virtual void OnExit(int code) {};
	};
}

#endif