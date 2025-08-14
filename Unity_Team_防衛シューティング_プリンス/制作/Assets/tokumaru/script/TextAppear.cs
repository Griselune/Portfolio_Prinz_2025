using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class TextAppear : MonoBehaviour
{
    public Image image;
    Color color;

    [SerializeField]
    private float addAlpha;
    // Start is called before the first frame update
    void Start()
    {
        color = image.color;
        color.a = 0.0f;
        image.color = color;
        //addAlpha = 0.01f;
    }

    // Update is called once per frame
    void Update()
    {
        if (color.a < 1.0f)
        {
            color.a += addAlpha;
            image.color = color;
        }
    }
}
