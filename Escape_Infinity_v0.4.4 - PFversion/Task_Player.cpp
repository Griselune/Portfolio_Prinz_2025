#include  "MyPG.h"
#include  "Task_Game.h"
#include  "Task_Player.h"
#include  "Task_Cannon.h"
#include  "Task_Missile.h"
#include  "Upgrades.h"
#include  "sound.h"
#include  "Debug_Display.h"

namespace  Player
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgPlayer = DG::Image::Create("./data/image/Characters/sprite_ship_placeholder_64x64_v1.png");
		this->imgPlayerDeath = DG::Image::Create("./data/effect/explosionplayer.png");
		this->imgFlames = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgDebug = DG::Image::Create("./data/effect/debugrect.png");

		//sound effects
		//level up sound
		se::LoadFile("levelUp", "./data/Sounds/UI/reformed/cut-mixkit-quick-positive-video-game-notification-interface-265.wav");
		se::SetVolume("levelUp", 80);
		 
		//player death sound effect
		se::LoadFile("playerdeath", "./data/Sounds/UI/reformed/base_cut_-system-break-2942.wav");
		se::SetVolume("playerdeath", 85);

		//cannon sound effect
		se::LoadFile(cannonSE[0], "./data/Sounds/Weapons/Reformed/8bit_1downpitch_laser_gun.wav");
		se::LoadFile(cannonSE[1], "./data/Sounds/Weapons/Reformed/8bit_0pitch_laser_gun.wav");
		se::LoadFile(cannonSE[2], "./data/Sounds/Weapons/Reformed/8bit_1uppitch_laser_gun.wav");

		for (int i = 0; i < _countof(cannonSE); i++) {
			se::SetVolume(cannonSE[i], 50);
		}

		//missile sound effect
		se::LoadFile("missileSoundRight", "./data/Sounds/Weapons/missile_sound.wav");
		se::SetVolume("missileSoundRight", 55);
		se::LoadFile("missileSoundLeft", "./data/Sounds/Weapons/missile_sound.wav");
		se::SetVolume("missileSoundLeft", 55);

		//*****//



		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgPlayer.reset();
		this->imgPlayerDeath.reset();
		this->imgFlames.reset();
		this->imgDebug.reset();

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
		this->debugmode = false;
		this->godmode = false;
		this->IsForceField = false;

		//★データ初期化
		this->controller = ge->in1;
		this->state = State::Normal;
		this->render2D_Priority[1] = 0.6f;
		this->pos = ML::Vec2(1280.f / 2.f, 600.f);
	//	this->pre = { 0.f, 0.f };
		this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->hitFlag = false;
		this->hitBox = ML::Box2D(-32, -32, 64, 64);
		this->drawBase = ML::Box2D(-32, -32, 64, 64);
		this->drawFlames = ML::Box2D(-8, 20, 16, 16);
		this->src = ML::Box2D(0, 0, 64, 64);
		this->srcFlames = ML::Box2D(40, 24, 8, 8);
		
		//Player stats
		this->speedBase = 10.0f;
		this->speed = this->speedBase;
		this->HPBase = 100.f;
		this->HP = this->HPBase;
		this->HPRecoveryBase = 0.0f;
		this->HPRecovery = this->HPRecoveryBase;
		this->HPRecoveryTimerBase = 30;
		this->HPRecoveryTimer = this->HPRecoveryTimerBase;
		this->energyBase = 100.f;
		this->energy = this->energyBase;
		this->totalEXP = 0;
		this->currentEXP = 0;
		this->level = 1;
		this->armorPercentageBase = 0.3f;
		this->armorPercentage = this->armorPercentageBase;
		this->armorFlatBase = 4;
		this->armorFlat = this->armorFlatBase;

		//Cannon values
		this->shootingSpeedBase = 7.0f;  //controls the player's fire rate : small number = fast - base is 12
		this->shootingSpeed = this->shootingSpeedBase;
		this->shootingOverHeatThresholdBase = 25;  // if reached, cannon overheats and firerate starts to slow down - base is 25
		this->shootingOverHeatThreshold = shootingOverHeatThresholdBase;
		this->pierceBase = 1;	//cannon pierces enemies. Base value to set at 0, goes up with upgrades
		this->pierce = this->pierceBase;
		this->cannonDmgBase = 10;
		this->cannonDmg = this->cannonDmgBase;

		//Missile and Blast values
		this->exploRadiusBase = 200; //size of the explosion from the missile - base is 100
		this->exploRadius = this->exploRadiusBase;
		this->exploDmgBase = 20.f;
		this->exploDmg = this->exploDmgBase;
		this->missileCoolDownBase = 300; //base is 300
		this->missileCoolDown = this->missileCoolDownBase;

		//Calculation values
		this->moveCnt = 0;
		this->animCnt = 0;
		this->shootingTimer = 2;
		this->shootingOverHeat = 0.0f;
		this->shootingOverHeatProgress = 0.0f;
		this->iFrame = 0;
		
		//★タスクの生成
		leftmissile = Missile::Object::Create(true);
		leftmissile->cnt = this->missileCoolDown;
		leftmissile->missileCDBase = this->missileCoolDownBase;
		rightmissile = Missile::Object::Create(true);
		rightmissile->cnt = this->missileCoolDown;
		rightmissile->missileCDBase = this->missileCoolDownBase;

		forcefield = ForceField::Object::Create(true); //Force field ability : using energy, protects from enemy projectiles
		forcefield->pos = this->pos;

		//anti lag - if removed, lag spikes at the first bullet shot	//**I guess it fixed itself somehow**//
		/*se::SetVolume(res->cannonSE[0], 0);
		se::Play(res->cannonSE[0]);
		se::Stop(res->cannonSE[0]);
		se::SetVolume(res->cannonSE[0], 50);*/

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
	//	if (this->controller) {										
			auto  inp = this->controller->GetState();

			//Movement is influenced by the blackhole
			this->pos -= this->gravityPull;

			//HP recovery over time
			if (this->state != State::Non && this->HP < this->HPBase) {
				if (this->HPRecoveryTimer == 0) {
					this->HPRecoveryTimer = this->HPRecoveryTimerBase;
					this->HP = min(this->HP + this->HPRecovery, this->HPBase);
				}
				this->HPRecoveryTimer--;
			}

			//Invulnerability frame after a collision because the programmer is very kind
			if (this->iFrame > 0) {
				this->iFrame--;
				this->hitFlag = true;
			}
			if(this->iFrame == 0 && this->state == State::Hit) {
				this->state = State::Normal;
				this->hitFlag = false;
			}

			//controls the firing rate of the gun
			if (this->shootingTimer > 0) {
				this->shootingTimer--;
			}

			//controls player's experience points and levels
			if (this->currentEXP >= 100) {
				this->level++;
				this->cannonDmg += 1;					//game balance test - each level, cannon damages goes up by 1
			//	this->armorPercentage += 0.01f;
				this->armorPercentage = min(this->armorPercentage + 0.01f, 1.0f);
				this->currentEXP -= 100;
				se::Play("levelUp");
			}


			//Analog movement via LStick
			float angle = inp.LStick.angleDYP;
			pos.x += float(cos(angle) * inp.LStick.volume * this->speed);
			pos.y -= float(-sin(angle) * inp.LStick.volume * this->speed);
			this->CheckMove(pos);
			

			//Create a force field that protects the player from projectiles. Cannot attack while the force field is deployed
			if (inp.B2.on && this->energy > 0) {
				this->IsForceField = true;
				forcefield->pos.x = this->pos.x;
				forcefield->pos.y = this->pos.y - 20.f;
				forcefield->state = ForceField::Object::State::Normal;
				this->energy = max(this->energy - 0.1f, 0);     //base energy conssumption is 0.1f !!debug -> set to 0!!
			}
			else {
				this->IsForceField = false;
				this->energy = min(this->energy + 0.05f, this->energyBase);
				forcefield->state = ForceField::Object::State::Non;
			}

			//Shoots bullets in a straight line using the cannon
			if (inp.B1.on /*&& this->IsForceField == false*/) {
				this->Cannon_Shot();
			}
			else if (this->shootingOverHeat > 0) { this->shootingOverHeat--; }

			//for the overheat indicator
			this->shootingOverHeatProgress = (this->shootingOverHeat / (static_cast<float>(this->shootingOverHeatThreshold)) * 2.5f); // "*(size of the overheat indicator image).f"

			//---------------------------------------
			//Missile management

				//left missile resting
			if(leftmissile->state == Missile::Object::State::Non){
				//aligns the missile where it should be
				leftmissile->pos.x = this->pos.x - 34.f;
				leftmissile->pos.y = this->pos.y + 41.f - 34.f;
				if (inp.L1.down) { //shoots the left missile
					leftmissile->state = Missile::Object::State::Normal;
					leftmissile->cnt = 0; //sets counter to 0 for reloading time
					leftmissile->missileCDBase = this->missileCoolDown;
					leftmissile->exploRadius = this->exploRadius;
					leftmissile->damage = this->exploDmg;
					se::Play("missileSoundLeft");
				}
			}
				//right missile resting
			if (rightmissile->state == Missile::Object::State::Non) {
				//aligns the missile where it should be
				rightmissile->pos.x = this->pos.x + this->drawBase.w + 1.f - 32.f - 6.f;
				rightmissile->pos.y = this->pos.y + 41.f - 34.f;
				if (inp.R1.down) { //shoots the right missile
					rightmissile->state = Missile::Object::State::Normal;
					rightmissile->cnt = 0; //sets counter to 0 for reloading time
					rightmissile->missileCDBase = this->missileCoolDown;
					rightmissile->exploRadius = this->exploRadius;
					rightmissile->damage = this->exploDmg;
					se::Play("missileSoundRight");
				}
			}

			//stop the sound effect when the missile dies
			if (rightmissile->state == Missile::Object::State::Dead) {
				se::Stop("missileSoundRight");
			}
			if (leftmissile->state == Missile::Object::State::Dead) {
				se::Stop("missileSoundLeft");
			}

			//Upgrade object collision
			ML::Box2D me = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
			auto upgrade = ge->GetTasks<Upgrades::Object>("Upgrades");
			for (auto it = upgrade->begin(); it != upgrade->end(); it++) {
				ML::Box2D you = (*it)->hitBox.OffsetCopy((*it)->pos);
				if (you.Hit(me) && this->state != State::Non) {
					switch ((*it)->state) {
					case Upgrades::Object::State::CannonCool:	this->shootingOverHeat = 0; (*it)->Kill();											
						break;//bonus - resets overheat to 0 - no limit

					case Upgrades::Object::State::Heal:			this->HP = min(this->HP + 5.0f, 100.f); (*it)->Kill();	
						break;//bonus - gives back some HP - base / max is 100

					case Upgrades::Object::State::ArmorPer:		this->armorPercentage = min(this->armorPercentage += 0.05f, 1.0f);	(*it)->Kill();
						break;//Upgrade - boost % armor - base is 0.3f

					case Upgrades::Object::State::ArmorFlat: 	this->armorFlat += 1;	(*it)->Kill();			
						break;//Upgrade - boost flat armor - base is 4

					case Upgrades::Object::State::ShootingSpeed:this->shootingSpeed = max(1.0f, this->shootingSpeed - 0.5f); (*it)->Kill();	
						break;//Upgrade - boost shooting speed - base value is 12.0f - less is better

					case Upgrades::Object::State::ShootingTh: 	this->shootingOverHeatThreshold += 10; this->cannonDmg += 2;	(*it)->Kill();  //test game balance
						break;//Upgrade - makes the overheat threshold higher - base is 10 - higher is better

					case Upgrades::Object::State::Pierce: 		this->pierce += 1;	(*it)->Kill();			
						break;//Upgrade - gives the bullet the ability to pierce X enemies - base is 0

					case Upgrades::Object::State::ExploRadius:	this->exploRadius += 25; this->exploDmg += 5.f;	(*it)->Kill(); //test game balance
						break;//Upgrade - boost blast maximal radius - base is 100 - higher is better

					case Upgrades::Object::State::MissileCD: 	this->missileCoolDown = max(10, this->missileCoolDown - 25);	(*it)->Kill();
						break;//Upgrade - makes the missile reloading faster - base is 300 - less is better

					case Upgrades::Object::State::HPRecovery:	this->HPRecovery += 0.3f;	(*it)->Kill();
						break;//Upgrade - boost HP recovery over time - base is 0.0f - higher is better

					case Upgrades::Object::State::Empty:		(*it)->Kill();						
						break;//Just in case the task did not die already

					default: ge->Dbg_ToConsole("Unexpected bonus type!!! Go to Upgrades.cpp to fix"); (*it)->Kill(); 
						break;
					}
				}
			}



			//-------------------------------------
			// Flames animation
			if (this->state == State::Normal) {
				int animFlames = this->animCnt;

				this->srcFlames.x = 40 + 8 * ((animFlames % 20) / 5);
				this->animCnt++;
			}
			//-------------------------------------
			//death animation
			if (this->state == State::Dead) {
				this->leftmissile->Kill();
				this->rightmissile->Kill();

				//*******  Death animation  ******//
				// Calculate the frame index for the 40-frame death animation
				int frameIndex = this->animCnt;

				// Calculate the row and column of the sprite sheet
				this->src.x = (frameIndex % 5) * 100; // 5 columns (0–3)
				this->src.y = (frameIndex / 5) * 100; // 3 rows
				this->src.w = 100;
				this->src.h = 100;
				this->drawBase = ML::Box2D(-100, -100, 200, 200);

				// Increment animation counter
				this->animCnt++;

				// End the animation after 40 frames
				if (this->animCnt >= 40) { // 60 frames at 1.5 increment per frame = 40 frames
					this->state = State::Non;
					//triggers reset data
					auto game = ge->GetTask<Game::Object>("Scenes", "Game");
					game->IsResetData = true;
				}
			}

			//death trigger
			if (this->HP <= 0 && (this->state == State::Normal || this->state == State::Hit)) {
				this->state = State::Dead;
				se::Play("playerdeath");
			}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw = this->drawBase;
		draw.Offset(this->pos);
		ML::Box2D drawF = this->drawFlames;
		drawF.Offset(this->pos);
		switch (this->state) {
		case State::Normal:	//player render
							if (this->godmode == false) {
								this->res->imgPlayer->Draw(draw, this->src);
							}
							else{ this->res->imgPlayer->Draw(draw, this->src, ML::Color(1.0f, 1.0f, float((rand() % 100) / 100.f), 1.0f)); }

							//flames render
							this->res->imgFlames->Draw(drawF, this->srcFlames, ML::Color(1.0f, 0.9f, 0.9f, 1.0f));
							
							if (this->debugmode) {
								this->res->imgDebug->Draw(draw, ML::Box2D(64, 32, 32, 32));
							}
							break;
		case State::Dead:	
							draw = this->drawBase;
							this->res->imgPlayerDeath->Draw(draw, this->src);
							break;

		case State::Hit:	//During the invicibility frame, makes the sprite flicker randomly
							this->res->imgPlayer->Draw(draw, this->src, ML::Color(float((rand() % 100) / 100.f), 1.0f, 1.0f, 1.0f));
							if (this->debugmode) {
								this->res->imgDebug->Draw(draw, ML::Box2D(0, 32, 32, 32));
							}
							break;
		}
	}

	//---------------------------------------------------------------------------------------------------------------//
	void  Object::Cannon_Shot()
	{
		if (this->shootingTimer <= 0) {

			auto shotleft = Cannon::Object::Create(true);
			shotleft->pos.x = this->pos.x - 12 + (rand() % 100) / 50 - (rand() % 100) / 50; //randomized offset for a bit of spread because it looks cool
			shotleft->pos.y = this->pos.y - 5;
			shotleft->speed = 20.0f;
			shotleft->pierce = this->pierce;
			shotleft->damage = this->cannonDmg;
			shotleft->gravityPull = this->gravityPull;

			auto shotright = Cannon::Object::Create(true);
			shotright->pos.x = this->pos.x + 10 + (rand() % 100) / 50 - (rand() % 100) / 50;
			shotright->pos.y = this->pos.y - 5;
			shotright->speed = 20.0f;
			shotright->pierce = this->pierce;
			shotright->damage = this->cannonDmg;
			shotright->gravityPull = this->gravityPull;

			//shooting sound effect
			int r = rand() % _countof(this->res->cannonSE);
			se::Play(res->cannonSE[r]);

			this->shootingOverHeat++;
			if (this->shootingOverHeat < this->shootingOverHeatThreshold) {
				this->shootingTimer = this->shootingSpeed + 1.0f;
			}
			//if cannon overheated, slows down with overheat threshold reached
			else if (this->shootingOverHeat < this->shootingOverHeatThreshold * 1.5f) { 
				this->shootingTimer = static_cast<int>(this->shootingSpeed * 2.f);
				this->shootingOverHeat++;
			}
			else if (this->shootingOverHeat < this->shootingOverHeatThreshold * 2.f) {
				this->shootingTimer = static_cast<int>(this->shootingSpeed * 3.f);
				this->shootingOverHeat += 2.f;
			}
			else if (this->shootingOverHeat < this->shootingOverHeatThreshold * 2.5f) {
				this->shootingTimer = static_cast<int>(this->shootingSpeed * 4.f);
				this->shootingOverHeat += 4.f;
			}
			else {
				this->shootingTimer = static_cast<int>(this->shootingSpeed * 5.f);
				this->shootingOverHeat += 8.f;
			}

		return;
		}
	}

