#include "triangle.h"

Triangle::Triangle(Shader & shader, vec2 pos, vec2 size, struct color color)
    : Shape(shader, pos, size, color) {
    // Check if a triangle has been initialized
    initVectors();
    initVAO();
    initVBO();
    initEBO();
}

Triangle::~Triangle() {
    glDeleteVertexArrays(1, &this->VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Triangle::draw() const {
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Triangle::initVectors() {
    this->vertices.insert(this->vertices.end(), {
            -0.5f, -0.5f,  // Bottom left
            0.5f, -0.5f,   // Bottom right
            0.0f, 0.5f     // Top
    });

    this->indices.insert(this->indices.end(), {
            0, 1, 2,
    });
}