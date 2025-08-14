//-------------------------------------------------------------------
//エンディング
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "Task_StartButton.h"	
#include  "randomLib.h"
#include  "sound.h"

namespace  Ending
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		this->imgEnding = DG::Image::Create("./data/image/grass_template2_0.png");
		this->font = DG::Image::Create("./data/image/font_text.png");

		se::LoadFile("Ding", "./data/sound/se/poku.wav");
		se::SetVolume("Ding", 100);

		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		this->imgEnding.reset();
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
		this->points = 0;
		this->frameCnt = 0;
		this->posX = ge->screenWidth / 2;
		this->posY = ge->screenHeight / 2;
		this->isScoreDisplayed = false;

		//★タスクの生成

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
		ge->KillAll_G("Start");

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto  nextTask = Title::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		this->frameCnt++;
		if (this->frameCnt % 60 == 0 && false == this->isScoreDisplayed) {
			se::Play("Ding");
		}
		auto startbutton = ge->GetTask<Start::Object>("Start", "NoName");

		if (this->frameCnt / 180 == 1 && startbutton == nullptr) {
			se::Play("Ding");
			auto startbutton = Start::Object::Create(true);
			startbutton->pos = ML::Box2D(this->posX - startbutton->pos.w / 2, this->posY + 100, startbutton->pos.w, startbutton->pos.h);
		}\

		if (inp.ST.down && this->isScoreDisplayed) {
			ge->StartCounter("test", 45); //フェードは90フレームなので半分の45で切り替え
			ge->CreateEffect(99, ML::Vec2(0, 0));
		}
		if (ge->getCounterFlag("test") == ge->LIMIT) {
			this->Kill();
		}

		//if (inp.B1.down) {
		//	ML::Vec2 p;
		//	ge->CreateEffect(0,p);
		//}
		//if (inp.B2.down) {
		//	ML::Vec2 p((const float)GetRandom(0, 300), (const float)GetRandom(0, 300));
		//	ge->CreateEffect((int)GetRandom(0,8), p);

		//}
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//-------------	背景	-------------------------------------------------------
		ML::Box2D draw(0, 0, 800, 600);
		ML::Box2D src(0, 0, 1280, 720);
		draw.Offset(0, 0);
		this->res->imgEnding->Draw(draw, src);
		//-------------------------------------------------------------------------
		
		//------------	テキスト	---------------------------------------------------
		std::string scoreText = "Final score:";
		std::string scoreNumber = to_string(this->points);

		int x = 0;
		int charWidth = 32;
		int charHeight = 32;
		int totalTextWidth = static_cast<int>(scoreText.size()) * charWidth;
		int startX = this->posX - totalTextWidth - 20; // 20px margin from the right

		for (char c : scoreText) {
			int charIndex = static_cast<int>(c); // No offset

			if (charIndex >= 32 && charIndex < 128) {
				int charX = (charIndex % 16) * charWidth;
				int charY = (charIndex / 16) * charHeight;

				ML::Box2D scoreBox(startX + x + 200, this->posY - 50, charWidth, charHeight);
				ML::Box2D src(charX, charY, charWidth, charHeight);

				if(this->frameCnt > 60){
					this->res->font->Draw(scoreBox, src); 
				}
				
				x += charWidth;
			}
		}
		for (char c : scoreNumber) {
			int charIndex = static_cast<int>(c); // No offset

			if (charIndex >= 32 && charIndex < 128) {
				int charX = (charIndex % 16) * charWidth;
				int charY = (charIndex / 16) * charHeight;

				ML::Box2D scoreBox(startX + x, posY + charHeight, charWidth, charHeight);
				ML::Box2D src(charX, charY, charWidth, charHeight);

				if (this->frameCnt > 120) { 
					this->res->font->Draw(scoreBox, src);
					this->isScoreDisplayed = true;
				}
				x += charWidth;
			}
		}
		//---------------------------------------------------------------------------
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