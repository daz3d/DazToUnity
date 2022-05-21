#pragma once
#ifdef UNITTEST_DZBRIDGE

#include <QObject>
#include <UnitTest.h>

class UnitTest_DzUnityAction : public UnitTest {
	Q_OBJECT
public:
	UnitTest_DzUnityAction();
	bool runUnitTests();

private:
	bool _DzBridgeUnityAction(UnitTest::TestResult* testResult);
	bool setInstallUnityFiles(UnitTest::TestResult* testResult);
	bool getInstallUnityFiles(UnitTest::TestResult* testResult);
	bool executeAction(UnitTest::TestResult* testResult);
	bool createUI(UnitTest::TestResult* testResult);
	bool writeConfiguration(UnitTest::TestResult* testResult);
	bool setExportOptions(UnitTest::TestResult* testResult);
	bool createUnityFiles(UnitTest::TestResult* testResult);
	bool readGuiRootFolder(UnitTest::TestResult* testResult);

};

#endif