using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(ClothTools))]
[CanEditMultipleObjects]

public class ClothToolsEditor : Editor
{
    private SerializedObject m_Object;
    private float[] floatArray;

    public void OnEnable()
    {
        m_Object = new SerializedObject(target);
    }

    public override void OnInspectorGUI()
    {
        //ClothTools clothtools = (ClothTools)target;
        ClothTools clothtools = (ClothTools) m_Object.targetObject;

        m_Object.Update();

        GUILayout.Space(10);
        GUILayout.Label("Set Max Distance for Material Group:", EditorStyles.whiteLargeLabel);

        //DrawDefaultInspector();

        SkinnedMeshRenderer skinned = clothtools.gameObject.GetComponent<SkinnedMeshRenderer>();
        int numMaterials = skinned.sharedMaterials.Length;
        if (floatArray == null)
        {
            floatArray = new float[numMaterials];
        }
        else if (floatArray.Length != numMaterials)
        {
            System.Array.Resize(ref floatArray, numMaterials);
        }
        foreach (Material mat in skinned.sharedMaterials)
        {
            if (mat)
            {
                int matIndex = System.Array.IndexOf(skinned.sharedMaterials, mat);

//                GUILayout.Space(10);
                GUILayout.BeginHorizontal();
                //GUILayout.Label(mat.name);
                floatArray[matIndex] = EditorGUILayout.Slider(mat.name,floatArray[matIndex], 0f, 0.2f);

                //if (GUILayout.Button("Zero"))
                //{
                //    clothtools.SetSubMeshWeights(matIndex, 0.0f);
                //    //Debug.Log("Zero Material Weights: " + mat.name);
                //}
                //if (GUILayout.Button("One"))
                //{
                //    clothtools.SetSubMeshWeights(matIndex, 1.0f);
                //    //Debug.Log("Set Material Weights to 1.0: " + mat.name);
                //}
                if (GUILayout.Button("Set"))
                {
                    clothtools.SetSubMeshWeights(matIndex, floatArray[matIndex]);
                    floatArray[matIndex] = 0f;
                    //Debug.Log("Clear Material Weights: " + mat.name);
                }
                if (GUILayout.Button("Clear"))
                {
                    clothtools.ClearSubMeshWeights(matIndex);
                    floatArray[matIndex] = 0f;
                    //Debug.Log("Clear Material Weights: " + mat.name);
                }
                //floatArray[matIndex] = EditorGUILayout.TextField(floatArray[matIndex]);

                GUILayout.EndHorizontal();

                //GUILayout.Space(5);
            }
        }

        GUILayout.Space(10);
        if (GUILayout.Button("Load Weightmap data"))
        {
            string path = EditorUtility.OpenFilePanelWithFilters(
                "Load Weightmap data",
                "Assets/Daz3D",
                new string[] { "Unity Weight Maps", "unity_weightmap.bytes", "DForce Weight maps", "dforce_weightmap.bytes", "All files", "*"});
            if (path.Length != 0)
            {
                if (path.Contains(".unity_weightmap.bytes"))
                {
                    //Debug.Log("DEBUG: load from file: " + path);
                    clothtools.LoadWeightMap(path);
                }

                if (path.Contains("dforce_weightmap.bytes"))
                {
                    //Debug.Log("DEBUG: import from file: " + path);
                    clothtools.ImportWeightMap(path);
                }

            }

            //clothtools.LoadRawWeightMap();
            ////Debug.Log("Load Weightmap data.");
        }

        //GUILayout.Space(10);
        if (GUILayout.Button("Save Weightmap data"))
        {
            var path = EditorUtility.SaveFilePanel(
                "Save Weightmap data",
                "Assets/Daz3D",
                clothtools.gameObject.name,
                "unity_weightmap.bytes");

            if (path.Length != 0)
            {
                path = path.Replace(".unity_weightmap.bytes", "") + ".unity_weightmap.bytes";
                //Debug.Log("DEBUG: write to file: " + path);
                clothtools.SaveWeightMap(path);
            }

        }

        //GUILayout.Space(10);
        if (GUILayout.Button("Load Gradient Pattern"))
        {
            clothtools.LoadGradientPattern();
            //Debug.Log("Load Gradient Pattern.");
        }

        //GUILayout.Space(10);
        if (GUILayout.Button("Zero All Weights"))
        {
            for (int i=0; i < skinned.sharedMaterials.Length; i++)
            {
                clothtools.SetSubMeshWeights(i, 0f);
            }
        }

        //GUILayout.Space(10);
        if (GUILayout.Button("Clear All Weights"))
        {
            //Undo.RecordObject(clothtools.m_Cloth, "Clear All Weights");
            clothtools.ClearWeightMap();
            //Debug.Log("Clear Weights.");
        }

        //GUILayout.Space(10);
        //if (GUILayout.Button("Load Stepped Gradient"))
        //{
        //    clothtools.LoadSteppedGradient();
        //    //Debug.Log("Load Stepped Gradient.");
        //}

        //GUILayout.Space(10);
        //if (GUILayout.Button("Generate Lookup Tables"))
        //{
        //    clothtools.GenerateLookupTables();
        //    //Debug.Log("Generate Lookup Tables Called....");
        //}

        //GUILayout.Space(10);
        //if (GUILayout.Button("Run Vertex Data Test"))
        //{
        //    clothtools.TestVertData();
        //    //Debug.Log("Running Vertex Data Test....");
        //}

        m_Object.ApplyModifiedProperties();
    }

}
