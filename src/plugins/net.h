#ifndef NET_PLUGIN_H
#define NET_PLUGIN_H

#include "plugin.h"
#include "include/poe.h"

namespace Blank {
	class Net: public Plugin {
		protected:
			Poe::Server * _server;
			int _port = 0;
		public:
			using Plugin::Plugin;
			void OnInit();
			void OnTick();
			void SetPort(int);
			int GetPort();
			static void ExtendShellTemplate(v8::Isolate *, v8::Local<v8::ObjectTemplate>);
	};
}

#endif