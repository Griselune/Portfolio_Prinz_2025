using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct DeployInfo
{
    /// <summary>
    /// 派遣する間隔
    /// </summary>
    public float deployDelay;

    /// <summary>
    /// 派遣する間隔の範囲
    /// </summary>
    public float deployDelayRange;

    /// <summary>
    /// 派遣するカウンター
    /// </summary>
    [HideInInspector]
    public float deployCount;

    /// <summary>
    /// 次に派遣するタイミング
    /// </summary>
    [HideInInspector]
    public float deployTime;

    public void NextDeployTime()
    {
        deployTime = Random.Range(deployDelay - deployDelayRange, deployDelay + deployDelayRange);
    }
}
