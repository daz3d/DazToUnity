#include <QtGui/qcheckbox.h>
#include <QtGui/QMessageBox>
#include <QtNetwork/qudpsocket.h>
#include <QtNetwork/qabstractsocket.h>
#include <QCryptographicHash>
#include <QtCore/qdir.h>

#include <dzapp.h>
#include <dzscene.h>
#include <dzmainwindow.h>
#include <dzshape.h>
#include <dzproperty.h>
#include <dzobject.h>
#include <dzpresentation.h>
#include <dznumericproperty.h>
#include <dzimageproperty.h>
#include <dzcolorproperty.h>
#include <dpcimages.h>

#include "QtCore/qmetaobject.h"
#include "dzmodifier.h"
#include "dzgeometry.h"
#include "dzweightmap.h"
#include "dzfacetshape.h"
#include "dzfacetmesh.h"
#include "dzfacegroup.h"
#include "dzprogress.h"

#include "DzUnityAction.h"
#include "DzUnityDialog.h"
#include "DzBridgeMorphSelectionDialog.h"
#include "DzBridgeSubdivisionDialog.h"

#ifdef WIN32
#include <shellapi.h>
#endif

#include "dzbridge.h"

DzUnityAction::DzUnityAction() :
	DzBridgeAction(tr("Daz To &Unity"), tr("Send the selected node to Unity."))
{
	m_nNonInteractiveMode = 0;
	m_sAssetType = QString("SkeletalMesh");
	//Setup Icon
	QString iconName = "icon";
	QPixmap basePixmap = QPixmap::fromImage(getEmbeddedImage(iconName.toLatin1()));
	QIcon icon;
	icon.addPixmap(basePixmap, QIcon::Normal, QIcon::Off);
	QAction::setIcon(icon);

}

bool DzUnityAction::createUI()
{
	// Check if the main window has been created yet.
	// If it hasn't, alert the user and exit early.
	DzMainWindow* mw = dzApp->getInterface();
	if (!mw)
	{
		if (m_nNonInteractiveMode == 0) QMessageBox::warning(0, tr("Error"),
			tr("The main window has not been created yet."), QMessageBox::Ok);

		return false;
	}

	// m_subdivisionDialog creation REQUIRES valid Character or Prop selected
	if (dzScene->getNumSelectedNodes() != 1)
	{
		if (m_nNonInteractiveMode == 0) QMessageBox::warning(0, tr("Error"),
			tr("Please select one Character or Prop to send."), QMessageBox::Ok);

		return false;
	}

	 // Create the dialog
	if (!m_bridgeDialog)
	{
		m_bridgeDialog = new DzUnityDialog(mw);
	}
	else
	{
		DzUnityDialog* unityDialog = qobject_cast<DzUnityDialog*>(m_bridgeDialog);
		if (unityDialog)
		{
			unityDialog->resetToDefaults();
			unityDialog->loadSavedSettings();
		}
	}

	if (!m_subdivisionDialog) m_subdivisionDialog = DZ_BRIDGE_NAMESPACE::DzBridgeSubdivisionDialog::Get(m_bridgeDialog);
	if (!m_morphSelectionDialog) m_morphSelectionDialog = DZ_BRIDGE_NAMESPACE::DzBridgeMorphSelectionDialog::Get(m_bridgeDialog);

	return true;
}

