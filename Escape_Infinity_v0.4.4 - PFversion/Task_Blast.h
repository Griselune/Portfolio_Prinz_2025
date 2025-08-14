#pragma warning(disable:4996)
#pragma once
#include "GameEngine_Ver3_83.h"
#include "Task_Enemy000.h"

namespace  Blast
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("Weapons");	//グループ名
	const  string  defName("Blast");	//タスク名
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
		DG::Image::SP imgBlast;
		string blastSE[4]{ "Blast1", "Blast2", "Blast3", "Blast4" }; //for blast explosion sounds
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
		Resource::SP	res;								//skibidi test
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
	//	void Blast_HitDetection(Enemy000::Object& target_);
		int			ID; //each blast task gets an ID

	//	int  DamageCalc(Blast::Object& blast_, Enemy000::Object& target_) { return 1; };

		float		x;
		float		y;
		int			speed;
		int			animCnt;
		float		damage;
		int			radiusMax;
		int			radius;
		float		distanceCenter; //distance from blast radius to center of target
		ML::Box2D	hitBox;
		ML::Box2D   drawBase;
		ML::Box2D	src;
		enum class	State { Non, Normal, Dead };
		State		state;


		//test camera shake
		int originalposX;
		int originalposY;
	};
}