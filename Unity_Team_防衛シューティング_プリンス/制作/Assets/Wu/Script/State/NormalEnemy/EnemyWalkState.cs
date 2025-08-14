using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyWalkState : BaseState
{
    public EnemyWalkState(Character c, StateManager m) : base(c, m) { }

    public override void EnterState()
    {
        Enemy mine = (Enemy)me;
        mine.GetAnimator().CrossFadeInFixedTime("Walk", 0);
    }

    public override void ExitState()
    {

    }

    public override void UpdateState(Character target)
    {
        Enemy mine = (Enemy)me;
        // 攻撃
        if (mine.isAttack)
        {
            manager.ChangeState(new EnemyAttackState(me, manager));
            return;
        }

        // 移動
        if (mine.IsAlive())
        {
            mine.Move();
        }

    }
}
