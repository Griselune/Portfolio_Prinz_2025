using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class PushButton : MonoBehaviour
{
    public Image image;
    public Animator animator;
    public AudioClip push;//必要ないかも
    AudioSource audioSouce;
    Color color;
    bool pushButton;
    float timer;
    float limit;

    GameObject BO;
    CreateBO createBO;

    GameObject bgm;
    AudioSource audioSourceBGM;

    // Start is called before the first frame update
    void Start()
    {

        bgm = GameObject.Find("EndingBGM");
        if (bgm)
        {
            audioSourceBGM = bgm.GetComponent<AudioSource>();
        }
        audioSouce = GetComponent<AudioSource>();
        color = image.color;
        animator = gameObject.GetComponent<Animator>();
        pushButton = false;
        timer = 0.0f;
        limit = 30.0f;

        BO = GameObject.Find("BBG");
        if (BO)
        {
            createBO = BO.GetComponent<CreateBO>();
        }

    }

    // Update is called once per frame
    void Update()
    {
        if (pushButton)
        {
            if (timer >= limit)
            {
                animator.SetBool("OnPush", false);
                pushButton = false;
                //GManager.instance.sceneChange = true;
                Debug.Log("animator,PushButton false");
            }
            timer += 1.0f;
        }
    }

    public void OnButtonClick()
    {
        audioSourceBGM.Stop();
        pushButton = true;
        animator.SetBool("OnPush", true);
        createBO.countstart = true;
        audioSouce.PlayOneShot(push);
        //audioSouce.Play();
        Debug.Log("animator,PushButton true");
    }
}
