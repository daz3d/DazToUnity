#pragma once
#include "dzbasicdialog.h"
#include <QtGui/qcombobox.h>
#include <QtCore/qsettings.h>
#include <DzBridgeDialog.h>

class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QWidget;
class DzUnityAction;

class UnitTest_DzUnityDialog;

#include "dzbridge.h"

class DzUnityDialog : public DZ_BRIDGE_NAMESPACE::DzBridgeDialog{
	friend DzUnityAction;
	Q_OBJECT
	Q_PROPERTY(QWidget* assetsFolderEdit READ getAssetsFolderEdit)
public:
	Q_INVOKABLE QLineEdit* getAssetsFolderEdit() { return assetsFolderEdit; }

	/** Constructor **/
	 DzUnityDialog(QWidget *parent=nullptr);

	/** Destructor **/
	virtual ~DzUnityDialog() {}

	Q_INVOKABLE void resetToDefaults() override;
	Q_INVOKABLE bool loadSavedSettings() override;

protected slots:
	void HandleSelectAssetsFolderButton();
	void HandleInstallUnityFilesCheckBoxChange(int state);
	void HandleAssetTypeComboChange(int state);
	void HandleAssetFolderChanged(const QString& directoryName);

protected:
	QLineEdit* projectEdit;
	QPushButton* projectButton;
	QLineEdit* assetsFolderEdit;
	QPushButton* assetsFolderButton;

	QLabel* installOrOverwriteUnityFilesLabel;
	QCheckBox* installUnityFilesCheckBox;

#ifdef UNITTEST_DZBRIDGE
	friend class UnitTest_DzUnityDialog;
#endif
};
