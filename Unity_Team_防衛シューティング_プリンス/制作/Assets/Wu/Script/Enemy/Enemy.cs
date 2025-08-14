using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : Character
{
    public enum HitDir
    {
        front,
        back
    }

    protected Rigidbody2D body;
    protected EnemyManager manager;
    protected Animator animator;
    protected Collider2D attackCollider;
    protected PlayerScript target;
    protected StateManager stateManager;
    protected AudioSource soundPlayer;
    private GManager gManager;
    [SerializeField]
    private AudioClip deathSE;
    [SerializeField]
    private AudioClip[] hitSEList;
    [SerializeField]
    private AudioClip attackSE;

    /// <summary>
    /// 最大体力
    /// </summary>
    [SerializeField]
    protected float MAX_HP = 100;

    /// <summary>
    /// 体力
    /// </summary>
    protected float HP;

    /// <summary>
    /// 移動速度
    /// </summary>
    [SerializeField]
    protected float moveSpeed;

    /// <summary>
    /// 攻撃力
    /// </summary>
    [SerializeField]
    protected float attackPower;

    /// <summary>
    /// 活動しているか
    /// </summary>
    public bool isActive { get; private set; }

    /// <summary>
    /// 攻撃してるか
    /// </summary>
    public bool isAttack { get; private set; }

    /// <summary>
    /// 攻撃されてるか
    /// </summary>
    public bool isHit { get; private set; }

    /// <summary>
    /// 攻撃されてる方向
    /// </summary>
    public HitDir hitDir { get; private set; }

    public virtual void Awake()
    {
        body = gameObject.GetComponent<Rigidbody2D>();
        animator = gameObject.GetComponent<Animator>();
        stateManager = new StateManager();
        soundPlayer = gameObject.GetComponent<AudioSource>();
        gManager = GameObject.Find("GameManager").GetComponent<GManager>();
    }

    // Start is called before the first frame update
    void Start()
    {
        Initialized();
    }

    // Update is called once per frame
    void Update()
    {
        stateManager.Update(target);
    }

    public Animator GetAnimator()
    {
        return animator;
    }

    public Rigidbody2D GetBody()
    {
        return body;
    }

    public virtual void Move()
    {
        body.MovePosition(transform.position + moveSpeed * Time.deltaTime * Vector3.left);
    }

    public virtual void Initialized()
    {
        HP = MAX_HP;
        CloseAttackCollider();
        isAttack = false;
        isHit = false;
        Inactive();
        gameObject.GetComponent<CapsuleCollider2D>().enabled = true;
    }

    public void Active()
    {
        isActive = true;
    }

    public void Inactive()
    {
        isActive = false;
    }

    public void SetManager(EnemyManager manager)
    {
        this.manager = manager;
    }

    public void SetTarget(PlayerScript target)
    {
        this.target = target;
    }

    protected virtual void ReduceHP(int damage)
    {
        if (HP <= 0.0f)
            return;

        HP -= damage;
        if (HP <= 0.0f)
        {
            HP = 0;
            gameObject.GetComponent<CapsuleCollider2D>().enabled = false;
            gManager.GetSetScore = 1;
        }
        else
        {
            isHit = true;
        }
    }

    public void PlayDeathSE()
    {
        soundPlayer.clip = deathSE;
        soundPlayer.Play();
    }

    public void PlayHitSE()
    {
        int hitSEIndex = Random.Range(0, hitSEList.Length);
        soundPlayer.clip = hitSEList[hitSEIndex];
        soundPlayer.Play();
    }

    public void PlayAttackSE()
    {
        soundPlayer.clip = attackSE;
        soundPlayer.Play();
    }

    public float GetAttackPower()
    {
        return attackPower;
    }

    public bool IsAlive()
    {
        return HP > 0;
    }

    public void OpenAttackCollider()
    {
        if (attackCollider)
            attackCollider.enabled = true;
    }

    public void CloseAttackCollider()
    {
        if (attackCollider)
            attackCollider.enabled = false;
    }

    public virtual void AttackFinished()
    {
        isAttack = false;
    }

    public virtual void HitFinished()
    {
        isHit = false;
    }

    public virtual void DeathFinished()
    {
        manager.RecycleEnemy(this);
    }

    void OnCollisionEnter2D(Collision2D collision)
    {
        // 攻撃された
        if (!isHit && collision.gameObject.CompareTag("PlayerBullet"))
        {
            if (transform.position.x < collision.transform.position.x)
                hitDir = HitDir.back;
            else
                hitDir = HitDir.front;
            // get Bullet attack power
            int damage = collision.gameObject.GetComponent<Shot1Script>().GetDamageValue();
            ReduceHP(damage);
        }
    }

    void OnCollisionStay2D(Collision2D collision)
    {
        // 攻撃する
        if (!isHit && !isAttack && collision.gameObject.CompareTag("Player"))
        {
            isAttack = true;
        }
    }

    void OnTriggerEnter2D(Collider2D collision)
    {
        // 爆発攻撃を受ける
        if (!isHit && collision.gameObject.CompareTag("PlayerBullet"))
        {
            // get Bullet attack power
            ReduceHP(Mathf.CeilToInt(MAX_HP));
        }
    }
}
