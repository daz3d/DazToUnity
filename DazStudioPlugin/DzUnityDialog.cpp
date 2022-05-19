#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QToolTip>
#include <QtGui/QWhatsThis>
#include <QtGui/qlineedit.h>
#include <QtGui/qboxlayout.h>
#include <QtGui/qfiledialog.h>
#include <QtCore/qsettings.h>
#include <QtGui/qformlayout.h>
#include <QtGui/qcombobox.h>
#include <QtGui/qdesktopservices.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qlistwidget.h>
#include <QtGui/qgroupbox.h>

#include "dzapp.h"
#include "dzscene.h"
#include "dzstyle.h"
#include "dzmainwindow.h"
#include "dzactionmgr.h"
#include "dzaction.h"
#include "dzskeleton.h"
#include "qstandarditemmodel.h"

#include "DzUnityDialog.h"
#include "DzBridgeMorphSelectionDialog.h"
#include "DzBridgeSubdivisionDialog.h"

#include "version.h"

/*****************************
Local definitions
*****************************/
#define DAZ_TO_UNITY_PLUGIN_NAME "DazToUnity"

#include "dzbridge.h"

DzUnityDialog::DzUnityDialog(QWidget* parent) :
	 DzBridgeDialog(parent, DAZ_TO_UNITY_PLUGIN_NAME)
{
	 projectEdit = nullptr;
	 projectButton = nullptr;
	 assetsFolderEdit = nullptr;
	 assetsFolderButton = nullptr;
	 installUnityFilesCheckBox = nullptr;

	 settings = new QSettings("Daz 3D", "DazToUnity");

	 // Declarations
	 int margin = style()->pixelMetric(DZ_PM_GeneralMargin);
	 int wgtHeight = style()->pixelMetric(DZ_PM_ButtonHeight);
	 int btnMinWidth = style()->pixelMetric(DZ_PM_ButtonMinWidth);

	 // Set the dialog title
	 int revision = PLUGIN_REV % 1000;
#ifdef _DEBUG
	 setWindowTitle(tr("DazToUnity Bridge v%1.%2 Pre-Release Build %3.%4").arg(PLUGIN_MAJOR).arg(PLUGIN_MINOR).arg(revision).arg(PLUGIN_BUILD));
#else
	 setWindowTitle(tr("DazToUnity Bridge v%1.%2").arg(PLUGIN_MAJOR).arg(PLUGIN_MINOR));
#endif

	 QStandardItemModel* model = qobject_cast<QStandardItemModel*>(assetTypeCombo->model());
	 QStandardItem* item = nullptr;
	 item = model->findItems("Environment").first();
	 if (item) item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
	 item = model->findItems("Pose").first();
	 if (item) item->setFlags(item->flags() & ~Qt::ItemIsEnabled);

	 // Connect new asset type handler
	 connect(assetTypeCombo, SIGNAL(activated(int)), this, SLOT(HandleAssetTypeComboChange(int)));

	 // Intermediate Folder
	 QHBoxLayout* assetsFolderLayout = new QHBoxLayout();
	 assetsFolderEdit = new QLineEdit(this);
	 assetsFolderButton = new QPushButton("...", this);
	 assetsFolderLayout->addWidget(assetsFolderEdit);
	 assetsFolderLayout->addWidget(assetsFolderButton);
	 connect(assetsFolderEdit, SIGNAL(textChanged(const QString&)), this, SLOT(HandleAssetFolderChanged(const QString&)));
	 connect(assetsFolderButton, SIGNAL(released()), this, SLOT(HandleSelectAssetsFolderButton()));

	 // Advanced Options
	 installOrOverwriteUnityFilesLabel = new QLabel(tr("Install Unity Files"));
	 installUnityFilesCheckBox = new QCheckBox("", this);
	 connect(installUnityFilesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(HandleInstallUnityFilesCheckBoxChange(int)));

	 // Add the widget to the basic dialog
	 mainLayout->insertRow(0, "Unity Assets Folder", assetsFolderLayout);
	 mainLayout->insertRow(1, installOrOverwriteUnityFilesLabel, installUnityFilesCheckBox);

	 // Make the dialog fit its contents, with a minimum width, and lock it down
	 resize(QSize(500, 0).expandedTo(minimumSizeHint()));
	 setFixedWidth(width());
	 setFixedHeight(height());

	 update();

	 // Help
	 assetNameEdit->setWhatsThis("This is the name the asset will use in Unity.");
	 assetTypeCombo->setWhatsThis("Skeletal Mesh for something with moving parts, like a character\nStatic Mesh for things like props\nAnimation for a character animation.");
	 assetsFolderEdit->setWhatsThis("Unity Assets folder. DazStudio assets will be exported into a subfolder inside this folder.");
	 assetsFolderButton->setWhatsThis("Unity Assets folder. DazStudio assets will be exported into a subfolder inside this folder.");

	 // Set Defaults
	 resetToDefaults();

	 // Load Settings
	 loadSavedSettings();

}

bool DzUnityDialog::loadSavedSettings()
{
	DzBridgeDialog::loadSavedSettings();

	if (!settings->value("AssetsPath").isNull())
	{
		// DB (2021-05-15): check AssetsPath folder and set InstallUnityFiles if Daz3D subfolder does not exist
		QString directoryName = settings->value("AssetsPath").toString();
		assetsFolderEdit->setText(directoryName);
	}
	else
	{
		QString DefaultPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + "DazToUnity";
		assetsFolderEdit->setText(DefaultPath);
	}

	return true;
}

