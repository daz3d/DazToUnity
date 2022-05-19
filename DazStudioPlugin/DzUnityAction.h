#pragma once
#include <dzaction.h>
#include <dznode.h>
#include <dzjsonwriter.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <DzBridgeAction.h>
#include "DzUnityDialog.h"

class UnitTest_DzUnityAction;

#include "dzbridge.h"

class DzUnityAction : public DZ_BRIDGE_NAMESPACE::DzBridgeAction {
	 Q_OBJECT
	 Q_PROPERTY(bool InstallUnityFiles READ getInstallUnityFiles WRITE setInstallUnityFiles)
public:
	DzUnityAction();

	void setInstallUnityFiles(bool arg) { m_bInstallUnityFiles = arg; }
	bool getInstallUnityFiles() { return m_bInstallUnityFiles; }

protected:
	 bool m_bInstallUnityFiles;

	 void executeAction();
	 Q_INVOKABLE bool createUI();
	 Q_INVOKABLE void writeConfiguration();
	 Q_INVOKABLE void setExportOptions(DzFileIOSettings& ExportOptions);
	 Q_INVOKABLE QString createUnityFiles(bool replace = true);
	 QString readGuiRootFolder();

#ifdef UNITTEST_DZBRIDGE
	friend class UnitTest_DzUnityAction;
#endif

};
