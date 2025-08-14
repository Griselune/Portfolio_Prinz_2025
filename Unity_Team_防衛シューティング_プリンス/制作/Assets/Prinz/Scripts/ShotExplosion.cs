using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ShotExplosion : MonoBehaviour
{
    public GameObject ExplosionPrefab;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void EffectExplosion(Vector3 bulletPos)
    {
        Instantiate(ExplosionPrefab, Camera.main.ScreenToWorldPoint(bulletPos), ExplosionPrefab.transform.rotation);
    }
}
