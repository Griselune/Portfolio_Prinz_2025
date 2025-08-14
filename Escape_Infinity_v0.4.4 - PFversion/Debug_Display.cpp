#include  "MyPG.h"
#include  "Debug_Display.h"
#include  "Task_Game.h"
#include  "Task_Player.h"
#include  "Task_Cannon.h"
#include  "Task_Game.h"
#include  "Task_Blast.h"
#include  "EXP_Bar.h"


namespace  DebugDisplay
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->font = DG::Font::Create("HG丸ゴシックM-PRO", 8, 16);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->font.reset();
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
		this->render2D_Priority[1] = 0.f;
		this->scale = 2;

		this->src = ML::Box2D(0, 0, 480, 720);

		this->flag = false;
		this->cannoncnt = 0;

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

		
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//Controller values
		auto  inp = ge->in1->GetState();
		auto dataplayer = ge->GetTask<Player::Object>("PlayerGroup", "Player");
		//auto datablast = ge->GetTasks<Blast::Object>("Weapons", "Blast");
		//auto blast0 = datablast->begin();
		auto dataenemies = ge->GetTasks<Enemy000::Object>("EnemyGroup");
		auto getGameTask = ge->GetTasks<Game::Object>("Scenes", "Game");
		auto getgametask = getGameTask->begin();
		auto expbar = ge->GetTask<EXPBar::Object>("UI", "EXPBar");

	//	auto datacannon = ge->GetTasks<Cannon::Object>("Weapons", "Cannon");
		//for (auto cnt = datacannon->begin(); cnt != datacannon->end(); cnt++) {
		//	this->cannoncnt ++;
		//}
	
		
	

		if ((*getgametask)->debugmode) {
			for (auto e = dataenemies->begin(); e != dataenemies->end(); e++) {
				//enemy debug values
				if ((*e)->state == Enemy000::Object::State::Normal) {
					ML::Box2D textBoxEne((*e)->pos.x - ((*e)->hitBox.w / 2), (*e)->pos.y - ((*e)->hitBox.h / 2) + 0, 200, 100); //change this to center the text box
					int eneArmorPer = static_cast<int>((*e)->armorPercentage * 100); //gets an easy to understand value for debugging
					string textEne =
						"arFlat:" + to_string((*e)->armorFlat) + "\n" +
						"ar%: " + to_string(eneArmorPer) + "\n" +
						"hp: " + to_string((*e)->HP) + "\n" +
						"exp: " + to_string((*e)->exp);
					
					this->res->font->Draw(textBoxEne, textEne, ML::Color(1.0f, 0.0f, 0.0f, 0.0f));
				}
			}


			ML::Box2D textBox(0, 0, 480, 720);
			string text =
				"LStick Vol :" + to_string(inp.LStick.volume) + "\n" +
				"LStick rad DYP :" + to_string(inp.LStick.angleDYP) + "\n" +
				"cos DYP :" + to_string(cos(inp.LStick.angleDYP)) + "\n" +
				"sin DYP :" + to_string(sin(inp.LStick.angleDYP)) + "\n" +
				"LStick rad UYP :" + to_string(inp.LStick.angleUYP) + "\n" +
				"LStick X axis :" + to_string(inp.LStick.axis.x) + "\n" +
				"LStick Y axis :" + to_string(inp.LStick.axis.y) + "\n" +

				//		"pitch :" + to_string(pitch)											+ "\n" +

					//***object values***//
				//		"enemy.src.x :" + to_string(enemy[0].src.x)								+ "\n" +
				//		"enemy.src.y :" + to_string(enemy[0].src.y)								+ "\n" +
				//		"enemy.animCnt :" + to_string(enemy[0].animCnt)							+ "\n" +
				//		"Enemy Alive :" + to_string(EnemyAlive)									+ "\n" +
				//		"Enemy Killed :" + to_string(EnemyKilled)								+ "\n" +
				"Game debug :" + to_string((*getgametask)->debugmode)						+ "\n" +
				"GODMODE :" + to_string((*getgametask)->IsGodMode)							+ "\n" +
				"IsPause :" + to_string((*getgametask)->IsPause)							+ "\n" +
				"time game :" + to_string((*getgametask)->timeCnt)							+ "\n" +
				"flames x :" + to_string(dataplayer->drawFlames.x)							+ "\n" +
				"dataplayer draw x :" + to_string(dataplayer->drawBase.x)					+ "\n" +
				"dataplayer draw y :" + to_string(dataplayer->drawBase.y)					+ "\n" +
				"dataplayer draw w :" + to_string(dataplayer->drawBase.w)					+ "\n" +
				"dataplayer draw h :" + to_string(dataplayer->drawBase.h)					+ "\n" +
				"dataplayer src x :" + to_string(dataplayer->src.x)							+ "\n" +
				"dataplayer src y :" + to_string(dataplayer->src.y)							+ "\n" +
				"dataplayer src w :" + to_string(dataplayer->src.w)							+ "\n" +
				"dataplayer src h :" + to_string(dataplayer->src.h)							+ "\n" +
				"Player hitflag :" + to_string(dataplayer->hitFlag)							+ "\n" +
				"Player x :" + to_string(dataplayer->pos.x)									+ "\n" +
				"Player y :" + to_string(dataplayer->pos.y)									+ "\n" +
				"shooting timer :" + to_string(dataplayer->shootingTimer)					+ "\n" +
				"shooting speed :" + to_string(dataplayer->shootingSpeed)					+ "\n" +
				"shooting overheat :" + to_string(dataplayer->shootingOverHeat)				+ "\n" +
				"Missile Left cnt :" + to_string(dataplayer->leftmissile->cnt)				+ "\n" +
				"Missile Right cnt :" + to_string(dataplayer->rightmissile->cnt)			+ "\n" +
				"Missile Left CD prog :" + to_string(dataplayer->leftmissile->missileCDProgress) + "\n" +
				"Missile Right CD prog :" + to_string(dataplayer->rightmissile->missileCDProgress) + "\n" +
				"Overheat progress :" + to_string(dataplayer->shootingOverHeatProgress);	  /*+ "\n" +*/
				//		"Blast radius :" + to_string((*blast0)->radius);	  /*+ "\n" +*/

				//		"gameclock :" + to_string(gameclock);
			this->res->font->Draw(textBox, text, ML::Color(1.0f, 0.0f, 0.0f, 0.0f));

			//Game values / resources = placeholder for definitive UI
			ML::Box2D GameUIinfo(1280 - 250, 0, 480, 720);
			string textgameinfo =
				"player HP :"			+ to_string(dataplayer->HP)			+ "\n" +
				"player exp :"			+ to_string(dataplayer->currentEXP) + "\n" +
				"player Total exp :"	+ to_string(dataplayer->totalEXP)	+ "\n" +
				"player level :"		+ to_string(dataplayer->level)		+ "\n" +
				"Energy level :"		+ to_string(dataplayer->energy);

			this->res->font->Draw(GameUIinfo, textgameinfo, ML::Color(1.0f, 0.0f, 0.0f, 0.0f));

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