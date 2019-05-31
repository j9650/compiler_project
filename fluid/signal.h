#pragma once
#include <string>


class Signal {
	std::string sig;
public:
	Signal(std::string _sig) {
		sig = _sig;	
	}
	std::string & msg() {
		return sig;
	}
};

class InternalSignal : public Signal {


};

class ExternalSignal : public Signal {


};
