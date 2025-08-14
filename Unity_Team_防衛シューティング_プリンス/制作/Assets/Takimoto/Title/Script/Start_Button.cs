using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;

public class Start_Button : MonoBehaviour
{
    public FadeSceneLoader fadeSceneLoader;
    public Vector2 position = new Vector2(400, 40);

    // Start is called before the first frame update
    void Start()
    {
        RectTransform rt = GetComponent<RectTransform>();
        rt.anchoredPosition = position;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void OnClick_Start()
    {
        GetComponent<AudioSource>().Play();
        fadeSceneLoader.CallCoroutine();
    }
}
