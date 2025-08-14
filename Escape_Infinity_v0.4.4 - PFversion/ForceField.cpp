#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"
#include  "ForceField.h"


namespace  ForceField
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgForceField = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
	//	this->imgForceField->Rotation(ML::ToRadian(180.f), { 40.f, 20.f });
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgForceField.reset();
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
		this->render2D_Priority[1] = 0.2f;

		this->pos = ML::Vec2(0.0f, 0.0f);
		this->state = State::Non;
		this->hitBox = ML::Box2D(0, 0, 80, 40);
		this->drawBase = ML::Box2D(-40, -20, 80, 40);
		this->src = ML::Box2D(0, 16, 16, 8);

		//★タスクの生成
		//auto debugdisplay = ge->GetTask<DebugDisplay::Object>("Debug", "DebugDisplay");
		//debugdisplay->cannoncnt++;
		//ge->Dbg_ToBox("");

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
		if (this->state == State::Non) {
			return;
		}
	//	else if (this->state == State::Normal) {
			ML::Box2D  draw = this->drawBase;
			ML::Box2D  src = this->src;
			draw.Offset(this->pos.x, this->pos.y);
			this->res->imgForceField->Rotation(ML::ToRadian(180.f), { 40.f, 20.f });
			this->res->imgForceField->Draw(draw, src);
			this->res->imgForceField->Rotation(0, {0, 0}); //14mai2025 reset rotation
	//	}
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