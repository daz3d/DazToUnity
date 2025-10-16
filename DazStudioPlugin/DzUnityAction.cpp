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
#include "dzexporter.h"
#include "dzexportmgr.h"

#include "DzUnityAction.h"
#include "DzUnityDialog.h"
#include "DzBridgeMorphSelectionDialog.h"
#include "DzBridgeSubdivisionDialog.h"

#ifdef WIN32
#include <shellapi.h>
#endif

#include "dzbridge.h"

DzUnityAction::DzUnityAction() :
	DzBridgeAction(tr("Send to &Unity..."), tr("Send the selected node to Unity."))
{
	this->setObjectName("DzBridge_DazToUnity_Action");

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
	m_nExecuteActionResult = DZ_OPERATION_FAILED_ERROR;
	m_eSelectedNodeAssetType = DZ_BRIDGE_NAMESPACE::EAssetType::None;

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

	if (m_nNonInteractiveMode != DZ_BRIDGE_NAMESPACE::eNonInteractiveMode::DzExporterMode) {
		m_eSelectedNodeAssetType = SelectBestRootNodeForTransfer(true);
		m_pSelectedNode = dzScene->getPrimarySelection();
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
			m_MorphNamesToExport.clear();
			foreach(QString morphName, m_aMorphListOverride)
			{
				QString label = MorphTools::GetMorphLabelFromName(morphName, m_pSelectedNode);
				m_MorphNamesToExport.append(morphName);
			}
		}
		else
		{
			m_bEnableMorphs = false;
			m_sMorphSelectionRule = "";
			m_MorphNamesToExport.clear();
		}

	}

	if (m_nNonInteractiveMode != DZ_BRIDGE_NAMESPACE::eNonInteractiveMode::DzExporterMode) {
		m_bridgeDialog->setEAssetType(m_eSelectedNodeAssetType);
	}

	// If the Accept button was pressed, start the export
	int dlgResult = -1;
	if (m_nNonInteractiveMode == 0)
	{
		dlgResult = m_bridgeDialog->exec();
	}
	if (m_nNonInteractiveMode == 1 || dlgResult == QDialog::Accepted)
	{

		// Read GUI values
		if (readGui(m_bridgeDialog) == false)
		{
			m_nExecuteActionResult = DZ_OPERATION_FAILED_ERROR;
			return;
		}

		// DB 2021-10-11: Progress Bar
		DzProgress* exportProgress = new DzProgress("Sending to Unity...", 10, false, true);

		DzError result = doPromptableObjectBaking();
		if (result != DZ_NO_ERROR) {
			exportProgress->finish();
			exportProgress->cancel();
			m_nExecuteActionResult = result;
			return;
		}
		exportProgress->step();

		//Create Daz3D folder if it doesn't exist
		QDir dir;
		dir.mkpath(m_sRootFolder);
		exportProgress->step();

		if (m_sAssetType == "Environment") {
			// Sanity Check if zero nodes
			if (dzScene->getNumNodes() == 0) {
				dzApp->log("DazToBlender: CRITICAL ERROR: executeAction() Environment Export with zero nodes. Aborting.");
				exportProgress->finish();
				exportProgress->cancel();
				m_nExecuteActionResult = DZ_OPERATION_FAILED_ERROR;
				return;
			}

			QDir().mkdir(m_sDestinationPath);
			m_pSelectedNode = dzScene->getPrimarySelection();

			exportProgress->step();
			DzNodeList rootNodeList = BuildRootNodeList();
			if (rootNodeList.isEmpty()) {
				exportProgress->finish();
				exportProgress->cancel();
				m_nExecuteActionResult = DZ_OPERATION_FAILED_ERROR;
				return;
			}
			m_pSelectedNode = rootNodeList[0];
			preProcessScene(NULL);

			DzExportMgr* ExportManager = dzApp->getExportMgr();
			DzExporter* Exporter = ExportManager->findExporterByClassName("DzFbxExporter");
			DzFileIOSettings ExportOptions;
			ExportOptions.setBoolValue("IncludeSelectedOnly", false);
			ExportOptions.setBoolValue("IncludeVisibleOnly", true);
			ExportOptions.setBoolValue("IncludeFigures", true);
			ExportOptions.setBoolValue("IncludeProps", true);
			ExportOptions.setBoolValue("IncludeLights", false);
			ExportOptions.setBoolValue("IncludeCameras", false);
			ExportOptions.setBoolValue("IncludeAnimations", true);
			ExportOptions.setIntValue("RunSilent", !m_bShowFbxOptions);
			setExportOptions(ExportOptions);
			// NOTE: be careful to use m_sExportFbx and NOT m_sExportFilename since FBX and DTU base name may differ
			QString sEnvironmentFbx = m_sDestinationPath + m_sExportFbx + ".fbx";
			DzError result = Exporter->writeFile(sEnvironmentFbx, &ExportOptions);
			if (result != DZ_NO_ERROR) {
				undoPreProcessScene();
				m_nExecuteActionResult = result;
				exportProgress->finish();
				exportProgress->cancel();
				return;
			}
			exportProgress->step();

			writeConfiguration();
			exportProgress->step();

			undoPreProcessScene();
			exportProgress->step();

		}
		else
		{
			DzNode* pParentNode = NULL;
			if (m_pSelectedNode->isRootNode() == false) {
				dzApp->log("INFO: Selected Node for Export is not a Root Node, unparenting now....");
				pParentNode = m_pSelectedNode->getNodeParent();
				pParentNode->removeNodeChild(m_pSelectedNode, true);
				dzApp->log("INFO: Parent stored: " + pParentNode->getLabel() + ", New Root Node: " + m_pSelectedNode->getLabel());
			}
			exportProgress->step();
			exportHD(exportProgress);
			exportProgress->step();
			if (pParentNode) {
				dzApp->log("INFO: Restoring Parent relationship: " + pParentNode->getLabel() + ", child node: " + m_pSelectedNode->getLabel());
				pParentNode->addNodeChild(m_pSelectedNode, true);
			}
		}

		exportProgress->update(10);
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

		// DB 2021-10-11: Progress Bar
		exportProgress->finish();

	}

	m_nExecuteActionResult = DZ_NO_ERROR;

}

