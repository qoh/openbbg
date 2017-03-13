#pragma once

// OpenBBG
#include <OpenBBG/Config.h>


namespace openbbg {

typedef struct Game Game;
typedef struct Module Module;
typedef struct ModuleDef ModuleDef;
typedef struct DependencyDesc DependencyDesc;
typedef struct DependencyRelationship DependencyRelationship;

#define DECLARE_MODULE(ModuleClass) \
	static Module::ModuleDef *s_moduleDefinition; \
	static ModuleClass *s_moduleInstance;

#define IMPLEMENT_MODULE(ModuleClass, Phase, IsEnabled) \
	ModuleClass *ModuleClass::s_moduleInstance = new ModuleClass(); \
	Module::ModuleDef *ModuleClass::s_moduleDefinition = new Module::ModuleDef(#ModuleClass, ModuleClass::s_moduleInstance, Module::Phase, IsEnabled);

#define MODULE_DEPENDENCY(ModuleClass, DependencyClass, IsStrict) \
	bool __t##ModuleClass##DependencyClass = ModuleClass::s_moduleDefinition->AddDependency(#DependencyClass, IsStrict);


struct DependencyDesc
{
	string dependee;
	string dependency;
	bool strict;
};

struct Module
{
	typedef enum LoadingPhase {
		Phase_Unknown,
		Phase_Startup
	} LoadingPhase;
	
	struct ModuleDef
	{
		string name;
		Module *instance;
		LoadingPhase loadingPhase;
		bool isEnabled;
		bool isLoaded;
		deque<DependencyDesc> dependencies;
		
		ModuleDef(string name, Module *instance, LoadingPhase phase, bool isEnabled)
			: instance(instance)
			, isEnabled(isEnabled)
			, isLoaded(false)
			, loadingPhase(phase)
			, name(name)
		{
			s_moduleMap[phase].push_back(this);
			assert(s_modulesByName.count(name) == 0);
			s_modulesByName[name] = this;
		}
		~ModuleDef()
		{
			deque<ModuleDef *> &q = s_moduleMap[loadingPhase];
			q.erase(remove(q.begin(), q.end(), this));
			s_modulesByName.erase(name);
		}
		inline bool AddDependency(string moduleName, bool isStrict)
		{
			dependencies.push_back({ name, moduleName, isStrict });
			return true;
		}
	};

	struct DependencyRelationship
	{
		ModuleDef *source, *target;
	};

	virtual bool ModuleInit(Game *game) = 0;
	virtual void ModuleCleanup(Game *game) = 0;

	Module()
	{
	}

	virtual ~Module()
	{
	};
	
	static map<LoadingPhase, deque<ModuleDef *> > s_moduleMap, s_modulesByOrder;
	static map<Module::ModuleDef *, deque<DependencyDesc> > s_moduleDependencyMap;
	static map<string, ModuleDef *> s_modulesByName;
	static void PhaseInit(Game *game, LoadingPhase phase);
	static void PhaseCleanup(Game *game, LoadingPhase phase);
};

}