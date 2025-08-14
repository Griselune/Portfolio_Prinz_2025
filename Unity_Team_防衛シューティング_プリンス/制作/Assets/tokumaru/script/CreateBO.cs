using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;


public class CreateBO : MonoBehaviour
{
    public Image image;
    Color color;
    PushButton pushButton;
    GameObject button;
    public bool countstart;

    [SerializeField]
    private float addAlpha;

    // Start is called before the first frame update
    void Start()
    {
        color = image.color;
        color.a = 0.0f;
        image.color = color;
        button = GameObject.Find("Button");
        countstart = false;
        if (button)
        {
            pushButton = button.GetComponent<PushButton>();
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (!pushButton)
        {
            return;
        }

        if (countstart)
        {
            color.a += addAlpha;
            image.color = color;
            if (color.a >= 1.0f)
            {
                GManager.instance.SceneChange();
                countstart = false;
            }
        }
    }
}
