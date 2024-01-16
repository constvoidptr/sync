#include <euroscope/EuroScopePlugIn.h>
#include "plugin.h"

void __declspec (dllexport) EuroScopePlugInInit(EuroScopePlugIn::CPlugIn** ppPlugInInstance) {
    Plugin& plugin = Plugin::instance();
    *ppPlugInInstance = &plugin;
}

void __declspec (dllexport) EuroScopePlugInExit() {
    Plugin::reset();
}
