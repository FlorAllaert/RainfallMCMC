#ifndef AGENT
#define AGENT

#include "chain.hpp"
#include "options.hpp"

class agent {
public:
    agent() {}
    virtual ~agent() { }
    virtual int status() const { return 0; }
    virtual void invoke(chain *c, options *o) { }
};

// the types of the class factories
typedef agent* spawn_agent();
typedef void clean_agent(agent*);

// Registration macro - to be included at the end of all agent files
#define REGISTERAGENT(AGENTNAME) \
	extern "C" agent* spawn() { return new AGENTNAME; } \
	extern "C" void clean(agent* a) { delete a; }

#endif