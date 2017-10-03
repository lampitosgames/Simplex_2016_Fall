#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 20.0f),
		vector3(0.0f,0.0f,0.0f),
		vector3(0.0f, 1.0f, 0.0f)
		);

	//init the mesh
	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
	m_pMesh->GenerateCone(1.0f, 1.5f, 10, C_GREEN);
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4Model = IDENTITY_M4;

	vector3 v3Start = vector3(0.0f, 0.0f, 0.0f);
	vector3 v3End = vector3(5.0f, 0.0f, 0.0f);

	static DWORD startTime = GetTickCount();
	DWORD currentTime = GetTickCount();
	//Current time in seconds
	float fCurrentTime = (currentTime - startTime) / 1000.0f;

	float fPercent = MapValue(fCurrentTime, 0.0f, 5.0f, 0.0f, 1.0f);

	vector3 v3Current = glm::lerp(v3Start, v3End, fPercent);

	m4Model = glm::translate(IDENTITY_M4, v3Current);

	m_pMesh->Render(m4Projection, m4View, m4Model);

#pragma region Debugging Information
	//Print info on the screen
	uint nEmptyLines = 21;
	for (uint i = 0; i < nEmptyLines; ++i)
		m_pMeshMngr->PrintLine("");

	//m_pMeshMngr->Print("						");
	m_pMeshMngr->Print("CurrentTime: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string((currentTime - startTime)/ 1000.0f), C_YELLOW);
#pragma endregion
	
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
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}