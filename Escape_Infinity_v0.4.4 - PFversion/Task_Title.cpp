//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Title.h"
#include  "Task_Game.h"
#include  "easing.h"
#include  "sound.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{

	

		this->img = DG::Image::Create("./data/image/Title/Title_v1.png");
		this->font = DG::Font::Create("HG丸ゴシックM-PRO", 32, 64);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		
		this->img.reset();
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
		easing::Set("titleY", easing::QUINTOUT, -720, 0, 120);
		easing::Start("titleY");

		//★データ初期化
	//	this->logoPosY = -720;
		this->cnt = 0;
		this->IsBlink = false;

		//sound effect
        bgm::LoadFile("introBGM", "./data/Sounds/UI/Intro/mixkit-introduction-bell-sound-1150.wav");
		bgm::SetStartPos("introBGM", 9400000);
		bgm::VolumeControl("introBGM", 100);
		bgm::Play("introBGM");
		this->isDebug = false;
		

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		bgm::Stop("introBGM");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto  nextTask = Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		//debug sound effect
		this->timingSE = bgm::GetCurrentPos("introBGM");

		auto inp = ge->in1->GetState();

		if (inp.SE.down) { this->isDebug = !this->isDebug; }

		//text blinking
		this->cnt++;
		if (this->cnt % 45 == 0) {
			this->IsBlink = !this->IsBlink;
			this->cnt = 0;
		}

	//	this->logoPosY += 20;
		easing::UpDate();
		this->logoPosY = easing::GetPos("titleY");
	//	if (this->logoPosY >= 0) {
	//		this->logoPosY = 0;
	//	}

		if (this->logoPosY == 0) {
			if (this->timingSE > 46000000) { bgm::Stop("introBGM"); }
			if (inp.B1.down) {
				//自身に消滅要請
				this->Kill();
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ML::Box2D  draw(0, 0, 1280, 720);
		ML::Box2D  src(0, 0, 1280, 720);

		draw.Offset(0, this->logoPosY);
		this->res->img->Draw(draw, src);

		if (this->logoPosY == 0 && this->IsBlink) {
			ML::Box2D textbox(1270 / 2 - 100, 600, 400, 200);
			string text = "Press A";
			this->res->font->Draw(textbox, text, ML::Color(1.0f, 1.0f, 1.0f, 1.0f));
		}

		if (this->isDebug) {
			ML::Box2D debugText(0, 0, 800, 500);
			string text = "SE timer: " + to_string(this->timingSE);
			this->res->font->Draw(debugText, text, ML::Color(1.0f, 1.0f, 1.0f, 1.0f));
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