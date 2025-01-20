#ifndef MATH_H
#define MATH_H

#define M_PI 3.141592653589793238462643383279502884L
//Para as funções matematicas usadas na logica do bot
//Pode ser que isso tenha que virar uma familia de classes.

#include "includes.h"


// Determina se um ponto (px, py) está dentro de um polígono definido por uma lista de vértices.
bool isPointInPolygon(float px, float py, const std::vector<std::pair<float, float>>& polygon);

// Rotaciona um ponto (x, y) em torno de um ponto de origem (origX, origY) por um ângulo theta em radianos.
std::pair<float, float> rotate_point(float x, float y, float origX, float origY, float theta);

//Calcula o ponto (x,y) que fica no meio de um poligono
std::pair<float, float> calculatePolygonCentroid(const std::vector<std::pair<float, float>>& polygon);

////Caso o ponto central do poligono fique fora dos limites do cenario
std::pair<float, float> adjustPointToStageBounds(
    const std::pair<float, float>& point,
    float minX, float maxY, float maxX, float minY);

//Calcula o ponto para o personagem fugir  
std::pair<float, float> calculateEscapePoint(
    const std::pair<float, float>& midpoint,
    float bossVectorX,
    float bossVectorY,
    float distance,
    float minX, float maxY, float maxX, float minY);

//Ponto bugado  
bool isPointInHELL(const std::pair<float, float>);


//Calcula se o ponto esta dentro do cenario/arena/stage
bool isPointInBounds(const std::pair<float, float>& point, float minX, float maxY, float maxX, float minY);

//TODO: rever isso mas pode ser que esta correto.
//Calcula o angulo do boss.
float calculateAngleUsingVector(float vectorX, float vectorY);

//Rotaciona o retangulo 
std::vector<std::pair<float, float>> calculateRotatedRectangle(
    const std::pair<float, float>& center,
    float width, float height, float angle);


const std::vector<std::pair<float, float>> tranformTwoCordsInRectangle(float minX, float maxY,
    float maxX, float minY);

double distanceTo(double x1, double y1, double x2, double y2);

bool distanceBetween(float value, float limInf, float limSup);

std::pair<float, float> calculateDelta(float targetX, float targetY, float playerX, float playerY);

float calculateDistance(float deltaX, float deltaY);

std::vector < std::pair<float, float>> pointToSquare(std::pair<float, float> center, float width, float height);

#endif // !MATHBOT