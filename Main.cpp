

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

	GLFWwindow* window = glfwCreateWindow(width, height, "BlackHole Simulation", NULL, NULL);
	
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
	Shader marsShader("planet.vert", "planet.frag");
	Shader spaceShipShader("ss.vert", "ss.frag");

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	glEnable(GL_DEPTH_TEST);

	HDRLoad spaceBox(spaceShader, "space_env.hdr");
	Model blackHole = Model("blackhole/blackhole.obj");
	Model Mars = Model("mars/mars.obj");
	Model spaceShip = Model("spaceship/spaceship.obj");


	while (!glfwWindowShouldClose(window))
	{
		
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.1f, 100000.0f);

		//spacebox
		glDepthMask(GL_FALSE);   // Disable depth writing
		glDepthFunc(GL_LEQUAL);

		spaceShader.Activate();
		glm::mat4 spaceTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 spaceScaling = glm::scale(glm::mat4(1.0f), glm::vec3(100000.0f));
		glm::mat4 spaceModel = spaceTranslation* spaceScaling;
		glm::vec3 spacePos = glm::vec3(spaceModel[3]);

		glUniformMatrix4fv(glGetUniformLocation(spaceShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(spaceModel));
		camera.createMatrix(spaceShader, "camMatrix");
		spaceBox.Draw();

		glDepthMask(GL_TRUE);    // Re-enable depth writing
		glDepthFunc(GL_LESS);

		float time = glfwGetTime();

		//blackhole
		blackHoleShader.Activate();
		glm::mat4 blackHoleTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::mat4 blackHoleRotationX = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0));
		glm::mat4 blackHoleRotationZ = glm::rotate(glm::mat4(1.0f), 0.1f * time * glm::radians(10.0f), glm::vec3(0, 0, 1));
		glm::mat4 blackHoleScaling = glm::scale(glm::mat4(1.0f), glm::vec3(75.0f, 75.0f, 75.0f));

		glm::mat4 blackHoleModel = blackHoleRotationZ * blackHoleRotationX * blackHoleTranslation * blackHoleScaling;
		glm::mat4 parent = blackHoleTranslation;

		glUniformMatrix4fv(glGetUniformLocation(blackHoleShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(blackHoleModel));
		blackHole.Draw(blackHoleShader, camera);


		// planet
		marsShader.Activate();
		glm::mat4 planetOrbit = glm::rotate(glm::mat4(1.0f), 0.1f * time * glm::radians(25.0f), glm::vec3(0, 1, 0));
		glm::mat4 planetTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0f));
		glm::mat4 planetScale = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));

		glm::mat4 planetModel = parent * planetOrbit * planetTranslation * planetScale;
		glm::mat4 planetTransformNoScale = planetOrbit * planetTranslation;

		glUniformMatrix4fv(glGetUniformLocation(marsShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(planetModel));
		Mars.Draw(marsShader, camera);

		// spaceship
		spaceShipShader.Activate();
	
		glm::mat4 spaceShipTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f)); // translate to origin
		glm::mat4 spaceShipRotation = glm::rotate(glm::mat4(1.0f), 0.5f * time * glm::radians(50.0f), glm::vec3(0, 1, 0)); // rotate around mars
		glm::mat4 spaceShipScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));	// scale down

		glm::mat4 spaceShipModel = spaceShipRotation * spaceShipTranslation * spaceShipScale;
		glm::mat4 finalSpaceShipModel = parent * planetTransformNoScale * spaceShipModel;

		glUniformMatrix4fv(glGetUniformLocation(spaceShipShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(finalSpaceShipModel));
		spaceShip.Draw(spaceShipShader, camera);


		
		//glm::vec3 planetPos = glm::vec3(0.0f);
		//float orbitRadius = 80000.0f;

		//// 1?? Compute orbit position (ship’s current world-space position)
		//glm::vec3 shipPos;
		//shipPos.x = orbitRadius * cos(glm::radians(time * 50.0f)); // orbit around Y
		//shipPos.y = 0.0f;
		//shipPos.z = orbitRadius * sin(glm::radians(time * 50.0f));

		//// 2?? Define planet “up” direction (axis of orbit)
		//glm::vec3 up = glm::vec3(0, 1, 0);

		//// 3?? Radial direction (from planet to ship)
		//glm::vec3 toPlanet = glm::normalize(planetPos - shipPos);

		//// 4?? Tangent direction (direction of motion, along orbit)
		//glm::vec3 forward = glm::normalize(glm::cross(up, toPlanet));

		//// 5?? Recompute orthogonal right axis
		//glm::vec3 right = glm::normalize(glm::cross(forward, up));

		//// 6?? Build rotation matrix (ship orientation)
		//glm::mat4 orientation = glm::mat4(
		//	glm::vec4(right, 0.0f),
		//	glm::vec4(up, 0.0f),
		//	glm::vec4(-forward, 0.0f), // -Z is “forward” in most models
		//	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
		//);

		//// 7?? Combine transforms: translate then apply orientation and scale
		//glm::mat4 translation = glm::translate(glm::mat4(1.0f), shipPos);
		//glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f));

		//glm::mat4 spaceShipModel = translation * orientation * scale;

		glm::vec3 spaceshipPos = glm::vec3(finalSpaceShipModel[3]);
		glm::vec3 spaceshipForward = glm::normalize(-glm::vec3(finalSpaceShipModel[2]));

		// Update camera to follow the ship
		/*camera.Position = spaceshipPos - spaceshipForward * 10.0f + glm::vec3(2.0f, -2.0f, 0.0f);
		camera.Orientation = spaceshipForward;
		camera.updateMatrix(45.0f, 0.1f, 100000.0f);*/
	

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	blackHoleShader.Delete();
	spaceShipShader.Delete();
	marsShader.Delete();
	spaceShader.Delete();
	spaceBox.Cleanup();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}