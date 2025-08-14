//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  <time.h> 
#include  <cmath>
#include  "sound.h"
#include  "Square.h"
#include  "randomLib.h"
#include  "Task_Game.h"

namespace Square
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->font = DG::Font::Create("HG丸ゴシックM-PRO", 8, 16);
		this->imgSquare = DG::Image::Create("./data/image/square.png");
		this->imgBoundaries = DG::Image::Create("./data/image/debugrect.png"); //DEBUG - 各オブジェクトに対する画面の壁を表示するため

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->font.reset();
		this->imgSquare.reset();
		this->imgBoundaries.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//「初期化」タスク生成時に１回だけ行う処理
	bool  Object::Initialize()
	{
		//スーパークラス初期化
		__super::Initialize(defGroupName, defName, true);
		//リソースクラス生成orリソース共有
		this->res = Resource::Create();

		//★データ初期化

		this->mass = GetRandom(1.f, 10.0f);	//オブジェクトの重さ
		float scaleSize = 10.f;				//オブジェクトの大きさ：1＝小さい（衝突が少ない）、10＝普通、100＝大き（デバッグに相応しいが、大量に生成したらパフォーマンスが落ちる）
		this->moveVec = ML::Vec2(GetRandom(-3.0f, 3.0f), GetRandom(-3.0f, 3.0f)); //移動ベクトル（速度）
		this->sizeVec = ML::Vec2(pow(mass, 1.f/3.f) * scaleSize, pow(mass, 1.f / 3.f) * scaleSize); //大きさベクトル（現実的に計算されている）
		ML::Vec2 sizeHalf;
		sizeHalf.x = sizeVec.x / 2.f;
		sizeHalf.y = sizeVec.y / 2.f;
		this->hitBase = ML::Box2D(-sizeHalf.x + this->pos.x, -sizeHalf.y + this->pos.y, this->sizeVec.x, this->sizeVec.y); //中心基準の当たり判定ボックス

		this->color = ML::Color(1.f, 1.f, 1.f, 1.f);
		this->boundaries = ML::Box2D(this->hitBase.w / 2.0f //外側の壁
			, this->hitBase.h / 2.0f
			, (float)ge->screen2DWidth - this->hitBase.w
			, (float)ge->screen2DHeight - this->hitBase.h);

		this->moveCnt = 0;
		this->IsOut = false; //壁の外にあるかどうか
		this->IsColliding = false;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//this->moveCnt++;

		//当たり判定ボックスの更新
		UpdateHitBase();

		//予測ベース移動処理
		if (!this->IsCorrected) {
			this->est = this->moveVec;
			MovementPrediction(this->est, this->boundaries);
		}
		else this->IsCorrected = false;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D draw = this->hitBase;
		ML::Box2D src(0.f, 0.f, 400.f, 400.f);
		this->res->imgSquare->Draw(draw, src, this->color);


		///
		///DEBUG - Use with big objects only - 大型オブジェクトにのみ使用してください　( scaleSize >= 100.f )

		//ML::Box2D drawBoundaries = this->boundaries;
		//ML::Box2D srcB(32, 32, 32, 32);
		//this->res->imgBoundaries->Draw(drawBoundaries, srcB);
		
		//auto game = ge->GetTask<Game::Object>("MainGame", "NoName");
		//ML::Box2D textBox(this->hitBase.x + 5, this->hitBase.y + 5, 500, 500);
		//string text = "ID: " + to_string(this->ID)
		//	+ "\n" + "POSx: " + to_string(this->pos.x)
		//	+ "\n" + "POSy: " + to_string(this->pos.y)
		//	+ "\n" + "MVx: " + to_string(this->moveVec.x)
		//	+ "\n" + "MVy: " + to_string(this->moveVec.y)
		//	+ "\n" + "Bndx: " + to_string(this->boundaries.x)
		//	+ "\n" + "Bndy: " + to_string(this->boundaries.y)
		//	+ "\n" + "Bndw: " + to_string(this->boundaries.w)
		//	+ "\n" + "Bndh: " + to_string(this->boundaries.h);
		//if (game->IsDebug) {
		//	this->res->font->Draw(textBox, text, ML::Color(1.0f, 0.0f, 0.0f, 1.0f));
		//}
	}

	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//以下は基本的に変更不要なメソッド
	//★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//-------------------------------------------------------------------
	//タスク生成窓口
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//ゲームエンジンに登録
				
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//イニシャライズに失敗したらKill
			}
			return  ob;
		}
		return nullptr;
	}
	//-------------------------------------------------------------------
	bool  Object::B_Initialize()
	{
		return  this->Initialize();
	}
	//-------------------------------------------------------------------
	Object::~Object() { this->B_Finalize(); }
	bool  Object::B_Finalize()
	{
		auto  rtv = this->Finalize();
		return  rtv;
	}
	//-------------------------------------------------------------------
	Object::Object() {	}
	//-------------------------------------------------------------------
	//リソースクラスの生成
	Resource::SP  Resource::Create()
	{
		if (auto sp = instance.lock()) {
			return sp;
		}
		else {
			sp = Resource::SP(new  Resource());
			if (sp) {
				sp->Initialize();
				instance = sp;
			}
			return sp;
		}
	}
	//-------------------------------------------------------------------
	Resource::Resource() {}
	//-------------------------------------------------------------------
	Resource::~Resource() { this->Finalize(); }
}