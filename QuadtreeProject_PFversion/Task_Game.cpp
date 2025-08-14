//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Game.h"
#include  "randomLib.h"
#include  <string>
#include  "Task_Background.h"
#include  "Square.h"
#include  "debug.h"
#include  "Quadtree.h"
#include  "FPSCounter.h"


namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->scoreFont = DG::Image::Create("./data/image/font_text.png");
		this->imgQT = DG::Image::Create("./data/image/whitepixel.png");

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->scoreFont.reset();
		this->imgQT.reset();

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
		this->render2D_Priority[1] = -1.f;

		//★データ初期化
		
		//デバッグ用フォントの準備
		this->TestFont = DG::Font::Create("ＭＳ ゴシック", 30, 30);

		//★タスクの生成
		auto debug = debug::Object::Create(true);
		auto bg = BG::Object::Create(true);

		this->IsDebug = false;

		Quadtree* quadTree = nullptr;
		this->IsQuadtreeOn = false;
		this->IsShowQuadtree = false;
		this->QTsize = ML::Box2D(0, 0, ge->screen2DWidth, ge->screen2DHeight); //画面サイズ
		this->QTlevel = 0;
		this->QTcnt = 0;

		this->squareCnt = 0;

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		this->TestFont.reset();
		ge->mouse.reset();

		ge->KillAll_G("MainGame");
		ge->KillAll_G("BG");
		ge->KillAll_G("Square");
		ge->KillAll_G("debug");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto ms = ge->mouse->GetState();
		auto inp = ge->in1->GetState();
		auto debug = ge->GetTask<debug::Object>("debug", "NoName");

		//左クリックで毎フレームオブジェクト10個を生成する
		if (ms.LB.on) {
			for (int i = 0; i < 10; i++) {
				auto spawnsquare = Square::Object::Create(true);
				spawnsquare->pos = ML::Vec2((float)ms.pos.x, (float)ms.pos.y);
				spawnsquare->UpdateHitBase();
				spawnsquare->ID = this->squareCnt; //身元を与える
				this->squareCnt++;
			}
		}
		//右クリックでオブジェクト1個を生成する
		if (ms.RB.down) {
			auto spawnsquare = Square::Object::Create(true);
			spawnsquare->pos = ML::Vec2((float)ms.pos.x , (float)ms.pos.y);
			spawnsquare->UpdateHitBase();
			spawnsquare->ID = this->squareCnt; //身元を与える
			this->squareCnt++;

		}



		//クワッドツリーのON/OFFボタン
		if (inp.B1.down) { //Zボタン
			this->IsQuadtreeOn = !this->IsQuadtreeOn;
		}
		if (inp.B2.down) {
			this->IsShowQuadtree = !this->IsShowQuadtree;
		}

		//クワッドツリー(4分木)でオブジェクトの領域を管理する
		if (this->IsQuadtreeOn) {
			if (this->quadTree) {
				delete this->quadTree; //クワッドツリーが存在する場合は削除する
			}
			this->QTcnt = 0; //クワッドツリーの数をリセットする
			this->quadTree = new Quadtree(this->QTlevel, this->QTsize);
			auto squareList = ge->GetTasks<BChara>("Square");
			if (squareList != nullptr) { //オブジェクトが存在する場合はクワッドツリーに入れ始める
				for (auto& it : *squareList) {
					this->quadTree->Insert(it);
				}
			}
		}

		//-------------------デバッグ機能対応------------------
		if (inp.SE.down) { //Aボタン
			this->IsDebug = !this->IsDebug;
			if (this->IsDebug) {
				debug->isOn = true;
			}
			else {
				debug->isOn = false;
			}
		}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		ge->c->DrawFps();
		int fps = ge->c->GetRealTimeFPS();
		{
			std::string score =
				"FPS : " + to_string(fps)
				+ "  Objects:" + to_string(this->squareCnt);
			int x = 0;
			int charWidth = 32;
			int charHeight = 32;

			int screenWidth = ge->screen2DWidth;
			int totalTextWidth = static_cast<int>(score.size()) * charWidth;
			int startX = screenWidth - totalTextWidth - 20; // 左から20ピクセルの余白

			for (char c : score) {
				int charIndex = static_cast<int>(c); // オフセットなし

				if (charIndex >= 32 && charIndex < 128) {
					int charX = (charIndex % 16) * charWidth;
					int charY = (charIndex / 16) * charHeight;

					ML::Box2D scoreBox(startX + x, 20, charWidth, charHeight);
					ML::Box2D src(charX, charY, charWidth, charHeight);

					this->res->scoreFont->Draw(scoreBox, src);
					x += charWidth;
				}
			}
		}

		//操作方法説明の表示
		{
			std::string controls =
				"Use Debug: A ";
			int x = 0;
			int charWidth = 32;
			int charHeight = 32;

			int screenWidth = ge->screen2DWidth;

			int controlsTotalTextWidth = static_cast<int>(controls.size()) * charWidth;
			int controlsStartX = screenWidth - controlsTotalTextWidth - 20; // 左から20ピクセルの余白

			for (char c : controls) {
				int charIndex = static_cast<int>(c); // オフセットなし

				if (charIndex >= 32 && charIndex < 128) {
					int charX = (charIndex % 16) * charWidth;
					int charY = (charIndex / 16) * charHeight;

					ML::Box2D controlsBox(controlsStartX + x, 50, charWidth, charHeight);
					ML::Box2D controlsSrc(charX, charY, charWidth, charHeight);
					if (this->IsDebug) {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 0.0f, 1.0f, 0.0f));
					}
					else {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 1.0f, 0.0f, 1.0f));
					}
					x += charWidth;
				}
			}
		}
		{
			std::string controls =
				"Use Quadtree: Z ";
			int x = 0;
			int charWidth = 32;
			int charHeight = 32;

			int screenWidth = ge->screen2DWidth;

			int controlsTotalTextWidth = static_cast<int>(controls.size()) * charWidth;
			int controlsStartX = screenWidth - controlsTotalTextWidth - 20; // 左から20ピクセルの余白

			for (char c : controls) {
				int charIndex = static_cast<int>(c); // オフセットなし

				if (charIndex >= 32 && charIndex < 128) {
					int charX = (charIndex % 16) * charWidth;
					int charY = (charIndex / 16) * charHeight;

					ML::Box2D controlsBox(controlsStartX + x, 80, charWidth, charHeight);
					ML::Box2D controlsSrc(charX, charY, charWidth, charHeight);

					if (this->IsQuadtreeOn) {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 0.0f, 1.0f, 0.0f));
					}
					else {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 1.0f, 0.0f, 1.0f));
					}
					x += charWidth;
				}
			}
		}
		{
			std::string controls =
				"Show Quadtree: X ";
			int x = 0;
			int charWidth = 32;
			int charHeight = 32;

			int screenWidth = ge->screen2DWidth;

			int controlsTotalTextWidth = static_cast<int>(controls.size()) * charWidth;
			int controlsStartX = screenWidth - controlsTotalTextWidth - 20; // 左から20ピクセルの余白

			for (char c : controls) {
				int charIndex = static_cast<int>(c); // オフセットなし

				if (charIndex >= 32 && charIndex < 128) {
					int charX = (charIndex % 16) * charWidth;
					int charY = (charIndex / 16) * charHeight;

					ML::Box2D controlsBox(controlsStartX + x, 110, charWidth, charHeight);
					ML::Box2D controlsSrc(charX, charY, charWidth, charHeight);

					if (this->IsShowQuadtree) {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 0.0f, 1.0f, 0.0f));
					}
					else {
						this->res->scoreFont->Draw(controlsBox, controlsSrc, ML::Color(1.0f, 1.0f, 0.0f, 1.0f));
					}
					x += charWidth;
				}
			}
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