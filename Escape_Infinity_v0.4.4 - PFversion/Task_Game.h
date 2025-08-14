#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "Task_Player.h"
#include "Enemy_Generator.h"

namespace  Game
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Scenes");	//グループ名
	const  string  defName("Game");	//タスク名
	//-------------------------------------------------------------------
	class  Resource : public BResource
	{
		bool  Initialize()	override;
		bool  Finalize()	override;
		Resource();
	public:
		~Resource();
		typedef  shared_ptr<Resource>	SP;
		typedef  weak_ptr<Resource>		WP;
		static   WP  instance;
		static  Resource::SP  Create();
		//共有する変数はここに追加する
		

	};
	//-------------------------------------------------------------------
	class  Object : public  BTask
	{
		//変更不可◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆◆
	public:
		virtual  ~Object();
		typedef  shared_ptr<Object>		SP;
		typedef  weak_ptr<Object>		WP;
		//生成窓口 引数はtrueでタスクシステムへ自動登録
		static  Object::SP  Create(bool flagGameEnginePushBack_);
		Resource::SP	res;
	private:
		Object();
		bool  B_Initialize();
		bool  B_Finalize();
		bool  Initialize();	//「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();	//「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		Player::Object::SP			player;      //tracks the player
		EnemyGenerator::Object::SP	enemyGenerator;

		ML::Vec2		gravityPull; //influence of the blackhole
		bool			debugmode;	 //debug mode toggle with select button
		bool			IsPause;
		bool			IsResetData;
		bool			IsChangingGeneratorToSpawn;
		bool			IsGodMode;
		LONGLONG		start;		//gets the current background music timecode to pause/unpause it
		const int		TIMECNTBASE = 0;
		int				timeCnt;	//counts frames
		const int		TIMEWAVEBASE = 0;
		int				timeWave;	//counts frames for deciding the state of the enemy generator
		int				durationWave;

		//Save management---------------------------------------------------------
		struct GameSaveData {
			Player::Object::PlayerSaveData playerData;
			int				timeCnt;	//counts frames
			int				timeWave;	//counts frames for deciding the state of the enemy generator
			bool			IsSpawningDrifter;
			bool			IsSpawningShooter;
		};
		struct GameResetData {
			Player::Object::PlayerResetData playerReset;
			const int		timeCnt = 0;	//counts frames
			const int		timeWave = 0;	//counts frames for deciding the state of the enemy generator
			const bool		IsSpawningDrifter = false;
			const bool		IsSpawningShooter = false;
		};
		void		SaveGame(const std::string& fileName);
		bool		LoadGame(const std::string& fileName);
		float		ReadFloat(std::ifstream& file);
		int 		ReadInt(std::ifstream& file);
		bool		ReadBool(std::ifstream& file);
		long		ReadLong(std::ifstream& file);
		ML::Vec2	ReadVec2(std::ifstream& file);
		void		ResetGame(const std::string& fileName);
		void		DeleteSaveFile(const std::string& fileName);
		//------------------------------------------------------------------------
	};
}
