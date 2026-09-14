#include "wheel.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

MoneyWheel::MoneyWheel(int width, int height)
    : screenWidth(width), screenHeight(height),
      centerX(width / 2), centerY(height / 2),
      wheelRadius(200), flapperRadius(15),
      currentRotation(0), targetRotation(0), rotationSpeed(0),
      spinning(false), winningValue(0) {
    srand(static_cast<unsigned>(time(nullptr)));
    initializeSegments();
}

MoneyWheel::~MoneyWheel() {
}

void MoneyWheel::initializeSegments() {
    segments.clear();

    // 20 segments with value 1
    SDL_Color color1 = {255, 50, 50, 255};   // Red
    for (int i = 0; i < 20; i++) {
        double angle = (360.0 / 38.0) * i;
        Segment seg;
        seg.value = 1;
        seg.color = color1;
        seg.startAngle = angle;
        seg.endAngle = angle + (360.0 / 38.0);
        segments.push_back(seg);
    }

    // 10 segments with value 2
    SDL_Color color2 = {50, 150, 255, 255};  // Blue
    for (int i = 0; i < 10; i++) {
        double angle = (360.0 / 38.0) * (20 + i);
        Segment seg;
        seg.value = 2;
        seg.color = color2;
        seg.startAngle = angle;
        seg.endAngle = angle + (360.0 / 38.0);
        segments.push_back(seg);
    }

    // 5 segments with value 5
    SDL_Color color5 = {50, 255, 100, 255};  // Green
    for (int i = 0; i < 5; i++) {
        double angle = (360.0 / 38.0) * (30 + i);
        Segment seg;
        seg.value = 5;
        seg.color = color5;
        seg.startAngle = angle;
        seg.endAngle = angle + (360.0 / 38.0);
        segments.push_back(seg);
    }

    // 3 segments with value "BONUS"
    SDL_Color colorBonus = {255, 215, 0, 255};  // Gold
    for (int i = 0; i < 3; i++) {
        double angle = (360.0 / 38.0) * (35 + i);
        Segment seg;
        seg.value = -1;  // -1 represents BONUS
        seg.color = colorBonus;
        seg.startAngle = angle;
        seg.endAngle = angle + (360.0 / 38.0);
        segments.push_back(seg);
    }
}

void MoneyWheel::spin() {
    if (!spinning) {
        spinning = true;
        rotationSpeed = 20.0f + (rand() % 10);  // Random speed between 20-30
        targetRotation = currentRotation + (3600 + rand() % 3600);  // 10+ full rotations
        winningValue = 0;
    }
}

void MoneyWheel::update(float deltaTime) {
    if (spinning) {
        // Apply friction to slow down
        rotationSpeed *= FRICTION;

        // Update rotation
        currentRotation += rotationSpeed * deltaTime;

        // Check if spinning should stop
        if (rotationSpeed < MIN_SPEED && currentRotation >= targetRotation) {
            spinning = false;
            currentRotation = fmod(currentRotation, 360.0);
            calculateWinningSegment();
            std::cout << "Wheel stopped! Winning segment value: ";
            if (winningValue == -1) {
                std::cout << "BONUS!" << std::endl;
            } else {
                std::cout << winningValue << std::endl;
            }
        }

        // Wrap rotation
        if (currentRotation >= 360.0) {
            currentRotation = fmod(currentRotation, 360.0);
        }
    }
}

void MoneyWheel::calculateWinningSegment() {
    // The flapper is at the top (angle 0), so we check which segment is at the top
    double topAngle = fmod(currentRotation, 360.0);
    
    for (const auto& seg : segments) {
        if (seg.startAngle <= topAngle && topAngle < seg.endAngle) {
            winningValue = seg.value;
            return;
        }
    }
    
    // Fallback
    winningValue = segments[0].value;
}

int MoneyWheel::getWinningSegment() const {
    return winningValue;
}

bool MoneyWheel::isSpinning() const {
    return spinning;
}

void MoneyWheel::drawSegment(SDL_Renderer* renderer, const Segment& segment) {
    double startRad = (segment.startAngle - currentRotation) * M_PI / 180.0;
    double endRad = (segment.endAngle - currentRotation) * M_PI / 180.0;

    // Draw filled pie slice
    SDL_SetRenderDrawColor(renderer, segment.color.r, segment.color.g, segment.color.b, segment.color.a);

    // Draw pie slice using triangles
    int numPoints = 50;
    for (int i = 0; i < numPoints; i++) {
        double t1 = static_cast<double>(i) / numPoints;
        double t2 = static_cast<double>(i + 1) / numPoints;

        double angle1 = startRad + (endRad - startRad) * t1;
        double angle2 = startRad + (endRad - startRad) * t2;

        int x1 = centerX + static_cast<int>(wheelRadius * cos(angle1));
        int y1 = centerY + static_cast<int>(wheelRadius * sin(angle1));
        int x2 = centerX + static_cast<int>(wheelRadius * cos(angle2));
        int y2 = centerY + static_cast<int>(wheelRadius * sin(angle2));

        // Draw triangle from center
        SDL_RenderDrawLine(renderer, centerX, centerY, x1, y1);
        SDL_RenderDrawLine(renderer, centerX, centerY, x2, y2);
    }

    // Draw arc outline
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int numArcPoints = 100;
    for (int i = 0; i < numArcPoints; i++) {
        double t1 = static_cast<double>(i) / numArcPoints;
        double t2 = static_cast<double>(i + 1) / numArcPoints;

        double angle1 = startRad + (endRad - startRad) * t1;
        double angle2 = startRad + (endRad - startRad) * t2;

        int x1 = centerX + static_cast<int>(wheelRadius * cos(angle1));
        int y1 = centerY + static_cast<int>(wheelRadius * sin(angle1));
        int x2 = centerX + static_cast<int>(wheelRadius * cos(angle2));
        int y2 = centerY + static_cast<int>(wheelRadius * sin(angle2));

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

void MoneyWheel::drawFlapper(SDL_Renderer* renderer) {
    // Draw flapper at top center (angle 0)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Flapper triangle pointing down
    int x = centerX;
    int y = centerY - wheelRadius - 20;

    SDL_Point points[3] = {
        {x, y - 15},
        {x - 10, y + 10},
        {x + 10, y + 10}
    };

    SDL_RenderDrawLines(renderer, points, 3);
    SDL_RenderDrawLine(renderer, points[0].x, points[0].y, points[2].x, points[2].y);

    // Fill flapper
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    for (int px = x - 10; px <= x + 10; px++) {
        for (int py = y - 15; py <= y + 10; py++) {
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
}

void MoneyWheel::drawCenter(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    int centerDotRadius = 15;

    for (int dx = -centerDotRadius; dx <= centerDotRadius; dx++) {
        for (int dy = -centerDotRadius; dy <= centerDotRadius; dy++) {
            if (dx * dx + dy * dy <= centerDotRadius * centerDotRadius) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }

    // Border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int angle = 0; angle < 360; angle += 5) {
        double rad = angle * M_PI / 180.0;
        int x = centerX + static_cast<int>(centerDotRadius * cos(rad));
        int y = centerY + static_cast<int>(centerDotRadius * sin(rad));
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void MoneyWheel::render(SDL_Renderer* renderer) {
    // Draw all segments
    for (const auto& segment : segments) {
        drawSegment(renderer, segment);
    }

    // Draw center hub
    drawCenter(renderer);

    // Draw flapper
    drawFlapper(renderer);

    // Draw instructions
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
