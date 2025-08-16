#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//SHADERS

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 Color;\n"
"uniform vec2 moving;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x + moving.x, aPos.y + moving.y, aPos.z, 1.0);\n"
"	Color = aColor;\n"
"}";


const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Color;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(Color, 1.0f);\n"
"}";



//Object DATA -- This is were object attributes are assigned


class Object {

public:

	float x, y;
	float width, height;
	float r, g, b;
	float xThing;
	float yThing;

	Object(float initialX, float initialY, float w, float h, float red, float green, float blue)
		: x(initialX), y(initialY), width(w), height(h), r(red), g(green), b(blue) {
	}

	void updatePosition(float newX, float newY) {
		x = newX;
		y = newY;
	}

	void setColor(float newR, float newG, float newB) {
		r = newR;
		g = newG;
		b = newB;
	}
};

// This is were the "player" get's it's characteristics for movement, color, and dimensions(height, width) that come from the object class.

class Player : public Object {
public:
	float playerWidth;
	float playerHeight;
	float movementX;
	float movementY;

	Player(float initialX, float initialY, float w, float h, float red, float green, float blue)
		: Object(initialX, initialY, w, h, red, green, blue), playerWidth(w), playerHeight(h) {
	}



	void movement(GLFWwindow* window) {
		float speed = 0.01f;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && movementX + x >= -0.99) {
			movementX -= speed;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && movementX + x <= 0.99 - playerWidth) {
			movementX += speed;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && movementY + y >= -0.99 + playerHeight) {
			movementY -= speed;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && movementY + y <= 0.99) {
			movementY += speed;
		}




		xThing = movementX + x;
		yThing = movementY + y;

	}
};

//This is were Objects are "built" meaning their vertexs are placed and it outputs a VAO to be displayed to the screen.

class builder {
public:

	unsigned int buildSquareVAO(float x, float y, float width, float height, float red, float green, float blue) {
		float x1 = x;
		float y1 = y - height;
		float x2 = x + width;
		float y2 = y;

		float vertices[] = {
			x1, y1, 0.0f, red, green, blue, //Top-left
			x2, y1, 0.0f, red, green, blue, //top-right
			x1, y2, 0.0f, red, green, blue, //bottom-left
			x2, y2, 0.0f, red, green, blue //bottom-right
		};

		unsigned indices[] = {
		0, 1, 2,
		3, 2, 1
		};

		unsigned int VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		return VAO;

	}
};

//Check collision. I've gotten this to work. But I haven't made it connect and communicate to the player class to stop the player from passing through an object.

bool checkCollision(const Object& objA, const Object& objB) {
	bool collisionX = objA.xThing + objA.width > objB.x && objB.x + objB.width > objA.xThing;
	bool collisionY = objA.yThing > objB.y - objB.height && objB.y > objA.yThing - objA.height;



	return collisionX && collisionY;
}



int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//Window creation check and glad check

	GLFWwindow* window = glfwCreateWindow(500, 500, "GAME 1", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	//Vertex shader, fragment shader, and shader program.

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::VERTEX::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}



	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::FRAGMENT::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}



	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); // Fix: use GL_LINK_STATUS
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	glBindVertexArray(0);




	//PROCESSING



	builder objectBuilder; //object of builder class

	float playerWidth = 0.1f;
	float playerHeight = 0.1f;

	Player player(-0.5f, 0.0f, playerWidth, playerHeight, 0.0f, 1.0f, 0.0f); //object of player class
	unsigned int playerVAO = objectBuilder.buildSquareVAO(-0.5f, 0.0f, playerWidth, playerHeight, 0.0f, 1.0f, 0.0f); //Player Vertex Array object

	std::vector<Object> gameObjects; //Vector holding objects of Object class.
	gameObjects.push_back(Object(0.5f, 0.5f, 0.2f, 0.2f, 1.0f, 0.0f, 0.0f)); // Red block
	gameObjects.push_back(Object(-0.2f, -0.4f, 0.3f, 0.1f, 0.0f, 0.0f, 1.0f)); // Blue block
	gameObjects.push_back(Object(0.0f, 0.9f, 0.15f, 0.15f, 1.0f, 1.0f, 0.0f)); // Grey/brown block


	// Build VAOs for objects once

	std::vector<unsigned int> objVAOs;


	//Creates Vertex Array Objects of using objectBuilder class callingj on builder class.
	for (Object& obj : gameObjects) {
		unsigned int objVAO = objectBuilder.buildSquareVAO(obj.x, obj.y, obj.width, obj.height, obj.r, obj.g, obj.b);
		objVAOs.push_back(objVAO);
	}


	//Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		player.movement(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		player.setColor(0.0f, 0.0f, 0.0f);



		// Collision detection and color update
		//for (Object& obj : gameObjects) {
		if (checkCollision(player, gameObjects[0])) {
			player.setColor(1.0f, 0.0f, 0.0f);
			//obj.setColor(0.0f, 1.0f, 1.0f);
			std::cout << "On" << "\n";
		}
		//}


		glUseProgram(shaderProgram);

		// Draws player
		glUniform2f(glGetUniformLocation(shaderProgram, "moving"), player.movementX, player.movementY);
		glBindVertexArray(playerVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glUniform2f(glGetUniformLocation(shaderProgram, "moving"), 0, 0);


		// Draws objects
		glBindVertexArray(objVAOs[0]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(objVAOs[1]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(objVAOs[2]);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);




		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}


//Checks for esc key
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

//Framebuffer, size of openGl screen
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
