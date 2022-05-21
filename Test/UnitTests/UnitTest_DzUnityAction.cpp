#ifdef UNITTEST_DZBRIDGE

#include "UnitTest_DzUnityAction.h"
#include "DzUnityAction.h"


UnitTest_DzUnityAction::UnitTest_DzUnityAction()
{
	m_testObject = (QObject*) new DzUnityAction();
}

bool UnitTest_DzUnityAction::runUnitTests()
{
	RUNTEST(_DzBridgeUnityAction);
	RUNTEST(setInstallUnityFiles);
	RUNTEST(getInstallUnityFiles);
	RUNTEST(executeAction);
	RUNTEST(createUI);
	RUNTEST(writeConfiguration);
	RUNTEST(setExportOptions);
	RUNTEST(createUnityFiles);
	RUNTEST(readGuiRootFolder);

	return true;
}

bool UnitTest_DzUnityAction::_DzBridgeUnityAction(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(new DzUnityAction());
	return bResult;
}

bool UnitTest_DzUnityAction::setInstallUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->setInstallUnityFiles(false));
	return bResult;
}

bool UnitTest_DzUnityAction::getInstallUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->getInstallUnityFiles());
	return bResult;
}

bool UnitTest_DzUnityAction::executeAction(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->executeAction());
	return bResult;
}

bool UnitTest_DzUnityAction::createUI(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->createUI());
	return bResult;
}

bool UnitTest_DzUnityAction::writeConfiguration(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->writeConfiguration());
	return bResult;
}

bool UnitTest_DzUnityAction::setExportOptions(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	DzFileIOSettings arg;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->setExportOptions(arg));
	return bResult;
}

bool UnitTest_DzUnityAction::createUnityFiles(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->createUnityFiles());
	return bResult;
}

bool UnitTest_DzUnityAction::readGuiRootFolder(UnitTest::TestResult* testResult)
{
	bool bResult = true;
	TRY_METHODCALL(qobject_cast<DzUnityAction*>(m_testObject)->readGuiRootFolder());
	return bResult;
}


#include "moc_UnitTest_DzUnityAction.cpp"

#endif