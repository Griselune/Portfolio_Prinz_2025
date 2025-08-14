using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class BaseState
{
    public BaseState(Character c, StateManager manager)
    {
        me = c;
        this.manager = manager;
    }

    public abstract void EnterState();

    public abstract void UpdateState(Character target);

    public abstract void ExitState();

    protected Character me;

    protected StateManager manager;
    
}
