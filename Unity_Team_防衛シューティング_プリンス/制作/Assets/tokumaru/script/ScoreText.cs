using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ScoreText : MonoBehaviour
{
    Color color;

    [SerializeField]
    private float addAlpha;

    TextMeshProUGUI text;
    // Start is called before the first frame update
    void Start()
    {
        text = gameObject.GetComponent<TextMeshProUGUI>();
        color = text.color;
        color.a = 0.0f;
        text.color = color;
        text.text = "撃破数 " + GManager.instance.GetSetScore.ToString() + "体";
    }

    // Update is called once per frame
    void Update()
    {
        if (color.a <= 1.0f)
        {
            color.a += addAlpha;
            text.color = color;
        }
    }

    public void SetText(string text_)
    {
        text.text = text_;
    }
}
