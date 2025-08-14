//-------------------------------------------------------------------
//
//-------------------------------------------------------------------
#include  "MyPG.h"
#include  "Quadtree.h"
#include  "Task_Game.h"

Quadtree::Quadtree(int level, const ML::Box2D& bounds)
    : level(level), bounds(bounds) {
    for (int i = 0; i < 4; ++i)
        children[i] = nullptr;

    auto game = ge->GetTask<Game::Object>("MainGame", "NoName");
    game->QTcnt++;

}

/// <summary>
/// クワッドツリーにオブジェクトを入れる
/// </summary>
/// <param name="obj"></param>
void Quadtree::Insert(BChara::SP obj)
{
    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            if (children[i]->bounds.Hit(obj->hitBase)) { //子ノードとしての領域に入っているオブジェクトだけを入れる
                this->children[i]->Insert(obj);
            }
        }
        return;
    }

    this->objects.push_back(obj);

    //オブジェクト数の限界を超えたら、分別して子ノードにオブジェクトを再配布する
    if (this->objects.size() > this->MAX_OBJECTS && this->level < this->MAX_LEVELS) {
        Subdivide();
        //現在のオブジェクトを子ノードに渡す
        for (auto& o : objects) {
            for (int i = 0; i < 4; ++i) {
                if (children[i]->bounds.Hit(o->hitBase)) {
                    children[i]->Insert(o);
                }
            }
        }
        //再配布後に現在のノードのオブジェクトリストを削除する
        objects.clear();
    }
}

/// <summary>
/// 分別処理
/// </summary>
void Quadtree::Subdivide() 
{
    //親ノードの領域を図って、4分の1を各子ノードに与える
    float halfW = bounds.w / 2.0f;
    float halfH = bounds.h / 2.0f;
    float x = bounds.x;
    float y = bounds.y;

    //子ノード生成
    // 0: 左上
    children[0] = new Quadtree(level + 1, ML::Box2D(x, y, halfW, halfH));
    // 1: 右上
    children[1] = new Quadtree(level + 1, ML::Box2D(x + halfW, y, halfW, halfH));
    // 2: 左下
    children[2] = new Quadtree(level + 1, ML::Box2D(x, y + halfH, halfW, halfH));
    // 3: 右下
    children[3] = new Quadtree(level + 1, ML::Box2D(x + halfW, y + halfH, halfW, halfH));
}

/// <summary>
/// クワッドツリーの各ノードの境界線を描画する処理
/// </summary>
/// <param name="img"></param>
void Quadtree::Draw(DG::Image::SP& img) 
{
    // 再帰的に子ノードをたどり、それぞれの矩形領域（bounds）に対して、
    // 白い1×1ピクセルの画像をスケーリングして線として描画する
    float thickness = 1.0f;
    ML::Box2D src(0, 0, 1, 1);
    ML::Color color = {1.0f, 1.0f, 1.0f, 1.0f};
    img->Draw({ bounds.x, bounds.y, bounds.w, thickness}, src, color); //上の境界線
    img->Draw({ bounds.x, bounds.y + bounds.h + thickness, bounds.w, thickness }, src, color); //下の境界線
    img->Draw({ bounds.x, bounds.y, thickness, bounds.h }, src, color); //左の境界線
    img->Draw({ bounds.x + bounds.w + thickness, bounds.y, thickness, bounds.h }, src, color); //右の境界線

    // 子ノードが存在したら、再帰的に子ノードをたどる
    for (int i = 0; i < 4; ++i) {
        if (children[i] != nullptr) {
            children[i]->Draw(img);
        }
    }
}

/// <summary>
/// 指定領域と衝突しているオブジェクトを再帰的に検索し、結果リストに追加する。
/// </summary>
/// <param name="area"></param>
/// <param name="result"></param>
void Quadtree::Query(const ML::Box2D& area, std::vector<BChara::SP>& result) 
{
    if (!this->bounds.Hit(area)) {
        return;
    }
    
    for (auto& obj : this->objects) {
        if (area.Hit(obj->hitBase)) {
            result.push_back(obj);    
        }
    }

    for (int i = 0; i < 4; ++i) {
        if (children[i] != nullptr) {
            children[i]->Query(area, result);
        }
    }
}