#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Make some meshes
	for (int i = 0; i < 46; i++) {
		m_aMeshes[i] = new MyMesh();
		m_aMeshes[i]->GenerateCube(1.0f, C_BLACK);
		m_aMeshTranslations[i] = new vector3();
	}
	m_aMeshTranslations[0] = new vector3(-3.0f, 4.0f, 0.0f);
	m_aMeshTranslations[1] = new vector3(3.0f, 4.0f, 0.0f);
	m_aMeshTranslations[2] = new vector3(-2.0f, 3.0f, 0.0f);
	m_aMeshTranslations[3] = new vector3(2.0f, 3.0f, 0.0f);
	m_aMeshTranslations[4] = new vector3(-3.0f, 2.0f, 0.0f);
	m_aMeshTranslations[5] = new vector3(-2.0f, 2.0f, 0.0f);
	m_aMeshTranslations[6] = new vector3(-1.0f, 2.0f, 0.0f);
	m_aMeshTranslations[7] = new vector3(0.0f, 2.0f, 0.0f);
	m_aMeshTranslations[8] = new vector3(1.0f, 2.0f, 0.0f);
	m_aMeshTranslations[9] = new vector3(2.0f, 2.0f, 0.0f);
	m_aMeshTranslations[10] = new vector3(3.0f, 2.0f, 0.0f);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	for (int i = 0; i < 46; i++) {
		//get a matrix transformation
		matrix4 m_mTransform =  glm::translate(*m_aMeshTranslations[i]);
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
void Application::Release(void)
{
	if (m_pMesh != nullptr)
	{
		delete m_pMesh;
		m_pMesh = nullptr;
	}
	SafeDelete(m_pMesh1);
	//release GUI
	ShutdownGUI();
}