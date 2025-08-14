using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class ObjectPool : MonoBehaviour
{
    /// <summary>
    /// エネミーサンプル
    /// </summary>
    public GameObject enemyPrefab;

    /// <summary>
    /// 同時に出現する敵の最大数
    /// </summary>
    [SerializeField]
    private int MAX_ENEMY;

    /// <summary>
    /// 足りない時補充するか
    /// </summary>
    [SerializeField]
    private bool NeedSupplement;

    /// <summary>
    /// 生成した時の初期位置
    /// </summary>
    [SerializeField]
    private Vector3 initPos;

    private List<Enemy> unUse;
    private List<Enemy> used;
    // Start is called before the first frame update
    void Start()
    {
        unUse = new List<Enemy>();
        used = new List<Enemy>();
        PrepareEnemies();
    }

    // Update is called once per frame
    void Update()
    {

    }

    private void PrepareEnemies()
    {
        for (int index = 0; index < MAX_ENEMY; index++)
        {
            unUse.Add(CreateEnemy());
        }
    }

    private Enemy CreateEnemy()
    {
        GameObject obj = GameObject.Instantiate(enemyPrefab, initPos, Quaternion.identity, transform);
        Enemy enemy = obj.GetComponent<Enemy>();
        Initialized(enemy);
        return enemy;
    }

    private void Initialized(Enemy enemy)
    {
        enemy.transform.localPosition = initPos;
        enemy.transform.localRotation = Quaternion.identity;
        enemy.Initialized();
    }

    public Enemy Borrow()
    {
        if (unUse.Count <= 0)
        {
            if (!NeedSupplement)
                return null;

            unUse.Add(CreateEnemy());
        }

        Enemy enemy = unUse.First();
        used.Add(enemy);
        unUse.Remove(enemy);

        return enemy;
    }

    public void Recycle(Enemy enemy)
    {
        Initialized(enemy);

        unUse.Add(enemy);
        used.Remove(enemy);
    }

    public void RecycleAllEnemy()
    {
        for (int index = 0; index < used.Count;)
        {
            Recycle(used[index]);
        }
    }
}