void DzUnityDialog::resetToDefaults()
{
	DzBridgeDialog::resetToDefaults();

	QString DefaultPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + "DazToUnity";
	assetsFolderEdit->setText(DefaultPath);

	DzNode* Selection = dzScene->getPrimarySelection();
	if (dzScene->getFilename().length() > 0)
	{
		QFileInfo fileInfo = QFileInfo(dzScene->getFilename());
		assetNameEdit->setText(fileInfo.baseName().remove(QRegExp("[^A-Za-z0-9_]")));
	}
	else if (dzScene->getPrimarySelection())
	{
		assetNameEdit->setText(Selection->getLabel().remove(QRegExp("[^A-Za-z0-9_]")));
	}

	if (qobject_cast<DzSkeleton*>(Selection))
	{
		assetTypeCombo->setCurrentIndex(0);
	}
	else
	{
		assetTypeCombo->setCurrentIndex(1);
	}

}

void DzUnityDialog::HandleAssetFolderChanged(const QString& directoryName)
{
	// DB (2021-05-15): Check for presence of Daz3D folder, and set installUnityFiles if not present
	if (QDir(directoryName + QDir::separator() + "Daz3D").exists())
	{
		// deselect install unity files
		settings->setValue("InstallUnityFiles", false);
		installUnityFilesCheckBox->setChecked(false);
		// rename label to show "Overwrite"
		installOrOverwriteUnityFilesLabel->setText(tr("Overwrite Unity Files"));
	}
	else
	{
		settings->setValue("InstallUnityFiles", true);
		installUnityFilesCheckBox->setChecked(true);
		// rename label to show "Install"
		installOrOverwriteUnityFilesLabel->setText(tr("Install Unity Files"));
	}

}

void DzUnityDialog::HandleSelectAssetsFolderButton()
{
	 // DB (2021-05-15): prepopulate with existing folder string
	 QString directoryName = "/home";
	 if (!settings->value("AssetsPath").isNull())
	 {
		 directoryName = settings->value("AssetsPath").toString();
	 }
	 directoryName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"),
		  directoryName,
		  QFileDialog::ShowDirsOnly
		  | QFileDialog::DontResolveSymlinks);

	 if (directoryName != NULL)
	 {
		  QDir parentDir = QFileInfo(directoryName).dir();
		  if (!parentDir.exists())
		  {
				QMessageBox::warning(0, tr("Error"), tr("Please select Unity Root Assets Folder."), QMessageBox::Ok);
				return;
		  }
		  else
		  {
				bool found1 = false;
				bool found2 = false;
				QFileInfoList list = parentDir.entryInfoList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);
				for (int i = 0; i < list.size(); i++)
				{
					 QFileInfo file = list[i];
					 if (file.baseName() == QString("ProjectSettings"))
						  found1 = true;
					 if (file.baseName() == QString("Library"))
						  found2 = true;
				}

				if (!found1 || !found2)
				{
					 QMessageBox::warning(0, tr("Error"), tr("Please select Unity Root Assets Folder."), QMessageBox::Ok);
					 return;
				}

				assetsFolderEdit->setText(directoryName);
				settings->setValue("AssetsPath", directoryName);
		  }
	 }
    
}

void DzUnityDialog::HandleInstallUnityFilesCheckBoxChange(int state)
{
	 settings->setValue("InstallUnityFiles", state == Qt::Checked);
}

void DzUnityDialog::HandleAssetTypeComboChange(int state)
{
	QString assetNameString = assetNameEdit->text();

	// enable/disable Morphs and Subdivision only if Skeletal selected
	if (assetTypeCombo->currentText() != "Skeletal Mesh")
	{
		morphsEnabledCheckBox->setChecked(false);
		subdivisionEnabledCheckBox->setChecked(false);
	}

	// if "Animation", change assetname
	if (assetTypeCombo->currentText() == "Animation")
	{
		// check assetname is in @anim[0000] format
		if (!assetNameString.contains("@") || assetNameString.contains(QRegExp("@anim[0-9]*")))
		{
			// extract true assetName and recompose animString
			assetNameString = assetNameString.left(assetNameString.indexOf("@"));
			// get importfolder using corrected assetNameString
			QString importFolderPath = settings->value("AssetsPath").toString() + QDir::separator() + "Daz3D" + QDir::separator() + assetNameString + QDir::separator();

			// create anim filepath
			uint animCounter = 0;
			QString animString = assetNameString + QString("@anim%1").arg(animCounter, 4, 10, QChar('0'));
			QString filePath = importFolderPath + animString + ".fbx";

			// if anim file exists, then increment anim filename counter
			while (QFileInfo(filePath).exists())
			{
				if (++animCounter > 9999)
				{
					break;
				}
				animString = assetNameString + QString("@anim%1").arg(animCounter, 4, 10, QChar('0'));
				filePath = importFolderPath + animString + ".fbx";
			}
			assetNameEdit->setText(animString);
		}

	}
	else
	{
		// remove @anim if present
		if (assetNameString.contains("@")) {
			assetNameString = assetNameString.left(assetNameString.indexOf("@"));
		}
		assetNameEdit->setText(assetNameString);
	}

}

#include "moc_DzUnityDialog.cpp"
