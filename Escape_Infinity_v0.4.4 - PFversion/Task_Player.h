#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"
#include "Task_Missile.h"
#include "ForceField.h"

namespace  Player
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("PlayerGroup");	//グループ名
	const  string  defName("Player");	//タスク名
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
		DG::Image::SP	imgPlayer, imgPlayerDeath, imgFlames, imgDebug;
		string	cannonSE[3]{ "DPcannon", "cannon", "UPcannon" }; //for cannon sound, enables different pitches
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


	//	bool  CheckCollision(ML::Point& pre_, ML::Box2D hit_, ML::Box2D src_);  //bugged
		XI::GamePad::SP		controller;
		bool				debugmode;
		bool				godmode;
		bool				IsForceField;

		Missile::Object::SP leftmissile;
		Missile::Object::SP rightmissile;

		ForceField::Object::SP forcefield;

		enum class State { Non, Normal, Dead, Hit };
		State		state;
		ML::Vec2	pos;
		ML::Vec2	gravityPull; //influence of the blackhole over pos
	//	ML::Vec2		pre;
		bool		hitFlag; //used for debug for now
		ML::Box2D	hitBox;
		ML::Box2D	drawBase;
		ML::Box2D	drawFlames;
		ML::Box2D	src;
		ML::Box2D	srcFlames;

		//Player stats
		float		speedBase;
		float		speed; 
		float		HPBase;
		float		HP;
		float		HPRecoveryBase;
		float		HPRecovery;
		int			HPRecoveryTimerBase;
		int			HPRecoveryTimer;
		float		energyBase;
		float		energy;
		long		totalEXP; //experience points, "long" for scaling possibilities. ** enemy gives exp, player takes **
		int			currentEXP;
		int			level;
		float		armorPercentageBase;
		float		armorPercentage;
		int			armorFlatBase;
		int			armorFlat;
	//	int			damageValue;//base damage from this object -> possibility for collision damage upgrade?

		//Cannon values
		float		shootingSpeedBase; //controls the player's fire rate : small number = fast
		float		shootingSpeed;
		int			shootingOverHeatThresholdBase; // if reached, cannon overheats and firerate starts to slow down
		int			shootingOverHeatThreshold;
		int			pierceBase; //つらぬき　－　for cannon piercing upgrade
		int			pierce; 
		int			cannonDmgBase;
		int			cannonDmg;

		//Missile and Blast values
		int			exploRadiusBase; //for missile explosion max radius
		int			exploRadius;
		float		exploDmgBase;
		float		exploDmg;
		int			missileCoolDownBase;
		int			missileCoolDown;

		//Calculation values
		int			moveCnt;
		int			animCnt;
		int			shootingTimer;
		float		shootingOverHeat;
		float		shootingOverHeatProgress; //for overheat gauge display
		int			iFrame;
	//	int			combo; //killed enemies combo ? unused
	//	int			comboTimer; //unused for now

		void  Cannon_Shot();
		void  CheckMove(ML::Vec2& pre_);

		//***************************************Save file management : saves important gameplay data every ennemy wave************************************//
		struct PlayerSaveData {
			//player stats data to save
			float		speed;
			float		HP;
			float		HPRecovery;//
			int			HPRecoveryTimer;//
			float		energy;
			long		totalEXP; //experience points, "long" for scaling possibilities. ** enemy gives exp, player takes **
			int			currentEXP;
			int			level;
			float		armorPercentage;
			int			armorFlat;

			//cannon related data to save
			float		shootingSpeed;
			int			shootingOverHeatThreshold;
			int			pierce; //つらぬき　－　for cannon piercing upgrade
			int			cannonDmg;

			//missile related data to save
			int			exploRadius;
			float		exploDmg;
			int			missileCoolDown;
		};
		void SaveData(PlayerSaveData& data);
		void LoadData(const PlayerSaveData& data);


		//reset data - WIP
		struct PlayerResetData{
			//player stats data to save
			float		speedBase;
			float		HPBase;
			float		HPRecoveryBase;//
			int			HPRecoveryTimerBase;//
			float		energyBase;
			long		totalEXP; //experience points, "long" for scaling possibilities
			int			currentEXP;
			int			level;
			float		armorPercentageBase;
			int			armorFlatBase;

			//cannon related data to save
			float		shootingSpeedBase;
			int			shootingOverHeatThresholdBase;
			int			pierceBase; //つらぬき　－　for cannon piercing upgrade
			int			cannonDmgBase;

			//missile related data to save
			int			exploRadiusBase;
			float		exploDmgBase;
			int			missileCoolDownBase;
		};
		void ResetData(PlayerResetData& data);
		
		//*****************************************************************************************************************************************************//
	};
}