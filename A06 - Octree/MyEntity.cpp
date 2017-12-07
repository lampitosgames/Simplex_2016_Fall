#include "MyEntity.h"
#include "Octree.h"
using namespace Simplex;
std::map<String, MyEntity*> MyEntity::m_IDMap;
//  Accessors
matrix4 Simplex::MyEntity::GetModelMatrix(void){ return m_m4ToWorld; }
void Simplex::MyEntity::SetModelMatrix(matrix4 a_m4ToWorld)
{
	if (!m_bInMemory)
		return;

	m_m4ToWorld = a_m4ToWorld;
	m_pModel->SetModelMatrix(m_m4ToWorld);
	m_pRigidBody->SetModelMatrix(m_m4ToWorld);
}
bool Simplex::MyEntity::GetHasChanged(void) {
	if (!m_bInMemory)
		return false;
	return m_pRigidBody->GetHasChanged();
}
void Simplex::MyEntity::ChangesAccepted(void) {
	if (!m_bInMemory)
		return;
	m_pRigidBody->ChangesAccepted();
}
Model* Simplex::MyEntity::GetModel(void){return m_pModel;}
MyRigidBody* Simplex::MyEntity::GetRigidBody(void){	return m_pRigidBody; }
bool Simplex::MyEntity::IsInitialized(void){ return m_bInMemory; }
String Simplex::MyEntity::GetUniqueID(void) { return m_sUniqueID; }
void Simplex::MyEntity::SetAxisVisible(bool a_bSetAxis) { m_bSetAxis = a_bSetAxis; }
//  MyEntity
void Simplex::MyEntity::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bInMemory = false;
	m_bSetAxis = false;
	m_pModel = nullptr;
	m_pRigidBody = nullptr;
	m_m4ToWorld = IDENTITY_M4;
	m_sUniqueID = "";
	m_pOctants = nullptr;
	m_uOctantCount = 0;
}
void Simplex::MyEntity::Swap(MyEntity& other)
{
	m_bInMemory = false;
	std::swap(m_pModel, other.m_pModel);
	std::swap(m_pRigidBody, other.m_pRigidBody);
	std::swap(m_m4ToWorld, other.m_m4ToWorld);
	std::swap(m_pMeshMngr, other.m_pMeshMngr);
	std::swap(m_bInMemory, other.m_bInMemory);
	std::swap(m_sUniqueID, other.m_sUniqueID);
	std::swap(m_bSetAxis, other.m_bSetAxis);
	std::swap(m_uOctantCount, other.m_uOctantCount);
	std::swap(m_pOctants, other.m_pOctants);
}
void Simplex::MyEntity::Release(void)
{
	m_pMeshMngr = nullptr;
	//it is not the job of the entity to release the model, 
	//it is for the mesh manager to do so.
	m_pModel = nullptr;
	if (m_pOctants)
	{
		delete[] m_pOctants;
		m_pOctants = nullptr;
	}
	SafeDelete(m_pRigidBody);
	m_IDMap.erase(m_sUniqueID);
}
//The big 3
Simplex::MyEntity::MyEntity(String a_sFileName, String a_sUniqueID)
{
	Init();
	m_pModel = new Model();
	m_pModel->Load(a_sFileName);
	//if the model is loaded
	if (m_pModel->GetName() != "")
	{
		GenUniqueID(a_sUniqueID);
		m_sUniqueID = a_sUniqueID;
		m_IDMap[a_sUniqueID] = this;
		m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); //generate a rigid body
		m_bInMemory = true; //mark this entity as viable
	}
}
Simplex::MyEntity::MyEntity(MyEntity const& other)
{
	m_bInMemory = other.m_bInMemory;
	m_pModel = other.m_pModel;
	//generate a new rigid body we do not share the same rigid body as we do the model
	m_pRigidBody = new MyRigidBody(m_pModel->GetVertexList()); 
	m_m4ToWorld = other.m_m4ToWorld;
	m_pMeshMngr = other.m_pMeshMngr;
	m_sUniqueID = other.m_sUniqueID;
	m_bSetAxis = other.m_bSetAxis;
	m_uOctantCount = other.m_uOctantCount;
	m_pOctants = other.m_pOctants;
}
MyEntity& Simplex::MyEntity::operator=(MyEntity const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		MyEntity temp(other);
		Swap(temp);
	}
	return *this;
}
MyEntity::~MyEntity(){Release();}
//--- Methods
void Simplex::MyEntity::AddToRenderList(bool a_bDrawRigidBody)
{
	//if not in memory return
	if (!m_bInMemory)
		return;

	//draw model
	m_pModel->AddToRenderList();
	
	//draw rigid body
	if(a_bDrawRigidBody)
		m_pRigidBody->AddToRenderList();

	if (m_bSetAxis)
		m_pMeshMngr->AddAxisToRenderList(m_m4ToWorld);
}
MyEntity* Simplex::MyEntity::GetEntity(String a_sUniqueID)
{
	//look the entity based on the unique id
	auto entity = m_IDMap.find(a_sUniqueID);
	//if not found return nullptr, if found return it
	return entity == m_IDMap.end() ? nullptr : entity->second;
}
void Simplex::MyEntity::GenUniqueID(String& a_sUniqueID)
{
	static uint index = 0;
	String sName = a_sUniqueID;
	MyEntity* pEntity = GetEntity(a_sUniqueID);
	//while MyEntity exists keep changing name
	while (pEntity)
	{
		a_sUniqueID = sName + "_" + std::to_string(index);
		index++;
		pEntity = GetEntity(a_sUniqueID);
	}
	return;
}
bool Simplex::MyEntity::IsColliding(MyEntity* const other)
{
	//if not in memory return
	if (!m_bInMemory || !other->m_bInMemory)
		return true;

	if (!SharesOctant(other)) {
		return false;
	}

	return m_pRigidBody->IsColliding(other->GetRigidBody());
}
void Simplex::MyEntity::ClearCollisionList(void)
{
	m_pRigidBody->ClearCollidingList();
}

