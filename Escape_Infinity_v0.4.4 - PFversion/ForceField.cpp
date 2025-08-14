#include  "MyPG.h"
#include  "Task_Player.h"
#include  "Debug_Display.h"
#include  "ForceField.h"


namespace  ForceField
{
	Resource::WP  Resource::instance;
	//-------------------------------------------------------------------
	//���\�[�X�̏�����
	bool  Resource::Initialize()
	{
		this->imgForceField = DG::Image::Create("./data/image/Objects/SpaceShooterAssetPack_Miscellaneous.png");
	//	this->imgForceField->Rotation(ML::ToRadian(180.f), { 40.f, 20.f });
		return true;
	}
	//-------------------------------------------------------------------
	//���\�[�X�̉��
	bool  Resource::Finalize()
	{
		this->imgForceField.reset();
		return true;
	}
	//-------------------------------------------------------------------
	//�u�������v�^�X�N�������ɂP�񂾂��s������
	bool  Object::Initialize()
	{
		//�X�[�p�[�N���X������
		__super::Initialize(defGroupName, defName, true);
		//���\�[�X�N���X����or���\�[�X���L
		this->res = Resource::Create();

		//���f�[�^������
		this->render2D_Priority[1] = 0.2f;

		this->pos = ML::Vec2(0.0f, 0.0f);
		this->state = State::Non;
		this->hitBox = ML::Box2D(0, 0, 80, 40);
		this->drawBase = ML::Box2D(-40, -20, 80, 40);
		this->src = ML::Box2D(0, 16, 16, 8);

		//���^�X�N�̐���
		//auto debugdisplay = ge->GetTask<DebugDisplay::Object>("Debug", "DebugDisplay");
		//debugdisplay->cannoncnt++;
		//ge->Dbg_ToBox("");

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�I���v�^�X�N���Ŏ��ɂP�񂾂��s������
	bool  Object::Finalize()
	{
		//���f�[�^���^�X�N���


		if (!ge->QuitFlag() && this->nextTaskCreate) {
			//�������p���^�X�N�̐���
		}

		return  true;
	}
	//-------------------------------------------------------------------
	//�u�X�V�v�P�t���[�����ɍs������
	void  Object::UpDate()
	{

	}
	//-------------------------------------------------------------------
	//�u�Q�c�`��v�P�t���[�����ɍs������
	void  Object::Render2D_AF()
	{
		if (this->state == State::Non) {
			return;
		}
	//	else if (this->state == State::Normal) {
			ML::Box2D  draw = this->drawBase;
			ML::Box2D  src = this->src;
			draw.Offset(this->pos.x, this->pos.y);
			this->res->imgForceField->Rotation(ML::ToRadian(180.f), { 40.f, 20.f });
			this->res->imgForceField->Draw(draw, src);
			this->res->imgForceField->Rotation(0, {0, 0}); //14mai2025 reset rotation
	//	}
	}

	//������������������������������������������������������������������������������������
	//�ȉ��͊�{�I�ɕύX�s�v�ȃ��\�b�h
	//������������������������������������������������������������������������������������
	//-------------------------------------------------------------------
	//�^�X�N��������
	Object::SP  Object::Create(bool  flagGameEnginePushBack_)
	{
		Object::SP  ob = Object::SP(new  Object());
		if (ob) {
			ob->me = ob;
			if (flagGameEnginePushBack_) {
				ge->PushBack(ob);//�Q�[���G���W���ɓo�^
			}
			if (!ob->B_Initialize()) {
				ob->Kill();//�C�j�V�����C�Y�Ɏ��s������Kill
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
	//���\�[�X�N���X�̐���
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