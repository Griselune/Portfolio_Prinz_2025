using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

public class GManager : MonoBehaviour
{

    //外側から使える
    //参照が絡まってる
    //GManager.instance.任意の関数・変数
    //GManager.GameScene.GameSceneの任意のシーン
    //GManager.GameScene.GetSetSceneで 取得・入れ替え可能GetSetSceneの部分を変えると別の変数も取得入れ替え可能　今のところsceneとscoreのみ
    //GManager.GameScene.SceneChange()　シーンのロードこれでロードすると
    //GManagerから出すものがあればそれの初期化処理を行う
    public enum GameScene
    {
        Title,
        Game,
        GameOver,
        Ending
    }

    public bool sceneChange;

    [SerializeField]
    public GameScene nowScene;
    private GameScene scene;


    public static GManager instance = null;
    public static int score = 0;
    //public bool sceneCnageOK = true;


    private void Awake()
    {
        if (!instance)
        {
            DontDestroyOnLoad(gameObject);
            instance = this;
            scene = nowScene;
            sceneChange = true;
        }
        else
        {
            Destroy(this.gameObject);
        }
    }

    //public static GManager GetSetInstance
    //{
    //    get { return instance; }
    //    private set { }
    //}
    
    public GameScene GetSetScene
    {
        get { return nowScene; }
        set { nowScene = value;}
    }

    //スコアの取得・加算
    public int GetSetScore
    {
        get { return score; }
        set { score += value; }
    }


    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (sceneChange)
        {
            switch (nowScene)
            {
                case GameScene.Title:
                    LoadSceneTitle();
                    Debug.Log("titlestart");
                    break;
                case GameScene.Game:
                    LoadSceneGame();
                    Debug.Log("Gamestart");
                    break; 
                case GameScene.Ending:
                    Debug.Log("Endingstart");
                    LoadSceneEnding();
                    break;
                default:
                    Debug.Log("default");
                    break;
            }
        }

        if (Input.GetKeyDown(KeyCode.Escape))
        {
            //UnityEditor.EditorApplication.isPlaying = false;
            Application.Quit();
        }
    }

    //それぞれのシーンにゲームマネージャーから追加するものの初期化処理
    void LoadSceneTitle()
    {
        if (sceneChange)
        {
            sceneChange = false;
        }
        else
        {
            return;
        }
    }
    void LoadSceneGame()
    {
        if (sceneChange)
        {

            UIManager.uiInstance.AppearScoreUIToGame();
            sceneChange = false;
        }
        else
        {
            return;
        }
    }
    void LoadSceneGameOver()
    {
        if (sceneChange)
        {
            sceneChange = false;
        }
        else
        {
            return;
        }
    }

    void LoadSceneEnding()
    {
        if (sceneChange)
        {
            sceneChange = false;
        }
        else
        {
            return;
        }
    }
    //-------------------------------------------------------------------


    bool ColectScene()
    {
        if(nowScene != scene)
        {
            scene = nowScene;
            return true;
        }
        return false;
    }

    //スコアのリセット
    public void ResetScore()
    {
        score = 0;
    }

    public void SetScore(int score_)
    {
        score = score_;
    }


    //シーンの切り替えを行う、マネージャーから追加したものの削除もここで行う。（あれば）
    public void SceneChange()
    {
        switch (nowScene)
        {
            case GameScene.Title:
                nowScene = GameScene.Game;
                SceneManager.LoadScene("Game");
                Debug.Log("GameLoad");
                break;
            case GameScene.Game:
                nowScene = GameScene.Ending;
                UIManager.uiInstance.DestroyUIToGame();
                SceneManager.LoadScene("EndingScene");
                Debug.Log("EndingLoad");
                break;
            case GameScene.Ending:
                nowScene = GameScene.Title;
                ResetScore();
                SceneManager.LoadScene("Title");
                Debug.Log("titleload");
                break;
            default:
                break;
        }
        sceneChange = true;

    }

}