void Simplex::MyEntity::AddOctant(uint octantID) {
	uint* tempOctants = new uint[m_uOctantCount + 1];
	for (uint i = 0; i < m_uOctantCount; i++) {
		tempOctants[i] = m_pOctants[i];
	}
	tempOctants[m_uOctantCount] = octantID;
	if (m_pOctants != nullptr) {
		delete[] m_pOctants;
	}
	m_pOctants = tempOctants;
	m_uOctantCount++;

	//we need to check that this octant is not already allocated in the list
	if (HasOctant(octantID))
		return;//it is, so there is no need to add

    //insert the entry
	uint* pTemp;
	pTemp = new uint[m_uOctantCount + 1];
	if (m_pOctants) {
		memcpy(pTemp, m_pOctants, sizeof(uint) * m_uOctantCount);
		delete[] m_pOctants;
		m_pOctants = nullptr;
	}
	pTemp[m_uOctantCount] = octantID;
	m_pOctants = pTemp;

	m_uOctantCount++;

	SortOctants();
}

void Simplex::MyEntity::ClearOctantList() {
	if (m_pOctants == nullptr) {
		return;
	}
	delete[] m_pOctants;
	m_pOctants = nullptr;
	m_uOctantCount = 0;
}

void Simplex::MyEntity::RemoveOctant(uint octantID) {
	if (m_uOctantCount == 0) {
		return;
	}
	
	for (uint i = 0; i < m_uOctantCount; i++) {
		if (m_pOctants[i] == octantID) {
			//if it is, then we swap it with the last one and then we pop
			std::swap(m_pOctants[i], m_pOctants[m_uOctantCount - 1]);
			uint* pTemp;
			pTemp = new uint[m_uOctantCount - 1];
			if (m_pOctants) {
				memcpy(pTemp, m_pOctants, sizeof(uint) * (m_uOctantCount - 1));
				delete[] m_pOctants;
				m_pOctants = nullptr;
			}
			m_pOctants = pTemp;
			m_uOctantCount--;
			SortOctants();
			return;
		}
	}
}

bool Simplex::MyEntity::SharesOctant(MyEntity* const other) {
	//If neither entity has a dimension, both are in global space
	if (m_uOctantCount == 0 && other->m_uOctantCount == 0) {
		return true;
	}

	for (uint i = 0; i < m_uOctantCount; i++) {
		for (uint j = 0; j < other->m_uOctantCount; j++) {
			if (m_pOctants[i] == other->m_pOctants[j]) {
				return true;
			}
		}
	}
	return false;
}

bool Simplex::MyEntity::HasOctant(uint octantID) {
	for (uint i = 0; i < m_uOctantCount; i++) {
		if (m_pOctants[i] == octantID) {
			return true;
		}
	}
	return false;
}

void Simplex::MyEntity::SortOctants(void) {
	std::sort(m_pOctants, m_pOctants + m_uOctantCount);
}
