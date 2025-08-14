#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"

namespace  Enemy001
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("EnemyGroup");	//グループ名
	const  string  defName("Shooter");	//タスク名
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

		DG::Image::SP	imgEnemyDebug, imgEnemy, imgExplosionEnemy;
		string		enemydeathsound[5]{ "enemyDeathSE1", "enemyDeathSE1", "enemyDeathSE1", "enemyDeathSE4", "enemyDeathSE5" }; //5 different pitched sounds because why not
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
		//void  Cannon_Shot(); //imported from player, not used for now

		void  CheckMove(ML::Point& p_);

		//Enemy position, display, state values
		enum class State { Non, Normal, Dead, Attack };
		State		state;
		ML::Vec2	pos;
		ML::Vec2	gravityPull; //influence of the blackhole
		float		rotation;
		ML::Box2D	hitBox;
		ML::Box2D	drawBase;
		ML::Box2D	src, srcDebug, srcDead;
		bool		debug;
		bool		hitFlag; //debug value, useless

		//Enemy stats
		float		speedBase;
		float		speed;
		int			HPMax;
		int			HP;
		int			energyMax;
		int			energy; //can be used to damage force field...
		long		expMax;
		long		exp; //experience points, "long" for scaling possibilities. ** enemy gives exp, player takes **
		float		armorPercentageMax;
		float		armorPercentage;
		int			armorFlatMax;
		int			armorFlat;
		float		shootingSpeed;
		int			collisionDamageMax;
		int			collisionDamage;
		int			shotDamageMax;
		int			shotDamage;

		//Calculation values
		int			moveCnt;
		int			animCnt;
		int			shootingTimer;

		// 5 points hitflag for missile blast, prevent blast from dealing damage more than 5 times - Top left, top right, bottom left, bottom right, center
		bool hitFlags[100][5];

	};
}