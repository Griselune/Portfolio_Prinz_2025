using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.Rendering;

enum EnemyType
{
    BaseEnemy,
    AttackEnemy
}

public class EnemyManager : MonoBehaviour
{
    private ObjectPool baseEnemyPool;
    private ObjectPool baseWeaponEnemyPool;
    private PlayerScript player;

    /// <summary>
    /// 一般敵を派遣するための関連資料
    /// </summary>
    [SerializeField]
    private DeployInfo baseEnemyDI;

    /// <summary>
    /// 武器持ってる一般敵を派遣するための関連資料
    /// </summary>
    [SerializeField]
    private DeployInfo baseWeaponEnemyDI;

    /// <summary>
    /// 開始位置(上)
    /// </summary>
    [SerializeField]
    private Vector3 startTopPos;

    /// <summary>
    /// 開始位置(下)
    /// </summary>
    [SerializeField]
    private Vector3 startBottomPos;

    // Start is called before the first frame update
    void Start()
    {
        Random.InitState((int)System.DateTime.Now.Ticks);
        baseEnemyDI.deployCount = 0.0f;
        baseEnemyDI.NextDeployTime();
        baseWeaponEnemyDI.deployCount = 0.0f;
        baseWeaponEnemyDI.NextDeployTime();
        baseEnemyPool = GetComponentsInChildren<ObjectPool>().Where(c => c.CompareTag("BaseOP")).First();
        baseWeaponEnemyPool = GetComponentsInChildren<ObjectPool>().Where(c => c.CompareTag("BaseSwordOP")).First();
        player = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerScript>();
    }

    // Update is called once per frame
    void Update()
    {
    }

    void FixedUpdate()
    {
        float deltaTime = Time.fixedDeltaTime;
        // 一般敵の派遣
        if (IsTrigDeploy(ref baseEnemyDI, deltaTime))
        {
            DeployEnemy(baseEnemyPool);
        }

        // 武器持ってる一般敵
        if (IsTrigDeploy(ref baseWeaponEnemyDI, deltaTime))
        {
            DeployEnemy(baseWeaponEnemyPool);
        }
    }

    private bool IsTrigDeploy(ref DeployInfo DI, float deltaTime)
    {
        DI.deployCount += deltaTime;
        if (DI.deployCount >= DI.deployTime)
        {
            DI.deployCount -= DI.deployTime;
            DI.NextDeployTime();
            return true;
        }
        return false;
    }

    private void DeployEnemy(ObjectPool pool)
    {
        // 表示するlayerはintですから、敵のY座標もintにする、そうしないと変になる
        float startPosY = GetStartPos();
        Vector3 startPos = new(startTopPos.x, (int)startPosY, startTopPos.z);

        Enemy enemy = pool.Borrow();
        if (enemy)
        {
            enemy.transform.position = startPos;
            enemy.SetManager(this);
            enemy.SetTarget(player);
            enemy.GetComponent<SortingGroup>().sortingOrder = Mathf.Abs((int)(startPosY - startTopPos.y));
            enemy.Active();
        }
    }

    private float GetStartPos()
    {
        return Random.Range(startTopPos.y, startBottomPos.y);
    }

    public void RecycleEnemy(Enemy enemy)
    {
        baseEnemyPool.Recycle(enemy);
    }

    public void RecycleAllEnemy()
    {
        baseEnemyPool.RecycleAllEnemy();
    }
}