void DzUnityAction::executeAction()
{
	// CreateUI() disabled for debugging -- 2022-Feb-25
	/*
		 // Create and show the dialog. If the user cancels, exit early,
		 // otherwise continue on and do the thing that required modal
		 // input from the user.
		 if (createUI() == false)
			 return;
	*/

	// Check if the main window has been created yet.
	// If it hasn't, alert the user and exit early.
	DzMainWindow* mw = dzApp->getInterface();
	if (!mw)
	{
		if (m_nNonInteractiveMode == 0)
		{
			QMessageBox::warning(0, tr("Error"),
				tr("The main window has not been created yet."), QMessageBox::Ok);
		}
		return;
	}

	// Create and show the dialog. If the user cancels, exit early,
	// otherwise continue on and do the thing that required modal
	// input from the user.
	if (dzScene->getNumSelectedNodes() != 1)
	{
		if (m_nNonInteractiveMode == 0)
		{
			QMessageBox::warning(0, tr("Error"),
				tr("Please select one Character or Prop to send."), QMessageBox::Ok);
		}
		return;
	}

	// Create the dialog
	if (m_bridgeDialog == nullptr)
	{
		m_bridgeDialog = new DzUnityDialog(mw);
	}
	else
	{
		if (m_nNonInteractiveMode == 0)
		{
			m_bridgeDialog->resetToDefaults();
			m_bridgeDialog->loadSavedSettings();
		}
	}

	// Prepare member variables when not using GUI
	if (m_nNonInteractiveMode == 1)
	{
//		if (m_sRootFolder != "") m_bridgeDialog->getIntermediateFolderEdit()->setText(m_sRootFolder);

		if (m_aMorphListOverride.isEmpty() == false)
		{
			m_bEnableMorphs = true;
			m_sMorphSelectionRule = m_aMorphListOverride.join("\n1\n");
			m_sMorphSelectionRule += "\n1\n.CTRLVS\n2\nAnything\n0";
			if (m_morphSelectionDialog == nullptr)
			{
				m_morphSelectionDialog = DZ_BRIDGE_NAMESPACE::DzBridgeMorphSelectionDialog::Get(m_bridgeDialog);
			}
			m_mMorphNameToLabel.clear();
			foreach(QString morphName, m_aMorphListOverride)
			{
				QString label = m_morphSelectionDialog->GetMorphLabelFromName(morphName);
				m_mMorphNameToLabel.insert(morphName, label);
			}
		}
		else
		{
			m_bEnableMorphs = false;
			m_sMorphSelectionRule = "";
			m_mMorphNameToLabel.clear();
		}

	}

	// If the Accept button was pressed, start the export
	int dlgResult = -1;
	if (m_nNonInteractiveMode == 0)
	{
		dlgResult = m_bridgeDialog->exec();
	}
	if (m_nNonInteractiveMode == 1 || dlgResult == QDialog::Accepted)
	{
		// DB 2021-10-11: Progress Bar
		DzProgress* exportProgress = new DzProgress("Sending to Unity...", 10);

		// Read Common GUI values
		readGui(m_bridgeDialog);

		// Read Custom GUI values
		DzUnityDialog* unityDialog = qobject_cast<DzUnityDialog*>(m_bridgeDialog);
		if (unityDialog)
			m_bInstallUnityFiles = unityDialog->installUnityFilesCheckBox->isChecked();
		// custom animation filename correction for Unity
		if (m_sAssetType == "Animation")
		{
			if (m_nNonInteractiveMode == 0)
			{
				// correct CharacterFolder
				m_sExportSubfolder = m_sAssetName.left(m_sAssetName.indexOf("@"));
				m_sDestinationPath = m_sRootFolder + "/" + m_sExportSubfolder + "/";
				// correct animation filename
				m_sDestinationFBX = m_sDestinationPath + m_sAssetName + ".fbx";
			}
		}

		//Create Daz3D folder if it doesn't exist
		QDir dir;
		dir.mkpath(m_sRootFolder);
		exportProgress->step();

		exportHD(exportProgress);

		// DB 2021-10-11: Progress Bar
		exportProgress->finish();

		// DB 2021-09-02: messagebox "Export Complete"
		if (m_nNonInteractiveMode == 0)
		{
			if (m_bInstallUnityFiles)
			{
				QMessageBox::information(0, "Daz To Unity Bridge",
					tr("Export phase from Daz Studio complete. Please switch to Unity to continue.\n\n\
If Unity Import dialog does not appear, then please double-click the \"DazToUnity HDRP\" UnityPackage \
file located in the Assets\\Daz3D\\Support\\ folder of your Unity Project."), QMessageBox::Ok);
				QString destPath = createUnityFiles(true);
#ifdef WIN32
				ShellExecute(0, 0, destPath.toLocal8Bit().data(), 0, 0, SW_SHOW);
#endif
			}
			else
			{
				QMessageBox::information(0, "Daz To Unity Bridge",
					tr("Export phase from Daz Studio complete. Please switch to Unity to begin Import phase."), QMessageBox::Ok);
			}
		}

	}
}

QString DzUnityAction::createUnityFiles(bool replace)
{
	if (!m_bInstallUnityFiles)
		return "";

	QString destinationFolder = m_sRootFolder + "/Support";
	QDir dir;
	dir.mkpath(destinationFolder);

	QString srcPathHDRP = ":/DazBridgeUnity/daztounity-hdrp.unitypackage";
	QFile srcFileHDRP(srcPathHDRP);
	QString destPathHDRP = destinationFolder + "/DazToUnity HDRP.unitypackage";
	this->copyFile(&srcFileHDRP, &destPathHDRP, replace);
	srcFileHDRP.close();

	QString srcPathURP = ":/DazBridgeUnity/daztounity-urp.unitypackage";
	QFile srcFileURP(srcPathURP);
	QString destPathURP = destinationFolder + "/DazToUnity URP.unitypackage";
	this->copyFile(&srcFileURP, &destPathURP, replace);
	srcFileURP.close();

	QString srcPathStandard = ":/DazBridgeUnity/daztounity-standard-shader.unitypackage";
	QFile srcFileStandard(srcPathStandard);
	QString destPathStandard = destinationFolder + "/DazToUnity Standard Shader.unitypackage";
	this->copyFile(&srcFileStandard, &destPathStandard, replace);
	srcFileStandard.close();


	return destPathHDRP;
}

void DzUnityAction::writeConfiguration()
{
	QString DTUfilename = m_sDestinationPath + m_sAssetName + ".dtu";
	QFile DTUfile(DTUfilename);
	DTUfile.open(QIODevice::WriteOnly);
	DzJsonWriter writer(&DTUfile);
	writer.startObject(true);

	writeDTUHeader(writer);

	if (m_sAssetType.toLower().contains("mesh"))
	{
		writeAllMaterials(m_pSelectedNode, writer);
		writeAllMorphs(writer);
		writeAllSubdivisions(writer);
		writeAllDforceInfo(m_pSelectedNode, writer);
	}

	if (m_sAssetType == "Pose")
	{
		writeAllPoses(writer);
	}

	if (m_sAssetType == "Environment")
	{
		writeEnvironment(writer);
	}

	writer.finishObject();
	DTUfile.close();
}

// Setup custom FBX export options
void DzUnityAction::setExportOptions(DzFileIOSettings& ExportOptions)
{
	ExportOptions.setBoolValue("doEmbed", false);
	ExportOptions.setBoolValue("doDiffuseOpacity", false);
	ExportOptions.setBoolValue("doCopyTextures", false);

}

QString DzUnityAction::readGuiRootFolder()
{
	QString rootFolder = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + QDir::separator() + "DazToUnity";

	if (m_bridgeDialog)
	{
		QLineEdit* assetsFolderEdit = nullptr;
		DzUnityDialog* unityDialog = qobject_cast<DzUnityDialog*>(m_bridgeDialog);

		if (unityDialog)
			assetsFolderEdit = unityDialog->getAssetsFolderEdit();

		if (assetsFolderEdit)
			rootFolder = assetsFolderEdit->text().replace("\\", "/") + "/Daz3D";
	}
	return rootFolder;
}

#include "moc_DzUnityAction.cpp"
