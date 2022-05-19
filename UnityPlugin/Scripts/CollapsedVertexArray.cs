using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/////////////////////////////////////////////////////////////////
// SkinnedMesh vertex index TO Cloth vertex index
/////////////////////////////////////////////////////////////////
[System.Serializable]
public class CollapsedVertexArray
{
    private Dictionary<int, int> m_LookupTable;
    private Dictionary<int, List<CollapsedVertex>> m_CollapsedVertices;
    private int m_UniqueVertexCount;

    public class CollapsedVertex
    {
        public Vector3 vertex;
        public int unique_index;
        public List<int> indexes;

        public CollapsedVertex(Vector3 a_vertex, int index, int a_unique_index)
        {
            vertex = a_vertex;
            indexes = new List<int>(1) { index };
            unique_index = a_unique_index;
        }

        public void AddIndex(int index)
        {
            indexes.Add(index);
        }

        public static bool operator ==(CollapsedVertex a, Vector3 b) => a.vertex == b;
        public static bool operator !=(CollapsedVertex a, Vector3 b) => a.vertex != b;

        public static bool operator ==(CollapsedVertex a, CollapsedVertex b) => a.vertex == b.vertex;
        public static bool operator !=(CollapsedVertex a, CollapsedVertex b) => a.vertex != b.vertex;
        public override bool Equals(object obj) => this.vertex.Equals(obj);
        public override int GetHashCode() => this.vertex.GetHashCode();

    }

    public int Length
    {
        get
        {
            if (m_LookupTable == null || m_LookupTable.Count == 0)
            {
                //Debug.LogError("CollapsedVertexArray.Length: m_LookupTable is null");
                return -1;
            }

            if (m_LookupTable.Count < m_UniqueVertexCount)
            {
                Debug.LogWarning("mLookupTable.Count[" + m_LookupTable.Count + "] may not contain all unique vertices [" + m_UniqueVertexCount + "]");
            }

            return m_UniqueVertexCount;
        }
    }

    public int LookupIndex(int a_index)
    {
        if (m_LookupTable != null)
        {
            if (m_LookupTable.ContainsKey(a_index))
                return m_LookupTable[a_index];
        }

        return -1;

    }

    public CollapsedVertexArray(Vector3[] a_vertices)
    {
        if (a_vertices.Length <= 0)
        {
            m_UniqueVertexCount = -1;
            return;
        }

        m_CollapsedVertices = new Dictionary<int, List<CollapsedVertex>>(a_vertices.Length);
        for (int i = 0; i < a_vertices.Length; i++)
        {
            Vector3 a_vert = a_vertices[i];
            bool vert_is_unique = true;
            if (m_CollapsedVertices.ContainsKey(a_vert.GetHashCode()))
            {
                // get optimized (hashcode filtered) list of verts, check against each one for uniqueness
                List<CollapsedVertex> cvert_list = m_CollapsedVertices[a_vert.GetHashCode()];
                foreach (CollapsedVertex cvert in cvert_list)
                {
                    if (cvert == a_vert)
                    {
                        vert_is_unique = false;
                        cvert.AddIndex(i);
                        break;
                    }
                }
                if (vert_is_unique)
                {
                    // add to end of unqiue verts array
                    //m_CollapsedVertices[m_UniqueVertexCount++] = new CollapsedVertex(a_vert, i);
                    cvert_list.Add(new CollapsedVertex(a_vert, i, m_UniqueVertexCount++));
                }

            }
            else
            {
                // assume unqiue
                m_CollapsedVertices.Add(a_vert.GetHashCode(), new List<CollapsedVertex>(1) { new CollapsedVertex(a_vert, i, m_UniqueVertexCount++) });
            }

        }

        // build lookup tables / Dictionaries
        m_LookupTable = new Dictionary<int, int>(a_vertices.Length);
        foreach (List<CollapsedVertex> cvert_list in m_CollapsedVertices.Values)
        {
            foreach (CollapsedVertex cvert in cvert_list)
            {
                foreach (int key in cvert.indexes)
                {
                    if (m_LookupTable.ContainsKey(key))
                    {
                        if (m_LookupTable[key] != cvert.unique_index)
                        {
                            Debug.LogWarning("CollapsedVertexArray: lookup table generator is overwriting unique index with another value");
                            m_LookupTable[key] = cvert.unique_index;
                        }
                    }
                    else
                    {
                        m_LookupTable.Add(key, cvert.unique_index);
                    }
                }

            }
        }

        //Debug.Log("Finished CollapsedVertexArray: original Verts=" + a_vertices.Length + ", unique Verts=" + m_UniqueVertexCount);
        return;

    }

}
/////////////////////////////////////////////////////////////////
// End: SkinnedMesh vertex index TO Cloth vertex index
/////////////////////////////////////////////////////////////////
