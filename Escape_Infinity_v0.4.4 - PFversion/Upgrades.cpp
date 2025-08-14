#include  "MyPG.h"
#include  "randomLib.h"
#include  "Upgrades.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"
#include  "Task_Game.h"


namespace  Upgrades
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgCool = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgHeal = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgHPRecovery = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgArmPer = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgArmFlat = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
		this->imgShootSpeed = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgShootTH = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgPierce = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgExploRadius = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->imgMissileCD = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Projectiles.png");
		this->debugrect = DG::Image::Create("./data/effect/debugrect.png");


		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgCool.reset();
		this->imgHeal.reset();
		this->imgHPRecovery.reset();
		this->imgArmPer.reset();
		this->imgArmFlat.reset();
		this->imgShootSpeed.reset();
		this->imgShootTH.reset();
		this->imgPierce.reset();
		this->imgExploRadius.reset();
		this->imgMissileCD.reset();
		this->debugrect.reset();
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

		this->state = State::Empty;
		this->pos = ML::Vec2(0.0f, 0.0f);
	//	this->gravityPull = ML::Vec2(0.0f, 0.0f);
		this->hitBox = ML::Box2D(0, 0, 32, 32);
		this->drawBase = ML::Box2D(-16, -16, 32, 32);
		this->src = ML::Box2D(0, 0, 8, 8); //sprite is 8x8 pixels

		this->rotation = 180.0f; //used to flip over the imgCool sprite
		this->debugmode = false;
		//★タスクの生成

		int Random = GetRandom(0, 1000); // Get a random number for upgrade selection

		//40% chance to get an upgrade - ************************WIP************************
		if (Random < 600) {
			this->state = State::Empty; // 60% chance of nothing
			this->Kill();
			return true;
		}
		else if (Random < 650) {
			this->state = State::Heal; // 5% chance - HP recovery
			return true;
		}
		else if (Random < 700) {
			this->state = State::CannonCool; // 5% chance - Resets overheat
			return true;
		}
		else if (Random < 750) {
			this->state = State::ExploRadius; // 5% chance - Bigger explosion radius
			return true;
		}
		else if (Random < 800) {
			this->state = State::MissileCD; // 5% chance - Faster missile reload
			return true;
		}
		else if (Random < 850) {
			this->state = State::ArmorPer; // 5% chance - % armor boost
			return true;
		}
		else if (Random < 900) {
			this->state = State::ShootingTh; // 5% chance - Higher overheat threshold
			return true;
		}
		else if (Random < 950) {
			this->state = State::ShootingSpeed; // 5% chance - Faster shooting speed
			return true;
		}
		else if (Random < 998) {
			this->state = State::HPRecovery; // 5% chance - HP recovery over time
			return true;
		}
		else if (Random < 999) {
			this->state = State::Pierce; // 0.1% chance - Bullets pierce enemies (very rare)
			return true;
		}
		else {
			this->state = State::ArmorFlat; // 0.1% chance - Flat armor boost (very rare)
			return true;
		}
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
	//	this->pos -= this->gravityPull;
	
		if (this->pos.y > 750) { this->Kill(); }
		else {
			this->pos.y++;
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//debug mode display
		if (auto game = ge->GetTask<Game::Object>("Scenes", "Game")) {
			if (game->debugmode) {
				ML::Box2D drawdebug = this->drawBase;
				ML::Box2D srcdebug = ML::Box2D(64, 32, 32, 32);
				drawdebug.Offset(this->pos.x, this->pos.y);
				this->res->debugrect->Draw(drawdebug, srcdebug);
			}
		}
	

		switch (this->state) {
		case State::CannonCool: {
			ML::Box2D draw1 = this->drawBase;
			ML::Box2D src1 = ML::Box2D(0, 24, 8, 8);
			draw1.Offset(this->pos.x, this->pos.y);
			this->res->imgCool->Rotation(ML::ToRadian(this->rotation), { 16.f, 16.f });
			this->res->imgCool->Draw(draw1, src1);
			this->res->imgCool->Rotation(0, { 0, 0 }); //reset rotation
			break;
		}
		case State::Heal: {
			ML::Box2D draw2 = this->drawBase;
			ML::Box2D src2 = ML::Box2D(16, 0, 8, 8);
			draw2.Offset(this->pos.x, this->pos.y);
			this->res->imgHeal->Draw(draw2, src2);
			break;
		}
		case State::ArmorPer: {
			ML::Box2D draw3 = this->drawBase;
			ML::Box2D src3 = ML::Box2D(24, 0, 8, 8);
			draw3.Offset(this->pos.x, this->pos.y);
			this->res->imgArmPer->Draw(draw3, src3);
			break;
		}
		case State::ArmorFlat: {
			ML::Box2D draw4 = this->drawBase;
			ML::Box2D src4 = ML::Box2D(0, 24, 8, 8);
			draw4.Offset(this->pos.x, this->pos.y);
			this->res->imgArmFlat->Draw(draw4, src4);
			break;
		}
		case State::ShootingSpeed: {
			ML::Box2D draw5 = this->drawBase;
			ML::Box2D src5 = ML::Box2D(32, 40, 8, 8);
			draw5.Offset(this->pos.x, this->pos.y);
			this->res->imgShootSpeed->Draw(draw5, src5);
			break;
		}
		case State::ShootingTh: {
			ML::Box2D draw6 = this->drawBase;
			ML::Box2D src6 = ML::Box2D(40, 32, 8, 8);
			draw6.Offset(this->pos.x, this->pos.y);
			this->res->imgShootTH->Draw(draw6, src6);
			break;
		}
		case State::Pierce: {
			ML::Box2D draw7 = this->drawBase;
			ML::Box2D src7 = ML::Box2D(0, 8, 8, 8);
			draw7.Offset(this->pos.x, this->pos.y);
			this->res->imgPierce->Draw(draw7, src7);
			break;
		}
		case State::ExploRadius: {
			ML::Box2D draw8 = this->drawBase;
			ML::Box2D src8 = ML::Box2D(32, 24, 8, 8);
			draw8.Offset(this->pos.x, this->pos.y);
			this->res->imgExploRadius->Draw(draw8, src8);
			break;
		}
		case State::MissileCD: {
			ML::Box2D draw9 = this->drawBase;
			ML::Box2D src9 = ML::Box2D(16, 48, 8, 8);
			draw9.Offset(this->pos.x, this->pos.y);
			this->res->imgMissileCD->Draw(draw9, src9);
			break;
		}
		case State::HPRecovery: {
			ML::Box2D draw10 = this->drawBase;
			ML::Box2D src10 = ML::Box2D(0, 32, 8, 8);
			draw10.Offset(this->pos.x, this->pos.y);
			this->res->imgHPRecovery->Draw(draw10, src10);
			break;
		}
		default:
			return;
		}


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