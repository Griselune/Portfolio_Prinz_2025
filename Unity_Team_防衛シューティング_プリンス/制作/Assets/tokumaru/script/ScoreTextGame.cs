using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class ScoreTextGame : MonoBehaviour
{
    TextMeshProUGUI text;
    // Start is called before the first frame update
    void Start()
    {
        text = gameObject.GetComponent<TextMeshProUGUI>();
        text.text = "撃破数 " + GManager.instance.GetSetScore.ToString() + "体";
    }

    // Update is called once per frame
    void Update()
    {
        text.text = "撃破数 " + GManager.instance.GetSetScore.ToString() + "体";
    }
}
