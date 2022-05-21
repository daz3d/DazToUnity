using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor (typeof (ClothCollisionAssigner))]
[CanEditMultipleObjects]
public class ClothCollisionAssignerEditor : Editor
{
    private SerializedObject m_Object;

        public void OnEnable()
    {
        m_Object = new SerializedObject(target);
    }

    public override void OnInspectorGUI()
    {
        m_Object.Update();

        GUILayout.Label("**Cloth Collision Assigner**", EditorStyles.boldLabel);

        GUILayout.Space(10);
        base.OnInspectorGUI();
        GUILayout.Space(10);



        GUILayout.Space(10);
        GUILayout.Label("When this script is enabled, Cloth Collision Rigs are assigned during Runtime.  Click \"Assign Cloth Collision Rigs\" to assign the rigs in editor mode.", EditorStyles.wordWrappedLabel );
        
        GUILayout.BeginHorizontal();
        if (GUILayout.Button("Assign Cloth Collision Rigs"))
        {
            ClothCollisionAssigner assigner = (ClothCollisionAssigner)target;
            assigner.AssignClothCollisionRigs();
            Debug.Log("Assigned Cloth Collision Rigs");
        }

        if (GUILayout.Button("Clear All Assignments"))
        {
            ClothCollisionAssigner assigner = (ClothCollisionAssigner)target;
            assigner.ClearAllAssignments();
            Debug.Log("Cleared All Assignments");
        }

        GUILayout.EndHorizontal();
        GUILayout.Space(10);

        m_Object.ApplyModifiedProperties();

    }

}
