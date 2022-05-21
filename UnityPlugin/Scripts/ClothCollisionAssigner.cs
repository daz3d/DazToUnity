using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Serialization;
using System;


public class ClothCollisionAssigner : MonoBehaviour
{
    [Header("Paired Sphere Colliders")]
    public SphereCollider[] m_UpperbodyColliders;
    public SphereCollider[] m_LowerbodyColliders;

    [Serializable]
    public class ClothConfig
    {
        public Cloth m_ClothToManage;
        public bool m_UpperBody;
        public bool m_LowerBody;
    }

    [Header("Cloth Collision Assignments")]
    public ClothConfig[] m_ClothConfigurationList;

    private void addCollidersToCloth(SphereCollider[] collider_list, Cloth cloth_component)
    {
        int collider_size = 0;
        foreach (SphereCollider sphere_collider in collider_list)
        {
            if (sphere_collider != null)
            {
                collider_size++;
            }
        }
        if (collider_size % 2 != 0)
        {
            collider_size += 1;
        }
        int original_size = 0;
        if (cloth_component.sphereColliders != null)
        {
            original_size = cloth_component.sphereColliders.Length;
        }
        ClothSphereColliderPair[] colliderpair_list = new ClothSphereColliderPair[original_size + (collider_size / 2)];
        if (original_size > 0)
        {
            Array.Copy(cloth_component.sphereColliders, colliderpair_list, original_size);
        }
        for (int i = 0; i < collider_size / 2; i++)
        {

            colliderpair_list[original_size + i].first = collider_list[i * 2];
            colliderpair_list[original_size + i].second = collider_list[i * 2 + 1];
        }
        cloth_component.sphereColliders = colliderpair_list;

    }

    public void addClothConfig(ClothConfig newConfig)
    {
        int new_size = m_ClothConfigurationList.Length + 1;
        Array.Resize<ClothConfig>(ref m_ClothConfigurationList, new_size);
        m_ClothConfigurationList[new_size - 1] = newConfig;
    }


    // merge cloth collision rig into rootnode of main figure rig
    public void mergeRig(Transform destination_rootTransform)
    {
        Transform source_rootTransform = this.transform.Find("hip");
        List<Transform> childTransform_stack = new List<Transform>();
        childTransform_stack.Add(source_rootTransform);

        while (childTransform_stack.Count > 0)
        {
            int next_Index = childTransform_stack.Count - 1;
            Transform child = childTransform_stack[next_Index];
            childTransform_stack.RemoveAt(next_Index);

            if (child == null) continue;

            // add all children to stack
            for (int i = 0; i < child.childCount; i++)
            {
                Transform grandchild = child.GetChild(i);
                if (grandchild != null)
                    childTransform_stack.Add(grandchild);
            }

            // check if child is a dforce collider
            if (child.CompareTag("dForceCollider"))
            {
                // 1. unroll parent tree to get full path
                Transform parent = child.parent;
                String path_name = parent.name;
                while (parent != null && parent.parent != source_rootTransform)
                {
                    if (parent.parent == null) break;
                    parent = parent.parent;
                    path_name = parent.name + "/" + path_name;
                }
                // 2. find destination parent node
                var dest_parent = destination_rootTransform.Find(path_name);
                if (dest_parent != null)
                {
                    // 3. move child to destination parent, worldspace = false
                    child.transform.SetParent(dest_parent, false);
                }
            }

        }

    }

    public void ClearAllAssignments()
    {
        foreach (ClothConfig cloth_config in m_ClothConfigurationList)
        {
            if (cloth_config == null)
                continue;
            // delete existing cloth collisions
            if (cloth_config.m_ClothToManage != null && cloth_config.m_ClothToManage.sphereColliders != null)
                cloth_config.m_ClothToManage.sphereColliders = null;
        }
    }

    public void AssignClothCollisionRigs()
    {
        foreach (ClothConfig cloth_config in m_ClothConfigurationList)
        {
            if (cloth_config == null)
                continue;
            if (cloth_config.m_UpperBody)
                addCollidersToCloth(m_UpperbodyColliders, cloth_config.m_ClothToManage);
            if (cloth_config.m_LowerBody)
                addCollidersToCloth(m_LowerbodyColliders, cloth_config.m_ClothToManage);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        ClearAllAssignments();

        AssignClothCollisionRigs();

    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
