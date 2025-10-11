/**
* Author: Justin Dutta
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/cs3113.h"
#include <math.h>
#include <vector>
using namespace std;

// Global Constants
constexpr int   SCREEN_WIDTH  = 1800,
                SCREEN_HEIGHT = 900,
                FPS           = 60,
                SIZE          = 200,
                FRAME_LIMIT   = 100;

const float     PADDLESPEED    = 400.0f;

constexpr char    BG_COLOUR[] = "#000000";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };

constexpr char BACKGROUND[] = "assets/background.png";
constexpr char SPELL_RIGHT[] = "assets/spell_book_right.png";
constexpr char SPELL_LEFT[] = "assets/spell_book_left.png";
constexpr char FIREBALL[] = "assets/fireball.png";
constexpr char ICEBALL[] = "assets/iceball.png";
constexpr char PLAYER_1_WINS[] = "assets/player_1_wins.png";
constexpr char PLAYER_2_WINS[] = "assets/player_2_wins.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gAngle         = 0.0f,
          gPulseTime     = 0.0f,
          gPreviousTicks = 0.0f;

int       gFrameCounter  = 0;
bool      gMultiplayer = true;
bool      gGameOver = false;
int       gWinner = 0;
int       gActiveBalls = 1;

Texture2D gTextureBackground;
Texture2D gTextureBookRight;
Texture2D gTextureBookLeft;
Texture2D gTextureFireball;
Texture2D gTextureIceball;
Texture2D gTexturePlayer1Wins;
Texture2D gTexturePlayer2Wins;

Vector2   gPosition         = ORIGIN;
Vector2   gPositionRight    = ORIGIN;
Vector2   gPositionLeft     = ORIGIN;

Vector2 gLeftMovement = {0,0};
Vector2 gRightMovement = {0,0};

float bookWidth = SCREEN_WIDTH / 12.0f;
float bookHeight = SCREEN_HEIGHT / 5.0f;
Vector2 paddleScale = { bookWidth, bookHeight };
float direction = 1.0f;

struct Ball {
    Vector2 position;
    Vector2 velocity;
    Vector2 scale;
    float size;
    bool fire = true;
    bool rendered = false;
};

vector<Ball> gBalls;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, const Vector2 *positionB, const Vector2 *scaleB);

// void spawnBalls(int count);

// void spawnBalls(int count) {
//     gBalls.clear();
//     for (int i = 0; i < count; i++) {
//         Ball ball;
//         ball.size = SCREEN_WIDTH / 20.0f;
//         ball.scale = { ball.size, ball.size };
//         ball.position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

//         float angle = (float)i * (PI / 6.0f) + 0.3f;
//         float speed = 600.0f;
//         ball.velocity = { speed * cos(angle), speed * sin(angle) };
//         gBalls.push_back(ball);
//     }
// }

// Function Definitions
bool isColliding(const Vector2 *postionA,  const Vector2 *scaleA, 
                 const Vector2 *positionB, const Vector2 *scaleB)
{
    float xDistance = fabs(postionA->x - positionB->x) - 
                      ((scaleA->x + scaleB->x) / 2.0f);
    float yDistance = fabs(postionA->y - positionB->y) - 
                      ((scaleA->y + scaleB->y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Battle of The Mages");

    gTextureBackground = LoadTexture(BACKGROUND);
    gTextureBookRight = LoadTexture(SPELL_RIGHT);
    gTextureBookLeft = LoadTexture(SPELL_LEFT);
    gTextureFireball = LoadTexture(FIREBALL);
    gTextureIceball = LoadTexture(ICEBALL);
    gTexturePlayer1Wins = LoadTexture(PLAYER_1_WINS);
    gTexturePlayer2Wins = LoadTexture(PLAYER_2_WINS);

    float centerX = SCREEN_WIDTH / 2.0f;
    float centerY = SCREEN_HEIGHT / 2.0f;

    gPositionLeft.x = centerX - (bookWidth * 6.0f);
    gPositionLeft.y = centerY - (bookHeight / 2.0f);

    gPositionRight.x = centerX + (bookWidth * 5.0f);
    gPositionRight.y = centerY - (bookHeight / 2.0f);

//    spawnBalls(1);

    Ball ball1;
    Ball ball2;
    Ball ball3;
    gBalls.push_back(ball1);
    gBalls.push_back(ball2);
    gBalls.push_back(ball3);

    for (Ball &ball : gBalls) {
        ball.size = SCREEN_WIDTH / 20.0f;
        ball.scale = { ball.size, ball.size };
        ball.position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    }

    float baseAngle = 0.3f;
    float speed = 600.0f;

    gBalls[0].velocity = { speed * cos(baseAngle), speed * sin(baseAngle) };
    gBalls[1].velocity = { -speed * cos(0.0f), -speed * sin(0.0f) };
    gBalls[2].velocity = { -speed * cos(baseAngle), -speed * sin(baseAngle) };

    gBalls[0].rendered = true;

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;

    gLeftMovement = {0, 0};
    gRightMovement = {0, 0};
    
    if (IsKeyPressed(KEY_R) && gGameOver == true) {
        gGameOver = false;
        gWinner = 0;

        for (Ball &ball : gBalls) {
            ball.rendered = false;
            ball.position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }

        float baseAngle = 0.3f;
        float speed = 600.0f;
        gBalls[0].rendered = true;
        gBalls[0].velocity = { speed * cos(baseAngle), speed * sin(baseAngle) };
        gBalls[1].velocity = { -speed * cos(0.0f), -speed * sin(0.0f) };
        gBalls[2].velocity = { -speed * cos(baseAngle), -speed * sin(baseAngle) };
    }
    
    if (IsKeyPressed(KEY_T)) {
        if (gMultiplayer == true) {
            gMultiplayer = false;
        } else {
            gMultiplayer = true;
        }
    }

    if      (IsKeyDown(KEY_W))      gLeftMovement.y =  -1;
    else if (IsKeyDown(KEY_S))      gLeftMovement.y =   1;

    if (gMultiplayer == true) {
        if      (IsKeyDown(KEY_UP))     gRightMovement.y = -1;
        else if (IsKeyDown(KEY_DOWN))   gRightMovement.y =  1;
    }

    if (IsKeyPressed(KEY_ONE) && !gGameOver) {
        if (gActiveBalls != 1) {
            for (Ball &ball : gBalls) {
                ball.rendered = false;
                gBalls[0].rendered = true;
            }
            gActiveBalls = 1;
        }
    }
    if (IsKeyPressed(KEY_TWO) && !gGameOver) {
        if (gActiveBalls != 2) {
            for (Ball &ball : gBalls) {
                ball.rendered = false;
                gBalls[0].rendered = true;
                gBalls[1].rendered = true;
            }
        }
        if (gActiveBalls == 1) {
            gBalls[1].position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
        }
        gActiveBalls = 2;
    }
    if (IsKeyPressed(KEY_THREE) && !gGameOver) {
        if (gActiveBalls != 3) {
            for (Ball &ball : gBalls) {
                ball.rendered = false;  
                gBalls[0].rendered = true;
                gBalls[1].rendered = true;
                gBalls[2].rendered = true;
                gBalls[2].position = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
            }
            gActiveBalls = 3;
        }
    }
}

void update() 
{

    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;
    gPulseTime += 1.0f * deltaTime;
    
    if (gGameOver) {
        for (Ball &b : gBalls) {
            b.velocity = {0.0f, 0.0f};
        }
        return;
    }

    gPositionLeft.y = gPositionLeft.y + PADDLESPEED * gLeftMovement.y * deltaTime;
    if (gMultiplayer == true) {
        gPositionRight.y = gPositionRight.y + PADDLESPEED * gRightMovement.y * deltaTime;
    } else {
        gPositionRight.y += direction * 200.0f * deltaTime; // move up or down

        // reverse direction when reaching top/bottom
        if (gPositionRight.y <= 0 || gPositionRight.y >= SCREEN_HEIGHT - bookHeight) {
            direction *= -1.0f;
        }
    }

    if (gPositionLeft.y < 0)
        gPositionLeft.y = 0;
    else if (gPositionLeft.y > SCREEN_HEIGHT - bookHeight)
        gPositionLeft.y = SCREEN_HEIGHT - bookHeight;

    if (gPositionRight.y < 0)
        gPositionRight.y = 0;
    else if (gPositionRight.y > SCREEN_HEIGHT - bookHeight)
        gPositionRight.y = SCREEN_HEIGHT - bookHeight;

    for (Ball &ball : gBalls) {
        if (gGameOver) break;

        if (ball.rendered == true) {
            ball.position.x += ball.velocity.x * deltaTime;
            ball.position.y += ball.velocity.y * deltaTime;

            if (ball.position.y <= 0 || ball.position.y >= SCREEN_HEIGHT - ball.size)
                ball.velocity.y *= -1;

            Vector2 leftPaddleCenter = {
                gPositionLeft.x + bookWidth / 2.0f,
                gPositionLeft.y + bookHeight / 2.0f
            };
            Vector2 rightPaddleCenter = {
                gPositionRight.x + bookWidth / 2.0f,
                gPositionRight.y + bookHeight / 2.0f
            };

            if (isColliding(&ball.position, &ball.scale, &leftPaddleCenter, &paddleScale)) {
                ball.velocity.x = fabs(ball.velocity.x);
                ball.fire = true;
            }

            if (isColliding(&ball.position, &ball.scale, &rightPaddleCenter, &paddleScale)) {
                ball.velocity.x = -fabs(ball.velocity.x);
                ball.fire = false;
            }
            
            if (ball.position.x >= SCREEN_WIDTH - ball.size) {
                gGameOver = true;
                gWinner = 1;
            }
            if (ball.position.x <= 0) {
                gGameOver = true;
                gWinner = 2;
            }
        }
    }

    gFrameCounter++;
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    // Background Texture
    Rectangle textureAreaBackground = {
        0.0f, 0.0f,
        static_cast<float>(gTextureBackground.width),
        static_cast<float>(gTextureBackground.height)
    };
    Rectangle destinationAreaBackground = {
        gPosition.x,
        gPosition.y,
        static_cast<float>(SCREEN_WIDTH),
        static_cast<float>(SCREEN_HEIGHT)
    };
    Vector2 objectOriginBackground = {
        static_cast<float>(SCREEN_WIDTH / 2.0f),
        static_cast<float>(SCREEN_HEIGHT / 2.0f)
    };
    DrawTexturePro(
        gTextureBackground, 
        textureAreaBackground, 
        destinationAreaBackground, 
        objectOriginBackground, 
        gAngle, 
        WHITE
    );

    // Fireball Texture
    for (const Ball &ball : gBalls) {
        if (ball.rendered == true) {
            if (ball.fire == false) {
                Rectangle textureAreaFireball = {
                    0.0f, 0.0f,
                    static_cast<float>(gTextureFireball.width),
                    static_cast<float>(gTextureFireball.height)
                };
                Rectangle destinationAreaFireball = {
                    ball.position.x,
                    ball.position.y,
                    ball.size,
                    ball.size
                };
                Vector2 objectOriginFireball = {
                    static_cast<float>(ball.size / 2.0f),
                    static_cast<float>(ball.size / 2.0f)
                };
                DrawTexturePro(
                    gTextureFireball, 
                    textureAreaFireball, 
                    destinationAreaFireball, 
                    objectOriginFireball, 
                    gAngle, 
                    WHITE
                );
            } else if (ball.fire == true) {
                Rectangle textureAreaFireball = {
                    0.0f, 0.0f,
                    static_cast<float>(gTextureFireball.width),
                    static_cast<float>(gTextureFireball.height)
                };
                Rectangle destinationAreaFireball = {
                    ball.position.x,
                    ball.position.y,
                    ball.size,
                    ball.size
                };
                Vector2 objectOriginFireball = {
                    static_cast<float>(ball.size / 2.0f),
                    static_cast<float>(ball.size / 2.0f)
                };
                DrawTexturePro(
                    gTextureIceball, 
                    textureAreaFireball, 
                    destinationAreaFireball, 
                    objectOriginFireball, 
                    gAngle, 
                    WHITE
                );
            }
        }
    }

    // Right Spell Book Texture
    Rectangle textureAreaBookRight = {
        0.0f, 0.0f,
        static_cast<float>(gTextureBookRight.width),
        static_cast<float>(gTextureBookRight.height)
    };
    Rectangle destinationAreaBookRight = {
        gPositionRight.x,
        gPositionRight.y,
        static_cast<float>(SCREEN_WIDTH / 12.0f),
        static_cast<float>(SCREEN_HEIGHT / 5.0f)
    };
    Vector2 objectOriginBookRight = { 0.0f, 0.0f };
    DrawTexturePro(
        gTextureBookRight, 
        textureAreaBookRight, 
        destinationAreaBookRight, 
        objectOriginBookRight, 
        gAngle, 
        WHITE
    );

    // Left Spell Book Texture
    Rectangle textureAreaBookLeft = {
        0.0f, 0.0f,
        static_cast<float>(gTextureBookLeft.width),
        static_cast<float>(gTextureBookLeft.height)
    };
    Rectangle destinationAreaBookLeft = {
        gPositionLeft.x,
        gPositionLeft.y,
        static_cast<float>(SCREEN_WIDTH / 12.0f),
        static_cast<float>(SCREEN_HEIGHT / 5.0f)
    };
    Vector2 objectOriginBookLeft = { 0.0f, 0.0f };
    DrawTexturePro(
        gTextureBookLeft, 
        textureAreaBookLeft, 
        destinationAreaBookLeft, 
        objectOriginBookLeft, 
        gAngle, 
        WHITE
    );

    if (gWinner == 1) {
            Rectangle textureAreaPlayer1Wins = {
            0.0f, 0.0f,
            static_cast<float>(gTexturePlayer1Wins.width),
            static_cast<float>(gTexturePlayer1Wins.height)
        };
        Rectangle destinationAreaPlayer1Wins = {
            SCREEN_WIDTH  / 2.0f,
            SCREEN_HEIGHT / 2.0f,
            static_cast<float>(SCREEN_WIDTH / 1.5f),
            static_cast<float>(SCREEN_HEIGHT / 3.0f)
        };
        Vector2 objectOriginPlayer1Wins = {
            destinationAreaPlayer1Wins.width  / 2.0f,
            destinationAreaPlayer1Wins.height / 2.0f
        };
        DrawTexturePro(
            gTexturePlayer1Wins, 
            textureAreaPlayer1Wins, 
            destinationAreaPlayer1Wins, 
            objectOriginPlayer1Wins, 
            gAngle, 
            WHITE
        );
    } else if (gWinner == 2) {
        Rectangle textureAreaPlayer2Wins = {
            0.0f, 0.0f,
            static_cast<float>(gTexturePlayer2Wins.width),
            static_cast<float>(gTexturePlayer2Wins.height)
        };
        Rectangle destinationAreaPlayer2Wins = {
            SCREEN_WIDTH  / 2.0f,
            SCREEN_HEIGHT / 2.0f,
            static_cast<float>(SCREEN_WIDTH / 1.5f),
            static_cast<float>(SCREEN_HEIGHT / 3.0f)
        };
        Vector2 objectOriginPlayer2Wins = {
            destinationAreaPlayer2Wins.width  / 2.0f,
            destinationAreaPlayer2Wins.height / 2.0f
        };
        DrawTexturePro(
            gTexturePlayer2Wins, 
            textureAreaPlayer2Wins, 
            destinationAreaPlayer2Wins, 
            objectOriginPlayer2Wins, 
            gAngle, 
            WHITE
        );
    }

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}