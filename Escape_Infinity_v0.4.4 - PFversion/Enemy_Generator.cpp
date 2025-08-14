#include  "MyPG.h"
#include  "randomLib.h"
#include  "sound.h"
#include  "Task_Enemy000.h"
#include  "Task_Enemy001.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"
#include  "Enemy_Generator.h"


namespace  EnemyGenerator
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
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
		this->render2D_Priority[1] = 1.f;

		this->state = State::Stop;
		this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->timeCnt = 0;
		this->drifterTimeCntMax = 60;
		this->drifterTimeCnt = this->drifterTimeCntMax;
		this->shooterTimeCntMax = 60;
		this->shooterTimeCnt = this->shooterTimeCntMax;

		this->IsSpawningDrifter = true;
		this->IsSpawningShooter = true;
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
		switch (this->state) {
		case State::Spawn:	
			this->drifterTimeCnt++;
			this->shooterTimeCnt++;
		//	this->timeCnt++; //rework in progress
			//spawns "drifters" enemy
			if (this->drifterTimeCnt % this->drifterTimeCntMax == 0 && this->IsSpawningDrifter) {
				auto drifter = Enemy000::Object::Create(true);
				drifter->pos = ML::Vec2(GetRandom(400.f, 900.f), -50.f);
				drifter->speed = GetRandom(0.2f, 2.0f);
				drifter->gravityPull = this->gravityPull;
				drifter->rotation = GetRandom(-180.0f, +180.0f);

				//each x seconds, enemy stats increases
				drifter->HPMax += (this->timeCnt / 600) * 10; 
				drifter->HP = drifter->HPMax;
				drifter->collisionDamage += (this->timeCnt / 600) * 5;
				drifter->exp += (this->timeCnt / 600) * 1;
				drifter->armorPercentage = min(static_cast<float>((this->timeCnt / 600)) * 0.02f, 1.0f);
				drifter->armorFlat += (this->timeCnt / 600) * 1;
			}
			//spawns "shooters" enemy
			if (this->shooterTimeCnt % this->shooterTimeCntMax == 0 && this->IsSpawningShooter) {
				auto shooter = Enemy001::Object::Create(true);
				shooter->pos = ML::Vec2(GetRandom(400.f, 900.f), -50.f);
				shooter->speed = GetRandom(0.2f, 2.0f);
				shooter->shootingTimer = GetRandom(0, 30);
				shooter->gravityPull = this->gravityPull;

				//each x seconds, enemy stats increases
				shooter->HPMax += (this->timeCnt / 600) * 10;
				shooter->HP = shooter->HPMax;
				shooter->collisionDamage += (this->timeCnt / 600) * 5;
				shooter->shotDamage += (this->timeCnt / 1200) * 1;
				shooter->exp += (this->timeCnt / 600) * 2;
				shooter->armorPercentage = min(static_cast<float>((this->timeCnt / 600)) * 0.02f, 1.0f);
				shooter->armorFlat += (this->timeCnt / 600) * 1;
			}
			break;

		case State::Stop: return; break;
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		
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