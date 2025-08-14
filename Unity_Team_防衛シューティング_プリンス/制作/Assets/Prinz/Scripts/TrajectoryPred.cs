using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting.Antlr3.Runtime.Misc;
using UnityEngine;

public class TrajectoryPred : MonoBehaviour
{
    [SerializeField] LineRenderer lineRenderer;
    [SerializeField] int numPoints = 50;
    [SerializeField] float timeStep = 0.05f;
   // [SerializeField] float gravity = 9.81f;
    [SerializeField] private GameObject shot1Prefab;



    // Start is called before the first frame update
    void Start()
    {
        Application.targetFrameRate = 60;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void ShowTrajectory(Vector3 startPos, Vector3 velocity)
    {
        Vector3[] points = new Vector3[numPoints];
        Rigidbody2D bulletRb = shot1Prefab.GetComponent<Rigidbody2D>();
        float gravity = Physics2D.gravity.y * 2.5f; //なぜこれが正しいか知らんけど、正しい

    //    Debug.Log($"gravity: {Physics2D.gravity.y}, scale: {bulletRb.gravityScale}, total: {Physics2D.gravity.y * bulletRb.gravityScale}");
        for (int i = 0; i < numPoints; i++)
        {
            float t = i * timeStep;
            Vector3 pos = startPos + velocity * t;
            pos.y += 0.5f * gravity * t * t;
            points[i] = pos;
        }

        lineRenderer.positionCount = numPoints;
        lineRenderer.SetPositions(points);
    }

    public void Hide()
    {
        lineRenderer.positionCount = 0;
    }
}
