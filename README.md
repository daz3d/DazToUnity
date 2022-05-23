# DazToUnity
A Daz Studio Plugin based on Daz Bridge Library, allowing transfer of Daz Studio characters and props to the Unity game engine.

# Table of Contents
1. About the Bridge
2. How to Install
3. How to Use
4. How to Build
5. How to QA Test
6. How to Develop


# 1. About the Bridge
This is a refactored version of the original DazToUnity Bridge using the Daz Bridge Library as a foundation. Using the Bridge Library allows it to share source code and features with other bridges such as the refactored DazToUnreal and DazToBlender bridges. This will improve development time and quality of all bridges.

The DazToUnity Bridge consists of two parts: a Daz Studio plugin which exports assets to a Unity Project and a Unity Package which contains shaders, scripts and other resources to help recreate the look of the original Daz Studio asset in the Unity game engine.


# 2. How to Install
### Daz Studio Plugin ###
After it's built you can copy the dzunitybridge.dll into the plugins folder of Daz Studio (example: "\Daz 3D\Applications\64-bit\DAZ 3D\DAZStudio4\plugins"). Daz Studio can then be started, and the bridge can be accessed from the main menu: File->Send To->Daz To Unity. The embedded Unity Packages for HDRP, URP and Built-in RenderPipelines can be installed with the "Install Unity Files" option from the export dialog window.  

### Unity Plugin ###
By default, the HDRP installer will be launched in Unity.  If it does not launch automatically or if you are using URP or Built-in Renderpipelines, then:
1. Go to your Unity Editor, open the Project Panel and navigate to the `Assets\Daz3D\Support` folder.
2. Inside that folder, you will find installation packages for all render-pipelines:
   - For HDRP: double-click "DazToUnity HDRP.unitypackage" and click Import.
   - For URP: double-click "DazToUnity URP.unitypackage" and click Import.
   - For Built-In Render-pipline: double-click "DazToUnity Standard Shader.unitypackage" and click Import.
3. If a popup window asks you to Update the Scripts or API, then click "Yes, for these and other all files".
4. For HDRP, you will also need to add a diffusion profile: Unity 2019: This list is found in the Material section of each HD RenderPipeline Asset, which can be found in the Quality->HDRP panel of the Project Settings dialog. Unity 2020 and above: This list is found at the bottom of the HDRP Default Settings panel in the Project Settings dialog.


# 3. How to Use
1. Start Daz Studio, add a Figure or Prop to your scene.
2. Select the top-most node for your Figure or Prop in the Scene Pane.
3. Select from the main menu: File->Send To->Daz to Unity.
4. Select the Asset Folder for your project.
5. Select the desired Asset Type. Tip: for Figures, select "Skeletal Mesh".  For Animation, select "Animation".  For all others, select "Static Mesh".
6. To enable Morphs or Subdivision levels, click the CheckBox to Enable that option, then click the "Choose Morphs" or "Choose Subdivisions" button to configure your selections.
7. Click Accept, then wait for a dialog popup to notify you when to switch to the Unity Window.


# 4. How to Build
Requirements: Daz Studio 4.5+ SDK, Qt 4.8.1, Autodesk Fbx SDK, Pixar OpenSubdiv Library, CMake, C++ development environment

Download or clone the DazToUnity github repository to your local machine. The Daz Bridge Library is linked as a git submodule to the DazToBridge repository. Depending on your git client, you may have to use `git submodule init` and `git submodule update` to properly clone the Daz Bridge Library.

Use CMake to configure the project files. Daz Bridge Library will be automatically configured to static-link with DazToUnity. If using the CMake gui, you will be prompted for folder paths to dependencies: Daz SDK, Qt 4.8.1, Fbx SDK and OpenSubdiv during the Configure process.


# 5. How to QA Test
The Test folder contains a `QA Manual Test Cases.md` document with instructions for performaing manual tests.  The Test folder also contains subfolders for UnitTests, TestCases and Results. To run automated Test Cases, run Daz Studio and load the `Test/testcases/test_runner.dsa` script, configure the sIncludePath on line 4, then execute the script. Results will be written to report files stored in the `Test/Reports` subfolder.

To run UnitTests, you must first build special Debug versions of the DzBridge-Unity and DzBridge Static sub-projects with Visual Studio configured for C++ Code Generation: Enable C++ Exceptions: Yes with SEH Exceptions (/EHa). This enables the memory exception handling features which are used during null pointer argument tests of the UnitTests. Once the special Debug version of DazToUnity dll is built and installed, run Daz Studio and load the `Test/UnitTests/RunUnitTests.dsa` script. Configure the sIncludePath and sOutputPath on lines 4 and 5, then execute the script. Several UI dialog prompts will appear on screen as part of the UnitTests of their related functions. Just click OK or Cancel to advance through them. Results will be written to report files stored in the `Test/Reports` subfolder.

For more information on running QA test scripts and writing your own test scripts, please refer to `How To Use QA Test Scripts.md` and `QA Script Documentation and Examples.dsa` which are located in the Daz Bridge Library repository: https://github.com/daz3d/DazBridgeUtils.

Special Note: The QA Report Files generated by the UnitTest and TestCase scripts have been designed and formatted so that the QA Reports will only change when there is a change in a test result.  This allows Github to conveniently track the history of test results with source-code changes, and allows developers and QA testers to notified by Github or their git client when there are any changes and the exact test that changed its result.

# 6. How to Modify and Develop
The Daz Studio Plugin source code is contained in the `DazStudioPlugin` folder. The Unity Package source code and resources are available in a separate github repository. Modifications to the Unity Package files can be embedded in the Daz Studio Plugin by exporting a .UnityPackage file from inside Unity. Then copying the updated UnityPackage to the `DazStudioPlugin/Resources` folder and replacing the existing .UnityPackage file there.

The DazToUnity Bridge uses a branch of the Daz Bridge Library which is modified to use the "DzUnityNS" namespace. This ensures that there are no C++ Namespace collisions when other plugins based on the Daz Bridge Library are also loaded in Daz Studio. In order to link and share C++ classes between this plugin and the Daz Bridge Library, a custom `CPP_PLUGIN_DEFINITION()` macro is used instead of the standard DZ_PLUGIN_DEFINITION macro and usual .DEF (DzUnityBridge.def) file. NOTE: Use of the DZ_PLUGIN_DEFINITION macro and DEF file use will disable C++ class export in the Visual Studio compiler.
