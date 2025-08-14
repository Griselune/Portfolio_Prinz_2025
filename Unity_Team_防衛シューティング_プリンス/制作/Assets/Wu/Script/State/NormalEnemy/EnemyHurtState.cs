using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyHurtState : BaseState
{
    public EnemyHurtState(Character c, StateManager m) : base(c, m) { }

    public override void EnterState()
    {
        Enemy mine = (Enemy)me;
        if(mine.hitDir == Enemy.HitDir.front)
            mine.GetAnimator().CrossFadeInFixedTime("GetHitFront", 0f);
        else
            mine.GetAnimator().CrossFadeInFixedTime("GetHitBack", 0f);
        mine.PlayHitSE();
        mine.GetBody().constraints = RigidbodyConstraints2D.FreezePositionX | RigidbodyConstraints2D.FreezeRotation;
    }

    public override void ExitState()
    {
        Enemy mine = (Enemy)me;
        mine.GetBody().constraints = RigidbodyConstraints2D.FreezeRotation;
    }

    public override void UpdateState(Character target)
    {
        Enemy mine = (Enemy)me;
        if (!mine.isHit)
        {
            manager.ChangeState(new EnemyWalkState(me, manager));
        }
    }
}
