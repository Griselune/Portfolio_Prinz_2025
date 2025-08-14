using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoundariesFloor : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("PlayerBullet"))
        {
            Shot1Script bullet = collision.GetComponent<Shot1Script>();
            if (bullet != null)
            {
                if (bullet.GetBounceCnt() <= 0)
                {
                 //   Destroy(collision.gameObject);
                }

            }
            
        }
    }
}
