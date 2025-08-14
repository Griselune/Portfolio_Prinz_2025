using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using DG.Tweening;

public class TitleLogo_Script : MonoBehaviour
{
    public GameObject Logo;

    // Start is called before the first frame update
    void Start()
    {
        DOTween.Init();
        RectTransform now = Logo.GetComponent<RectTransform>();
        now.DOAnchorPos(new Vector2(0, 0), 1.5f).SetEase(Ease.OutQuart);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
