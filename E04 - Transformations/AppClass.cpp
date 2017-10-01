#include "AppClass.h"
void Application::InitVariables(void) {
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make some meshes
	for (int i = 0; i < 46; i++) {
		m_aMeshes[i] = new MyMesh();
		m_aMeshes[i]->GenerateCube(1.0f, C_BLACK);
		m_aMeshTranslations[i] = vector3();
	}

	//Initialize the position to offscreen on the left
	m_vPosition = vector3(-10.0, 0.0f, 0.0f);

	//Row 1
	m_aMeshTranslations[0] = vector3(-3.0f, 4.0f, 0.0f);
	m_aMeshTranslations[1] = vector3(3.0f, 4.0f, 0.0f);
	//Row 2
	m_aMeshTranslations[2] = vector3(-2.0f, 3.0f, 0.0f);
	m_aMeshTranslations[3] = vector3(2.0f, 3.0f, 0.0f);
	//Row 3
	m_aMeshTranslations[4] = vector3(-3.0f, 2.0f, 0.0f);
	m_aMeshTranslations[5] = vector3(-2.0f, 2.0f, 0.0f);
	m_aMeshTranslations[6] = vector3(-1.0f, 2.0f, 0.0f);
	m_aMeshTranslations[7] = vector3(0.0f, 2.0f, 0.0f);
	m_aMeshTranslations[8] = vector3(1.0f, 2.0f, 0.0f);
	m_aMeshTranslations[9] = vector3(2.0f, 2.0f, 0.0f);
	m_aMeshTranslations[10] = vector3(3.0f, 2.0f, 0.0f);
	//Row 4
	m_aMeshTranslations[11] = vector3(-4.0f, 1.0f, 0.0f);
	m_aMeshTranslations[12] = vector3(-3.0f, 1.0f, 0.0f);
	m_aMeshTranslations[13] = vector3(-1.0f, 1.0f, 0.0f);
	m_aMeshTranslations[14] = vector3(0.0f, 1.0f, 0.0f);
	m_aMeshTranslations[15] = vector3(1.0f, 1.0f, 0.0f);
	m_aMeshTranslations[16] = vector3(3.0f, 1.0f, 0.0f);
	m_aMeshTranslations[17] = vector3(4.0f, 1.0f, 0.0f);
	//Row 5
	m_aMeshTranslations[18] = vector3(-5.0f, 0.0f, 0.0f);
	m_aMeshTranslations[19] = vector3(-4.0f, 0.0f, 0.0f);
	m_aMeshTranslations[20] = vector3(-3.0f, 0.0f, 0.0f);
	m_aMeshTranslations[21] = vector3(-2.0f, 0.0f, 0.0f);
	m_aMeshTranslations[22] = vector3(-1.0f, 0.0f, 0.0f);
	m_aMeshTranslations[23] = vector3(0.0f, 0.0f, 0.0f);
	m_aMeshTranslations[24] = vector3(1.0f, 0.0f, 0.0f);
	m_aMeshTranslations[25] = vector3(2.0f, 0.0f, 0.0f);
	m_aMeshTranslations[26] = vector3(3.0f, 0.0f, 0.0f);
	m_aMeshTranslations[27] = vector3(4.0f, 0.0f, 0.0f);
	m_aMeshTranslations[28] = vector3(5.0f, 0.0f, 0.0f);
	//Row 6
	m_aMeshTranslations[29] = vector3(-5.0f, -1.0f, 0.0f);
	m_aMeshTranslations[30] = vector3(-3.0f, -1.0f, 0.0f);
	m_aMeshTranslations[31] = vector3(-2.0f, -1.0f, 0.0f);
	m_aMeshTranslations[32] = vector3(-1.0f, -1.0f, 0.0f);
	m_aMeshTranslations[33] = vector3(0.0f, -1.0f, 0.0f);
	m_aMeshTranslations[34] = vector3(1.0f, -1.0f, 0.0f);
	m_aMeshTranslations[35] = vector3(2.0f, -1.0f, 0.0f);
	m_aMeshTranslations[36] = vector3(3.0f, -1.0f, 0.0f);
	m_aMeshTranslations[37] = vector3(5.0f, -1.0f, 0.0f);
	//Row 7
	m_aMeshTranslations[38] = vector3(-5.0f, -2.0f, 0.0f);
	m_aMeshTranslations[39] = vector3(-3.0f, -2.0f, 0.0f);
	m_aMeshTranslations[40] = vector3(3.0f, -2.0f, 0.0f);
	m_aMeshTranslations[41] = vector3(5.0f, -2.0f, 0.0f);
	//Row 8
	m_aMeshTranslations[42] = vector3(-2.0f, -3.0f, 0.0f);
	m_aMeshTranslations[43] = vector3(-1.0f, -3.0f, 0.0f);
	m_aMeshTranslations[44] = vector3(1.0f, -3.0f, 0.0f);
	m_aMeshTranslations[45] = vector3(2.0f, -3.0f, 0.0f);
}
void Application::Update(void) {
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void) {
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();


	for (int i = 0; i < 46; i++) {
		//Add to the vector to cause movement
		m_vPosition += vector3(0.0001, 0.0, 0.0);
		//get a matrix transformation
		matrix4 m_mTransform = glm::translate(m_aMeshTranslations[i] + m_vPosition);
		//Render the mesh at that position
		m_aMeshes[i]->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m_mTransform);
	}


	//m_pMesh->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(vector3(3.0f, 0.0f, 0.0f)));
	//m_pMesh1->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m_m4Object1);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void) {
	if (m_aMeshes != nullptr) {
		for (int i = 0; i < 46; i++) {
			if (m_aMeshes[i] != nullptr) {
				SafeDelete(m_aMeshes[i]);
			}
		}
	}
	//release GUI
	ShutdownGUI();
}