//	bool Object::CheckCollision(ML::Point pre_, ML::Box2D hit_, ML::Box2D src_)
	//{
	//	if (hit_.x - src_.w / 2 - pre_.x < 370)			{ return false; }
	//	if (hit_.x + src_.w / 2 + pre_.x > 1280 - 370)	{ return false; }
	//	if (hit_.y < 0 + src_.h / 2 + pre_.y)			{ return false; }
	//	if (hit_.y > 720 - src_.h / 2 - pre_.y)			{ return false; }
	//	else { return true; }
	//}

	void  Object::CheckMove(ML::Vec2& pre_) //movement prediction - not needed for now
	{
		//old simple method
		if (pre_.x - src.w / 2 < 370) { pre_.x = 370 + src.w / 2; }
		if (pre_.x + src.w / 2 > 1280 - 370) { pre_.x = 1280 - 370 - src.w / 2; }
		if (pre_.y < 0 + src.h / 2) { pre_.y = 0 + src.h / 2; }
		if (pre_.y > 720 - src.h / 2) { pre_.y = 720 - src.h / 2; }


		//this->hitFlag = false;

		////横軸に対する移動
		//while (pre_.x != 0) {					//".f"にすると、フリーズする
		//	float preX = this->pos.x;								//modif
		//	if (pre_.x >= +1.f) { this->pos.x += 1.f; pre_.x -= 1.f; }
		//	else if (pre_.x <= -1.f) { this->pos.x -= 1.f; pre_.x += 1.f; }
		//	else {																		//modif	
		//		this->pos.x += pre_.x;
		//		pre_.x = 0.f;
		//	} //小数点以下の移動を実装する際に使用する
		//	//ML::Box2D hit = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
		//	/*if (true ==this->CheckCollision(pre_, hit, this->src)) {*/   //bug
		//	if(this->pos.x - this->src.w / 2 - pre_.x < 370 || this->pos.x + this->src.w / 2 + pre_.x > 1280 - 370){
		//		this->pos.x = preX;
		//		this->hitFlag = true;
		//		break;
		//	}
		//}

		////縦軸に対する移動
		//while (pre_.y != 0) {					//".f"にすると、フリーズする
		//	float preY = this->pos.y;								//modif
		//	if (pre_.y >= +1.f) { this->pos.y += 1.f; pre_.y -= 1.f; }
		//	else if (pre_.y <= -1.f) { this->pos.y -= 1.f; pre_.y += 1.f; }
		//	else {																		//modif	
		//		this->pos.y += pre_.y;
		//		pre_.y = 0.f;
		//	} //小数点以下の移動を実装する際に使用する
		//	//ML::Box2D hit = this->hitBox.OffsetCopy(this->pos.x, this->pos.y);
		//	/*if (true == Player::Object::CheckCollision(pre_, hit, this->src)) {*/   //bug
		//	if (this->pos.y < 0 + this->src.h / 2 + pre_.y || this->pos.y > 720 - this->src.h / 2 - pre_.y){
		//		this->pos.y = preY;
		//		this->hitFlag = true;
		//		break;
		//	}
		//}


	}

	void Object::SaveData(PlayerSaveData& data) 
	{
		data.speed = speed;
		data.HP = HP;
		data.HPRecovery = HPRecovery;
		data.HPRecoveryTimer = HPRecoveryTimer;
		data.energy = energy;
		data.totalEXP = totalEXP;
		data.currentEXP = currentEXP;
		data.level = level;
		data.armorPercentage = armorPercentage;
		data.armorFlat = armorFlat;
		data.shootingSpeed = shootingSpeed;
		data.shootingOverHeatThreshold = shootingOverHeatThreshold;
		data.pierce = pierce;
		data.cannonDmg = cannonDmg;
		data.exploRadius = exploRadius;
		data.exploDmg = exploDmg;
		data.missileCoolDown = missileCoolDown;
	}

	void Object::LoadData(const PlayerSaveData& data) 
	{
		speed = data.speed;
		HP = data.HP;
		HPRecovery = data.HPRecovery;
		HPRecoveryTimer = data.HPRecoveryTimer;
		energy = data.energy;
		totalEXP = data.totalEXP;
		currentEXP = data.currentEXP;
		level = data.level;
		armorPercentage = data.armorPercentage;
		armorFlat = data.armorFlat;
		shootingSpeed = data.shootingSpeed;
		shootingOverHeatThreshold = data.shootingOverHeatThreshold;
		pierce = data.pierce;
		cannonDmg = data.cannonDmg;
		exploRadius = data.exploRadius;
		exploDmg = data.exploDmg;
		missileCoolDown = data.missileCoolDown;
	}

	void Object::ResetData(PlayerResetData& data)
	{
		data.speedBase = speedBase;
		data.HPBase = HPBase;
		data.HPRecoveryBase = HPRecoveryBase;
		data.HPRecoveryTimerBase = HPRecoveryTimerBase;
		data.energyBase = energyBase;
		data.totalEXP = 0;
		data.currentEXP = 0;
		data.level = 1;
		data.armorPercentageBase = armorPercentageBase;
		data.armorFlatBase = armorFlatBase;
		data.shootingSpeedBase = shootingSpeedBase;
		data.shootingOverHeatThresholdBase = shootingOverHeatThresholdBase;
		data.pierceBase = pierceBase;
		data.cannonDmgBase = cannonDmgBase;
		data.exploRadiusBase = exploRadiusBase;
		data.exploDmgBase = exploDmgBase;
		data.missileCoolDownBase = missileCoolDownBase;
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