QString DzUnityAction::createUnityFiles(bool replace)
{
	if (!m_bInstallUnityFiles)
		return "";

	QString destinationFolder = m_sRootFolder + "/Support";
	QDir dir;
	dir.mkpath(destinationFolder);

	QString srcPathHDRP = ":/DazBridgeUnity/2019-hdrp.unitypackage";
	QFile srcFileHDRP(srcPathHDRP);
	QString destPathHDRP = destinationFolder + "/DazToUnity HDRP.unitypackage";
	this->copyFile(&srcFileHDRP, &destPathHDRP, replace);
	srcFileHDRP.close();

	QString srcPathURP = ":/DazBridgeUnity/2019-urp.unitypackage";
	QFile srcFileURP(srcPathURP);
	QString destPathURP = destinationFolder + "/DazToUnity URP.unitypackage";
	this->copyFile(&srcFileURP, &destPathURP, replace);
	srcFileURP.close();

	QString srcPathStandard = ":/DazBridgeUnity/2019-builtin.unitypackage";
	QFile srcFileStandard(srcPathStandard);
	QString destPathStandard = destinationFolder + "/DazToUnity Standard Shader.unitypackage";
	this->copyFile(&srcFileStandard, &destPathStandard, replace);
	srcFileStandard.close();


	return destPathHDRP;
}

void DzUnityAction::writeConfiguration()
{
	DzProgress* pDtuProgress = new DzProgress("Writing DTU file", 10, false, true);

	QString DTUfilename = m_sDestinationPath + m_sExportFilename + ".dtu";
	QFile DTUfile(DTUfilename);
	if (!DTUfile.open(QIODevice::WriteOnly)) {
		QString sErrorMessage = tr("ERROR: DzBridge: writeConfigureation(): unable to open file for writing: ") + DTUfilename;
		dzApp->log(sErrorMessage);
		return;
	}
	DzJsonWriter writer(&DTUfile);
	writer.startObject(true);

	writeDTUHeader(writer);

	if (m_sAssetType.toLower().contains("mesh") || m_sAssetType == "Animation")
	{
		writeAllMaterials(m_pSelectedNode, writer);
		writeAllMorphs(writer);

		// DB, 2022-June-17: Daz To Unified Bridge Format support
		writeMorphLinks(writer);
		writeMorphNames(writer);
		DzBoneList aBoneList = getAllBones(m_pSelectedNode);
		writeSkeletonData(m_pSelectedNode, writer);
		writeHeadTailData(m_pSelectedNode, writer);
		writeJointOrientation(aBoneList, writer);
		writeLimitData(aBoneList, writer);
		writePoseData(m_pSelectedNode, writer, true);

		writeAllSubdivisions(writer);
		writeAllDforceInfo(m_pSelectedNode, writer);
	}

	if (m_sAssetType == "Pose")
	{
		writeAllPoses(writer);
	}

	if (m_sAssetType == "Environment")
	{
#define DZ_UNITY_TEMPORARY_ENV_EXPORT_WORKAROUND 1
#if DZ_UNITY_TEMPORARY_ENV_EXPORT_WORKAROUND
		QTextStream* pCVSStream = nullptr;
		if (m_bExportMaterialPropertiesCSV)
		{
			QString filename = m_sDestinationPath + m_sExportFilename + "_Maps.csv";
			QFile file(filename);
			file.open(QIODevice::WriteOnly);
			pCVSStream = new QTextStream(&file);
			*pCVSStream << "Version, Object, Material, Type, Color, Opacity, File" << endl;
	}
		pDtuProgress->update(6);
		if (m_sAssetType == "Environment") {
			writeSceneMaterials(writer, pCVSStream);
			pDtuProgress->step();
			writeSceneDefinition(writer);
		}
		else {
			writeAllMaterials(m_pSelectedNode, writer, pCVSStream);
			pDtuProgress->step();
		}

		writeAllMorphs(writer);
		writeMorphLinks(writer);
		writeMorphNames(writer);
		pDtuProgress->step();

		DzBoneList aBoneList = getAllBones(m_pSelectedNode);

		writeSkeletonData(m_pSelectedNode, writer);
		writeHeadTailData(m_pSelectedNode, writer);
		writeJointOrientation(aBoneList, writer);
		writeLimitData(aBoneList, writer);
		writePoseData(m_pSelectedNode, writer, true);
		pDtuProgress->step();

		writeAllSubdivisions(writer);
		pDtuProgress->step();
		writeAllDforceInfo(m_pSelectedNode, writer);
		pDtuProgress->step();

#else
		writeEnvironment(writer);
#endif
	}

	//m_ImageToolsJobsManager->processJobs();
	//m_ImageToolsJobsManager->clearJobs();

	writer.finishObject();
	DTUfile.close();

	pDtuProgress->finish();

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

bool DzUnityAction::readGui(DZ_BRIDGE_NAMESPACE::DzBridgeDialog* pBridgeDialog)
{
	bool bResult = DzBridgeAction::readGui(pBridgeDialog);
	if (!bResult)
	{
		return false;
	}

	// Read Custom GUI values
	DzUnityDialog* unityDialog = qobject_cast<DzUnityDialog*>(pBridgeDialog);
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

	return true;
}




#include "moc_DzUnityAction.cpp"
