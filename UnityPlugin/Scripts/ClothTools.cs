using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Cloth))]
//[ExecuteInEditMode]
public class ClothTools : MonoBehaviour
{
    [SerializeField, HideInInspector]
    public SkinnedMeshRenderer m_Skinned;
    [SerializeField, HideInInspector]
    public Cloth m_Cloth;

    [System.Serializable]
    class SubmeshMeta
    {
        public int submesh_index;
        public string submesh_name;
        public int vertex_offset;
        public int vertex_count;

        public SubmeshMeta(int index, string name, int offset, int count)
        {
            submesh_index = index;
            submesh_name = name;
            vertex_offset = offset;
            vertex_count = count;
        }

        public int Start()
        {
            return vertex_offset;
        }

        public int Stop()
        {
            return vertex_offset + vertex_count + 1;
        }

        public bool InSubmesh(int a_index)
        {
            if ( a_index >= vertex_offset && a_index <= (vertex_offset+vertex_count) )
            {
                return true;
            }
            else
            {
                return false;
            }
        }

    }
    [SerializeField, HideInInspector]
    private List<SubmeshMeta> m_SubmeshMeta;

    [SerializeField, HideInInspector]
    private CollapsedVertexArray m_CollapsedVerts;

    [HideInInspector]
    public TextAsset m_BinaryData;
    [HideInInspector]
    public TextAsset m_TestVertData;

    void Reset()
    {
        m_Skinned = GetComponent<SkinnedMeshRenderer>();
        m_Cloth = GetComponent<Cloth>();
        m_CollapsedVerts = null;
        GenerateLookupTables();
    }

    public void GenerateLookupTables()
    {
        m_CollapsedVerts = new CollapsedVertexArray(m_Skinned.sharedMesh.vertices);

        if (m_Cloth.vertices.Length == m_CollapsedVerts.Length)
        {
            //Debug.Log("collapsed == cloth. Ready for weightmap transfer.");
        }
        else
        {
            Debug.LogError("ClothTools.GenerateLookupTables() ERROR: # collapsed verts (" + m_CollapsedVerts.Length + ") != # cloth verts(" + m_Cloth.vertices.Length + ").  Please fix lookup table.");
        }

        // create submesh meta
        m_SubmeshMeta = new List<SubmeshMeta>();
        for (int i = 0; i <  m_Skinned.sharedMesh.subMeshCount; i++)
        {
            var submesh = m_Skinned.sharedMesh.GetSubMesh(i);
            string submesh_name = submesh.ToString();
            if (m_Skinned.sharedMaterials.Length == m_Skinned.sharedMesh.subMeshCount)
            {
                submesh_name = m_Skinned.sharedMaterials[i].name;
            }
            m_SubmeshMeta.Add(new SubmeshMeta(i, submesh_name, submesh.firstVertex, submesh.vertexCount));
        }

    }


    public void SetSubMeshWeights(int submesh_index, float weight_value)
    {
        if (m_CollapsedVerts == null || m_CollapsedVerts.Length <= 0)
        {
            //Debug.Log("ClothTools.SetSubMeshWeights(): Lookup tables were reset. Regenerating...");
            GenerateLookupTables();
        }

        if (submesh_index > m_Skinned.sharedMesh.subMeshCount)
        {
            Debug.LogError("ClothTools.SetSubMeshWeight(): invalid submesh_index=" + submesh_index);
            return;
        }

        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        //var submesh = m_Skinned.sharedMesh.GetSubMesh(submesh_index);
        //for (int vertex_index = submesh.firstVertex; vertex_index < (submesh.firstVertex + submesh.vertexCount); vertex_index++)
        //{
        //    int cloth_vertex = m_CollapsedVerts.LookupIndex(vertex_index);
        //    if (cloth_vertex != -1)
        //        newCoefficients[cloth_vertex].maxDistance = weight_value;
        //}

        var triangle_vertindex_array = m_Skinned.sharedMesh.GetTriangles(submesh_index);
        bool errorOnce = false;
        foreach (int vertex_index in triangle_vertindex_array)
        {
            int cloth_vertex = m_CollapsedVerts.LookupIndex(vertex_index);
            if (cloth_vertex != -1)
            {
                newCoefficients[cloth_vertex].maxDistance = weight_value;
            }
            else
            {
                if (errorOnce == false)
                {
                    errorOnce = true;
                    Debug.LogError("ClothTools.SetSubmeshWeights(): submesh[" + submesh_index + "] vertex index lookup did not return valid result for cloth vertex index. ");
                }
            }
        }

        m_Cloth.coefficients = newCoefficients;

    }

    public void ClearSubMeshWeights(int submesh_index)
    {
        SetSubMeshWeights(submesh_index, float.MaxValue);
    }

