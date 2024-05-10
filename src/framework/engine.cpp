#include "engine.h"
#include <cmath>

const color WHITE(1, 1, 1);
const color BLACK(0, 0, 0);
const color BLUE(0, 0, 1);
const color YELLOW(1, 1, 0);
const color RED(1, 0, 0);

Engine::Engine() {
    this->initWindow();
    this->initShaders();
    this->initShapes();
}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    window = glfwCreateWindow(WIDTH, HEIGHT, "engine", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    shaderManager = make_unique<ShaderManager>();
    shapeShader = this->shaderManager->loadShader("../res/shaders/circle.vert",
                                                  "../res/shaders/circle.frag",
                                                  nullptr, "circle");
    shapeShader.use();
    shapeShader.setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    int numberOfBoids = 75;
    int numberOfLeaders = numberOfBoids / 10;

    float radius = 5;
    float leaderRadius = 8;
    float maxSpeed = 100;
    float leaderMaxSpeed = maxSpeed * 0.60;

    // init red normals
    for (int i = 0; i < numberOfBoids; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        vec2 position(x, y);
        vec2 velocity(rand() % int(maxSpeed), rand() % int(maxSpeed));
        vec4 red(RED.red, RED.green, RED.blue, 1);
        unique_ptr<Circle> boid = make_unique<Circle>(shapeShader, position, radius, velocity, red);

        boid->setVelocity(velocity);
        boids.push_back(std::move(boid));
    }
    // init red leaders
    for (int i = 0; i < numberOfLeaders; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        vec2 position(x, y);
        vec2 velocity(rand() % int(leaderMaxSpeed), rand() % int(leaderMaxSpeed));
        vec4 red(RED.red, RED.green, RED.blue, 1);
        unique_ptr<Circle> boid = make_unique<Circle>(shapeShader, position, leaderRadius, velocity, red);

        boid->setVelocity(velocity);
        boids.push_back(std::move(boid));
    }

    // init blue normals
    for (int i = 0; i < numberOfBoids; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        vec2 position(x, y);
        vec2 velocity(rand() % int(maxSpeed), rand() % int(maxSpeed));
        // get 3 random floats between 0 and 1 for RGB
        vec4 blue(BLUE.red, BLUE.green, BLUE.blue, 1);
        unique_ptr<Circle> boid = make_unique<Circle>(shapeShader, position, radius, velocity, blue);

        boid->setVelocity(velocity);
        boids.push_back(std::move(boid));
    }
    // init blue leaders
    for (int i = 0; i < numberOfLeaders; ++i) {
        float x = rand() % WIDTH;
        float y = rand() % HEIGHT;
        vec2 position(x, y);
        vec2 velocity(rand() % int(leaderMaxSpeed), rand() % int(leaderMaxSpeed));
        // get 3 random floats between 0 and 1 for RGB
        vec4 blue(BLUE.red, BLUE.green, BLUE.blue, 1);
        unique_ptr<Circle> boid = make_unique<Circle>(shapeShader, position, leaderRadius, velocity, blue);

        boid->setVelocity(velocity);
        boids.push_back(std::move(boid));
    }
}

void Engine::processInput() {
    glfwPollEvents();

    // Close window if escape key is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &mouseX, &mouseY);
    mouseY = HEIGHT - mouseY; // make sure mouse y-axis isn't flipped
}

void Engine::checkBounds(unique_ptr<Circle> &boid1) const {
    vec2 position = boid1->getPos();
    vec2 velocity = boid1->getVelocity();
    const int rotation = 5;
    vec2 newVelocity;
    newVelocity.x = boid1->getVelocity().x;
    newVelocity.y = boid1->getVelocity().y;

    position += velocity * deltaTime;

    if (boid1->getPosX() < 125) {
        newVelocity.x += rotation;
        boid1->setVelocity(newVelocity);
        if (boid1->getPosX() - boid1->getRadius() < 0) {
            boid1->setPosX(boid1->getRadius());
        }
    }
    if (boid1->getPosX() > WIDTH - 125) {
        newVelocity.x -= rotation;
        boid1->setVelocity(newVelocity);
        if (boid1->getPosX() - boid1->getRadius() > WIDTH) {
            boid1->setPosX(boid1->getRadius());
        }
    }
    if (boid1->getPosY() < 75) {
        newVelocity.y += rotation;
        boid1->setVelocity(newVelocity);
        if (boid1->getPosY() - boid1->getRadius() < 0) {
            boid1->setPosY(boid1->getRadius());
        }
    }
    if (boid1->getPosY() > HEIGHT - 75) {
        newVelocity.y -= rotation;
        boid1->setVelocity(newVelocity);
        if (boid1->getPosY() - boid1->getRadius() > HEIGHT) {
            boid1->setPosY(boid1->getRadius());
        }
    }

    boid1->setPos(position);
    boid1->setVelocity(newVelocity);
}

