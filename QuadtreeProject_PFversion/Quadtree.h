#pragma warning(disable:4996)
#pragma once
//-------------------------------------------------------------------
//Quadtree
//-------------------------------------------------------------------
#include "GameEngine_Ver3_83.h"
#include "BChara.h"

class Quadtree
{
private:
	//各ノードに分割させる前に入れられるオブジェクト数
	const int MAX_OBJECTS = 10;

	// 深さ（分割レベル）
	// 完全に分割された場合：最深レベルの葉数 = 4^MAX_LEVELS、全ノード数 = (4^(MAX_LEVELS+1)-1)/3
	const int MAX_LEVELS = 5;

	int level;
	std::vector<BChara::SP> objects;

	Quadtree* children[4];
	ML::Box2D bounds;

	void Subdivide();

public:
	Quadtree(int level, const ML::Box2D& bounds);
	
    void Insert(BChara::SP obj);

	void Draw(DG::Image::SP& img);

	void Query(const ML::Box2D& region, std::vector<BChara::SP>& result);

};