#include "plugin.h"
#include "v8.h"

namespace Blank {
	class Main: public Plugin {
	protected:
		bool is_started = false;
		v8::Local<v8::ObjectTemplate> shell;
	public:
		void OnInit();
		void OnTick();
	};
}