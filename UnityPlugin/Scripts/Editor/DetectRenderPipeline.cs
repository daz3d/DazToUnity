using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;

#if UNITY_EDITOR
//[InitializeOnLoad]
public static class DetectRenderPipeline
{

	public static readonly string[] usingRPSymbols = new string[] {
			 "USING_HDRP",
			 "USING_URP",
			 "USING_BUILTIN",
			 "USING_2019",
		 };

	static string CleanPreprocessorDirectives(string definedSymbolsString)
    {
		List<string> allDefinedSymbols = definedSymbolsString.Split(';').ToList();

		foreach (string removeSymbol in usingRPSymbols)
		{
			if (allDefinedSymbols.Contains(removeSymbol))
			{
				allDefinedSymbols.Remove(removeSymbol);
				if (!allDefinedSymbols.Contains(removeSymbol))
                {
//					Debug.Log("Defined Symbol removed: " + removeSymbol);
				}
				else
                {
//					Debug.LogError("Defined Symbol could not be removed: " + removeSymbol);
				}
			}
			else
            {
//				Debug.Log("Defined Symbol not detected: " + removeSymbol);
			}
		}

		return string.Join(";", allDefinedSymbols.ToArray());
	}

	static string DetectAndSetSymbolString(string definedSymbols)
    {
		string newSymbolString = "";

		if (definedSymbols != "")
		{
			newSymbolString = definedSymbols + ";";
		}
        if (IsHDRPInstalled())
        {
			newSymbolString += "USING_HDRP";
        }
        else if (IsURPInstalled())
        {
			newSymbolString += "USING_URP";
			// check for 2019
			if (Application.unityVersion.Contains("2019"))
            {
				newSymbolString += ";USING_2019";
            }
		}
		else
        {
			newSymbolString += "USING_BUILTIN";
		}

		return newSymbolString;
	}

	//[UnityEditor.Callbacks.DidReloadScripts]
	static void CommitDefinedSymbols(string newSymbolsString)
    {
		Debug.Log("Attempting to write new PreprocessorDirectives: " + newSymbolsString);
		PlayerSettings.SetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup, newSymbolsString);
	}

	static bool IsHDRPInstalled()
    {
		if (UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline != null)
		{
			string renderAssetType = "dummy string";
			renderAssetType = UnityEngine.Rendering.GraphicsSettings.renderPipelineAsset.GetType().ToString();
			if (renderAssetType.Contains("HDRenderPipeline"))
			{
				return true;
			}
		}

		return false;
    }

	static bool IsURPInstalled()
	{
		if (UnityEngine.Rendering.GraphicsSettings.currentRenderPipeline != null)
		{
			string renderAssetType = "dummy string";
			renderAssetType = UnityEngine.Rendering.GraphicsSettings.renderPipelineAsset.GetType().ToString();
			if (renderAssetType.Contains("UniversalRenderPipeline"))
			{
				return true;
			}
		}

		return false;
	}

	public static void RunOnce()
	{
		string oldSymbolsString = PlayerSettings.GetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup);

		string newSymbolsString = oldSymbolsString;

		newSymbolsString = CleanPreprocessorDirectives(newSymbolsString);
		newSymbolsString = DetectAndSetSymbolString(newSymbolsString);

		string renderPipelineString = "Built-In Pipeline";
		if (IsHDRPInstalled())
			renderPipelineString = "HDRP";
		else if (IsURPInstalled())
			renderPipelineString = "URP";

//		Debug.Log("OldString=" + oldSymbolsString + ", NewString=" + newSymbolsString);
		if (oldSymbolsString != newSymbolsString)
        {
			// DB: 2021-08-31, showmodal to ask to change defined symbols, warn that it will take time (minutes)
			// 1. DISPLAY DETECTED RP, ASK TO COMMIT, WARN OPERATION WILL TAKE TIME (MINUTES)
			// 2. IF YES, DO COMMIT
			// 3. IF NO, RESET VALUES SO DETECTION CAN BE RUN LATER...
			// OPTIONAL: IMPORT ASSETS WITH DEFAULT SHADER?
			Daz3D.Daz3DBridge.CurrentToolbarMode = Daz3D.Daz3DBridge.ToolbarMode.Options;
			string dtu_detectrp_message = "Detected [" + renderPipelineString + "]\n\nDTU Bridge must update symbol definitions to continue the Import Procedure.  This may take a few minutes.  " +
				"You may Cancel now, and rerun the renderpipeline detection process from the DTU Bridge at another time.";
			bool bUpdateSymbols = EditorUtility.DisplayDialog("RenderPipeline Detection", dtu_detectrp_message, "Update Symbol Definitions Now", "Cancel and Redetect RenderPipeline Later");
			if (bUpdateSymbols)
            {
				Daz3D.Daz3DBridge.CurrentToolbarMode = Daz3D.Daz3DBridge.ToolbarMode.History;
				CommitDefinedSymbols(newSymbolsString);
				Daz3D.Daz3DDTUImporter.ImportEventRecord record = new Daz3D.Daz3DDTUImporter.ImportEventRecord();
				Daz3D.Daz3DDTUImporter.EventQueue.Enqueue(record);
				record.AddToken("Updating Symbol Definitions.\nThis will trigger Unity Editor to recompile all scripts and may take several minutes...");
			}
			else
            {
				Daz3D.Daz3DDTUImporter.ImportEventRecord record = new Daz3D.Daz3DDTUImporter.ImportEventRecord();
				Daz3D.Daz3DDTUImporter.EventQueue.Enqueue(record);
				record.AddToken("Autodetection cancelled.\nPlease click the \"Detect RenderPipeline\" button \nin the Options tab to continue import procedure.");
			}

		}
        else
        {
			string dtu_detectrp_message = "Detected [" + renderPipelineString + "]\n\nNo changes need to be made to Symbol Definitions.";
			EditorUtility.DisplayDialog("RenderPipeline Detection", dtu_detectrp_message, "OK");
		}

	}
}
#endif // UNITY_EDITOR