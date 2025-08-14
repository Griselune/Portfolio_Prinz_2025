//-------------------------------------------------------------------
//ゲーム本編
//-------------------------------------------------------------------
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include  "MyPG.h"
#include  "sound.h"
#include  "Task_Game.h"
#include  "Task_GameBGBack.h"
#include  "Task_GameBGMid.h"
#include  "Task_GameBGFront.h"
#include  "Task_Player.h"
#include  "Task_Ending.h"
#include  "Task_Title.h"
#include  "UI_Panel.h"
#include  "Debug_Display.h"
#include  "Task_Enemy000.h"
#include  "Task_Pause.h"
#include  "Enemy_Generator.h"

namespace  Game
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//リソースの初期化
	bool  Resource::Initialize()
	{
		bgm::LoadFile("mainBGM", "./data/Sounds/Music/funky_bgm_loop.wav");
		bgm::SetStartPos("mainBGM", 0);
		bgm::VolumeControl("mainBGM", 100);
		bgm::Play("mainBGM");
		return true;
	}
	//-------------------------------------------------------------------
	//リソースの解放
	bool  Resource::Finalize()
	{
		bgm::Stop("mainBGM");
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
		this->debugmode = false;
		this->IsPause = false;
		this->IsResetData = false;
		this->IsChangingGeneratorToSpawn = true; //sets the enemy generator to ON
		this->IsGodMode = false; //useful for game testing and debugging
		this->start = 0; //starting point for bgm
		this->gravityPull = ML::Vec2(0.05f, 0.f); //influence of the black hole
		//sets the timers to 0
		this->timeCnt = this->TIMECNTBASE;
		this->timeWave = this->TIMEWAVEBASE;
		this->durationWave = 1800; //time before the wave pattern changes - base 3600 (1mn)

		//★タスクの生成
		//背景タスク
		for (int i = 0; i < 2; i++) {
			auto  bgb = GameBGBack::Object::Create(true);  
			auto  bgm = GameBGMid::Object::Create(true);   
			auto  bgf = GameBGFront::Object::Create(true); 
			bgb->y = -960 * i;
			bgm->y = -960 * i;
			bgf->y = -960 * i;
		}

		//Game UI
		auto uip = UIPanel::Object::Create(true);

		//プレイヤ
		player = Player::Object::Create(true);
	//	player->gravityPull = this->gravityPull;
		player->leftmissile->gravityPull = this->gravityPull * 1;
		player->rightmissile->gravityPull = this->gravityPull * 1;

		//debug mode
		auto debug = DebugDisplay::Object::Create(true);

		//enemy waves
		enemyGenerator = EnemyGenerator::Object::Create(true);
		enemyGenerator->state = EnemyGenerator::Object::State::Stop;
		enemyGenerator->gravityPull = this->gravityPull;
		
																	//***************************//

		if(player->HP <= 0){	//To avoid the game starting with 0HP
			this->ResetGame("./data/Saves/save1.txt");
		}
		//Loading the saved game
		this->LoadGame("./data/Saves/save1.txt");

		return  true;
	}
	//-------------------------------------------------------------------
	//「終了」タスク消滅時に１回だけ行う処理
	bool  Object::Finalize()
	{
		//★データ＆タスク解放
					//Resets the save file if needed ( triggers : player death / delete save )
		if (this->IsResetData == true) {
			this->ResetGame("./data/Saves/save1.txt");
			this->IsResetData = false;
		}

	//	bgm::AllStop();

		ge->KillAll_G("Debug", false);				//1
		ge->KillAll_G("MainBG", false);				//2
		ge->KillAll_G("UI", false);					//3
		ge->KillAll_G("Weapons", false);			//4
		ge->KillAll_G("EnemyGenerator", false);		//5
		ge->KillAll_G("EnemyGroup", false);			//6
		ge->KillAll_G("PlayerGroup", false);		//7
		ge->KillAll_G("Upgrades", false);			//8
	//	ge->KillAll_GN("Scenes", "Game", false);	//9
		ge->KillAll_G("Menu", false);				//10

		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//★引き継ぎタスクの生成
			auto next = Ending::Object::Create(true);
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//「更新」１フレーム毎に行う処理
	void  Object::UpDate()
	{
		auto inp = ge->in1->GetState();

		if (this->IsResetData == true) {
			this->Kill();
			return;
		}

		//toggle debug mode
		if (inp.SE.down) { 
			debugmode = !debugmode; 
			player->debugmode = this->debugmode;
		}

		//-------GOD MODE--------//
		 //toggle GOD MODE with R3 button
		if (inp.R3.down) {
			this->IsGodMode = !this->IsGodMode;
			player->godmode = this->IsGodMode;
		}
		//////////////////////////

		//Pause the game
		if (inp.ST.down) {
			this->start = bgm::GetCurrentPos("mainBGM");
			bgm::Pause("mainBGM");
			this->IsPause = true;
			auto pausemenu = Pause::Object::Create(true);
		}
		else if (IsPause == true) {
			bgm::SetStartPos("mainBGM", this->start);
			bgm::Play("mainBGM");
			IsPause = false;
		}

		//************************************************Enemy Wave management***************************************************************//

		if (this->IsChangingGeneratorToSpawn) { //optimization - prevents from operating each frame
			this->IsChangingGeneratorToSpawn = false;
			this->enemyGenerator->state = EnemyGenerator::Object::State::Spawn;
		}

	//	this->timeCnt = this->durationWave; //debug - force a value on timeCnt
	//	this->timeWave = this->durationWave; //debug

		
		if (this->timeWave % this->durationWave == 0) {
			this->SaveGame("./data/Saves/save1.txt"); //saves the game at each wave

			//WAVE MANAGER - simple version : cycles through different patterns and spawns enemy a bit faster every 5 minutes cycle
			// If the wave duration is 1minute, enemies are spawned 10 frames faster each cycle to a minimun of 1 each 10 frames per kind
			switch (this->timeWave / this->durationWave) {		//Wave pattern changes every minute (= 3600)
			case 0:
				this->enemyGenerator->IsSpawningDrifter = true; //allows asteroids to spawn
				this->enemyGenerator->drifterTimeCntMax = max(10, 120 - static_cast<int>(this->timeCnt / this->durationWave) * 10);  //adjusts the spawn rate of asteroids(base 1 each 120 frames)
				this->enemyGenerator->IsSpawningShooter = false;
				break;
			case 1:
				this->enemyGenerator->IsSpawningShooter = true; //allows shooter enemies to spawn
				this->enemyGenerator->shooterTimeCntMax = max(10, 240 - static_cast<int>(this->timeCnt / this->durationWave) * 10);	//adjusts the spawn rate of shooters(base 1 each 240 frames)
				break;
			case 2:
				this->enemyGenerator->drifterTimeCntMax = max(10, 90 - static_cast<int>(this->timeCnt / this->durationWave) * 10); // base is 90
				this->enemyGenerator->shooterTimeCntMax = max(10, 200 - static_cast<int>(this->timeCnt / this->durationWave) * 10); //base is 200
				break;
			case 3:
				this->enemyGenerator->drifterTimeCntMax = max(10, 90 - static_cast<int>(this->timeCnt / this->durationWave) * 10); //base is 90
				this->enemyGenerator->shooterTimeCntMax = max(10, 150 - static_cast<int>(this->timeCnt / this->durationWave) * 10); //base is 150
				break;
			case 4:
				this->enemyGenerator->drifterTimeCntMax = max(10, 200 - static_cast<int>(this->timeCnt / this->durationWave) * 10); //base is 200
				this->enemyGenerator->shooterTimeCntMax = max(10, 40 - static_cast<int>(this->timeCnt / this->durationWave) * 10); // base is 40
				break;

			default: //resets everything and returns to first spawn pattern - needs rework in the future
				this->timeWave = -10; //let some time to reset the algo
				this->enemyGenerator->IsSpawningDrifter = false;
				this->enemyGenerator->IsSpawningShooter = false;
				break;					
			}
		}
		this->timeCnt++;
		enemyGenerator->timeCnt = this->timeCnt; //pass down the clock to the generator to do enemy stats calculations
		this->timeWave++;
		//*********************************************************************end of enemy wave management ************************************************//
	}
	//-------------------------------------------------------------------
	//「２Ｄ描画」１フレーム毎に行う処理
	void  Object::Render2D_AF()
	{
		//nothing to render
	}


	void Object::SaveGame(const std::string& fileName) 
	{
		GameSaveData saveData;

		// Collect player data
		player->SaveData(saveData.playerData);

		// Collect game time data
		saveData.timeCnt = this->timeCnt;
		saveData.timeWave = this->timeWave;
		saveData.IsSpawningDrifter = this->enemyGenerator->IsSpawningDrifter;
		saveData.IsSpawningShooter = this->enemyGenerator->IsSpawningShooter;

		std::ofstream outFile(fileName);
		if (outFile.is_open()) {
			//player stats save
			outFile << "Player speed = " << saveData.playerData.speed << "\n";
			outFile << "Player HP = " << saveData.playerData.HP << "\n";
			outFile << "Player HPRecovery = " << saveData.playerData.HPRecovery << "\n";
			outFile << "Player HPRecoveryTimer = " << saveData.playerData.HPRecoveryTimer << "\n";
			outFile << "Player energy = " << saveData.playerData.energy << "\n";
			outFile << "Player totalEXP = " << saveData.playerData.totalEXP << "\n";
			outFile << "Player currentEXP = " << saveData.playerData.currentEXP << "\n";
			outFile << "Player level = " << saveData.playerData.level << "\n";
			outFile << "Player armorPercentage = " << saveData.playerData.armorPercentage << "\n";
			outFile << "Player armorFlat = " << saveData.playerData.armorFlat << "\n";
			outFile << "Player shootingSpeed = " << saveData.playerData.shootingSpeed << "\n";
			outFile << "Player shootingOverHeatThreshold = " << saveData.playerData.shootingOverHeatThreshold << "\n";
			outFile << "Player pierce = " << saveData.playerData.pierce << "\n";
			outFile << "Player cannonDmg = " << saveData.playerData.cannonDmg << "\n";
			outFile << "Player exploRadius = " << saveData.playerData.exploRadius << "\n";
			outFile << "Player exploDmg = " << saveData.playerData.exploDmg << "\n";
			outFile << "Player missileCoolDown = " << saveData.playerData.missileCoolDown << "\n";

			// game time save
			outFile << "timeCnt = " << saveData.timeCnt << "\n";
			outFile << "timeWave = " << saveData.timeWave << "\n";
			outFile << "IsSpawningDrifter = " << static_cast<int>(saveData.IsSpawningDrifter) << "\n";
			outFile << "IsSpawningShooter = " << static_cast<int>(saveData.IsSpawningShooter) << "\n";

			//end
			outFile.close();
			ge->Dbg_ToConsole("Game saved successfully!");
		}
		else {
			ge->Dbg_ToConsole("Failed to open file for saving.");
		}
	}

	bool Object::LoadGame(const std::string& fileName) 
	{
		GameSaveData saveData;
		std::ifstream inFile(fileName);
		if (inFile.peek() == std::ifstream::traits_type::eof()) { //if no data found (first time launching the game), saves the base values
			ge->Dbg_ToConsole("No game save found. Creating a new save.");
			ResetGame(fileName);
			return false;
		}
		if (inFile.is_open()) {
			//player stats
			saveData.playerData.speed = ReadFloat(inFile);
			saveData.playerData.HP = ReadFloat(inFile);
			saveData.playerData.HPRecovery = ReadFloat(inFile);
			saveData.playerData.HPRecoveryTimer = ReadInt(inFile);
			saveData.playerData.energy = ReadFloat(inFile);
			saveData.playerData.totalEXP = ReadLong(inFile); //long
			saveData.playerData.currentEXP = ReadInt(inFile);
			saveData.playerData.level = ReadInt(inFile);
			saveData.playerData.armorPercentage = ReadFloat(inFile);
			saveData.playerData.armorFlat = ReadInt(inFile);
			saveData.playerData.shootingSpeed = ReadFloat(inFile);
			saveData.playerData.shootingOverHeatThreshold = ReadInt(inFile);
			saveData.playerData.pierce = ReadInt(inFile);
			saveData.playerData.cannonDmg = ReadInt(inFile);
			saveData.playerData.exploRadius = ReadInt(inFile);
			saveData.playerData.exploDmg = ReadFloat(inFile);
			saveData.playerData.missileCoolDown = ReadInt(inFile);

			//game time
			saveData.timeCnt = ReadInt(inFile);
			saveData.timeWave = ReadInt(inFile);
			saveData.IsSpawningDrifter = ReadBool(inFile);
			saveData.IsSpawningShooter = ReadBool(inFile);

			inFile.close();

			// Distribute to tasks
			player->LoadData(saveData.playerData);
			// sets the loaded time
			this->timeCnt = saveData.timeCnt;
			this->timeWave = saveData.timeWave;
			this->enemyGenerator->IsSpawningDrifter = saveData.IsSpawningDrifter;
			this->enemyGenerator->IsSpawningShooter = saveData.IsSpawningShooter;

			ge->Dbg_ToConsole("Game loaded successfully!");
			return true;
		}
		else {
			ge->Dbg_ToConsole("Failed to open file for loading.");
			return false;
		}
	}

	//-------Readers----------------------------------------------------------
	float Object::ReadFloat(std::ifstream& file)
	{
		std::string line;
		std::getline(file, line);             // Read the whole line
		std::stringstream ss(line);
		std::string label;
		float value;
		std::getline(ss, label, '=');         // Skip the label
		ss >> value;                          // Extract value after '='
		return value;
	}
	int Object::ReadInt(std::ifstream& file)
	{
		std::string line;
		std::getline(file, line);             // Read the whole line
		std::stringstream ss(line);
		std::string label;
		int value;
		std::getline(ss, label, '=');         // Skip the label
		ss >> value;                          // Extract value after '='
		return value;
	}
	bool Object::ReadBool(std::ifstream& file)
	{
		std::string line;
		std::getline(file, line);             // Read the whole line
		std::stringstream ss(line);
		std::string label;
		int value;
		std::getline(ss, label, '=');         // Skip the label
		ss >> value;                          // Extract value after '='
		return static_cast<bool>(ReadInt(file));
	}
	long Object::ReadLong(std::ifstream& file)
	{
		std::string line;
		std::getline(file, line);             // Read the whole line
		std::stringstream ss(line);
		std::string label;
		long value;
		std::getline(ss, label, '=');         // Skip the label
		ss >> value;                          // Extract value after '='
		return value;
	}
	ML::Vec2 Object::ReadVec2(std::ifstream& file) 
	{
		std::string line;
		std::getline(file, line);
		std::stringstream ss(line);
		std::string label;
		ML::Vec2 vec;
		std::getline(ss, label, '=');
		ss >> vec.x >> vec.y;
		return vec;
	}
	//-------------------------------------------------------------------------------------


	void Object::ResetGame(const std::string& fileName)
	{
		GameResetData resetData;
		// Collect player data
		player->ResetData(resetData.playerReset); //needs rework, access to void -> exception **probably OK**


		std::ofstream outFile(fileName);
		if (outFile.is_open()) {
			//player stats save
			outFile << "Player speed = " << resetData.playerReset.speedBase << "\n";
			outFile << "Player HP = " << resetData.playerReset.HPBase << "\n";
			outFile << "Player HPRecovery = " << resetData.playerReset.HPRecoveryBase << "\n";
			outFile << "Player HPRecoveryTimer = " << resetData.playerReset.HPRecoveryTimerBase << "\n";
			outFile << "Player energy = " << resetData.playerReset.energyBase << "\n";
			outFile << "Player totalEXP = " << resetData.playerReset.totalEXP << "\n";
			outFile << "Player currentEXP = " << resetData.playerReset.currentEXP << "\n";
			outFile << "Player level = " << resetData.playerReset.level << "\n";
			outFile << "Player armorPercentage = " << resetData.playerReset.armorPercentageBase << "\n";
			outFile << "Player armorFlat = " << resetData.playerReset.armorFlatBase << "\n";
			outFile << "Player shootingSpeed = " << resetData.playerReset.shootingSpeedBase << "\n";
			outFile << "Player shootingOverHeatThreshold = " << resetData.playerReset.shootingOverHeatThresholdBase << "\n";
			outFile << "Player pierce = " << resetData.playerReset.pierceBase << "\n";
			outFile << "Player cannonDmg = " << resetData.playerReset.cannonDmgBase << "\n";
			outFile << "Player exploRadius = " << resetData.playerReset.exploRadiusBase << "\n";
			outFile << "Player exploDmg = " << resetData.playerReset.exploDmgBase << "\n";
			outFile << "Player missileCoolDown = " << resetData.playerReset.missileCoolDownBase << "\n";

			// game time save
			outFile << "timeCnt = " << resetData.timeCnt << "\n";
			outFile << "timeWave = " << resetData.timeWave << "\n";
			outFile << "IsSpawningDrifter = " << static_cast<int>(resetData.IsSpawningDrifter) << "\n";
			outFile << "IsSpawningShooter = " << static_cast<int>(resetData.IsSpawningShooter) << "\n";

			//end
			outFile.close();
			ge->Dbg_ToConsole("Game reset successfully!");
		}
		else {
			ge->Dbg_ToConsole("Failed to open file for reset.");
		}
	}

	//deletes the selected save file, useful for game over or save file management
	void Object::DeleteSaveFile(const std::string& fileName) {
		if (std::remove(fileName.c_str()) == 0) {
			ge->Dbg_ToConsole("Save file '%s' deleted successfully.", fileName.c_str());
		}
		else {
			ge->Dbg_ToConsole("Failed to delete save file '%s' or file does not exist.", fileName.c_str());
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