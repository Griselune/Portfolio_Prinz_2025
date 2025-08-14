using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponEnemy : Enemy
{
    public override void Awake()
    {
        base.Awake();
        attackCollider = gameObject.GetComponentInChildren<BoxCollider2D>();
    }

    public override void Initialized()
    {
        base.Initialized();
        stateManager.ChangeState(new WeaponEnemyIdleState(this, stateManager));
    }

    public override void Move()
    {
        base.Move();
    }

    protected override void ReduceHP(int damage)
    {
        base.ReduceHP(damage);
        if (HP <= 0.0f)
        {
            stateManager.ChangeState(new WeaponEnemyDeathState(this, stateManager));
        }
        else
        {
            stateManager.ChangeState(new WeaponEnemyHurtState(this, stateManager));
        }
    }

    public override void AttackFinished()
    {
        base.AttackFinished();
        stateManager.ChangeState(new WeaponEnemyWalkState(this, stateManager));
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