void Engine::update() {

    // Calculate delta time
    float currentFrame = glfwGetTime();
    double distance;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;



    for (unique_ptr<Circle> &boid1: boids) {

        vec2 position = boid1->getPos();
        vec2 velocity = boid1->getVelocity();
        position += velocity * deltaTime;
        boid1->setPos(position);


        for (unique_ptr<Circle> &boid2: boids) {
            // centroid boid vector
            center(boid1, boid2);
            // boid spacing
            avoid(boid1, boid2);
        }

        matchVelocity(boid1);

        // Check for collisions
        for (unique_ptr<Circle> &other: boids) {
            if (boid1 != other && boid1->isOverlapping(*other)) {
                boid1->bounce(*other);

                // change the color of regular boids hit by leader boids of opposing color
                if (boid1->getRadius() == 8 && other->getRadius() != 8 && boid1->getColor4() != other->getColor4()) {
                    other->setColor(boid1->getColor4());
                }
            }
        }

        // Prevent boids from moving off screen
        checkBounds(boid1);


        // ensure no boid goes above the speed cap and flies off the screen
        speedLimit(boid1);
    }
}

float distance(unique_ptr<Circle>& boid1, unique_ptr<Circle>& boid2) {
    return sqrt(
            (boid2->getPosX() - boid1->getPosX()) * (boid2->getPosX() - boid1->getPosX()) +
            (boid2->getPosY() - boid1->getPosY()) * (boid2->getPosY() - boid1->getPosY())
            );
}

void Engine::center(unique_ptr<Circle> &boid1, unique_ptr<Circle> &boid2) {
    const float centerCoefficient = 0.00001;
    float centerX;
    float centerY;
    int numBoidsNear = 0;
    vec2 newVelocity;
    int dist = 200;
    int minDist = 20;
    // swarm leader
    if (boid1->getRadius() == 8) {
        if (boid1 != boid2 && boid1->getColor4() != boid2->getColor4()) {
            if (distance(boid1, boid2) < dist && distance(boid1, boid2) > minDist
                && boid1->getColor4() == boid2->getColor4()) {
                centerX += boid2->getPosX();
                centerY += boid2->getPosY();
                ++numBoidsNear;
            }
        }
        if (numBoidsNear) {
            centerX /= (float) numBoidsNear;
            centerY /= (float) numBoidsNear;

            newVelocity.x = boid1->getVelocity().x + ((centerX - boid1->getVelocity().x) * centerCoefficient);
            newVelocity.y = boid1->getVelocity().y + ((centerY - boid1->getVelocity().y) * centerCoefficient);

            boid1->setVelocity(newVelocity);
        }
    } else {

        if (boid1 != boid2 && boid1->getColor4() == boid2->getColor4()) {
            if (distance(boid1, boid2) < dist && distance(boid1, boid2) > minDist
                && boid1->getColor4() == boid2->getColor4()) {
                centerX += boid2->getPosX();
                centerY += boid2->getPosY();
                ++numBoidsNear;
            }
            if (numBoidsNear) {
                centerX /= (float) numBoidsNear;
                centerY /= (float) numBoidsNear;

                newVelocity.x = boid1->getVelocity().x + ((centerX - boid1->getVelocity().x) * centerCoefficient);
                newVelocity.y = boid1->getVelocity().y + ((centerY - boid1->getVelocity().y) * centerCoefficient);

                boid1->setVelocity(newVelocity);
            }
        }
        numBoidsNear = 0;
    }
}