    public void ClearWeightMap()
    {

        if (m_Cloth == null)
            return;

        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        for (int i = 0; i < newCoefficients.Length; i++)
        {
            float maxDistance = newCoefficients[i].maxDistance;
            newCoefficients[i].maxDistance = float.MaxValue;
        }

        m_Cloth.coefficients = newCoefficients;

    }

    public void LoadGradientPattern()
    {

        if (m_Cloth == null)
            return;

        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        for (int i = 0; i < newCoefficients.Length; i++)
        {
            float maxDistance = newCoefficients[i].maxDistance;
//            float gradientValue = (float)i / newCoefficients.Length * float.MaxValue;
            float gradientValue = (float)i / newCoefficients.Length;
            newCoefficients[i].maxDistance = gradientValue;
        }

        m_Cloth.coefficients = newCoefficients;

    }

    public void LoadSteppedGradient()
    {

        if (m_Cloth == null)
            return;

        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        for (int i = 0; i < newCoefficients.Length; i++)
        {
            float maxDistance = newCoefficients[i].maxDistance;
            float gradientValue = (float)i / newCoefficients.Length;
            int stepping = (int) (gradientValue * 10.0f);
            gradientValue = stepping / 10.0f;
            newCoefficients[i].maxDistance = gradientValue;
        }

        m_Cloth.coefficients = newCoefficients;

    }


    public void SaveWeightMap(string filename)
    {
        ClothSkinningCoefficient[] outCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, outCoefficients, outCoefficients.Length);
        int numVerts = outCoefficients.Length;
        float[] weights = new float[numVerts];
        int numBytes = sizeof(float) * numVerts;
        byte[] binaryBuffer = new byte[numBytes];

        for (int i=0; i < numVerts; i++)
        {
            weights[i] = outCoefficients[i].maxDistance;
        }

