//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "debug.h"
#include  "Square.h"
#include  "Task_Game.h"

namespace debug
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgDebugRectHitBase = DG::Image::Create("./data/image/debugrect.png");
		this->font = DG::Font::Create("HG丸ゴシックM-PRO", 8, 16);
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgDebugRectHitBase.reset();
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
		this->render2D_Priority[1] = -1.f;

		//★データ初期化
		this->isOn = false;
		this->hitFlag = false;
		
		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->mouse.reset();

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
		auto game =		ge->GetTask<Game::Object>("MainGame", "NoName");
		if (true == isOn) {

			auto square =	ge->GetTasks<Square::Object>("Square");
			auto mouse =	ge->mouse->GetState();

			//当たり判定ボックスの表示
			if (square != nullptr) {
				for (auto& it : *square) {
					ML::Box2D drawRectApple = it->hitBase;
					ML::Box2D src(64 + 32 * this->hitFlag, 32, 32, 32);
					this->res->imgDebugRectHitBase->Draw(drawRectApple, src);
				}
			}

			//マウスの位置
			ML::Box2D textBox(0, 0, 480, 720);
			string text =
				"mouse pos X: " + to_string(mouse.pos.x) + "\n" +
				"mouse pos Y: " + to_string(mouse.pos.y) + "\n" ;
			this->res->font->Draw(textBox, text, ML::Color(1.0f, 1.0f, 0.0f, 0.0f));
			//
			//


		}
			//
			//クワッドツリー描画とデータ表示
			if (game->IsQuadtreeOn && game->IsShowQuadtree) {
				if (game->quadTree != nullptr) {
					game->quadTree->Draw(game->res->imgQT);
				}


				std::string QTtext = "QT:" + to_string(game->QTcnt);
				int x = 0;
				int charWidth = 32;
				int charHeight = 32;

				int screenWidth = ge->screen2DWidth;
				int totalTextWidth = static_cast<int>(QTtext.size()) * charWidth;
				int startX = 0 + totalTextWidth - 20; // 左から20ピクセルの余白

				for (char c : QTtext) {
					int charIndex = static_cast<int>(c); // オフセットなし

					if (charIndex >= 32 && charIndex < 128) {
						int charX = (charIndex % 16) * charWidth;
						int charY = (charIndex / 16) * charHeight;

						ML::Box2D QTtextbox(startX + x, 20, charWidth, charHeight);
						ML::Box2D srcQT(charX, charY, charWidth, charHeight);

						game->res->scoreFont->Draw(QTtextbox, srcQT);
						x += charWidth;
					}
				}
			}
			//
			//
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