Daz To Unity Change Log:
=================================
Version 2022.2.4.8:
- Bugfixed URP shader code
- Updated to latest Daz Bridge Library v2.2
- Fixed morph undo bug (distorted faces on export)
- Improved Geograft material support
- Preliminary steps for Geograft Morph support (exports blendshapes)

Version 2022.1.3.7:
- New version numbering based on Year-Revision-Bugfix
- Updated and integrated latest updates to Daz Bridge Library
- Improved Morph and Subdivision Selection dialogs
- Improved Unity Plugin Installer
- DTU updates to support future add-ons such as auto-JCM

Version 2.0:
- Many uDTU features now merged with official Daz To Unity bridge, including:
- Mac support,
- Animation support,
- Improved HDRP Shaders,
- New URP Shader support,
- Built-In Standard Shader support,
- Genesis 8.1 and PBRSkin support,
- Improved Subdivision support,
- Improved Emission support.

Version 1.3 alpha 4:
- Fixed assembly definition for Scripts/Editor folder.  Projects should now properly build.
- Fixed error when importing with dForce Support enabled.

Version 1.3 alpha 3:
- You will need to delete any existing DTU plugin from "Assets/Daz3D" folder.  You can save your previous exported assets.
- UseNewShaders is now enabled by default.
- Added improved Hair shaders for HDRP and URP.
- Alpha-value Bugfixes to Hair shaders: OOT, OmUberSurface.

Version 1.3 alpha 1:
- The Unity Package Source Code is now separated from the Daz Studio Plugin Source Code Repository.

Version 1.2:
- Bugfixed crash when exporting subdivisions for more than one figure in a single session.
- Initial refactoring for planned open-source FBX and OpenSubdiv Daz-plugins.
- Updated CMakeFiles for more convenient building of FBX/OpenSubdiv/Mac support.
- Full MacOS support on versions 10.9 to 11.
- (Baked) Subdivision support, up to Subdivision Level 4.  Based on https://github.com/cocktailboy/DazToRuntime implementation.
- FBX SDK and OpenSubDiv are static linked into the Unofficial DTU Bridge and requires both libraries to build the Daz Plugin.
- OpenSubDiv is used under a Modified Apache License 2.0: https://github.com/PixarAnimationStudios/OpenSubdiv/blob/release/LICENSE.txt
- FBX SDK is used under the Autodesk® FBX® SDK 2020 license: https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2

Version 1.1:
- MacOS filesystem support.
- Experimental uDTU shaders added for HDRP and URP, made from refactored and unified shadersubgraph codebase.
   - "Use New Shaders" option added to DTU Bridge Options panel (disabled by default).
   - Translucency Map, SSS, Dual Lobe Specular, Glossy Specular, Specular Strength, Top Coat implemented.
   - URP Transparency support via URP-Transparent shading mode.
   - Dual Lobe Specular and Glossy Specular simultaneously supported in all shading modes (SSS, Metallic, Specular, URP-Transparent).
   - Metallic emulation implemented in Specular and URP-Transparent shading modes.
   - SSS supported for all non-transparent materials (previously only Skin).
- Fixed Alpha Clip Threshold bug in URP: affected depth-testing, especially hair.
- Glossy Anisotropy, Roughness and Weight fixes.
- "eyelash" material assigned to Hair shader.

Version 1.0:
- Bugfix: Imported asset files with different hash values are appropriately overwritten.
- Bugfix: Emission strength values are properly set for IrayUber materials.
- Bugfix: Emission Color now working for URP and Built-in RenderPipeline.
- "Enable dForce" checkbox added to Options tab of DTU Bridge window.
- RenderPipeline Detection procedure will ask to confirm Symbol Definition updates before proceeding.
- Notification windows will popup when Daz Export and Unity Import steps are complete.
- Daz Studio Subdivision settings are restored after Send To operation.
- Changed plugin name and window titles to "Unofficial DTU Bridge".

Version 0.5-alpha:
- Smoother Unity Files installation with automatic dialog popup, RP detection and proper importing of first asset.
- UI tweaks such as Daz3D menu command order, Install/Overwrite Unity Files checkbox.
- New Unity Cloth Tools component to bulk edit weights by material groups, save/load weight maps.
- Optimized HDRP and URP shadergraphs to use a single Sampler node.

Version 0.4-alpha:
- Preliminary support for dForce clothing, cloth physics export (only via the Simulation Properties of the Surfaces Pane of DazStudio).
- Pregenerated cloth collision skeleton which is automatically merged into animation skeleton of Prefabs created by the Bridge.

Version 0.3-alpha:
- Animation exporting is now enabled through the Animation asset type and disabled when exporting Skeletal or Static Mesh.
- Timeline animations are exported with sequentially numbered "@anim0000.fbx" filenames, which increment with each export operation.
- Reverted/Fixed issue caused by removal of .meta files in v0.2-alpha which can lead to problems with mismatched GUID files when upgrading the unity plugin.

Version 0.1-alpha:
- Combined support for all three rendering pipelines and an autodetection/configuration system.
