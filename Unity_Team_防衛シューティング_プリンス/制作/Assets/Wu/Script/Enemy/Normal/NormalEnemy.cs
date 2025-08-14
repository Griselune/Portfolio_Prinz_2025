using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NormalEnemy : Enemy
{
    public override void Awake()
    {
        base.Awake();
        attackCollider = gameObject.GetComponentInChildren<CircleCollider2D>();
    }

    public override void Initialized()
    {
        base.Initialized();
        stateManager.ChangeState(new EnemyIdleState(this, stateManager));
    }

    public override void Move()
    {
        base.Move();
    }

    protected override void ReduceHP(int damage)
    {
        base.ReduceHP(damage);
        
        if(HP <= 0.0f)
        {
            stateManager.ChangeState(new EnemyDeathState(this, stateManager));
        }
        else
        {
            stateManager.ChangeState(new EnemyHurtState(this, stateManager));
        }
    }

    public override void AttackFinished()
    {
        base.AttackFinished();
        stateManager.ChangeState(new EnemyWalkState(this, stateManager));
    }

    public override void HitFinished()
    {
        base.HitFinished();
    }

    public override void DeathFinished()
    {
        base.DeathFinished();
    }
}
