#pragma once
#ifdef UNITTEST_DZBRIDGE

#include <QObject>
#include "UnitTest.h"

class UnitTest_DzUnityDialog : public UnitTest {
	Q_OBJECT
public:
	UnitTest_DzUnityDialog();
	bool runUnitTests();

private:
	bool _DzBridgeUnityDialog(UnitTest::TestResult* testResult);
	bool getAssetsFolderEdit(UnitTest::TestResult* testResult);
	bool resetToDefaults(UnitTest::TestResult* testResult);
	bool loadSavedSettings(UnitTest::TestResult* testResult);
	bool HandleSelectAssetsFolderButton(UnitTest::TestResult* testResult);
	bool HandleInstallUnityFilesCheckBoxChange(UnitTest::TestResult* testResult);
	bool HandleAssetTypeComboChange(UnitTest::TestResult* testResult);
	bool HandleAssetFolderChanged(UnitTest::TestResult* testResult);

};


#endif