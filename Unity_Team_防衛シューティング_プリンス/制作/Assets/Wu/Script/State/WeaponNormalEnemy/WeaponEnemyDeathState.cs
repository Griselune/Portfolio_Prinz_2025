using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WeaponEnemyDeathState : BaseState
{
    public WeaponEnemyDeathState(Character c, StateManager m) : base(c, m) { }

    public override void EnterState()
    {
        Enemy mine = (Enemy)me;
        mine.GetAnimator().CrossFadeInFixedTime("Death Skeleton", 0f);
        mine.PlayDeathSE();
    }

    public override void ExitState()
    {
    }

    public override void UpdateState(Character target)
    {

    }
}
