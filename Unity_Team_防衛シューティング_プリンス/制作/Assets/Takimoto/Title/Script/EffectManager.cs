using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EffectManager : MonoBehaviour
{
    public GameObject ExplosionPrefab;
    Vector3 mousePos;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if(Input.GetMouseButtonDown(0))
        {
            mousePos = Input.mousePosition;
            mousePos.z = 10.0f;

            Instantiate(ExplosionPrefab, Camera.main.ScreenToWorldPoint(mousePos), ExplosionPrefab.transform.rotation);
        }
    }
}