        System.Buffer.BlockCopy(weights, 0, binaryBuffer, 0, numBytes);
        System.IO.File.WriteAllBytes(filename, binaryBuffer);

    }

    public void LoadWeightMap(string filename)
    {
        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        /////////////////////////////////////////////////////
        /// Load Raw Weight Map
        /////////////////////////////////////////////////////
        int numVerts = newCoefficients.Length;
        float[] weights = new float[numVerts];
        int numBytes = sizeof(float) * numVerts;

        byte[] binaryBuffer = System.IO.File.ReadAllBytes(filename);
        System.Buffer.BlockCopy(binaryBuffer, 0, weights, 0, numBytes);

        for (int i=0; i < numVerts; i++)
        {
            newCoefficients[i].maxDistance = weights[i];
        }

        m_Cloth.coefficients = newCoefficients;

    }

    public void ImportWeightMap(string filename)
    {
        ClothSkinningCoefficient[] newCoefficients = new ClothSkinningCoefficient[m_Cloth.coefficients.Length];
        System.Array.Copy(m_Cloth.coefficients, newCoefficients, newCoefficients.Length);

        /////////////////////////////////////////////////////
        /// Load Raw Weight Map
        /////////////////////////////////////////////////////
        int numVerts = newCoefficients.Length ;
        ushort[] weights = new ushort[numVerts];
        int numBytes = sizeof(ushort) * numVerts;

        byte[] binaryBuffer = System.IO.File.ReadAllBytes(filename);
        System.Buffer.BlockCopy(binaryBuffer, 0, weights, 0, numBytes);

        float simulation_strength = 0.0f;
        for (int vertex_index = 0; vertex_index < numVerts; vertex_index++)
        {
            int cloth_index = vertex_index;
            if (cloth_index >= newCoefficients.Length)
            {
                Debug.LogError("ClothTools.LoadRawWeightMap(): cloth_index is greater than coefficient array: " + vertex_index + " vs " + newCoefficients.Length);
                break;
            }

            simulation_strength = (float) weights[vertex_index] / ushort.MaxValue;

            //// DEBUG TESTING: set zero value (red) to maxvalue(black)
            //if (simulation_strength == 0) simulation_strength = float.MaxValue;

            //float strength_max = 1.0f;
            //float strength_min = 0.0f;
            //float strength_scale_threshold = 0.5f;
            float adjusted_simulation_strength = simulation_strength;

            //// tiered scaling
            //if (simulation_strength <= strength_scale_threshold)
            //{
            //    // stronger compression of values below threshold
            //    float scale = 0.075f;
            //    float offset = 0.2f;
            //    adjusted_simulation_strength = (simulation_strength - offset) * scale;
            //}
            //else
            //{
            //    float offset = (strength_scale_threshold - 0.2f) * 0.075f; // offset = (threshold - previous tier's offset) * previous teir's scale
            //    float scale = 0.2f;
            //    adjusted_simulation_strength = (simulation_strength - offset) / (1 - offset); // apply offset, then normalize to 1.0
            //    adjusted_simulation_strength *= scale;

            //}
            //// clamp to 0.0f to 0.2f
            //float coeff_min = 0.0f;
            //float coeff_max = 0.2f;
            //adjusted_simulation_strength = (adjusted_simulation_strength > coeff_min) ? adjusted_simulation_strength : coeff_min;
            //adjusted_simulation_strength = (adjusted_simulation_strength < coeff_max) ? adjusted_simulation_strength : coeff_max;

            newCoefficients[cloth_index].maxDistance = adjusted_simulation_strength;
        }

        m_Cloth.coefficients = newCoefficients;

    }

    public void TestVertData()
    {
        // get verts
        Vector3[] unityVerts = m_Cloth.vertices;

        int numVerts = m_Cloth.vertices.Length;
        float[] dazVertBuffer = new float[numVerts*3];
        int byte_length = sizeof(float) * numVerts*3;
        Debug.Log("byte_length = " + byte_length);
        System.Buffer.BlockCopy(m_TestVertData.bytes, 0, dazVertBuffer, 0, numVerts*3);

        Vector3 unityMax = new Vector3();
        Vector3 unityMin = new Vector3();
        Vector3 dazMax = new Vector3();
        Vector3 dazMin = new Vector3();

        int i;
        for (i=0; i < unityVerts.Length; i++)
        {
            Vector3 a_vert = m_Skinned.rootBone.TransformPoint(unityVerts[i]);
//            Vector3 a_vert = m_Cloth.transform.TransformPoint(unityVerts[i]);
//            Vector3 a_vert = unityVerts[i];
            a_vert *= 100f;

            // calc bounds for unity verts
            unityMax.x = (a_vert.x > unityMax.x) ? a_vert.x : unityMax.x;
            unityMax.y = (a_vert.y > unityMax.y) ? a_vert.y : unityMax.y;
            unityMax.z = (a_vert.z > unityMax.z) ? a_vert.z : unityMax.z;
            unityMin.x = (a_vert.x < unityMin.x) ? a_vert.x : unityMin.x;
            unityMin.y = (a_vert.y < unityMin.y) ? a_vert.y : unityMin.y;
            unityMin.z = (a_vert.z < unityMin.z) ? a_vert.z : unityMin.z;

            //int j;
            //for (j=0; j < numVerts; j++)
            //{
            //    Vector3 b_vert = new Vector3();
            //    b_vert.x = -dazVertBuffer[(j*3)+0] * 0.01f;
            //    b_vert.y = dazVertBuffer[(j*3)+1] * 0.01f;
            //    b_vert.z = dazVertBuffer[(j*3)+2] * 0.01f;
            //    if (Vector3.Distance(a_vert, b_vert) < 0.001f)
            //    {
            //        Debug.Log("unity[" + i + "] == daz[" + j + "]");
            //        break;
            //    }
            //}
            //if (j == numVerts)
            //{
            //    Debug.LogError("Could not match skinned[" + i + "] in any vertex of daz[]");
            //    calcDazBounds = false;
            //}

        }

        int j;
        for (j = 0; j < numVerts; j++)
        {
            Vector3 a_vert = new Vector3();
            a_vert.y = dazVertBuffer[(j * 3) + 0] * 0.95830578062345271389800693281935f;
            a_vert.x = dazVertBuffer[(j * 3) + 1] * 0.7561518763918325407088556644264f;
            a_vert.z = dazVertBuffer[(j * 3) + 2] * 1.1470080563670463526164516598318f + 14.86038f;
            // calc bounds for unity verts
            dazMax.x = (a_vert.x > dazMax.x) ? a_vert.x : dazMax.x;
            dazMax.y = (a_vert.y > dazMax.y) ? a_vert.y : dazMax.y;
            dazMax.z = (a_vert.z > dazMax.z) ? a_vert.z : dazMax.z;
            dazMin.x = (a_vert.x < dazMin.x) ? a_vert.x : dazMin.x;
            dazMin.y = (a_vert.y < dazMin.y) ? a_vert.y : dazMin.y;
            dazMin.z = (a_vert.z < dazMin.z) ? a_vert.z : dazMin.z;
        }
        Debug.Log("unityBounds: (" + unityMin.x + " to " + unityMax.x + " [" + (unityMax.x - unityMin.x) + "], " + unityMin.y + " to " + unityMax.y + " [" + (unityMax.y - unityMin.y) + "], " + unityMin.z + " to " + unityMax.z + " [" + (unityMax.z - unityMin.z) + "])");
        Debug.Log("dazBounds: (" + dazMin.x + " to " + dazMax.x + " [" + (dazMax.x - dazMin.x) + "], " + dazMin.y + " to " + dazMax.y + " [" + (dazMax.y - dazMin.y) + "], " + dazMin.z + " to " + dazMax.z + " [" + (dazMax.z - dazMin.z) + "])");

        Debug.Log("Done");

    }

}
