#include "stdafx.h"

// OpenBBG
#include <OpenBBG/Module.h>
#include <OpenBBG/Game.h>

// Standard Library
#include <cstdio>

namespace openbbg {

map<Module::LoadingPhase, deque<Module::ModuleDef *> > Module::s_moduleMap = map<Module::LoadingPhase, deque<Module::ModuleDef *> >();
map<Module::LoadingPhase, deque<Module::ModuleDef *> > Module::s_modulesByOrder = map<Module::LoadingPhase, deque<Module::ModuleDef *> >();
map<Module::ModuleDef *, deque<DependencyDesc> > Module::s_moduleDependencyMap = map<Module::ModuleDef *, deque<DependencyDesc> >();
map<string, Module::ModuleDef *> Module::s_modulesByName = map<string, Module::ModuleDef *>();

void Module::PhaseInit(Game *game, LoadingPhase phase)
{
	if (s_moduleMap.count(phase) == 0)
		return;
	map<ModuleDef *, uint16_t> dependencyCounter;

	// Gather list of modules and build dependency map
	{
		for (auto m : Module::s_moduleMap[phase]) {
			for (auto d : m->dependencies) {
				assert(s_modulesByName.count(d.dependency) != 0);
				ModuleDef *dependencyDef = Module::s_modulesByName[d.dependency];
				s_moduleDependencyMap[dependencyDef].push_back(d);
			}
			dependencyCounter[m] = (uint16_t)m->dependencies.size();
		}
	}

	// Load modules
	queue<ModuleDef *> modulesToLoad;
	for (auto m : dependencyCounter)
		if (m.second == 0)
			modulesToLoad.push(m.first);

	while (modulesToLoad.size() != 0) {
		ModuleDef *def = modulesToLoad.front();
		s_modulesByOrder[phase].push_back(def);
		modulesToLoad.pop();
		
		// Attempt to load module
		if (def->isEnabled) {
			LOG_DEBUG("Init: {}", def->name);
			def->isLoaded = def->instance->ModuleInit(game);
		}

		// Resolve dependencies
		// TODO: Account for phase
		if (s_moduleDependencyMap.count(def))
			for (auto desc : s_moduleDependencyMap[def]) {
				ModuleDef *m = s_modulesByName[desc.dependee];
				if ((def->isEnabled && def->isLoaded) || (def->isLoaded == false && desc.strict == false))
					if (--dependencyCounter[m] == 0)
						modulesToLoad.push(m);
			}
	}
}

void Module::PhaseCleanup(Game *game, LoadingPhase phase)
{
	if (s_moduleMap.count(phase) == 0)
		return;
	// TODO: Reverse order cleanup
	auto it = s_modulesByOrder[phase].rbegin();
	auto end = s_modulesByOrder[phase].rend();
	while (it != end) {
		const ModuleDef *def = *it;
		if (def->isLoaded) {
			LOG_DEBUG("Cleanup: {}", def->name);
			def->instance->ModuleCleanup(game);
		}
		++it;
	}
}

}