#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "Quadtree.h"

namespace  Game
{
	//タスクに割り当てるグループ名と固有名
	const  string  defGroupName("MainGame");	//グループ名
	const  string  defName("NoName");	//タスク名
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
		DG::Image::SP   scoreFont;
		DG::Image::SP	imgQT;			//クワッドツリーを描画するため
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
		bool  Initialize();	 //「初期化」タスク生成時に１回だけ行う処理
		void  UpDate()			override;	//「実行」１フレーム毎に行う処理
		void  Render2D_AF()		override;	//「2D描画」１フレーム毎に行う処理
		bool  Finalize();	 //「終了」タスク消滅時に１回だけ行う処理
		//変更可◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇◇
	public:
		//追加したい変数・メソッドはここに追加する
		DG::Font::SP TestFont;

		bool IsDebug;		//デバッグ機能を使用するかどうか

		//クワッドツリーの値
		Quadtree* quadTree;
		bool IsQuadtreeOn;	 //クワッドツリーを利用するかどうか（パフォーマンス最適化デモのため）
		bool IsShowQuadtree; //クワッドツリーを描画するかどうか
		ML::Box2D QTsize;	
		int QTlevel;		 //クワッドツリーの深さの初期化
		int QTcnt;			 //クワッドツリーの数（子ノードを含め）


		int squareCnt;		 //オブジェクトの数
	};
}