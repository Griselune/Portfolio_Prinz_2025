#include  "MyPG.h"
#include  "UI_Panel.h"
#include  "HP_Bar.h"
#include  "Bar_Socket.h"
#include  "Energy_Bar.h"
#include  "OverHeat_Socket.h"
#include  "OverHeat_Bar.h"
#include  "MissileReload_Socket.h"
#include  "MissileReload_Bar.h"
#include  "Task_Player.h"
#include  "EXP_Bar.h"
#include  "EXP_Socket.h"
#include  "Player_Level.h"



namespace  UIPanel
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgUILeft = DG::Image::Create("./data/image/UI/testUILeft.png");
		this->imgUIRight = DG::Image::Create("./data/image/UI/testUIRight.png");

		//Upgrade display
		this->font = DG::Font::Create("HG丸ゴシックM-PRO", 8, 16);
		this->imgHPRecovery = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgArmPer = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgArmFlat = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgShootSpeed = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgShootTH = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgPierce = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgExploRadius = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgMissileCD = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgUILeft.reset();
		this->imgUIRight.reset();
		this->font.reset();
		this->imgHPRecovery.reset();
		this->imgArmPer.reset();
		this->imgArmFlat.reset();
		this->imgShootSpeed.reset();
		this->imgShootTH.reset();
		this->imgPierce.reset();
		this->imgExploRadius.reset();
		this->imgMissileCD.reset();
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
		this->render2D_Priority[1] = 0.1f;
		this->scale = 3;

		this->src = ML::Box2D(0, 0, 370, 720);

		//HP bar
		auto hpbarsocket = BarSocket::Object::Create(true);
		hpbarsocket->scale = this->scale;
		hpbar = HPBar::Object::Create(true);
		hpbar->scale = this->scale;

		//Energy bar
		auto energybarsocket = BarSocket::Object::Create(true);
		energybarsocket->scale = this->scale;
		energybarsocket->y = 150;
		energybar = EnergyBar::Object::Create(true);
		energybar->scale = this->scale;

		//OverHeat bar
		auto overheatsocket = OverHeatSocket::Object::Create(true);
		overheatsocket->scale = this->scale;
		auto overheatbar = OverHeatBar::Object::Create(true);
		overheatbar->scale = this->scale;

		//EXP bar
		auto expsocket = EXPSocket::Object::Create(true);
		expsocket->scale = this->scale;
		auto expbar = EXPBar::Object::Create(true);
		expbar->scale = this->scale;

		//Player levels
		auto playerlevel = PlayerLevel::Object::Create(true);
		playerlevel->scale = this->scale;

		//missile reload bar
		//left
		auto missilereloadsocketleft = MissileReloadSocket::Object::Create(true);
		missilereloadsocketleft->scale = this->scale;

		missileReloadBarLeft = MissileReloadBar::Object::Create(true);
		missileReloadBarLeft->scale = this->scale;

		//right
		auto missilereloadsocketright = MissileReloadSocket::Object::Create(true);
		missilereloadsocketright->x = 980;
		missilereloadsocketright->scale = this->scale;

		missileReloadBarRight = MissileReloadBar::Object::Create(true);
		missileReloadBarRight->scale = this->scale;
		missileReloadBarRight->x = 980;

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
		//Missile Reload bar
		auto player = ge->GetTask<Player::Object>("PlayerGroup", "Player"); //tracks the player for missile cooldown
		//Left
		missileReloadBarLeft->src.h = int(player->leftmissile->missileCDProgress);
		missileReloadBarLeft->src.y = 69 + (16 - int(player->leftmissile->missileCDProgress));
		missileReloadBarLeft->draw.h = int(player->leftmissile->missileCDProgress);
		missileReloadBarLeft->progress = player->leftmissile->missileCDProgress;

		//Right
		missileReloadBarRight->src.h = int(player->rightmissile->missileCDProgress);
		missileReloadBarRight->src.y = 69 + (16 - int(player->rightmissile->missileCDProgress));
		missileReloadBarRight->draw.h = int(player->rightmissile->missileCDProgress);
		missileReloadBarRight->progress = (player->rightmissile->missileCDProgress);


		//HP bar controller
		hpbar->drawBase.w = player->HP;
		hpbar->src.w = player->HP;

		//Energy bar controller
		energybar->drawBase.w = player->energy;
		energybar->src.w = player->energy;
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景描画
		ML::Box2D  drawLeft(0, 0, 370, 720);
		ML::Box2D  drawRight(0, 0, 370, 720);
		drawLeft.Offset(0, 0);
		drawRight.Offset(1280 - 370, 0);
		this->res->imgUILeft->Draw(drawLeft, src);
		this->res->imgUIRight->Draw(drawRight, src);




		//Upgrade display - needs rework with better font
		auto player = ge->GetTask<Player::Object>("PlayerGroup", "Player");
		ML::Box2D textBox(950, 400, 330, 320);
		string text =
			"Percentage Armor " + to_string_fixed(player->armorPercentage) + "\n" +
			" " + "\n" +

			"Flat Armor "		+ to_string(player->armorFlat) + "\n" +
			" " + "\n" +

			"Shooting Speed "	+ to_string_fixed(player->shootingSpeed) + "\n" +
			" " + "\n" +

			"Overheat Limit "	+ to_string(player->shootingOverHeatThreshold) + "\n" +
			" " + "\n" +

			"Pierce "			+ to_string(player->pierce) + "\n" +
			" " + "\n" +

			"Explosion Size "	+ to_string(player->exploRadius) + "\n" +
			" " + "\n" +

			"Missile Cooldown " + to_string(player->missileCoolDown) + "\n" +
			" " + "\n" +

			"HP Recovery "		+ to_string_fixed(player->HPRecovery) + "\n" +
			" " + "\n" + 

			"Blast Damage  "	+ to_string_fixed(player->exploDmg) + "\n" +
			" " + "\n" +

			"Cannon Damage "	+to_string(player->cannonDmg);
		this->res->font->Draw(textBox, text, ML::Color(1.0f, 0.0f, 0.0f, 0.0f));


		//Upgrades logo
		ML::Box2D draw1(930, 400, 16, 16);
		ML::Box2D src1 = ML::Box2D(24, 0, 8, 8);
		draw1.Offset(0, 0);
		this->res->imgArmPer->Draw(draw1, src1);

		ML::Box2D draw2(930, 400, 16, 16);
		ML::Box2D src2 = ML::Box2D(0, 24, 8, 8);
		draw2.Offset(0, 32);
		this->res->imgArmFlat->Draw(draw2, src2);

		ML::Box2D draw3(930, 400, 16, 16);
		ML::Box2D src3 = ML::Box2D(32, 40, 8, 8);
		draw3.Offset(0, 64);
		this->res->imgShootSpeed->Draw(draw3, src3);

		ML::Box2D draw4(930, 400, 16, 16);
		ML::Box2D src4 = ML::Box2D(40, 32, 8, 8);
		draw4.Offset(0, 96);
		this->res->imgShootTH->Draw(draw4, src4);

		ML::Box2D draw5(930, 400, 16, 16);
		ML::Box2D src5 = ML::Box2D(0, 8, 8, 8);
		draw5.Offset(0, 128);
		this->res->imgPierce->Draw(draw5, src5);

		ML::Box2D draw6(930, 400, 16, 16);
		ML::Box2D src6 = ML::Box2D(32, 24, 8, 8);
		draw6.Offset(0, 160);
		this->res->imgExploRadius->Draw(draw6, src6);

		ML::Box2D draw7(930, 400, 16, 16);
		ML::Box2D src7 = ML::Box2D(16, 48, 8, 8);
		draw7.Offset(0, 192);
		this->res->imgMissileCD->Draw(draw7, src7);

		ML::Box2D draw8(930, 400, 16, 16);
		ML::Box2D src8 = ML::Box2D(0, 32, 8, 8);
		draw8.Offset(0, 224);
		this->res->imgHPRecovery->Draw(draw8, src8);

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