Daz To Unity 2023 version 1.1.11
=============================================

Description:
============
The Daz To Unity Bridge consists of two major parts:
1) a plug-in for Daz3D Studio responsible for exporting models and related data, and
2) a set of UnityEditor scripts (C#) which consume the exported data.


New in this version:
====================
- Integration with latest Daz Bridge Library v1.1


Known Issues:
=============
- Subdivision procedure is currently only single-threaded CPU-based and may take several minutes to bake Subdivision level 3 and 4.
- dForce strand-based hair is not yet supported.
- Geograft morphs are not yet supported.
- Genesis 8/8.1 prop rigging is not yet 100% accurately converted.
- Identical / duplicate materials are not yet detected for merger in Unity.


Setup:
======
No setup is required in Unity Editor.
The Daz to Unity plugin will write all the necessary files (scripts, profiles, export data) into each targeted Unity project.
The import process is automatic.


Workflow:
=========
Daz3D Studio - Have the root of one model selected. A menu command File >> Send To >> Daz to Unity summons a dialog, in which you specify the target Unity Project folder and other options.
Unity Editor - Once the export has completed, put the Unity Editor app back into focus in Windows. The importer scripts will activate automatically and begin importing fresh export data.
The first import takes longer than subsequent imports, because the Editor must compile shaders first.
The first import of a given asset takes longer because textures are often copied across from Daz3D studio, and these take time to import.


Folders:
========
The first time an export happens, a folder tree is written into your Unity project.
Assets\Daz3D is the parent folder; the Daz to Unity Bridge only deals with files under \Daz3D\
Exports data are written to subfolders of \Daz3D\ and bear the name of the exported model.


Files:
======
FBX - Much of the model data (skeletal hierarchy, weighted mesh, material mappings, lights, cameras, etc.) are conveyed in FBX format. These FBX files are constructed by Daz3D's built-in FBX exporter, and maintain the same variety of options and level of fidelity as a routine FBX export.
DTU - The Bridge plug-in also writes a Daz-to-Unity file (in JSON format) which details the material properties so the importer can construct and assign high-fidelity Unity materials.
Shaders - A suite of shader graphs are copied into your unity project the first time an export happens. These are imported by Unity Editor and compiled into shaders compatible with Unity's HD Render Pipeline.
Scripts - A suite of C# scripts are copied into your unity project the first time an export happens. These include a custom importer for DTU file types, a custom EditorWindow and helper classes used by the importer.
Diffusion Profiles - A generic diffusion profile and an IrayUberSkinDiffusionProfile are copied into your Unity project the first time an export happens. These are used by the custom shaders to replicate skin surface properties found in Daz3D Studio.
