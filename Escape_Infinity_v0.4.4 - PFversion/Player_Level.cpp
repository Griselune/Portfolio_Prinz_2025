#include  "MyPG.h"
#include  "Player_Level.h"
#include  "Task_Player.h"

namespace  PlayerLevel
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgPlayerLevelUnits = DG::Image::Create("./data/image/UI/font_text.png");
		this->imgPlayerLevelTens = DG::Image::Create("./data/image/UI/font_text.png");
		this->imgPlayerLevelHundreds = DG::Image::Create("./data/image/UI/font_text.png");
		this->imgPlayerLevelThousands = DG::Image::Create("./data/image/UI/font_text.png");

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgPlayerLevelUnits.reset();
		this->imgPlayerLevelTens.reset();
		this->imgPlayerLevelHundreds.reset();
		this->imgPlayerLevelThousands.reset();

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
		this->render2D_Priority[1] = 0.09f;

		this->srcBase = ML::Box2D(0, 95, 32, 32);
		this->drawBase = ML::Box2D(0, 0, 8, 8);
		this->x = 1040;
		this->y = 225;
		this->scale = 1;
		this->pLevel = 0;

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

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		auto getplayerlevel = ge->GetTask<Player::Object>("PlayerGroup", "Player");
		this->pLevel = getplayerlevel->level;

		ML::Box2D draw = this->drawBase;
		draw.w *= this->scale;
		draw.h *= this->scale;

		ML::Box2D drawTh = draw;
		ML::Box2D drawHu = draw;
		ML::Box2D drawTe = draw;
		ML::Box2D drawUn = draw;
		drawTh.Offset(this->x, this->y);
		drawHu.Offset(this->x + (this->drawBase.w * this->scale), this->y);
		drawTe.Offset(this->x + (this->drawBase.w * 2 * this->scale), this->y);
		drawUn.Offset(this->x + (this->drawBase.w * 3 * this->scale), this->y);

		ML::Box2D src = this->srcBase;

		ML::Box2D srcTh = src;
		ML::Box2D srcHu = src;
		ML::Box2D srcTe = src;
		ML::Box2D srcUn = src;
		srcTh.x = this->srcBase.w * ((pLevel % 10000) / 1000);
		srcHu.x = this->srcBase.w * ((pLevel % 1000) / 100);
		srcTe.x = this->srcBase.w * ((pLevel % 100) / 10);
		srcUn.x = this->srcBase.w * (pLevel % 10);


		this->res->imgPlayerLevelThousands->Draw(drawTh, srcTh);
		this->res->imgPlayerLevelHundreds->Draw(drawHu, srcHu);
		this->res->imgPlayerLevelTens->Draw(drawTe, srcTe);
		this->res->imgPlayerLevelUnits->Draw(drawUn, srcUn);

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