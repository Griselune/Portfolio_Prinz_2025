using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class Shot1Script : MonoBehaviour
{
    float timer = 0.0f; //bullet timer
    const float MAXTIMER = 10.0f; //bullet max timer
    public AudioClip[] SEbounce;
    AudioSource aud;

    [SerializeField]
    private int damageValue = 0;
    [SerializeField]
    private int MINDAMAGEVALUE = 2;
    [SerializeField]
    private int bounceCnt = 1;

    private bool isMaxPower = false;

    private ShotGenerator generator;
    public GameObject ExplosionPrefab;
    public GameObject ExpandingExplosion;

    public void SetGenerator(ShotGenerator gen)
    {
        generator = gen;
    }
    public void Shoot(Vector3 dir, float spin, int damageValue, bool isMaxPower)
    {
        Vector2 dir2D = new Vector2(dir.x, dir.y);
        GetComponent<Rigidbody2D>().AddForce(dir, ForceMode2D.Impulse);
        GetComponent<Rigidbody2D>().AddTorque(spin, ForceMode2D.Impulse);
        this.damageValue =  Mathf.Max(this.MINDAMAGEVALUE , damageValue);

        Debug.Log($"Damage Value : {this.damageValue}"); //debug

        this.aud = GetComponent<AudioSource>();

        this.isMaxPower = isMaxPower;
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if (collision.gameObject.CompareTag("Enemy") || collision.gameObject.CompareTag("Boundaries"))
        {
            //当たり判定
            Rigidbody2D rb = collision.gameObject.GetComponent<Rigidbody2D>();
            if (rb != null /*&& enemy state active*/)
            {
                Vector2 knockback = collision.transform.position - transform.position;
                rb.AddForce(knockback.normalized * 10000f); // adjust force value
                if (this.isMaxPower)
                {
                    Instantiate(ExpandingExplosion, this.transform.position, ExpandingExplosion.transform.rotation);
                    Destroy(gameObject);
                }
                else if (this.bounceCnt <= 0)
                {
                    ExplosionPrefab.transform.localScale = new Vector3(20, 20, 0);
                    Instantiate(ExplosionPrefab, this.transform.position, ExplosionPrefab.transform.rotation);
                //    generator?.PlaySEexplosion(); // play SE safely
                    //弾消滅
                    Destroy(gameObject);
                }
                else
                {
                    this.bounceCnt--;
                    int randomSE = Random.Range(0, SEbounce.Length); // Random index 0 to 2
                    aud.PlayOneShot(SEbounce[randomSE]);
                }
            }

        }
    }


    public int GetDamageValue()
    {
        return damageValue;
    }

    public int GetBounceCnt()
    {
        return this.bounceCnt;
    }

    // Start is called before the first frame update
    void Start()
    {
        Application.targetFrameRate = 60;
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;


        if(timer >= MAXTIMER)
        {
            timer = 0.0f;
            Destroy(gameObject);
        }
    }
}
