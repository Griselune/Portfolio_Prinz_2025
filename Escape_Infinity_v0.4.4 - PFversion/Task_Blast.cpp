#include  "MyPG.h"
#include  "sound.h"
#include  "randomLib.h"
#include  "Task_Player.h"
#include  "Task_Enemy000.h"
#include  "Task_Blast.h"
#include  "minwindef.h"


namespace  Blast
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgBlast = DG::Image::Create("./data/image/Objects/Blast_placeholder_v1.png");


		//Blast explosion sound effect
		se::LoadFile(blastSE[0], "./data/Sounds/Weapons/explosionBlast1.wav");
		se::LoadFile(blastSE[1], "./data/Sounds/Weapons/explosionBlast2.wav");
		se::LoadFile(blastSE[2], "./data/Sounds/Weapons/explosionBlast3.wav");
		se::LoadFile(blastSE[3], "./data/Sounds/Weapons/explosionBlast4.wav");

		for (int i = 0; i < _countof(blastSE); i++) {
			se::SetVolume(blastSE[i], 50);
		}

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgBlast.reset();
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
		this->render2D_Priority[1] = 0.53f;

		this->x = 0.f;
		this->y = 0.f;
		this->speed = 4.0f;
		this->radiusMax = 300; //base max size
		this->radius = 10;
		this->animCnt = 0;
		this->damage = 20.f;
		this->distanceCenter = 0.f;
		this->drawBase = ML::Box2D(-5, -5, 10, 10);
		this->src = ML::Box2D(0, 0, 800, 800);
		this->state = State::Normal;

		//test camera shake
		this->originalposX = ge->camera2D.x;
		this->originalposY = ge->camera2D.y;

		//★タスクの生成
		this->ID = ge->blastcnt;
		if (this->ID < 100) {
			ge->blastcnt++;
		}
		else {
			ge->blastcnt = 0;
			this->ID = ge->blastcnt;
		}
		int r = rand() % _countof(this->res->blastSE);
		se::Play(this->res->blastSE[r]);

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		se::AllStop();

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//scan player if the player is not immune to blast - NOT IMPLEMENTED YET
	//	auto playertracker = ge->GetTask<Player::Object>("PlayerGroup", "Player");
	
		//scan for all enemies
		auto enemies = ge->GetTasks<Enemy000::Object>("EnemyGroup");

		if (this->radius >= this->radiusMax) {
			this->Kill();
		}

		// Blast radius expands with time
		if (this->state == State::Normal && this->radius < this->radiusMax) {
			this->speed *= 1.03f;
			this->radius += this->speed;

			// Damage decreases as the radius grows
			if (static_cast<int>(this->radius) % 100 == 0) {
				this->damage *= 0.8f;
			}

			for (auto& enemy : *enemies) {
				float enemyX = enemy->pos.x;
				float enemyY = enemy->pos.y;
				float enemyWidth = enemy->hitBox.w;
				float enemyHeight = enemy->hitBox.h;

				// Precompute squares for distance checks to avoid sqrt() where possible
				float radiusSquared = this->radius * this->radius;

				// Positions of enemy corners and center
				float corners[5][2] = {
					{enemyX, enemyY},                            // Top-left
					{enemyX + enemyWidth, enemyY},              // Top-right
					{enemyX, enemyY + enemyHeight},             // Bottom-left
					{enemyX + enemyWidth, enemyY + enemyHeight}, // Bottom-right
					{enemyX + enemyWidth / 2.0f, enemyY + enemyHeight / 2.0f} // Center
				};

				// Check each corner and center for collisions
				for (int i = 0; i < 5; ++i) {
					float dx = corners[i][0] - this->x;
					float dy = corners[i][1] - this->y;

					// Distance squared comparison for performance
					if ((dx * dx + dy * dy) <= radiusSquared) {
						// Mark this corner/center as hit if not already
						if (!enemy->hitFlags[this->ID][i]) {
							enemy->hitFlags[this->ID][i] = true;
							//using max to ensure HP doesn't go negative
							enemy->HP = max(0, enemy->HP - (this->damage * (1.0f - enemy->armorPercentage)/* - enemy->armorFlat*/));
							enemy->armorFlat -= 1; //reduces the armor because it melts the surface of the enemy -> affects cannon damage
						}
					}
				}
			}
		}


		
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{


		if (this->state == State::Normal) {
		//	ge->dgi->Flush2D();
			//加算合成開始
		//	ge->dgi->EffectState().BS_AlphaAdd();      //bugged -> adds alpha to UI too + changes render prio

			ML::Box2D drawBlast(int(this->x) - int(this->radius), int(this->y) - int(this->radius), int(this->radius * 2), int(this->radius * 2));
			this->res->imgBlast->Draw(drawBlast, this->src, ML::Color(0.3f, 1.0f, 1.0f, 1.0f));
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