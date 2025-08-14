//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "sound.h"
#include  "easing.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgEnding = DG::Image::Create("./data/image/Ending/Ending_v1.png");
		this->imgCursor = DG::Image::Create("./data/image/Ending/FrameQuit.png");

		bgm::LoadFile("endingBGM", "./data/Sounds/Music/Space_Music_Pack/in-the-wreckage.wav");
		bgm::VolumeControl("endingBGM", 100);

		
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgEnding.reset();
		this->imgCursor.reset();
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
		easing::Set("endingY", easing::QUINTOUT, 720, 0, 120);
		easing::Start("endingY");

		//★データ初期化
		this->logoPosY = 720;

		this->cursorPosition = 0; //to navigate menu

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放


		if (!ge->QuitFlag() && this->nextTaskCreate && this->cursorPosition == 0) {
			//★引き継ぎタスクの生成
			auto  nextTask = Title::Object::Create(true);
		}
		else if (!ge->QuitFlag() && this->nextTaskCreate && this->cursorPosition == 1) {
			auto  nextTask = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();
		bgm::Play("endingBGM");

	//	this->logoPosY -= 20;
	//	if (this->logoPosY <= 0) {
	//		this->logoPosY = 0;
	//	}
		easing::UpDate();
		this->logoPosY = easing::GetPos("endingY");

		if (this->logoPosY == 0) {
			if (inp.LStick.BL.down) {
				if (this->cursorPosition > 0) {
					this->cursorPosition -= 1;
				}
			}
			if (inp.LStick.BR.down) {
				if (this->cursorPosition < 1) {
					this->cursorPosition += 1;
				}
			}

			if (true == inp.B1.down) {
				bgm::AllStop();
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D drawEnding(0, 0, 1280, 720);
		ML::Box2D srcEnding(0, 0, 1280, 720);
		drawEnding.Offset(0, this->logoPosY);
		this->res->imgEnding->Draw(drawEnding, srcEnding);

		if (this->cursorPosition == 0 && this->logoPosY == 0) {
			ML::Box2D drawCursor(0, 0, 400, 150);
			ML::Box2D srcCursor(0, 0, 400, 150);
			drawCursor.Offset(120, 500);
			this->res->imgCursor->Draw(drawCursor, srcCursor);
		}
		else if (this->cursorPosition == 1 && this->logoPosY == 0) {
			ML::Box2D drawCursor(0, 0, 530, 150);
			ML::Box2D srcCursor(410, 0, 530, 150);
			drawCursor.Offset(690, 500);
			this->res->imgCursor->Draw(drawCursor, srcCursor);
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