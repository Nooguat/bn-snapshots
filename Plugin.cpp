#include <binaryninjaapi.h>
#include "Plugin.h"

#ifdef UI_BUILD
#include "binaryninja-api/ui/uitypes.h"
#include "UI/snap_viewer.h"
#endif

extern "C" {

BN_DECLARE_CORE_ABI_VERSION

#ifdef UI_BUILD
BN_DECLARE_UI_ABI_VERSION
#endif

BINARYNINJAPLUGIN bool UIPluginInit() {
    SnapNotif::init();
    return true;
}

}
