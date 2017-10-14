#include "AppClass.h"
void Application::InitVariables(void) {
	////Change this to your name and email
	//m_sProgrammer = "Daniel Timko - det2948@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER) {
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}

	//if there are no segments create 7
	if (m_uOrbits < 1)
		m_uOrbits = 7;

	//initial size of orbits
	float fSize = 1.0f;
	//Initial orbit radius
	float fRadius = 0.95f;
	//Store the value for PI
	float pi = glm::pi<float>();

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1 ? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++) {
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		//Create a path for this orbital
		std::vector<vector3> stopList;
		//Loop for every stop
		for (uint stop = 0; stop < i; stop++) {
			//Calculate the angle around the circle for this stop based on the total number of stops
			float fTheta = -(2.0f*pi*stop) / i;
			//Calculate and add a stop to the list
			stopList.push_back(vector3(glm::cos(fTheta)*fRadius, 0.0f, glm::sin(fTheta)*fRadius));
		}
		//Add this stop list to the list of paths
		m_pathList.push_back(stopList);

		//increment the size for the next orbit
		fSize += 0.5f;
		fRadius += 0.5f;
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	
	//offset of the orbits to face the camera
	matrix4 m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);
	m4Offset = glm::rotate(m4Offset, 90.0f, AXIS_X);

	//Get a timer
	static float fTimer = 0.0f;
	static uint uClock = m_pSystem->GenClock();
	//Increase the timer by the delta time
	fTimer += m_pSystem->GetDeltaTime(uClock);
	//Length of the animation between two stops
	float fTimeMax = 0.5f;
	//Get how far through the animation we are
	float fPercent = MapValue(fTimer, 0.0f, fTimeMax, 0.0f, 1.0f);

	//Current stop on the path
	static uint iStop = 0;

	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i) {
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], m4Offset);

		//Current path size
		int iPSize = m_pathList[i].size();
		//calculate the current position
		vector3 v3CurrentPos = glm::lerp(m_pathList[i][iStop % iPSize],
										 m_pathList[i][(iStop+1) % iPSize],
										 fPercent);
		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//If the timer has reached the max, increment the current stop and reset the timer
	if (fTimer >= fTimeMax) {
		fTimer = 0.0f;
		iStop++;
	}

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
	//release GUI
	ShutdownGUI();
}