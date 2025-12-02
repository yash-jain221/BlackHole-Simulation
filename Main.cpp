

#include"Model.h"
#include "HDRLoad.h"
const unsigned int width = 1600;
const unsigned int height = 1600;

float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
};

struct NebulaParticle {
	glm::vec3 pos;
	float size;
	glm::vec3 color;
};

std::vector<NebulaParticle> nebulaParticles;

float rand01() { return (float)rand() / RAND_MAX; }

void generateNebulaParticles(int count)
{
	nebulaParticles.reserve(count);

	for (int i = 0; i < count; i++)
	{
		NebulaParticle p;

		// Spherical distribution around origin
		float theta = rand01() * 2.0f * 3.14159f;  // 0 to 2?
		float phi = acos(2.0f * rand01() - 1.0f);  // Uniform sphere
		float r = 50.0f + rand01() * 30.0f;        // Radius 20-50 units

		p.pos = glm::vec3(
			r * sin(phi) * cos(theta),
			r * sin(phi) * sin(theta),
			r * cos(phi)
		);

		p.size = 1.5f + rand01() * 2.0f;  // Bigger particles

		// Mix red/orange colors
		float heat = rand01();
		p.color = glm::vec3(
			0.8f + heat * 0.2f,   // red
			0.2f + heat * 0.3f,   // orange tint
			0.05f                 // minimal blue
		);

		nebulaParticles.push_back(p);
	}
}



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


	generateNebulaParticles(100000);


	Shader spaceShader("space.vert", "space.frag");
	Shader blackHoleShader("bh.vert", "bh.frag");
	Shader marsShader("planet.vert", "planet.frag");
	Shader spaceShipShader("ss.vert", "ss.frag");
	Shader brightShader("fb.vert", "bright_tex.frag");
	Shader blurShader("fb.vert", "blur.frag");
	Shader combineShader("fb.vert", "combine.frag");
	Shader nebulaShader("nebula.vert", "nebula.frag");
	Shader starShader("planet.vert", "planet.frag");

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, -1000.0f));

	glEnable(GL_DEPTH_TEST);

	HDRLoad spaceBox(spaceShader, "space_env.hdr");
	Model blackHole = Model("blackhole/blackhole.obj");
	Model Mars = Model("mars/mars.obj");
	Model spaceShip = Model("spaceship/spaceship.obj");
	Model sun = Model("sun/sun.obj");

	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create texture for the framebuffer
	unsigned int texColorBuffer;
	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	// BLACK HOLE MASK ATTACHMENT (1 channel)
	unsigned int bhMaskTex;
	glGenTextures(1, &bhMaskTex);
	glBindTexture(GL_TEXTURE_2D, bhMaskTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bhMaskTex, 0);

	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Depth buffer
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer is incomplete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	// --------------------------------------------------
// 1. Bright pass texture (extract bright areas)
// --------------------------------------------------
	unsigned int brightTex;
	glGenTextures(1, &brightTex);
	glBindTexture(GL_TEXTURE_2D, brightTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Pingpong FBO not complete.\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &attachment); // only color0
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLuint nebulaVBO;
	glGenBuffers(1, &nebulaVBO);
	glBindBuffer(GL_ARRAY_BUFFER, nebulaVBO);
	glBufferData(GL_ARRAY_BUFFER, nebulaParticles.size() * sizeof(NebulaParticle), nebulaParticles.data(), GL_STATIC_DRAW);

	float quadNebula[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,

		-0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f
	};


	GLuint nebulaQuadVAO, nebulaQuadVBO;
	glGenVertexArrays(1, &nebulaQuadVAO);
	glGenBuffers(1, &nebulaQuadVBO);

	glBindVertexArray(nebulaQuadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, nebulaQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadNebula), quadNebula, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

	// INSTANCE DATA
	glBindBuffer(GL_ARRAY_BUFFER, nebulaVBO);

	// pos (vec3)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(NebulaParticle), (void*)0);
	glVertexAttribDivisor(1, 1);

	// size (float)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(NebulaParticle), (void*)offsetof(NebulaParticle, size));
	glVertexAttribDivisor(2, 1);

	// color (vec3)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(NebulaParticle), (void*)offsetof(NebulaParticle, color));
	glVertexAttribDivisor(3, 1);

	glBindVertexArray(0);


	while (!glfwWindowShouldClose(window))
	{
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);
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
		glm::mat4 blackHoleRotationZ = glm::rotate(glm::mat4(1.0f), 0.1f * time * glm::radians(50.0f), glm::vec3(0, 0, 1));
		glm::mat4 blackHoleScaling = glm::scale(glm::mat4(1.0f), glm::vec3(55.0f, 55.0f, 55.0f));
		glm::mat4 blackHoleModel = blackHoleRotationZ * blackHoleRotationX * blackHoleTranslation * blackHoleScaling;
		glm::mat4 parent = blackHoleTranslation;
		glUniformMatrix4fv(glGetUniformLocation(blackHoleShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(blackHoleModel));
		blackHole.Draw(blackHoleShader, camera);

		//star
		
		starShader.Activate();
		glm::mat4 starOrbit = glm::rotate(glm::mat4(1.0f), 0.1f * time * glm::radians(15.0f), glm::vec3(0, 1, 0));
		glm::mat4 starTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -800.0f));
		glm::mat4 starScale = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f));
		glm::mat4 starModel = parent * starOrbit * starTranslation * starScale;
		glm::mat4 starTransformNoScale = starOrbit * starTranslation;

		glUniform1i(glGetUniformLocation(starShader.ID, "isStar"), 1);
		glUniformMatrix4fv(glGetUniformLocation(starShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(starModel));
		sun.Draw(starShader, camera);

		// planet
		marsShader.Activate();
		glm::mat4 planetOrbit = glm::rotate(glm::mat4(1.0f), 0.1f * time * glm::radians(25.0f), glm::vec3(0, 1, 0));
		glm::mat4 planetTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -150.0f));
		glm::mat4 planetScale = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f));
		glm::mat4 planetModel = starTransformNoScale * planetOrbit * planetTranslation * planetScale;
		glm::mat4 planetTransformNoScale = planetOrbit * planetTranslation;

		glUniform1i(glGetUniformLocation(marsShader.ID, "isStar"), 0);
		glUniformMatrix4fv(glGetUniformLocation(marsShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(planetModel));
		Mars.Draw(marsShader, camera);

		// spaceship
		spaceShipShader.Activate();
		glm::mat4 spaceShipTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -50.0f)); // translate to origin
		glm::mat4 spaceShipRotation = glm::rotate(glm::mat4(1.0f), 0.5f * time * glm::radians(50.0f), glm::vec3(0, 1, 0)); // rotate around mars
		glm::mat4 spaceShipScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));	// scale down
		glm::mat4 spaceShipModel = spaceShipRotation * spaceShipTranslation * spaceShipScale;
		glm::mat4 finalSpaceShipModel = parent * starTransformNoScale * planetTransformNoScale * spaceShipModel;

		glUniformMatrix4fv(glGetUniformLocation(spaceShipShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(finalSpaceShipModel));
		spaceShip.Draw(spaceShipShader, camera);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);   // glowing fog looks better than normal alpha


		//nebula
		nebulaShader.Activate();
		camera.createMatrix(nebulaShader, "camMatrix");
		glm::mat4 nebulaTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 0.0f, -600.0f));
		glm::mat4 nebulaRotation = glm::rotate(glm::mat4(1.0f), 0.5f * time * glm::radians(20.0f), glm::vec3(0, 1, 0)); // rotate around blackhole
		glm::mat4 nebulaScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f));
		glm::mat4 nebulaModel = parent * nebulaRotation * nebulaTranslation * nebulaScale;

		glUniformMatrix4fv(glGetUniformLocation(nebulaShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(nebulaModel));
		glUniform1f(glGetUniformLocation(nebulaShader.ID, "time"), time*0.8f);

		glBindVertexArray(nebulaQuadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nebulaParticles.size());

		glDisable(GL_BLEND);

		//glDisable(GL_BLEND);

		// === 1. BRIGHT PASS (extract bright parts of scene) ===
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		brightShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glUniform1i(glGetUniformLocation(brightShader.ID, "sceneTex"), 0);
		glUniform1f(glGetUniformLocation(brightShader.ID, "threshold"), 0.1f);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// === 2. GAUSSIAN BLUR (ping-pong) ===
		bool horizontal = true;
		unsigned int amount = 15;

		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);

			blurShader.Activate();
			glUniform1i(glGetUniformLocation(blurShader.ID, "horizontal"), horizontal);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, i == 0 ? pingpongColorbuffers[0] : pingpongColorbuffers[!horizontal]);


			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
		}

		// blurred bloom texture:
		unsigned int bloomTex = pingpongColorbuffers[!horizontal];

		glDisable(GL_DEPTH_TEST);

		// === 3. FINAL COMPOSITE ===
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);

		combineShader.Activate();
		camera.createMatrix(combineShader, "camMatrix");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer);
		glUniform1i(glGetUniformLocation(combineShader.ID, "sceneTex"), 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bloomTex);
		glUniform1i(glGetUniformLocation(combineShader.ID, "bloomTex"), 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, bhMaskTex);
		glUniform1i(glGetUniformLocation(combineShader.ID, "bhMaskTex"), 2);

		glUniform1f(glGetUniformLocation(combineShader.ID, "exposure"), 0.8f);
		glUniform1f(glGetUniformLocation(combineShader.ID, "bloomIntensity"), 2.5f);
		glUniform3fv(glGetUniformLocation(combineShader.ID, "bhWorld"), 1, glm::value_ptr(glm::vec3(blackHoleModel[3])));

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
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