using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponEnemyAttackState : BaseState
{
    public WeaponEnemyAttackState(Character c, StateManager m) : base(c, m)
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
