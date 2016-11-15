#ifndef	TICKER_H
#define TICKER_H

#include "plugin.h"

void PersistentCallback();

class Ticker : public Blank::Plugin {
public:
	using Blank::Plugin::Plugin;
	
	void OnInit();
	void OnTick();
};

#endif