#include <raylib.h>

int main() {
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1200;
    const int screenHeight = 1200;

    InitWindow(screenWidth, screenHeight, "UNO Rectangle");

    // Define the rectangle's properties
    Rectangle rect = { screenWidth / 4, screenHeight / 3, screenWidth / 2, screenHeight / 3 }; // Center the rect, and make it half width and height
    Color rectColor = RED;
    const char* text = "UNO";
    int fontSize = 80; // Increased font size for better visibility

    // Calculate text position to center it within the rectangle
    int textX = rect.x + (rect.width / 2) - (MeasureText(text, fontSize) / 2);
    int textY = rect.y + (rect.height / 2) - (fontSize / 2);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // No updates needed for this simple example
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE); // Set background to white

        // Draw the red rectangle
        DrawRectangleRec(rect, rectColor);

        // Draw the text in the center of the rectangle
        DrawText(text, textX, textY, fontSize, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
