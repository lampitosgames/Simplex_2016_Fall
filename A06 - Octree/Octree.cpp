#include "Octree.h"
#include "MyEntityManager.h"
#include "MyRigidBody.h"
#include <list>
#include <vector>
#include <deque>

using namespace Simplex;

//Init static vars
uint Octree::s_uOctantCount = 0;
uint Octree::s_uMaxLevel = 3;
uint Octree::s_uIdealEntityCount = 5;
Octree* Octree::s_pRoot = nullptr;
bool Octree::s_bTreeReady = false;
bool Octree::s_bTreeBuilt = false;
std::deque<uint> Octree::s_qToInsert;
void Octree::Init(void) {
	//If there is no root yet, this is the root
	if (Octree::s_pRoot == nullptr) {
		Octree::s_pRoot = this;
	}

	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	m_uID = Octree::s_uOctantCount++;
	m_uLevel = 0;

	m_bbRegion = BoundingBox();

	m_bActiveNodes = 0;

	m_pParent = nullptr;

}

Octree::Octree() {
	Init();
}

Octree::Octree(BoundingBox a_bbRegion) {
	Init();
	m_bbRegion = a_bbRegion;
}

Octree::Octree(BoundingBox a_bbRegion, std::vector<uint> const &a_lObjectList) {
	Init();
	m_bbRegion = a_bbRegion;
	for (uint i = 0; i < a_lObjectList.size(); i++) {
		m_lEntities.push_back(a_lObjectList[i]);
	}
}

void Octree::UpdateTree(void) {
	//If the tree has not been built
	if (!Octree::s_bTreeBuilt) {
		//Loop through all queued objects waiting to be inserted
		while (Octree::s_qToInsert.size() != 0) {
			//Insert all into the entity list and remove from front of queue
			m_lEntities.push_back(Octree::s_qToInsert.front());
			Octree::s_qToInsert.pop_front();
		}
		//Recursively build the tree
		BuildTree();
	} else {
		while (Octree::s_qToInsert.size() != 0) {
			Insert(Octree::s_qToInsert.front());
			Octree::s_qToInsert.pop_front();
		}
	}
}

void Octree::Display(vector3 a_v3Color) {
	//Draw cube for this octant
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_bbRegion.center) * glm::scale(m_bbRegion.halfSize * 2.0f), a_v3Color);
	//Loop through and call display for all children
	for (uint i = 0; i < 8; i++) {
		//null check
		if (m_pChildren[i] == nullptr) { continue; }
		m_pChildren[i]->Display(a_v3Color);
	}
}

