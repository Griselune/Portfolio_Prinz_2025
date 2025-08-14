using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StateManager
{
    private BaseState state;

    public void Update(Character target)
    {
        state.UpdateState(target);
    }

    public void ChangeState(BaseState newState)
    {
        if (state != null)
            state.ExitState();

        state = newState;
        state.EnterState();
    }
}
