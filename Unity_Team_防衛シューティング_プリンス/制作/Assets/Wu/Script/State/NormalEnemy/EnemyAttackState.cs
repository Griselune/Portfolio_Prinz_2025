using System.Collections;
using System.Collections.Generic;
using System.Threading;
using UnityEngine;

public class EnemyAttackState : BaseState
{

    public EnemyAttackState(Character c, StateManager m) : base(c, m)
    {
    }

    public override void EnterState()
    {
        Enemy mine = (Enemy)me;
        mine.GetAnimator().CrossFadeInFixedTime("Attack", 0f);
        mine.PlayAttackSE();
    }

    public override void ExitState()
    {
    }

    public override void UpdateState(Character target)
    {
    }
}