void Octree::BuildTree(void) {
	//If there are no dimensions, expand to the extents of all objects in the world
	if (m_bbRegion.Dimensions() == ZERO_V3) {
		GenerateExtents();
	}
	//If there are fewer than the ideal entity count in this octant, don't do anything since this node is a leaf
	if (m_lEntities.size() <= Octree::s_uIdealEntityCount) {
		return;
	}
	//If this is a max-level octant, return
	if (m_uLevel == Octree::s_uMaxLevel) {
		return;
	}

	//Store stuff from bounding box
	vector3 l_min = m_bbRegion.Min();
	vector3 l_max = m_bbRegion.Max();
	vector3 l_c = m_bbRegion.center;

	//Generate bounding boxes for all 8 children
	BoundingBox childBounds[8];
	childBounds[0] = BoundingBox(l_min, l_c);
	childBounds[1] = BoundingBox(vector3(l_c.x, l_min.y, l_min.z), vector3(l_max.x, l_c.y, l_c.z));
	childBounds[2] = BoundingBox(vector3(l_c.x, l_min.y, l_c.z), vector3(l_max.x, l_c.y, l_max.z));
	childBounds[3] = BoundingBox(vector3(l_min.x, l_min.y, l_c.z), vector3(l_c.x, l_c.y, l_max.z));
	childBounds[4] = BoundingBox(vector3(l_min.x, l_c.y, l_min.z), vector3(l_c.x, l_max.y, l_c.z));
	childBounds[5] = BoundingBox(vector3(l_c.x, l_c.y, l_min.z), vector3(l_max.x, l_max.y, l_c.z));
	childBounds[6] = BoundingBox(l_c, l_max);
	childBounds[7] = BoundingBox(vector3(l_min.x, l_c.y, l_c.z), vector3(l_c.x, l_max.y, l_max.z));

	//List of entity ids that fit in each octant
	std::vector<uint> childLists[8];
	//List of objects no longer in this octant (they're in a child now)
	std::list<uint> toRemove;
	//Loop through all objects in this octant
	for (uint e = 0; e < m_lEntities.size(); e++) {
		//Get this entity's rigidbody
		Simplex::MyRigidBody* thisRB = m_pEntityMngr->GetEntity(m_lEntities[e])->GetRigidBody();
		//loop through every bounding box we just created
		for (uint b = 0; b < 8; b++) {
			//If the entity is in the bounding box
			if (childBounds[b].Contains(thisRB)) {
				//Add it to the child octant
				childLists[b].push_back(m_lEntities[e]);
				//Add it to the "toRemove" list so it will no longer be listed in this octant
				toRemove.push_back(m_lEntities[e]);
				break;
			}
		}
	}

	//Remove duplicate items from toRemove list
	toRemove.unique();
	std::vector<uint> newEntityList;
	//Remove every moved object from this octant
	for (uint r = 0; r < m_lEntities.size(); r++) {
		bool shouldRemove = false;
		//Check the "toRemove" list for matches
		for (std::list<uint>::const_iterator i = toRemove.begin(), end = toRemove.end(); i != end; ++i) {
			shouldRemove = m_lEntities[r] == *i;
		}
		//If the entity should remain in this octant
		if (!shouldRemove) {
			//Add it to the new list
			newEntityList.push_back(m_lEntities[r]);
		}
	}
	//Set the new vector to the current vector
	m_lEntities = newEntityList;

	for (uint a = 0; a < 8; a++) {
		if (childLists[a].size() != 0) {
			m_pChildren[a] = CreateChildOctant(childBounds[a], childLists[a]);
			m_pChildren[a]->BuildTree();
		}
	}

	Octree::s_bTreeBuilt = true;
	Octree::s_bTreeReady = true;
}

Octree* Octree::CreateChildOctant(BoundingBox bounds, std::vector<uint> entities) {
	if (entities.size() == 0) {
		return nullptr;
	}
	Octree* newOctant = new Octree(bounds, entities);
	newOctant->m_pParent = this;
	return newOctant;
}

Octree* Octree::CreateChildOctant(BoundingBox bounds, uint entityIndex) {
	std::vector<uint> entList(1);
	entList[0] = entityIndex;
	return CreateChildOctant(bounds, entList);
}

void Octree::Insert(uint a_uEntityIndex) {}


// Generates the bounding box for this octant based on the objects it contains
void Octree::GenerateExtents(void) {
	vector3 min = vector3(0.0f);
	vector3 max = vector3(0.0f);
	for (uint i = 0; i < m_lEntities.size(); i++) {
		MyRigidBody* thisRB = m_pEntityMngr->GetEntity(m_lEntities[i])->GetRigidBody();
		vector3 rbMin = thisRB->GetMinGlobal();
		vector3 rbMax = thisRB->GetMaxGlobal();
		//Check all axes for min
		min.x = (rbMin.x < min.x) ? rbMin.x : min.x;
		min.y = (rbMin.y < min.y) ? rbMin.y : min.y;
		min.z = (rbMin.z < min.z) ? rbMin.z : min.z;
		//Check all axes for max
		max.x = (rbMax.x > max.x) ? rbMax.x : max.x;
		max.y = (rbMax.y > max.y) ? rbMax.y : max.y;
		max.z = (rbMax.z > max.z) ? rbMax.z : max.z;
	}
	//Get the center point
	m_bbRegion.center = (min + max) / 2.0f;
	//Get the half width
	m_bbRegion.halfSize = max - m_bbRegion.center;
}

bool Simplex::BoundingBox::Contains(Simplex::MyRigidBody * _rb) {
	vector3 min = this->Min();
	vector3 max = this->Max();
	vector3 rbMin = _rb->GetMinGlobal();
	vector3 rbMax = _rb->GetMaxGlobal();
	//AABB check
	return (min.x <= rbMin.x && max.x >= rbMax.x &&
			min.y <= rbMin.y && max.y >= rbMax.y &&
			min.z <= rbMin.z && max.z >= rbMax.z);
}
