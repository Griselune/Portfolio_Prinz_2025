//-------------------------------------------------------------------
//タイトル画面
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "randomLib.h"
#include  "sound.h"
#include  "Task_Effect00.h"

#include  "Task_Title.h"
#include  "Task_StartButton.h"
#include  "Task_Game.h"

namespace  Title
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgGrass = DG::Image::Create("./data/image/grass_template2_0.png");
		this->imgTitle = DG::Image::Create("./data/image/title_text.png");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgGrass.reset();
		this->imgTitle.reset();
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
		this->controller = ge->in1;
		this->render2D_Priority[1] = 0.5f;
		int RenderTime = 0;

		ge->stage = 1;

		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		//音楽ファイルの読み込み
		bgm::LoadFile("bgm1", "./data/sound/bgm/bgm1.mp3");
		bgm::Play("bgm1");

		//★タスクの生成
		auto startbutton = Start::Object::Create(true);
		startbutton->pos = ML::Box2D(251, 400, 298, 45);


		this->Kill(); //no time for intros 

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		bgm::Stop("bgm1");
		if (!ge->QuitFlag() && this->nextTaskCreate) {
			ge->KillAll_G("Start");
			Game::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = this->controller->GetState();

		int input = 0;
		//							16進		2進	
		if (inp.B1.down) input |= 0x0001;	// 0000000001
		if (inp.B2.down) input |= 0x0002;	// 0000000010
		if (inp.B3.down) input |= 0x0004;	// 0000000100
		if (inp.B4.down) input |= 0x0008;	// 0000001000
		if (inp.ST.down) input |= 0x0100;	// 0100000000

		if (input & 0x0100) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(98, ML::Vec2(0, 0));
		}
		if (ge->getCounterFlag("test") == ge->LIMIT) {
			this->Kill();
		}

		if (input & 0x0002) {
			se::Play("se1");
		}
		if (input & 0x0004) {
			se::Play("se2");
		}

		return;

	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//背景
		ML::Box2D  draw1(0, 0, 800, 600);
		ML::Box2D  src1(0, 0, 1280, 720);
		this->res->imgGrass->Draw(draw1, src1);

		//タイトル名
		ML::Box2D  draw2(0, 0, 676, 88);
		draw2.Offset(62, 200);
		ML::Box2D  src2(0, 0, 676, 88);
		this->res->imgTitle->Draw(draw2, src2);

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