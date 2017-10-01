#include "MyMesh.h"
void MyMesh::Init(void) {
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void) {
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh() {
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other) {
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other) {
	if (this != &other) {
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other) {
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color) {
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i) {
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input) {
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input) {
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void) {
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++) {
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel) {
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader);

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));

	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft) {
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight) {
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color) {
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue, -fValue, fValue); //0
	vector3 point1(fValue, -fValue, fValue); //1
	vector3 point2(fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue, -fValue, -fValue); //4
	vector3 point5(fValue, -fValue, -fValue); //5
	vector3 point6(fValue, fValue, -fValue); //6
	vector3 point7(-fValue, fValue, -fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color) {
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color) {
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Get PI
	float pi = glm::pi<float>();
	//Get the top/bottom center points
	vector3 bottomCenter(0.0f, -0.3f*a_fHeight, 0.0f);
	vector3 topCenter(0.0f, 0.7f*a_fHeight, 0.0f);

	//Create an array of vertices that form a circle with the number of provided subdivisions at the base of the cone
	vector3* circleVerts = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++) {
		float theta = (2.0f*pi*i) / a_nSubdivisions;
		circleVerts[i] = vector3(glm::cos(theta)*a_fRadius, -0.3f*a_fHeight, glm::sin(theta)*a_fRadius);
	}

	//Create the bottom disc and connect it to the top vertex
	for (int i = 0; i < a_nSubdivisions; i++) {
		//The i'th vert in the circle array
		vector3 edgeVert1 = circleVerts[i];
		//Add 1 to i.  If it would go oob on the circleverts array, wrap to 0
		vector3 edgeVert2 = circleVerts[(i + 1) % a_nSubdivisions];
		//Create a bottom face with them
		AddTri(edgeVert1, bottomCenter, edgeVert2);
		//Add the vertical tri that connects these to the top vertex
		AddTri(edgeVert2, topCenter, edgeVert1);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	//clean up
	delete[] circleVerts;
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color) {
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Get PI
	float pi = glm::pi<float>();
	//Get the top/bottom center points
	vector3 bottomCenter(0.0f, -0.5f*a_fHeight, 0.0f);
	vector3 topCenter(0.0f, 0.5f*a_fHeight, 0.0f);

	//Create an array of vertices that form a circle with the number of provided subdivisions
	vector3* circleVerts = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++) {
		float theta = (2.0f*pi*i) / a_nSubdivisions;
		circleVerts[i] = vector3(glm::cos(theta)*a_fRadius, 0.0f, glm::sin(theta)*a_fRadius);
	}

	//Create the top and bottom discs and walls to connect them
	for (int i = 0; i < a_nSubdivisions; i++) {
		//The i'th vert in the circle array
		vector3 topEdgeVert1 = circleVerts[i] + topCenter;
		//Add 1 to i.  If it would go oob on the circleverts array, wrap to 0
		vector3 topEdgeVert2 = circleVerts[(i + 1) % a_nSubdivisions] + topCenter;
		//Add a top face with these two verts
		AddTri(topEdgeVert1, topCenter, topEdgeVert2);

		//The i'th vert in the circle array
		vector3 bottomEdgeVert1 = circleVerts[i] + bottomCenter;
		//Add 1 to i.  If it would go OOB on the circleverts array, wrap to 0
		vector3 bottomEdgeVert2 = circleVerts[(i + 1) % a_nSubdivisions] + bottomCenter;
		//Add a bottom face with these two verts
		AddTri(bottomEdgeVert2, bottomCenter, bottomEdgeVert1);

		//Add the vertical quad connecting them
		AddQuad(bottomEdgeVert2, bottomEdgeVert1, topEdgeVert2, topEdgeVert1);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	//clean up
	delete[] circleVerts;
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color) {
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Get PI
	float pi = glm::pi<float>();
	//Get the top/bottom heights
	vector3 bottom(0.0f, -0.5f*a_fHeight, 0.0f);
	vector3 top(0.0f, 0.5f*a_fHeight, 0.0f);

	//Create two arrays of vertices that form a circle with the number of provided subdivisions
	//One array for the outer radius, one for the inner radius
	vector3* innerCircleVerts = new vector3[a_nSubdivisions];
	vector3* outerCircleVerts = new vector3[a_nSubdivisions];
	for (int i = 0; i < a_nSubdivisions; i++) {
		float theta = (2.0f*pi*i) / a_nSubdivisions;
		innerCircleVerts[i] = vector3(glm::cos(theta)*a_fInnerRadius, 0.0f, glm::sin(theta)*a_fInnerRadius);
		outerCircleVerts[i] = vector3(glm::cos(theta)*a_fOuterRadius, 0.0f, glm::sin(theta)*a_fOuterRadius);
	}

	//Loop through all subdivisions
	for (int i = 0; i < a_nSubdivisions; i++) {
		//Inner verts for this subdivision
		vector3 topInnerVert1 = innerCircleVerts[i] + top;
		vector3 topInnerVert2 = innerCircleVerts[(i + 1) % a_nSubdivisions] + top;
		vector3 bottomInnerVert1 = innerCircleVerts[i] + bottom;
		vector3 bottomInnerVert2 = innerCircleVerts[(i + 1) % a_nSubdivisions] + bottom;
		//Outer verts for this subdivision
		vector3 topOuterVert1 = outerCircleVerts[i] + top;
		vector3 topOuterVert2 = outerCircleVerts[(i + 1) % a_nSubdivisions] + top;
		vector3 bottomOuterVert1 = outerCircleVerts[i] + bottom;
		vector3 bottomOuterVert2 = outerCircleVerts[(i + 1) % a_nSubdivisions] + bottom;

		//Add a top, inner, bottom, and outer faces for this subdivision
		AddQuad(topInnerVert1, topInnerVert2, topOuterVert1, topOuterVert2);
		AddQuad(topInnerVert2, topInnerVert1, bottomInnerVert2, bottomInnerVert1);
		AddQuad(bottomInnerVert2, bottomInnerVert1, bottomOuterVert2, bottomOuterVert1);
		AddQuad(topOuterVert1, topOuterVert2, bottomOuterVert1, bottomOuterVert2);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

	//clean up
	delete[] innerCircleVerts;
	delete[] outerCircleVerts;
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color) {
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	//Get pi
	float pi = glm::pi<float>();
	//Get the radius of the torus
	float torusRadius = (a_fOuterRadius + a_fInnerRadius) / 2.0f;
	//Get the radius of the tube
	float tubeRadius = (a_fOuterRadius - a_fInnerRadius) / 2.0f;

	//2d vector of vector3s. Every list in this vector represents a circle positioned on the tube of the torus
	std::vector<std::vector<vector3>> radialCircles(a_nSubdivisionsA);

	//Loop through the number of subdivisions of the torus
	for (int i = 0; i < a_nSubdivisionsA; i++) {
		//Get the theta at this location
		float torusTheta = (2.0f*pi*i) / a_nSubdivisionsA;
		
		//Create a new list to hold the positions around the tube at this torusTheta
		std::vector<vector3> circlePositions(a_nSubdivisionsB);
		//Loop through the number of subdivisions of the tube
		for (int j = 0; j < a_nSubdivisionsB; j++) {
			//Calculate the tubeTheta (how far around the tube this point will be)
			float tubeTheta = (2.0f*pi*j) / a_nSubdivisionsB;
			//Calculate the vertex location using the equation for a torus
			float x = (torusRadius + tubeRadius*glm::cos(tubeTheta))*glm::cos(torusTheta);
			float y = tubeRadius*glm::sin(tubeTheta);
			float z = (torusRadius + tubeRadius*glm::cos(tubeTheta))*glm::sin(torusTheta);
			circlePositions[j] = vector3(x, y, z);
		}
		//Add this circle to the array
		radialCircles[i] = circlePositions;
	}

	//Loop through all the edge circles
	for (int i = 0; i < a_nSubdivisionsA; i++) {
		//Get the current circle and the next one, looping if needed
		std::vector<vector3> circleA = radialCircles[i];
		std::vector<vector3> circleB = radialCircles[(i + 1) % a_nSubdivisionsA];

		//Loop for every vertex in the circle array
		for (int j = 0; j < a_nSubdivisionsB; j++) {
			//Grab all 4 points that will make up a quad and create said quad
			vector3 point1 = circleA[j];
			vector3 point2 = circleA[(j + 1) % a_nSubdivisionsB];
			vector3 point3 = circleB[j];
			vector3 point4 = circleB[(j + 1) % a_nSubdivisionsB];
			AddQuad(point1, point2, point3, point4);
		}
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color) {
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 0) {
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//I used (but didn't copy) this tutorial: https://schneide.wordpress.com/2016/07/15/generating-an-icosphere-in-c/
	//I used their hard-coded initial icosahedron so I could divide it further

	//Initialize the hard-coded icosahedron
	float x = glm::sin(glm::radians(31.7f))*a_fRadius;
	float z = glm::cos(glm::radians(31.7f))*a_fRadius;
	float n = 0.0f;
	//List of vertices making up the initial icosahedron to be subdivided
	std::vector<vector3> vertices = {
		vector3(-x, n, z), vector3(x, n, z), vector3(-x, n, -z), vector3(x, n, -z),
		vector3(n, z, x), vector3(n, z, -x), vector3(n, -z, x), vector3(n, -z, -x),
		vector3(z, x, n), vector3(-z, x, n), vector3(z, -x, n), vector3(-z, -x, n)
	};
	//List of tri faces that make up the circle
	std::vector<std::vector<int>> triangles = {
		{0,4,1}, {0,9,4}, {9,5,4}, {4,5,8}, {4,8,1},
		{8,10,1}, {8,3,10}, {5,3,8}, {5,2,3}, {2,7,3},
		{7,10,3}, {7,6,10}, {7,11,6}, {11,0,6}, {0,1,6},
		{6,1,10}, {9,0,11}, {9,11,2}, {9,2,5}, {7,2,11}
	};

	//For each requested subdivision, subdivide the icosahedron to make it a more accurate circle
	for (int s = 0; s < a_nSubdivisions; s++) {
		//All tris for the more accurate circle will be stored here
		std::vector<std::vector<int>> newTriangles = {};
		//Map that allows lookup of a middle vert's index in the vert array given the two points it is in the middle of
		std::map<std::pair<int, int>, int> edgesMiddleInd;

		//Subdivide each tri on the circle
		for (int i = 0; i < triangles.size(); i++) {
			//Grab the current triangle
			std::vector<int> oldTri = triangles[i];
			//Store the corner indices
			int corner1 = oldTri[0],
				corner2 = oldTri[1],
				corner3 = oldTri[2];
			//Store pairs of corners as lookup values (sides) for the map of middle points
			std::pair<int, int> sides[3];
			sides[0] = std::pair<int, int>(corner1, corner2);
			sides[1] = std::pair<int, int>(corner2, corner3);
			sides[2] = std::pair<int, int>(corner3, corner1);

			for (int j = 0; j < 3; j++) {
				//Order the current sides pair from smallest to largest. This prevents 0,1 and 1,0 from 
				//being two different values in the map(which would generate 2 different middle vertices)
				if (sides[j].first > sides[j].second) { std::swap(sides[j].first, sides[j].second); }
				//If the middle vert for this side doesn't exist, create it
				auto middleInsert = edgesMiddleInd.insert({ sides[j], vertices.size() });
				if (middleInsert.second) {
					//Generate the middle vertex and add it to the array.
					//radius * norm(point1 + point2) = a point equidistant from both point1 and point2, normalized onto the sphere's shell
					vertices.push_back(a_fRadius * glm::normalize(vertices[sides[j].first] + vertices[sides[j].second]));
				}
			}
			//Get the array index (in the vert array) of each middle point
			int middle1 = edgesMiddleInd[sides[0]];
			int middle2 = edgesMiddleInd[sides[1]];
			int middle3 = edgesMiddleInd[sides[2]];
			//Push all 4 new tris into the new triangles array
			/*	  *
				 / \
				/ 4 \
			   *-----*
			  / \ 3 / \
			 / 1 \ / 2 \
			*-----*-----*/
			newTriangles.push_back({ corner1,middle1,middle3 }); //1
			newTriangles.push_back({ middle1,corner2,middle2 }); //2
			newTriangles.push_back({ middle1,middle2,middle3 }); //3
			newTriangles.push_back({ middle3,middle2,corner3 }); //4
		}
		//Replace the old triangles array with the new one for the next iteration
		triangles = newTriangles;
	}


	//Construct the subdivided sphere
	for (int i = 0; i < triangles.size(); i++) {
		AddTri(vertices[triangles[i][0]], vertices[triangles[i][1]], vertices[triangles[i][2]]);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}