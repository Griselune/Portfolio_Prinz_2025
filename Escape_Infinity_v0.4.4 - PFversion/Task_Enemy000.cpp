#include  "MyPG.h"
#include  "sound.h"
#include  "Task_Game.h"
#include  "Task_Enemy000.h"
#include  "Task_Player.h"
#include  "Task_Cannon.h"
#include  "Debug_Display.h"
#include  "Upgrades.h"


namespace  Enemy000
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgEnemyDebug = DG::Image::Create("./data/image/Characters/sprite_enemy_placeholder_64x64_v2.png");
	//	this->imgEnemy = DG::Image::Create("./data/image/Characters/SpaceShooterAssetPack_Ships.png");
		this->imgEnemy = DG::Image::Create("./data/image/Characters/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgExplosionEnemy = DG::Image::Create("./data/effect/64x64_effect3_bomb1.png");
		

		//sound effects
		se::LoadFile("ContactWithPlayer", "./data/Sounds/Chara/enemyContact.wav");
		se::SetVolume("ContactWithPlayer", 60);

		se::LoadFile(this->enemydeathsound[0], "./data/Sounds/Chara/enemyDeath1.wav");
		se::LoadFile(this->enemydeathsound[1], "./data/Sounds/Chara/enemyDeath2.wav");
		se::LoadFile(this->enemydeathsound[2], "./data/Sounds/Chara/enemyDeath3.wav");
		se::LoadFile(this->enemydeathsound[3], "./data/Sounds/Chara/enemyDeath4.wav");
		se::LoadFile(this->enemydeathsound[4], "./data/Sounds/Chara/enemyDeath5.wav");
		//sets volume for every death sound effects variants
		for (int i = 0; i < _countof(enemydeathsound); i++) {
			se::SetVolume(this->enemydeathsound[i], 60);
		}
		
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgEnemy.reset();
		this->imgEnemyDebug.reset();
		this->imgExplosionEnemy.reset();
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
		//Enemy position, display, state values
		this->state = State::Normal;
		this->pos = ML::Vec2(1280.f / 2.f, 200.f);
		this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->rotation = 0.f;
		this->hitBox = ML::Box2D(-32, -32, 64, 64);
		this->drawBase = ML::Box2D(-32, -32, 64, 64);
		this->src = ML::Box2D(16, 16, 16, 16);
		this->srcDebug = ML::Box2D(0, 0, 64, 64);
		this->srcDead = ML::Box2D(0, 0, 64, 64);
		this->debug = false;
		this->hitFlag = false;

		//Enemy stats
		this->speedBase = 1.0f;
		this->speed = this->speedBase;
		this->HPMax = 100;
		this->HP = this->HPMax;
		this->armorPercentageMax = 0.1f;
		this->armorPercentage = this->armorPercentageMax;
		this->armorFlatMax = 3;
		this->armorFlat = this->armorFlatMax;
		this->energyMax = 100;
		this->energy = this->energyMax;
		this->expMax = 1;
		this->exp = this->expMax;
		this->collisionDamageMax = 20;
		this->collisionDamage = this->collisionDamageMax;

		//Calculation values
		this->moveCnt = 0;
		this->animCnt = 0;

		//resets hitflags for blast
		for (int i = 0; i < 100; ++i) {
			for (int j = 0; j < 5; ++j) {
				hitFlags[i][j] = false; // Reset all flags to false
			}
		}

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		if (this->state == State::Non) {
	//		auto upgrade = Upgrades::Object::Create(true);
	//		upgrade->pos = this->pos;
			return true;
		}

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//to switch to debug mode display
		auto getdebugmodetask = ge->GetTasks<Game::Object>("Scenes", "Game");
		for (auto getdebugmode = getdebugmodetask->begin(); getdebugmode != getdebugmodetask->end(); getdebugmode++) {
			this->debug = (*getdebugmode)->debugmode;
		}

		//tracks the player to interact with them
		auto playertracker = ge->GetTask<Player::Object>("PlayerGroup", "Player");

		//Movement is influenced by the blackhole
		this->pos -= this->gravityPull;

		//Object movement
		this->pos.y += this->speed;

		if (this->pos.y > 800) { this->Kill(); return; }
		//Checks if movement is valid
		//this->CheckMove(this->pos); //needs to change

		if (this->state == State::Normal) {
			//Collision check
			//creates a hitbox copy to check collision
			ML::Box2D me = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
			ML::Box2D you = playertracker->hitBox.OffsetCopy(playertracker->pos);
			if (true == you.Hit(me) && playertracker->state == Player::Object::State::Normal && playertracker->godmode == false) {
				playertracker->iFrame = 60;
				if (playertracker->state == Player::Object::State::Normal) {
					playertracker->state = Player::Object::State::Hit;
					//using max to ensure HP doesn't go negative
					playertracker->HP = max(0, playertracker->HP - (this->collisionDamage * (1.0f - playertracker->armorPercentage) - playertracker->armorFlat));  //damage calc
				}
				se::Play("ContactWithPlayer");
				this->state = State::Dead;
			}
		}
		
		//--------------------------------------------------------
					//death animation
		if (this->state == State::Dead) {



				// Calculate the frame index for the 40-frame animation
				int frameIndex = this->animCnt * 2;

				// Calculate the row and column of the sprite sheet
				this->srcDead.x = (frameIndex % 10) * 64; // 10 rows
				this->srcDead.y = (frameIndex / 10) * 64; // 4 columns (0–3)

				// Increment animation counter
				this->animCnt++;

				// End the animation after 40 frames
				if (this->animCnt >= 40) { // 60 frames at 1.5 increment per frame = 40 frames
					this->state = State::Non;
					this->Kill();
					return;
				}
		}

		//death trigger
		if (this->HP <= 0 && this->state == State::Normal) {
			this->state = State::Dead;
			playertracker->currentEXP += this->exp;
			playertracker->totalEXP += this->exp;
			int r = rand() % _countof(this->res->enemydeathsound);
			se::Play(this->res->enemydeathsound[r]);
			auto upgrade = Upgrades::Object::Create(true);
			upgrade->pos = this->pos;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw = this->drawBase.OffsetCopy(this->pos.x, this->pos.y);

		switch (this->state) {
		case State::Non: return;
		case State::Normal: 
			if (debug) {
				this->res->imgEnemyDebug->Draw(draw, this->srcDebug,
					ML::Color(1.0f, 1.0f, float(float(this->HP) / float(this->HPMax)), float(float(this->HP) / float(this->HPMax))));
			}
			else {
				this->res->imgEnemy->Rotation(ML::ToRadian(this->rotation), { 32.f, 32.f }); //randomized rotation of the sprite
				this->res->imgEnemy->Draw(draw, this->src,
					ML::Color(1.0f, 1.0f, float(float(this->HP) / float(this->HPMax)), float(float(this->HP) / float(this->HPMax))));
				this->res->imgEnemy->Rotation(0, { 0, 0 }); //reset rotation
			} break;
		case State::Dead: 
			//scale up the explosion effect
			draw.w *= 2;
			draw.h *= 2;
			//center the explosion effect
			draw.x -= 32;
			draw.y -= 32;

			this->res->imgExplosionEnemy->Draw(draw, this->srcDead); break;
		}
	}

	//
	
	void  Object::CheckMove(ML::Point& p_)
	{
		if (p_.x - src.w / 2 < 370) { p_.x = 370 + src.w / 2; }
		if (p_.x + src.w / 2 > 1280 - 370) { p_.x = 1280 - 370 - src.w / 2; }
		if (p_.y < 0) { p_.y = 0; }
		if (p_.y > 720) { p_.y = 720; }
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