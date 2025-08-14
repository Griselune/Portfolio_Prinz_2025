using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class PlayerScript : Character
{
    public Image BarHPCurrent;

    public FadeSceneLoader fadeSceneLoader;
    public GameObject ExplosionPrefab;
    public GameObject BarHPBase;
    public GameObject BarPowerBase;
    public GameObject BarPowerCurrent;

    public float HP = 100.0f;
    private const float MAXHP = 100.0f;

    // Start is called before the first frame update
    void Start()
    {
        Application.targetFrameRate = 60;
    }

    // Update is called once per frame
    void Update()
    {
        if (BarHPCurrent != null)
        {      
            BarHPCurrent.fillAmount = Mathf.Clamp01(this.HP / MAXHP);
        }
    }

    private void ReduceHP(float amount)
    {
        this.HP = Mathf.Max(0, this.HP - amount);
        if(this.HP <= 0)
        {
            //エンディングシーンに切り替え
            StartCoroutine(DeathAnimation());
            fadeSceneLoader.CallCoroutine();
        }    
    }

    IEnumerator DeathAnimation()
    {
     //   this.BarHPCurrent..gameObject.SetActive(false);
        BarHPBase.SetActive(false);
     //   BarPowerBase.SetActive(false);
      //  BarPowerCurrent.SetActive(false);

        float timer = fadeSceneLoader.fadeDuration - 0.5f;
        int cnt = 0;
        while (timer > 0)
        {
            cnt++;
            if (cnt % 5 == 0)
            {
                int randX = Random.Range(-50, 0);
                int randY = Random.Range(-50, +50);
                int randScale = Random.Range(10, 50);

                Vector3 exploPos = new Vector3 (randX, randY, 0);
                ExplosionPrefab.transform.localScale = new Vector3(randScale, randScale, 0);
                Instantiate(ExplosionPrefab, exploPos, Quaternion.identity);
            }
            yield return null; // wait for next frame
        }
        gameObject.SetActive(false);
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.CompareTag("EnemyAttackCollider"))
        {
            Enemy enemy = collision.GetComponentInParent<Enemy>();
            if(enemy != null)
            {
                float damage = enemy.GetAttackPower();
                if(this.HP > 0)
                {
                    ReduceHP(damage);
                    Debug.Log($"Damage : {damage}");
                    Debug.Log($"player hp : {this.HP}");
                }
            }
            else
            {
                Debug.LogWarning("Enemy component not found in parent!");
            }
        }
    }
}
