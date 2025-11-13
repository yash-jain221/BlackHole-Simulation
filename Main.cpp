

#include"Model.h"
//#include"spacebox.h"
#include "HDRLoad.h"
const unsigned int width = 1600;
const unsigned int height = 1600;


int main()
{
	// Initialize GLFW
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "YoutubeOpenGL", NULL, NULL);
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);

	gladLoadGL();
	
	glViewport(0, 0, 1600, 1600);

	Shader spaceShader("space.vert", "space.frag");
	Shader blackHoleShader("bh.vert", "bh.frag");
	Shader spaceShipShader("ss.vert", "ss.frag");
	Shader marsShader("planet.vert", "planet.frag");

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	//glEnable(GL_CULL_FACE);
	//// Keeps front faces
	//glCullFace(GL_FRONT);
	//// Uses counter clock-wise standard
	//glFrontFace(GL_CCW);

	Model blackHole = Model("bh/BlackHole.obj");
	Model spaceShip = Model("spaceship/spaceship.obj");
	Model Mars = Model("mars/mars.obj");

	/*Shader hdrShader("hdr_sphere.vert", "hdr_sphere.frag");
	HDRLoad hdr(hdrShader, "space_env.hdr");*/
	//Model spaceBox = Model("space_env.hdr");
	// Main while loop

	/*vector<string> faces = {
		"spacebox/right.png",
		"spacebox/left.png",
		"spacebox/up.png",
		"spacebox/down.png",
		"spacebox/front.png",
		"spacebox/back.png"
	};

	SpaceBox space(faces);*/

	while (!glfwWindowShouldClose(window))
	{
		
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		
		camera.updateMatrix(45.0f, 0.1f, 10000.0f);

		//spacebox
		/*spaceShader.Activate();
		spaceBox.Draw(spaceShader, camera);*/
		/*spaceShader.Activate();
		camera.createMatrix(spaceShader, "camMatrix");
		hdr.Draw();*/

		float time = glfwGetTime();

		//blackhole
		blackHoleShader.Activate();
		glm::mat4 modelObject = glm::mat4(1.0f);
		glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.0f);
		modelObject = glm::translate(modelObject, modelPos);
		//modelObject = glm::rotate(modelObject, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		modelObject = glm::scale(modelObject, glm::vec3(1.0f, 1.0f, 1.0f));
		modelPos = glm::vec3(0.0f, 0.0f, -5000.0f);
		modelObject = glm::translate(modelObject, modelPos);

		glm::mat4 parent = modelObject;

		glUniformMatrix4fv(glGetUniformLocation(blackHoleShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelObject));
		blackHole.Draw(blackHoleShader, camera);

		// planet
		marsShader.Activate();
		glm::mat4 marsModel = glm::mat4(1.0f);
		glm::vec3 marsPos = glm::vec3(0.0f, 0.0f, 0.0f);
		marsModel = glm::translate(marsModel, marsPos);

		marsModel = glm::rotate(marsModel, 0.5f * time * glm::radians(50.0f), glm::vec3(0, 1, 0));
		marsPos = glm::vec3(0.0f, 0.0f, 5000.0f);
		glm::mat4 marsModelNoScale = glm::translate(marsModel, marsPos);
		marsModel = glm::scale(marsModelNoScale, glm::vec3(50.0f, 50.0f, 50.0f));

		glm::mat4 finalmarsModel = parent * marsModel;

		glUniformMatrix4fv(glGetUniformLocation(marsShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(finalmarsModel));
		Mars.Draw(marsShader, camera);


		// spaceship
		spaceShipShader.Activate();
		glm::mat4 spaceShipModel = glm::mat4(1.0f);
		glm::vec3 spaceShipPos = glm::vec3(0.0f, 0.0f, 0.0f);
		spaceShipModel = glm::translate(spaceShipModel, spaceShipPos);

		

		// orbit the ship around Y-axis
		spaceShipModel = glm::rotate(spaceShipModel, time * glm::radians(50.0f), glm::vec3(0, 1, 0));
		spaceShipModel = glm::scale(spaceShipModel, glm::vec3(0.001f, 0.001f, 0.001f));
		spaceShipPos = glm::vec3(80000.0f, 0.0f, 500.0f);
		spaceShipModel = glm::translate(spaceShipModel, spaceShipPos);

		glm::mat4 finalSpaceShipModel = parent * marsModelNoScale * spaceShipModel;

		glm::vec3 spaceshipPos = glm::vec3(finalSpaceShipModel[3]);
		glm::vec3 spaceshipForward = glm::normalize(-glm::vec3(finalSpaceShipModel[2]));

		// Update camera to follow the ship
		camera.Position = spaceshipPos - spaceshipForward * 10.0f + glm::vec3(0.0f, 2.0f, 0.0f);
		camera.Orientation = spaceshipForward;
		camera.updateMatrix(45.0f, 0.1f, 10000.0f);

		glUniformMatrix4fv(glGetUniformLocation(spaceShipShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(finalSpaceShipModel));
		spaceShip.Draw(spaceShipShader, camera);
		
		glDepthFunc(GL_LESS);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	blackHoleShader.Delete();
	spaceShipShader.Delete();
	marsShader.Delete();

	spaceShader.Delete();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}