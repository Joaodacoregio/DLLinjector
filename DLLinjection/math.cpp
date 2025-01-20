#include "math.h"

// Determina se um ponto (px, py) est� dentro de um pol�gono definido por uma lista de v�rtices.
bool isPointInPolygon(float px, float py, const std::vector<std::pair<float, float>>& polygon) {
    int n = polygon.size(); // N�mero de v�rtices no pol�gono.
    bool inside = false;    // Flag indicando se o ponto est� dentro ou fora.

    // Percorre os v�rtices do pol�gono para verificar interse��es.
    for (int i = 0, j = n - 1; i < n; j = i++) {
        float xi = polygon[i].first, yi = polygon[i].second;  // Coordenadas do v�rtice atual.
        float xj = polygon[j].first, yj = polygon[j].second;  // Coordenadas do v�rtice anterior.

        // Testa se o ponto est� entre as arestas e cruza o pol�gono.
        bool intersect = ((yi > py) != (yj > py)) &&
            (px < (xj - xi) * (py - yi) / (yj - yi) + xi);

        // Alterna o estado `inside` se houver interse��o.
        if (intersect) {
            inside = !inside;
        }
    }

    // Retorna se o ponto est� dentro do pol�gono.
    return inside;
}


// Rotaciona um ponto (x, y) em torno de um ponto de origem (origX, origY) por um �ngulo theta em radianos.
std::pair<float, float> rotate_point(float x, float y, float origX, float origY, float theta) {
    float tempX = x - origX;
    float tempY = y - origY;

    float newX = origX + (tempX * std::cos(theta) - tempY * std::sin(theta));
    float newY = origY + (tempX * std::sin(theta) + tempY * std::cos(theta));

    return { newX, newY };
}

std::pair<float, float> calculatePolygonCentroid(const std::vector<std::pair<float, float>>& polygon) {
    if (polygon.empty()) {
        throw std::invalid_argument("O vetor de v�rtices est� vazio.");
    }

    float sumX = 0.0f;
    float sumY = 0.0f;

    // Soma todas as coordenadas dos v�rtices
    for (const auto& vertex : polygon) {
        sumX += vertex.first;
        sumY += vertex.second;
    }

    // Calcula a m�dia para obter o ponto m�dio
    float midX = sumX / polygon.size();
    float midY = sumY / polygon.size();

    return { midX, midY };
}

//Define um limite para um valor se ele passar do limite ele se torna o limite
template <typename T>
constexpr const T& clamp(const T& value, const T& min, const T& max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

//Caso o ponto central do poligono fique fora dos limites do cenario
std::pair<float, float> adjustPointToStageBounds(
    const std::pair<float, float>& point,
    float minX, float maxY, float maxX, float minY)
{
    return {
        clamp(point.first, minX, maxX),
        clamp(point.second, minY, maxY)
    };
}

std::pair<float, float> calculateEscapePoint(
    const std::pair<float, float>& midpoint,
    float bossVectorX,
    float bossVectorY,
    float distance,
    float minX, float maxY, float maxX, float minY)
{
    auto perpendicular = std::make_pair(bossVectorY, bossVectorX);
    auto targetX = midpoint.first + perpendicular.first * distance;
    auto targetY = midpoint.second + perpendicular.second * distance;
    return { targetX , targetY };
}

// quando o ponto esta em um lugar bugado
bool isPointInHELL(const std::pair<float, float> cords) {
    if (cords.first < 1.0f || cords.second < 1.0f) {
        return true;
    }
    return false;
}


bool isPointInBounds(const std::pair<float, float>& point, float minX, float maxY, float maxX, float minY) {
    return point.first >= minX && point.first <= maxX &&
        point.second >= minY && point.second <= maxY;
}



float calculateAngleUsingVector(float vectorX, float vectorY) {
    if (vectorX > 0 && vectorY > 0) return -M_PI / 4;    // Nordeste
    if (vectorX > 0 && vectorY < 0) return -3 * M_PI / 4; // Sudeste
    if (vectorX < 0 && vectorY > 0) return M_PI / 4;     // Noroeste
    if (vectorX < 0 && vectorY < 0) return 3 * M_PI / 4; // Sudoeste
    if (vectorX > 0) return -M_PI / 2;                  // Leste
    if (vectorX < 0) return M_PI / 2;                   // Oeste
    if (vectorY > 0) return 0;                          // Norte (geometria do game come�a no norte 0 graus)
    return M_PI;                                        // Sul
}

//Calcula o retangulo rotacionado aonde o boss vai usar a skill
std::vector<std::pair<float, float>> calculateRotatedRectangle(
    const std::pair<float, float>& center,
    float width, float height, float angle)
{
    float halfWidth = width / 2, halfHeight = height / 2;
    auto p1 = rotate_point(center.first - halfWidth, center.second, center.first, center.second, angle);
    auto p2 = rotate_point(center.first + halfWidth, center.second, center.first, center.second, angle);
    auto p3 = rotate_point(center.first + halfWidth, center.second + height, center.first, center.second, angle);
    auto p4 = rotate_point(center.first - halfWidth, center.second + height, center.first, center.second, angle);
    return { p1, p2, p3, p4 };
}


const std::vector<std::pair<float, float>> tranformTwoCordsInRectangle(float minX, float maxY, float maxX, float minY)
{
    // Define os v�rtices do pol�gono (em sentido hor�rio ou anti-hor�rio)
    std::vector<std::pair<float, float>> polygon = {
        {minX, maxY}, // Superior esquerdo
        {maxX, maxY}, // Superior direito
        {maxX, minY}, // Inferior direito
        {minX, minY}  // Inferior esquerdo
    };

    // Retorna o pol�gono representando a arena
    return polygon;
}

double distanceTo(double x1, double y1, double x2, double y2) {
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool distanceBetween(float value, float limInf, float limSup) {
    return value >= limInf && value <= limSup;
}


std::pair<float, float> calculateDelta(float targetX, float targetY, float playerX, float playerY) {
    float deltaX = targetX - playerX;
    float deltaY = targetY - playerY;
    return { deltaX, deltaY };
}

float calculateDistance(float deltaX, float deltaY) {
    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}


std::vector < std::pair<float, float>> pointToSquare(std::pair<float, float> point, float width, float height) {
    std::vector<std::pair<float, float>> vertices;

    // Metade das dimens�es do quadrado
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;

    // Calcular os v�rtices do quadrado
    vertices.push_back({ point.first - halfWidth, point.second - halfHeight }); // Top-left
    vertices.push_back({ point.first + halfWidth, point.second - halfHeight }); // Top-right
    vertices.push_back({ point.first + halfWidth, point.second + halfHeight }); // Bottom-right
    vertices.push_back({ point.first - halfWidth, point.second + halfHeight }); // Bottom-left

    return vertices;
}