#include "pyplug.h"

#include "debugging/debugging.h"

#include "iplugin.h"

#include "string/string.h"
#include "modulesystem/singletonmodule.h"

#include "iundo.h"       // declaration of undo system
#include "ientity.h"     // declaration of entity system
#include "iscenegraph.h" // declaration of datastructure of the map

#include "scenelib.h"  // declaration of datastructure of the map
#include "qerplugin.h" // declaration to use other interfaces as a plugin

#include "generic/vector.h"

void about_plugin_window();

class PyPlugPluginDependencies : public GlobalRadiantModuleRef,    // basic class for all other module refs
                                 public GlobalUndoModuleRef,       // used to say radiant that something has changed and to undo that
                                 public GlobalSceneGraphModuleRef, // necessary to handle data in the mapfile (change, retrieve data)
                                 public GlobalEntityModuleRef      // to access and modify the entities
{
public:
    PyPlugPluginDependencies() : GlobalEntityModuleRef(GlobalRadiant().getRequiredGameDescriptionKeyValue("entities"))
    {
    }
};

//  *************************
namespace PyPlug
{
    QWidget *main_window;

    const char *init(void *hApp, void *pMainWidget)
    {
        main_window = static_cast<QWidget *>(pMainWidget);
        return "Initializing PyPlug";
    }
    const char *getName()
    {
        return "PyPlug"; // name that is shown in the menue
    }
    const char *getCommandList()
    {
        return "About"; // ;zfefz
    }
    const char *getCommandTitleList()
    {
        return "";
    }
    void dispatch(const char *command, float *vMin, float *vMax, bool bSingleBrush)
    {
        if (string_equal(command, "About"))
        {
            GlobalRadiant().m_pfnMessageBox(PyPlug::main_window, "PyPlug", "About PyPlug", EMessageBoxType::Info, 0);
        }
    }
}

class PyPlugModule : public TypeSystemRef
{
    _QERPluginTable m_plugin;

public:
    typedef _QERPluginTable Type;
    STRING_CONSTANT(Name, "PyPlug");

    PyPlugModule()
    {
        m_plugin.m_pfnQERPlug_Init = &PyPlug::init;
        m_plugin.m_pfnQERPlug_GetName = &PyPlug::getName;
        m_plugin.m_pfnQERPlug_GetCommandList = &PyPlug::getCommandList;
        m_plugin.m_pfnQERPlug_GetCommandTitleList = &PyPlug::getCommandTitleList;
        m_plugin.m_pfnQERPlug_Dispatch = &PyPlug::dispatch;
    }
    _QERPluginTable *getTable()
    {
        return &m_plugin;
    }
};

typedef SingletonModule<PyPlugModule, PyPlugPluginDependencies> SingletonPyPlugModule;

SingletonPyPlugModule g_PyPlugModule;

extern "C" void RADIANT_DLLEXPORT Radiant_RegisterModules(ModuleServer &server)
{
    initialiseModule(server);

    g_PyPlugModule.selfRegister();
}