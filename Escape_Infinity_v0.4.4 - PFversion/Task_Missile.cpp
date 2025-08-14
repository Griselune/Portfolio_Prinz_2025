#include  "MyPG.h"
#include  "Task_Missile.h"
#include  "Task_Enemy000.h"
#include  "Task_Blast.h"


namespace  Missile
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgMissile = DG::Image::Create("./data/image/UI/HP_Energy_missile_bar_v3.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgMissile.reset();
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
		this->render2D_Priority[1] = 0.52f;

	//	auto playertracker = ge->GetTask<Player::Object>("PlayerGroup", "Player");

		this->state = State::Non;
		this->hitBox = ML::Box2D(0, 0, 7, 16);
		this->drawBase = ML::Box2D(0, 0, 7, 16);
		this->src = ML::Box2D(0, 69, 7, 16);

		this->x = 0.f;
		this->y = 0.f;
		this->pos = ML::Vec2(0.0f, 0.0f);
		this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->speed = 10.f;


	//	this->cnt = playertracker->missileCoolDownBase;
		this->missileCDBase = 0;
	//	this->missileCDNow = 0;
		this->damage = 0;

		this->missileCDProgress = 16.f; // = height of the sprite in pixel
		this->cnt = 0;

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
	//	auto playertracker = ge->GetTask<Player::Object>("PlayerGroup", "Player");
		this->missileCDProgress = float(this->cnt) / float(this->missileCDBase) * 16.f;



		if (this->state == State::Non) {
			this->src.h = 15;
			this->src.y = 69;
		}


		if (this->state == State::Normal) {
			//Movement is influenced by the blackhole
			this->pos -= this->gravityPull;
			this->pos.y -= this->speed;
			this->src.h = 21;
			this->src.y = 85;
			if (this->pos.y < -20) { this->state = State::Dead; }
			//creates a hitbox copy to check collision
			ML::Box2D me = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
			//scan for all enemies
			auto enemies = ge->GetTasks<Enemy000::Object>("EnemyGroup");
			for (auto it = enemies->begin(); it != enemies->end(); it++) {
				ML::Box2D you = (*it)->hitBox.OffsetCopy((*it)->pos);
				if (true == you.Hit(me) && (*it)->state == Enemy000::Object::State::Normal) {
					//using max to ensure HP doesn't go negative
					(*it)->HP = max(0, (*it)->HP - (this->damage - (*it)->armorFlat));  //damage calc
					auto blast = Blast::Object::Create(true);
					blast->x = this->pos.x;
					blast->y = this->pos.y;
					blast->radiusMax = this->exploRadius;
					this->state = State::Dead;
					return;
				}
			}
		}

		if (this->cnt < this->missileCDBase && (this->state == State::Normal || this->state == State::Dead)) {
			this->cnt++;
		}

		if (this->state == State::Dead && this->cnt >= this->missileCDBase) {
			this->state = State::Non;
		}

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->state == State::Non) {
			//stocked missiles
			ML::Box2D  drawidle(0, 0, 7, 16);
			ML::Box2D  srcidle = this->src;
			drawidle.Offset(this->pos.x, this->pos.y);
			this->res->imgMissile->Draw(drawidle, srcidle);
		}

		if (this->state == State::Normal) {
			//flying missiles
			ML::Box2D drawflying(0, 0, 7, 21);
			ML::Box2D srcflying = this->src;
			drawflying.Offset(this->pos.x, this->pos.y);
			this->res->imgMissile->Draw(drawflying, srcflying);
		}
		else { return; }


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