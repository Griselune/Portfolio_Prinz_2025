using System.Collections;
using System.Collections.Generic;
//using System.Numerics;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.UIElements.Experimental;

public class ShotGenerator : MonoBehaviour
{
    public GameObject   Shot1Prefab;
    [SerializeField] private TrajectoryPred trajectory;
    [SerializeField] private Transform shotSpawnPoint; // Reference to where the shot spawns


    public Image        BarPowerCurrent;
    public Image        BarPowerBase;

    public AudioClip    SECharge;
    public AudioClip[]  SEShoot;
    public AudioClip[]  SEExplosion;
    AudioSource         aud;

    [SerializeField] private float  MAXPOWER = 100.0f;
    [SerializeField] private int    MAXDAMAGE = 10;
    [SerializeField] private float  MAXCHARGETIMER = 2.0f;
    [SerializeField] private float  MINSPEED = 30.0f;
    Vector3 shotSpawnPos;
    private bool isMaxPower = false;


    // Start is called before the first frame update
    void Start()
    {
        Application.targetFrameRate = 60;
        this.aud = GetComponent<AudioSource>();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetMouseButtonDown(0))
        {
            this.aud.PlayOneShot(this.SECharge);
        //    Debug.Log("Mouse click");


            StartCoroutine(ChargeAndFire());
        //    Test();  //debug
        }
        if (Input.GetMouseButtonUp(0)) 
        {
            this.aud.Stop();

        }
    }

    IEnumerator ChargeAndFire()
    {
        float power = 0.0f;
        BarPowerCurrent.gameObject.SetActive(true); // show bars
        BarPowerBase.gameObject.SetActive(true);

        //shotGeneratorを見つける
        GameObject shotgen = GameObject.Find("ShotGenerator");
        //弾の座標を設定する
        Vector3 shotSpawnPos = shotgen.transform.position;


        if (Input.GetMouseButtonDown(1))  //debug
        {


        }

 
        while (Input.GetMouseButton(0))
        {
            //位置を毎フレーム計算
            //弾の位置を計算
            Vector3 mp = Input.mousePosition;                       //take mouse position
            mp.z = 60.0f - Camera.main.transform.position.z;                    //align mouse position in the Z axis
            Vector3 worldMousePos = Camera.main.ScreenToWorldPoint(mp);  //make target's coordinates
            worldMousePos.z = 60.0f;                                                 //ensure the Z alignment
            this.shotSpawnPos = shotgen.transform.position;      //give initial position to shot



            Vector3 direction = (worldMousePos - this.shotSpawnPos).normalized;
            Vector3 velocity = direction * (MINSPEED + power);

            //軌道予測線
            trajectory.ShowTrajectory(this.shotSpawnPos, velocity);

            //チャージ
            power += Time.deltaTime * MAXPOWER / MAXCHARGETIMER;
            BarPowerCurrent.fillAmount = Mathf.Clamp01(power / MAXPOWER);
            if (power >= MAXPOWER)
            {
                this.isMaxPower = true;
                break;
            }
            yield return null; // wait for next frame
        }

        //バーと軌道予測線のリセットと非表示
        BarPowerCurrent.fillAmount = 0;                 //バーを0にリセット
        BarPowerCurrent.gameObject.SetActive(false);    //バーを隠す
        BarPowerBase.gameObject.SetActive(false );      //バーを隠す
        trajectory.Hide();                              //予測線を隠す

        //弾の方向を決める
        Vector3 finalMousePos = Input.mousePosition;
        finalMousePos.z = 60.0f - Camera.main.transform.position.z;
        Vector3 finalWorldPos = Camera.main.ScreenToWorldPoint(finalMousePos);
        finalWorldPos.z = 60.0f;
        Vector3 finalDirection = (finalWorldPos - shotSpawnPos).normalized;
        Vector3 finalVelocity = finalDirection * (MINSPEED + power);

        //弾を生成する
        GameObject shot = Instantiate(Shot1Prefab, shotSpawnPos, UnityEngine.Quaternion.identity);
        int damageValue = this.MAXDAMAGE * (int)(power * 1000 / MAXPOWER) / 1000;
        shot.GetComponent<Shot1Script>().SetGenerator(this);
        shot.GetComponent<Shot1Script>().Shoot(finalVelocity, power / 3, damageValue, this.isMaxPower);

        //サウンドエフェクト
        int randomSE = Random.Range(0, SEShoot.Length); // Random index 0 to 2
        aud.PlayOneShot(SEShoot[randomSE]);

        //リセット
        this.isMaxPower = false;

        //--------------DEBUG LOGS-------------------------------
        //Debug.Log($"worldPos : {worldPos}");
        //Debug.Log($"bullSpanPos : {shotSpawnPos}");
        //Debug.Log($"distance : {worldPos - shotSpawnPos}");
        //Debug.Log($"distance norm : {(worldPos - shotSpawnPos).normalized}");
    }

    //弾の爆発SE
    public void PlaySEexplosion()
    {
        int randomSE = Random.Range(0, SEExplosion.Length);
    //    Debug.Log($"SE array select : {randomSE}");
        aud.PlayOneShot(SEExplosion[randomSE]);
    }

    //*******************************   DEBUG   **********************************
    void Test()
    {
        UnityEngine.Vector3 mp = Input.mousePosition;
        mp.z = 60.0f/*0.0f - Camera.main.transform.position.z*/;
        UnityEngine.Vector3 worldPos = Camera.main.ScreenToWorldPoint(mp);
        //UnityEngine.Vector2 mp2D = new UnityEngine.Vector2(mp.x, mp.y);
        GameObject shot = Instantiate(Shot1Prefab, worldPos, UnityEngine.Quaternion.identity);


        //Debug.Log($"worldpos test : {worldPos}");
    }
    //****************************************************************************
}

