#ifndef __MYOCTREE_H_
#define __MYOCTREE_H_

#include "Simplex\Simplex.h"

namespace Simplex {
	//Forward declare
	class MyRigidBody;
	class MyEntityManager;

	struct BoundingBox {
		vector3 center = ZERO_V3;
		vector3 halfSize = ZERO_V3;

		BoundingBox(vector3 _min, vector3 _max) {
			center = (_max + _min) / 2.0f;
			halfSize = _max - center;
		}
		BoundingBox() {}

		vector3 Dimensions() {
			return Max() - Min();
		}

		vector3 Min() {
			return center - halfSize;
		}
		vector3 Max() {
			return center + halfSize;
		}

		//AABB check for rigidbodies
		bool Contains(MyRigidBody* _rb);
	};

	class Octree {
		MeshManager* m_pMeshMngr;
		MyEntityManager* m_pEntityMngr;

		//Bounding box for this tree
		BoundingBox m_bbRegion;
		//List of entities in this octant
		std::vector<uint> m_lEntities;

		uint m_uID;
		uint m_uLevel;

		byte m_bActiveNodes;

		Octree* m_pParent;
		Octree* m_pChildren[8];

	public:
		//Total number of octants
		static uint s_uOctantCount;
		//Max level 
		static uint s_uMaxLevel;
		//Ideal number of entiti
		static uint s_uIdealEntityCount;
		//Root octant
		static Octree* s_pRoot;
		//Tree still has objects requiring insertion
		static bool s_bTreeReady;
		//Is there an existing tree?
		static bool s_bTreeBuilt;
		//Queue of objects waiting to be inserted
		static std::deque<uint> s_qToInsert;

		//Constructors
		Octree();
		Octree(BoundingBox a_bbRegion);
		Octree::Octree(BoundingBox a_bbRegion, std::vector<uint> const &a_lObjectList);
		//Copy constructor
		//Octree(Octant const &other);
		//Copy assignment operator
		//Octree& operator=(Octant const &other);
		//Destructor
		//~Octree(void);
		//Swap objects
		//void Swap(Octant &other);
		//Update the tree
		void UpdateTree(void);
		//Checks if colliding with entity
		//bool IsColliding(uint a_uEntityIndex);
		//Draw this octant and call the same method on children
		void Display(vector3 a_v3Color = C_YELLOW);
		//Only display octants that are leafs
		//void DisplayLeafs(vector3 a_v3Color = C_YELLOW);
		//Clears the entity list for this and child nodes
		//void ClearEntityList(void);
		//Subdivide into 8 smaller octants
		//void Subdivide(void);
		//Returns child octant by index
		//Octree GetChild(uint a_nChild);
		//Returns parent
		//Octree GetParent(void);
		//Is this octant a leaf?
		//bool IsLeaf(void);
		//Does this octant contain more than this many objects
		//bool ContainsMoreThan(uint a_nEntities);
		//Recursively destroys all children
		//void KillBranches(void);
		//Create a tree using subdivisions
		void BuildTree(void);
		//Insert entity into tree
		void Insert(uint a_uEntityIndex);
		//Traverse the tree down to the leafs and give all objects the index of the octant
		void AssignIDToEntity(void);

		//Generates a bounding area based on the entities in the list
		void GenerateExtents(void);
	private:
		void Release(void);
		void Init(void);
		void ConstructList(void);
		Octree* CreateChildOctant(BoundingBox bounds, std::vector<uint> entities);
		Octree* CreateChildOctant(BoundingBox bounds, uint entityIndex);
	};
};

#endif