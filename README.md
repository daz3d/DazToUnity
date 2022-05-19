# dzBridge-Unity
A renewed DazToUnity Bridge based on Daz Bridge Library, allowing transfer of Daz Studio characters and props to the Unity game engine.

# Table of Contents
1. About the Bridge
2. How to Install
3. How to Build
4. How to QA Test
5. How to Develop


# 1. About the Bridge
This is a refactored version of the original DazToUnity Bridge using the Daz Bridge Library as a foundation. Using the Bridge Library allows it to share source code and features with other bridges such as the refactored DazToUnreal and DazToBlender bridges. This will improve development time and quality of all bridges.

The DazToUnity Bridge consists of two parts: a Daz Studio plugin which exports assets to a Unity Project and a Unity Package which contains shaders, scripts and other resources to help recreate the look of the original Daz Studio asset in the Unity game engine.


# 2. How to Install
After it's built you can copy the dzunitybridge.dll into the plugins folder of Daz Studio (example: "\Daz 3D\Applications\64-bit\DAZ 3D\DAZStudio4\plugins"). Daz Studio can then be started, and the bridge can be accessed from the main menu: File->Send To->Daz To Unity. The embedded Unity Package can be installed with the "Install Unity Files" option from the export dialog window.


# 3. How to Build
Requirements: Daz Studio 4.5+ SDK, Qt 4.8.1, Autodesk Fbx SDK, Pixar OpenSubdiv Library, CMake, C++ development environment

Download or clone the DazToUnity github repository to your local machine. The Daz Bridge Library is linked as a git submodule to the DazToBridge repository. Depending on your git client, you may have to use `git submodule init` and `git submodule update` to properly clone the Daz Bridge Library.

Use CMake to configure the project files. Daz Bridge Library will be automatically configured to static-link with DazToUnity. If using the CMake gui, you will be prompted for folder paths to dependencies: Daz SDK, Qt 4.8.1, Fbx SDK and OpenSubdiv during the Configure process.


# 4. How to QA Test
The Test folder contains a `QA Manual Test Cases.md` document with instructions for performaing manual tests.  The Test folder also contains subfolders for UnitTests, TestCases and Results. To run automated Test Cases, run Daz Studio and load the `Test/testcases/test_runner.dsa` script, configure the sIncludePath on line 4, then execute the script. Results will be written to report files stored in the `Test/Reports` subfolder.

To run UnitTests, you must first build special Debug versions of the DzBridge-Unity and DzBridge Static sub-projects with Visual Studio configured for C++ Code Generation: Enable C++ Exceptions: Yes with SEH Exceptions (/EHa). This enables the memory exception handling features which are used during null pointer argument tests of the UnitTests. Once the special Debug version of DazToUnity dll is built and installed, run Daz Studio and load the `Test/UnitTests/RunUnitTests.dsa` script. Configure the sIncludePath and sOutputPath on lines 4 and 5, then execute the script. Several UI dialog prompts will appear on screen as part of the UnitTests of their related functions. Just click OK or Cancel to advance through them. Results will be written to report files stored in the `Test/Reports` subfolder.


# 5. How to Modify and Develop
The Daz Studio Plugin source code is contained in the `DazStudioPlugin` folder. The Unity Package source code and resources are available in a separate github repository. Modifications to the Unity Package files can be embedded in the Daz Studio Plugin by exporting a .UnityPackage file from inside Unity. Then copying the updated UnityPackage to the `DazStudioPlugin/Resources` folder and replacing the existing .UnityPackage file there.

The DazToUnity Bridge uses a branch of the Daz Bridge Library which is modified to use the "DzUnityNS" namespace. This ensures that there are no C++ Namespace collisions when other plugins based on the Daz Bridge Library are also loaded in Daz Studio. In order to link and share C++ classes between this plugin and the Daz Bridge Library, a custom `CPP_PLUGIN_DEFINITION()` macro is used instead of the standard DZ_PLUGIN_DEFINITION macro and usual .DEF (DzUnityBridge.def) file. NOTE: Use of the DZ_PLUGIN_DEFINITION macro and DEF file use will disable C++ class export in the Visual Studio compiler.
