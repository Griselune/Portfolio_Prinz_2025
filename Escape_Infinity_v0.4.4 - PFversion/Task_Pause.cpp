#include  "MyPG.h"
#include  "Task_Pause.h"
#include  "Task_Game.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "sound.h"

namespace  Pause
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgPauseMenu			= DG::Image::Create("./data/image/UI/PauseMenu.png");
		this->imgPauseMenuSelector	= DG::Image::Create("./data/image/UI/Pause_Menu_Selector.png");
		se::LoadFile("PauseSound", "./data/Sounds/UI_sound_effects/mixkit-unlock-game-notification-253.wav");
		se::SetVolume("PauseSound", 100);
		se::LoadFile("SelectorSound", "./data/Sounds/UI_sound_effects/click.wav");
		se::SetVolume("SelectorSound", 100);
		se::LoadFile("ConfirmSound", "./data/Sounds/UI_sound_effects/mixkit-gaming-lock-2848.wav");
		se::SetVolume("ConfirmSound", 100);

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgPauseMenu.reset();
		this->imgPauseMenuSelector.reset();
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
		this->render2D_Priority[1] = 0.0f;

		//Pause the game
		ge->StopAll_G("Scenes", true);
		ge->StopAll_G("PlayerGroup", true);
		ge->StopAll_G("MainBG", true);
		ge->StopAll_G("UI", true);
		ge->StopAll_G("Weapons", true);
		ge->StopAll_G("Debug", true);
		ge->StopAll_G("EnemyGroup", true);
		ge->StopAll_G("EnemyGenerator", true);
		ge->StopAll_G("Upgrades", true);

		//★データ初期化
		this->y = 200;
		this->x = 400;
		this->select = 0;
		this->displayMenu = true;

		//★タスクの生成
		se::Play("PauseSound");

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放

		//Resume
		if (!ge->QuitFlag() && this->nextTaskCreate && this->select == 0) {
			//unpause the game
			ge->StopAll_G("Scenes", false);
			ge->StopAll_G("PlayerGroup", false);
			ge->StopAll_G("MainBG", false);
			ge->StopAll_G("UI", false);
			ge->StopAll_G("Weapons", false);
			ge->StopAll_G("Debug", false);
			ge->StopAll_G("EnemyGroup", false);
			ge->StopAll_G("EnemyGenerator", false);
			ge->StopAll_G("Upgrades", false);
		}

		//Quit
		else if (!ge->QuitFlag() && this->nextTaskCreate && this->select == 1) {
			auto  nextTask = Ending::Object::Create(true);
		}

		//Restart
		else if (!ge->QuitFlag() && this->nextTaskCreate && this->select == 2) {
			auto  nextTask = Game::Object::Create(true);
		}

		//Delete save
		else if (!ge->QuitFlag() && this->nextTaskCreate && this->select == 3) {
			//unpause the game
			ge->StopAll_G("Scenes", false);
			ge->StopAll_G("PlayerGroup", false);
			ge->StopAll_G("MainBG", false);
			ge->StopAll_G("UI", false);
			ge->StopAll_G("Weapons", false);
			ge->StopAll_G("Debug", false);
			ge->StopAll_G("EnemyGroup", false);
			ge->StopAll_G("EnemyGenerator", false);
			ge->StopAll_G("Upgrades", false);
			auto game = ge->GetTask<Game::Object>("Scenes", "Game");
			game->IsResetData = true;
		//	auto g = game->begin();
		//	auto  nextTask = Ending::Object::Create(true);
		//	(*g)->ResetGame("./data/Saves/save1.txt");
		//	(*g)->Kill();
		//	ge->KillAll_GN("Scenes", "Game", true);
		}

		//Options - Not implemented


		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
	//	auto getgame = ge->GetTask<Game::Object>("Scenes", "Game");

		//press select to hide/display the menu -> useful for debug
		if (inp.SE.down) { this->displayMenu = !this->displayMenu; }
		//navigates the menu with LStick
		if (inp.LStick.BD.down && this->select < 4) {
			this->select++;
			se::Play("SelectorSound");
		}
		else if (inp.LStick.BU.down && this->select > 0) {
			this->select--;
			se::Play("SelectorSound");
		}

		//confirm with A
		if (inp.B1.down) { 
			switch (select) {
				//resume game
			case 0:		this->Kill(); se::Play("ConfirmSound"); return;

				//quit game - confirmation to implement later
			case 1:		ge->KillAll_GN("Scenes", "Game", false); this->Kill();  se::Play("ConfirmSound");  return;

				//restart game- confirmation to implement later
			case 2:		ge->KillAll_GN("Scenes", "Game", false); this->Kill();  se::Play("ConfirmSound");  return; 

				//delete save game	
			case 3:		this->Kill();	se::Play("ConfirmSound"); return;

				//options - godmode, fullscreen, sound volume, etc - not implemented yet
			case 4: break;  
			}
		}
		//exits the pause menu by pressing B
		if (inp.B2.down) { this->select = 0; this->Kill(); } 
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		if (this->displayMenu) {
			//menu render
			ML::Box2D  draw(0, 0, 500, 300);
			ML::Box2D  src(0, 0, 500, 300);

			draw.Offset(this->x, this->y);
			this->res->imgPauseMenu->Draw(draw, src);

			//selector render
			ML::Box2D draws(0, 0, 500, 45);
			ML::Box2D srcs(0, 0, 500, 40);

			draws.Offset(this->x, this->y + 45 + (45 * select));
			this->res->imgPauseMenuSelector->Draw(draws, srcs);
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