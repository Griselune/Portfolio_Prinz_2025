#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"

namespace  Upgrades
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Upgrades");	//グループ名
	const  string  defName("Upgrades");	//タスク名
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
		DG::Image::SP imgCool, imgHeal, imgHPRecovery, imgArmPer, imgArmFlat, imgShootSpeed, imgShootTH, imgPierce, imgExploRadius, imgMissileCD, imgShootOverheat, debugrect;
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
		enum class	State { 
			CannonCool,		//bonus - resets overheat to 0
			Heal ,			//bonus - gives back some HP
			ArmorPer,		//Upgrade - boost % armor
			ArmorFlat, 		//Upgrade - boost flat armor
			ShootingSpeed, 	//Upgrade - boost shooting speed
			ShootingTh, 	//Upgrade - makes the overheat threshold higher
			Pierce, 		//Upgrade - gives the bullet the ability to pierce X enemies
			ExploRadius, 	//Upgrade - boost blast maximal radius
			MissileCD, 		//Upgrade - makes the missile reloading faster
			HPRecovery,		//Upgrade - boost HP recovery over time
			Empty			//No upgrade
		};
		State		state;
		ML::Vec2	pos;
	//	ML::Vec2	gravityPull; //influence of the blackhole
		ML::Box2D	hitBox;
		ML::Box2D   drawBase;
		ML::Box2D	src;

		float		rotation;
		bool		debugmode;

	};
}