void Engine::avoid(unique_ptr<Circle> &boid1, unique_ptr<Circle> &boid2) {
    const int minDist = 20;
    const float avoidCoeff = 0.05;
    int moveX = 0;
    int moveY = 0;
    vec2 newVelocity;
    // if swarm leader
    if (boid1->getRadius() == 8) {
        if (boid1 != boid2) {
            if (distance(boid1, boid2) < minDist * 2 && boid1->getColor4() != boid2->getColor4()) {
                // chase after boids of other colors
                moveX += boid1->getPosX() - boid2->getPosX();
                moveY += boid1->getPosY() - boid2->getPosY();
                newVelocity.x = boid1->getVelocity().x + moveX;
                newVelocity.y = boid1->getVelocity().y + moveY;
                boid1->setVelocity(newVelocity);
            } else if (distance(boid1, boid2) < minDist && boid1->getColor4() == boid2->getColor4()) {
                moveX += boid1->getPosX() - boid2->getPosX();
                moveY += boid1->getPosY() - boid2->getPosY();
                newVelocity.x = boid1->getVelocity().x + moveX * avoidCoeff;
                newVelocity.y = boid1->getVelocity().y + moveY * avoidCoeff;
                boid1->setVelocity(newVelocity);
            }
        }
    } else {
        if (boid1 != boid2) {
            // case where boids are the same color
            if (distance(boid1, boid2) < minDist && boid1->getColor4() == boid2->getColor4()) {
                moveX += boid2->getPosX() - boid1->getPosX();
                moveY += boid2->getPosY() - boid1->getPosY();
                newVelocity.x = boid1->getVelocity().x + moveX * avoidCoeff;
                newVelocity.y = boid1->getVelocity().y + moveY * avoidCoeff;
                boid1->setVelocity(newVelocity);
            } else if (distance(boid1, boid2) < minDist * 4 && !(boid1->getColor4() == boid2->getColor4())) {
                moveX += boid1->getPosX() - boid2->getPosX();
                moveY += boid1->getPosY() - boid2->getPosY();
                newVelocity.x = boid1->getVelocity().x + moveX * 0.5 * avoidCoeff;
                newVelocity.y = boid1->getVelocity().y + moveY * 0.5 * avoidCoeff;
                boid1->setVelocity(newVelocity);
            }


        }
    }
}
void Engine::matchVelocity(unique_ptr<Circle> &boid1){
    const float matchCoeff = 0.05;
    float avgVelocityX;
    float avgVelocityY;
    const float dist = 55;
    vec2 newVelocity;
    int numBoidsNear;

    for (unique_ptr<Circle> &boid2: boids) {

        if (distance(boid1, boid2) < dist) {
            avgVelocityX += boid2->getVelocity().x;
            avgVelocityY += boid2->getVelocity().y;
            ++numBoidsNear;
        }
    }
    if (numBoidsNear) {
        avgVelocityX /= (float) numBoidsNear;
        avgVelocityY /= (float) numBoidsNear;

        newVelocity.x = boid1->getVelocity().x + (avgVelocityX - boid1->getVelocity().x) * matchCoeff;
        newVelocity.y = boid1->getVelocity().y + (avgVelocityY - boid1->getVelocity().y) * matchCoeff;
        boid1->setVelocity(newVelocity);
    }
}

void Engine::speedLimit(unique_ptr<Circle> &boid1){
    const float speedLimit = 80;
    float speed;
    vec2 newVelocity;
    speed = sqrt(boid1->getVelocity().x * boid1->getVelocity().x +
            boid1->getVelocity().y * boid1->getVelocity().y);
    if (boid1->getRadius() == 8) {
        if (speed > speedLimit * 1.1) {
            newVelocity.x = (boid1->getVelocity().x / speed) * speedLimit * 1.1;
            newVelocity.y = (boid1->getVelocity().y / speed) * speedLimit * 1.1;
            boid1->setVelocity(newVelocity);
        } else if (speed < speedLimit / 2) {
            if (boid1->getVelocity().x > 0) {
                newVelocity.x = (boid1->getVelocity().x + 5);
            } else {
                newVelocity.x = (boid1->getVelocity().x - 5);
            }
            if (boid1->getVelocity().y > 0) {
                newVelocity.y = (boid1->getVelocity().y + 5);
            } else {
                newVelocity.y = (boid1->getVelocity().y - 5);
            }
            boid1->setVelocity(newVelocity);
        }
    } else {
        if (speed > speedLimit) {
            newVelocity.x = (boid1->getVelocity().x / speed) * speedLimit;
            newVelocity.y = (boid1->getVelocity().y / speed) * speedLimit;
            boid1->setVelocity(newVelocity);
        } else if (speed < speedLimit / 2) {
            if (boid1->getVelocity().x > 0) {
                newVelocity.x = (boid1->getVelocity().x + 3);
            } else {
                newVelocity.x = (boid1->getVelocity().x - 3);
            }
            if (boid1->getVelocity().y > 0) {
                newVelocity.y = (boid1->getVelocity().y + 3);
            } else {
                newVelocity.y = (boid1->getVelocity().y - 3);
            }
            boid1->setVelocity(newVelocity);
        }
    }
}



void Engine::render() {
    glClearColor(BLACK.red, BLACK.green, BLACK.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    for (unique_ptr<Circle>& boid : boids) {
        boid->setUniforms();
        boid->draw();
    }

    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}