#include  "MyPG.h"
#include  "Task_Cannon.h"
#include  "Task_Enemy000.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"


namespace  Cannon
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCannonShot = DG::Image::Create("./data/image/Objects/sprite_bullets_placeholder_64x64_v1.png");


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
		this->hitBox = ML::Box2D(0, 0, 4, 8);
		this->drawBase = ML::Box2D(0, 0, 4, 8);
		this->src = ML::Box2D(1, 1, 1, 2);

		this->speed = 20.f;
		this->damage = 10;
		this->pierce = 0; //gets it's real value from player when task is created
		this->currentEnemy = nullptr; // Pointer to the enemy the bullet is currently interacting with (if any)
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

		this->pos.y -= this->speed;
		if (this->pos.y < -20) { this->Kill(); }

														//**************当たり判定****************//

		//creates a hitbox copy to check collision
		ML::Box2D me = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);

				// Scan for all enemies
		auto enemies = ge->GetTasks<Enemy000::Object>("EnemyGroup");

		for (auto it = enemies->begin(); it != enemies->end(); it++) {
			std::shared_ptr<Enemy000::Object> enemy = *it; // Dereference shared_ptr
			ML::Box2D you = enemy->hitBox.OffsetCopy(enemy->pos);

			if (this->state == State::Normal) {
				// If in Normal state, check for a new collision
				if (you.Hit(me) && enemy->state == Enemy000::Object::State::Normal) {
					//Apply damage only once
					//using max to ensure HP doesn't go negative
					enemy->HP = max(0, enemy->HP - max(0, this->damage - enemy->armorFlat));
					enemy->armorPercentage -= 0.01f; //reduces armor -> affects blast damage

					if (this->pierce > 0) {
						this->pierce--;
						this->state = State::Non;    // Transition to Non state to avoid further collision with the same enemy
						currentEnemy = enemy;       // Track the current enemy
					}
					else {
						this->Kill();                // Destroy bullet if no pierce is left
						return;                      // Exit since the bullet is destroyed
					}
				}
			}
			else if (this->state == State::Non) {
				// In Non state, check if still colliding with the current enemy
				if (currentEnemy == enemy) {
					if (!you.Hit(me)) {
						// Once no longer colliding with the current enemy
						currentEnemy = nullptr;    // Clear the current enemy
						this->state = State::Normal; // Reset to Normal state
					}
				}
			}
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