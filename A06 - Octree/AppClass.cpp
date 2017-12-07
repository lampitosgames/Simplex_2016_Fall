#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void) {
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	#ifdef DEBUG
	uint uInstances = 900;
	#else
	uint uInstances = 1849;
	#endif
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;

	rootNode = new Octree();

	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++) {
		for (int j = 0; j < nSquare; j++) {
			uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");
			vector3 v3Position = vector3(glm::sphericalRand(34.0f));
			matrix4 m4Position = glm::translate(v3Position) * glm::rotate(25.0f, vector3(glm::sphericalRand(10.0f)));
			m_pEntityMngr->SetModelMatrix(m4Position);

			Octree::s_qToInsert.push_back(m_pEntityMngr->GetEntityCount() - 1);
		}
	}
	m_uOctantLevels = 1;

	rootNode->UpdateTree();

	m_pEntityMngr->Update();
}
void Application::Update(void) {
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	if (m_bRealtimeOctreeActive) {
		matrix4 rotationOverTime = glm::rotate(0.5f, 1.0f, 0.0f, 0.0f);
		//Rotate all objects
		for (uint e = 0; e < m_pEntityMngr->GetEntityCount(); e++) {
			matrix4 modelMatrix = m_pEntityMngr->GetEntity(e)->GetModelMatrix();
			m_pEntityMngr->GetEntity(e)->SetModelMatrix(rotationOverTime * modelMatrix);
		}

		rootNode->Update();
	} else {
		if (Octree::s_bShouldRebuild) {
			matrix4 rotationOverTime = glm::rotate(0.5f, 1.0f, 0.0f, 0.0f);
			delete rootNode;
			rootNode = new Octree();

			//Rotate all objects
			for (uint e = 0; e < m_pEntityMngr->GetEntityCount(); e++) {
				matrix4 modelMatrix = m_pEntityMngr->GetEntity(e)->GetModelMatrix();
				m_pEntityMngr->GetEntity(e)->SetModelMatrix(rotationOverTime * modelMatrix);
				Octree::s_qToInsert.push_back(e);
			}
			rootNode->UpdateTree();
			Octree::s_bShouldRebuild = false;
		}
	}

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void) {
	// Clear the screen
	ClearScreen();


	//display octree
	rootNode->DisplayDifferently(C_YELLOW, C_RED);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void) {
	//release GUI
	ShutdownGUI();
}