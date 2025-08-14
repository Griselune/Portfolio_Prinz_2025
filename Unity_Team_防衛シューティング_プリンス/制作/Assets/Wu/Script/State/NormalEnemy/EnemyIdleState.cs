using System.Collections;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using UnityEngine;

public class EnemyIdleState : BaseState
{
    public EnemyIdleState(Character c, StateManager m) : base(c, m) { }


    public override void EnterState()
    {
        Enemy mine = (Enemy)me;
        mine.GetAnimator().Play("Idle");
        mine.GetComponent<CapsuleCollider2D>().enabled = false;
    }

    public override void ExitState()
    {
        Enemy mine = (Enemy)me;
        mine.GetComponent<CapsuleCollider2D>().enabled = true;
    }

    public override void UpdateState(Character target)
    {
        Enemy mine = (Enemy)me;
        if (mine.isActive)
        {
            manager.ChangeState(new EnemyWalkState(me, manager));
        }
    }
}
