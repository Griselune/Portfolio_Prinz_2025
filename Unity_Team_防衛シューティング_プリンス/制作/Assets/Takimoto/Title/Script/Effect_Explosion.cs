using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class Effect_Explosion : MonoBehaviour
{
    public void DeleteEffect()
    {
        Destroy(gameObject);
    }

    public void PlayExplosionAudio()
    {
        GetComponent<AudioSource>().Play();
    }


    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
