#include  "MyPG.h"
#include  "Cannon_Enemy.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"


namespace  CannonEnemy
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCannonShot = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");


		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgCannonShot.reset();
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
		this->render2D_Priority[1] = 0.7f;

		this->pos = ML::Vec2(0.0f, 0.0f);
		this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->state = State::Normal;
		this->hitBox = ML::Box2D(0, 0, 8, 8);
		this->drawBase = ML::Box2D(-4, -4, 8, 8);
		this->src = ML::Box2D(16, 8, 8, 8);

		this->speed = 20.f;
		this->damage = 4;
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
		//Movement is influenced by the blackhole
		this->pos -= this->gravityPull;

		this->pos.y += this->speed;
		if (this->pos.y > 800) { this->Kill(); }

		//**************当たり判定****************//

		//creates a hitbox copy to check collision
		ML::Box2D me = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
		//Gets the player task to create a copy of it's hitbox
		auto player = ge->GetTask<Player::Object>("PlayerGroup", "Player");
		ML::Box2D you = player->hitBox.OffsetCopy(player->pos);

		if (you.Hit(me) && player->state == Player::Object::State::Normal && player->godmode == false) {
			if (player->IsForceField) {
				player->energy = max(0, player->energy - 5.f);
				this->Kill();
				return;
			}
			player->HP = max(0, player->HP - (this->damage - player->armorFlat));
			player->iFrame = 30; //gives the player a short invicibility frame
			player->state = Player::Object::State::Hit;
			this->Kill();
			return;
		}


		//**************************************//
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{

		ML::Box2D  draw = this->drawBase;
		ML::Box2D  src = this->src;
		draw.Offset(this->pos.x, this->pos.y);
		this->res->imgCannonShot->Draw(draw, src);
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
		//	auto debugdisplay = ge->GetTask<DebugDisplay::Object>("Debug", "DebugDisplay");
		//	debugdisplay->cannoncnt--;
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