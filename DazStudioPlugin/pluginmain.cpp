#include "dzplugin.h"
#include "dzapp.h"

#include "version.h"
#include "DzUnityAction.h"
#include "DzUnityDialog.h"

#include "dzbridge.h"

CPP_PLUGIN_DEFINITION("Daz To Unity Bridge");

DZ_PLUGIN_AUTHOR("Daz 3D, Inc");

DZ_PLUGIN_VERSION(PLUGIN_MAJOR, PLUGIN_MINOR, PLUGIN_REV, PLUGIN_BUILD);

#ifdef _DEBUG
DZ_PLUGIN_DESCRIPTION(QString(
	"<b>Pre-Release DazToUnity Bridge v%1.%2.%3.%4 </b><br>\
<a href = \"https://github.com/daz3d/DazToUnity\">Github</a><br><br>"
).arg(PLUGIN_MAJOR).arg(PLUGIN_MINOR).arg(PLUGIN_REV).arg(PLUGIN_BUILD));
#else
DZ_PLUGIN_DESCRIPTION(QString(
"This plugin provides the ability to send assets to Unity. \
Documentation and source code are available on <a href = \"https://github.com/daz3d/DazToUnity\">Github</a>.<br>"
));
#endif

DZ_PLUGIN_CLASS_GUID(DzUnityAction, 2C2AA695-652C-4FA9-BE48-E0AB954E28AB);
NEW_PLUGIN_CUSTOM_CLASS_GUID(DzUnityDialog, 06cf5776-8e81-4a81-bad8-619ed1205b58);

#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityAction.h"
#include "UnitTest_DzUnityDialog.h"

DZ_PLUGIN_CLASS_GUID(UnitTest_DzUnityAction, 17637434-188f-46eb-81e2-8829f2440742);
DZ_PLUGIN_CLASS_GUID(UnitTest_DzUnityDialog, ca9c9f54-236d-4ab6-bca3-1cf6c3f93f6a);

#endif