#pragma once
//----------------------------------------------
//企画科用 音楽再生ライブラリ
// Ci-Lib派生 汎用関数
//      k.koyanagi
//----------------------------------------------

#include <assert.h> 
#include <string>
#	include <dshow.h>
#	include <dsound.h>

namespace bgm{
	//BGM再生関数
	void Init();
	void Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	bool Play(const std::string& resname);
	void RateControl(const std::string& resname, float rate);
	void EndCheck();
	void Stop(const std::string& resname);
	void AllStop();
	LONGLONG GetCurrentPos(const std::string& resname);
	void SetStartPos(const std::string& resname, LONGLONG start_); //modified by Prinz - added start_ for unpausing bgm
	void VolumeControl(const std::string& resname, int volume);
	void Pause(const std::string& resname);
	LONGLONG GetEndPosition(const std::string& resname);
}
namespace se{
	//SE再生関数
	void Init(HWND);
	bool Create(HWND);
	void CreateSecondaryBuffer();
	bool CreatePrimaryBuffer();
	bool Del();
	void Play(const std::string& resname);
	void PlayLoop(const std::string& resname);
	bool Del();
	void LoadFile(const std::string& resname, const std::string& filename);
	void AllStop();
	void Stop(const std::string& resname);
	void EndCheck();
	void SetFrequency(const std::string& resname, int Fre);
	void SetVolume(const std::string& resname, int Vol);
}


