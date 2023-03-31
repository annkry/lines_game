#include <stdlib.h>
#include <stdio.h>
#include <utility>
#include <iostream>
#include <cmath>

#include <Window.hpp>
#include <Drawable.hpp>

#define A 5
#define B 33
#define M 359

int N;
int bean;
bool finish;
float finishTime;
float scale;
float scaleAngle;

int angles[300 * 300];
GLfloat vert[300 * 300 * 2][2];

class MySquare : public AGLDrawable
{
public:
   MySquare() : AGLDrawable(0)
   {
      setShaders();
   }
   void setShaders()
   {
      compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         out vec2 coord;
         out float ntime;
         out float scaleF;
         out float aspectF;
         layout(location = 0) uniform float time;
         layout(location = 1) uniform float aspect;
         layout(location = 2) uniform float scale;
         layout(location = 3) uniform float scaleAngle;
         
         void main(void) {
         
            vec2 newV;
            const vec2 vertices[4] = vec2[4](vec2(-0.9, -0.9),
                                             vec2( 0.9, -0.9),
                                             vec2(-0.9,  0.9),
                                             vec2( 0.9,  0.9));
            if (aspect < 1.0)
               coord = vec2(vertices[gl_VertexID].x * aspect, vertices[gl_VertexID].y);
            else
               coord = vec2(vertices[gl_VertexID].x, vertices[gl_VertexID].y / aspect);
               
               
            coord.x = coord.x * scale;
            coord.y = coord.y * scale;
            
            aspectF = aspect;
            scaleF = scale;
            
            
            float cosinusScale = cos((scaleAngle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            float sinusScale   = sin((scaleAngle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            coord.x = coord.x * cosinusScale - coord.y * sinusScale;
            coord.y = coord.x * sinusScale + coord.y * cosinusScale;
            gl_Position = vec4(coord, 0.5, 1.0);
            ntime       = time;
         }

      )END",
                     R"END(

         #version 330 
         out vec4 color;
         in vec2 coord;
         in float ntime;
         in float scaleF;
         in float aspectF;

         void main(void) {
            vec2 middlePoint = vec2(0.5, 0.25);
            if (aspectF < 1.0)
               middlePoint = vec2(middlePoint.x * aspectF, middlePoint.y);
            else
               middlePoint = vec2(middlePoint.x, middlePoint.y / aspectF);
                
            middlePoint.x = middlePoint.x * scaleF;
            middlePoint.y = middlePoint.y * scaleF;
            
            float angle = atan(-coord.y + middlePoint.y, coord.x - middlePoint.x) * 0.1;
            float len = length(coord - vec2(middlePoint.x, middlePoint.y));
            vec3 col = vec3(2.0);
            float sat = 0.5;
            
            col.r += sin(len * 40.0 + angle * 40.0 + ntime);
            col.g += cos(len * 30.0 + angle * 70.0 - ntime);
            col.b += sin(len * 60.0 + angle * 50.0 + 3.0);
            col.r *= sat;
            col.g *= sat;
            col.b *= sat;
            color = vec4(col, 0.0);
         } 

      )END");
   }

   void setBuffers()
   {
      bindBuffers();
      GLfloat vertices[4][2] = {
          {-0.9, -0.9},
          {0.9, -0.9},
          {-0.9, 0.9},
          {0.9, 0.9}};

      glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), vert, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
          0,        // attribute 0, must match the layout in the shader.
          2,        // size
          GL_FLOAT, // type
          GL_FALSE, 
          0,        // 24,             // stride
          (void *)0 // array buffer offset
      );
   }

   void draw(float aspect)
   {
      bindProgram();
      setBuffers();
      double startTime = glfwGetTime();
      glUniform1f(0, (float)startTime);
      glUniform1f(1, aspect);
      glUniform1f(2, scale);
      glUniform1f(3, scaleAngle);
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
   }
};

class MyLines : public AGLDrawable
{
public:
   MyLines() : AGLDrawable(0)
   {
      setShaders();
   }
   void setShaders()
   {
      compileShaders(R"END(

         #version 330 
         #extension GL_ARB_explicit_uniform_location : require
         #extension GL_ARB_shading_language_420pack : require
         in vec4 pos;
         out vec4 vcol;
         layout(location = 0) uniform float angle;
         layout(location = 3) uniform vec2 center;
         layout(location = 4) uniform int Pawn;
         layout(location = 5) uniform vec2 left;
         layout(location = 6) uniform vec2 right;
         layout(location = 7) uniform float aspect;
         layout(location = 8) uniform float scale;
         layout(location = 9) uniform float scaleAngle;
         
         
         void main(void) {
         
            vec4 newPosition;
            int LeftOrRightSideOfLine; //0 - left side, 1 - right side
            float new_x;
            float new_y;

            newPosition.xyzw = vec4(pos.xy, 0.5, 1.0);
            float s1 = (right.x + left.x) / 2.0; 
            float s2 = right.y;
            float cosinus = cos((angle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            float sinus   = sin((angle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            if(newPosition.x > s1){ //right side of the line
               LeftOrRightSideOfLine = 1;
            }
            else
               LeftOrRightSideOfLine = 0;
               
            float a = newPosition.x - s1; 
            
            if (Pawn == 1){
            	s1 = center.x;
            	s2 = center.y;
            	vcol = vec4(1.0 - (pos.x - left.x) / (right.x - left.x), (pos.x - left.x) / (right.x - left.x), 0.0, 1.0);
            }
            else{
                vcol = vec4(0.0,0.0,0.0,1.0);
            }
            
            if(LeftOrRightSideOfLine == 1){//right side
                   newPosition.x = s1 + (abs(a) * cosinus);
                   if (angle <= 180.0)
                       newPosition.y = left.y + sqrt(a * a * (1 - cosinus * cosinus));
                   else
                       newPosition.y = left.y - sqrt(a * a * (1 - cosinus * cosinus));
                }
                else{
                   newPosition.x = s1 - (abs(a) * cosinus);
                   if (angle <= 180.0)
                       newPosition.y = left.y - sqrt(a * a * (1 - cosinus * cosinus));
                   else
                       newPosition.y = left.y + sqrt(a * a * (1 - cosinus * cosinus));
                }
                
            if (aspect < 1.0)
                newPosition = vec4(newPosition.x * aspect, newPosition.y, newPosition.zw);
            else
                newPosition = vec4(newPosition.x, newPosition.y / aspect, newPosition.zw);
                
                
            newPosition.x = newPosition.x * scale;
            newPosition.y = newPosition.y * scale;
            
            
            float cosinusScale = cos((scaleAngle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            float sinusScale   = sin((scaleAngle / 360.0) * 2.0 * 3.1415926535897932384626433832795);
            newPosition.x = newPosition.x * cosinusScale - newPosition.y * sinusScale;
            newPosition.y = newPosition.x * sinusScale + newPosition.y * cosinusScale;
            gl_Position = newPosition;
         }
      )END",
                     R"END(

         #version 330 
         out vec4 color;
         in vec4 vcol;

         void main(void) {
            color = vcol;
         } 
      )END");
   }
   void setBuffers(std::pair<float, float> center)
   {
      bindBuffers();
      srand(bean);
      float n = (float)N;
      for (unsigned int i = 0; i < N * N; i++)
      {
         angles[i] = rand() % 360;
      }
      for (int i = 0; i < N; i++)
      {
         for (int j = 0; j < N; j++)
         {
            if (i != N - 1 || j != 0)
            {
               vert[i * 2 * N + 2 * j + 0][0] = (-0.9 + j * 1.8 / (n - 1.0)) - 1.0 / n;
               vert[i * 2 * N + 2 * j + 0][1] = 0.9 - (i * 1.8) / (n - 1.0);
               vert[i * 2 * N + 2 * j + 1][0] = (-0.9 + (j * 1.8 / (n - 1.0))) + 1.0 / n;
               vert[i * 2 * N + 2 * j + 1][1] = 0.9 - (i * 1.8) / (n - 1.0);
            }
            else
            {
               vert[i * 2 * N + 2 * j + 0][0] = center.first - 1.0 / n;
               vert[i * 2 * N + 2 * j + 0][1] = center.second;
               vert[i * 2 * N + 2 * j + 1][0] = center.first + 1.0 / n;
               vert[i * 2 * N + 2 * j + 1][1] = center.second;
            }
         }
      }
      glBufferData(GL_ARRAY_BUFFER, N * N * 4 * sizeof(float), vert, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
          0,        // attribute 0, must match the layout in the shader.
          2,        // size
          GL_FLOAT, // type
          GL_FALSE, 
          0,        // 24,             // stride
          (void *)0 // array buffer offset
      );
   }

   bool checkRotation(std::pair<float, float> center, int angleOfPawn, int makeRotation)
   {
      if (makeRotation == 1)
         angleOfPawn += 1.0;
      if (makeRotation == 2)
         angleOfPawn -= 1.0;
      if (angleOfPawn < 0)
         angleOfPawn = 359;
      angleOfPawn = (int)angleOfPawn % 360;
      std::pair<float, float> newCoordLeftPawn = countPawn(angleOfPawn, center, false);
      std::pair<float, float> newCoordRightPawn = countPawn(angleOfPawn, center, true);

      float new_x1 = newCoordLeftPawn.first;
      float new_y1 = newCoordLeftPawn.second;
      float new_x2 = newCoordRightPawn.first;
      float new_y2 = newCoordRightPawn.second;
      int minYCoordLine = std::min(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
      int maxYCoordLine = std::max(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
      int minXCoordLine = std::min(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
      int maxXCoordLine = std::max(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
      for (int i = minXCoordLine; i <= maxXCoordLine; i++)
      {
         for (int j = minYCoordLine; j <= maxYCoordLine; j++)
         {
            if ((i < N - 1 || j > 0) && IsIntersecting(std::make_pair(new_x1, new_y1), std::make_pair(new_x2, new_y2), countPoint(i, j, 0), countPoint(i, j, 1)))
            {
               if (i == 0 && j == N - 1)
               {
                  finish = true;
                  scale = 1.05;
               }
               return false;
            }
         }
      }

      return true;
   }

   std::pair<float, float> countPawn(int angle, std::pair<float, float> center, bool leftOrRight)
   {
      std::pair<float, float> coord;
      GLfloat a = 1.0 / N;
      GLfloat cosinus = std::cos((angle / 360.0) * 2.0 * M_PI);
      GLfloat sinus = std::sin((angle / 360.0) * 2.0 * M_PI);
      std::pair<float, float> coordPawn = std::make_pair(1.0 / N, 0.0);
      float new_x = coordPawn.first * cosinus - coordPawn.second * sinus;
      float new_y = coordPawn.first * sinus + coordPawn.second * cosinus;

      if (leftOrRight)
      { // right coord
         coord = std::make_pair(new_x, new_y);
      }
      else
      {
         coord = std::make_pair(-new_x, -new_y);
      }
      coord.first = (float)(coord.first + center.first);
      coord.second = (float)(coord.second + center.second);
      return coord;
   }

   bool checkMove(std::pair<float, float> center, int angleOfPawn, int makeMove)
   {

      std::pair<float, float> coordLeftPawn = countPawn(angleOfPawn, center, false);
      std::pair<float, float> coordRightPawn = countPawn(angleOfPawn, center, true);
      if (makeMove == 1)
      { // move up
         float new_x1 = coordLeftPawn.first + 0.001 * cos(angleOfPawn / 360.0 * 2.0 * M_PI);
         float new_y1 = coordLeftPawn.second + 0.001 * sin(angleOfPawn / 360.0 * 2.0 * M_PI);
         float new_x2 = coordRightPawn.first + 0.001 * cos(angleOfPawn / 360.0 * 2.0 * M_PI);
         float new_y2 = coordRightPawn.second + 0.001 * sin(angleOfPawn / 360.0 * 2.0 * M_PI);
         int minYCoordLine = std::min(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
         int maxYCoordLine = std::max(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
         int minXCoordLine = std::min(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
         int maxXCoordLine = std::max(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
         for (int i = minXCoordLine; i <= maxXCoordLine; i++)
         {
            for (int j = minYCoordLine; j <= maxYCoordLine; j++)
            {
               if ((i < N - 1 || j > 0) && IsIntersecting(std::make_pair(new_x1, new_y1), std::make_pair(new_x2, new_y2), countPoint(i, j, 0), countPoint(i, j, 1)))
               {
                  if (i == 0 && j == N - 1)
                  {
                     finish = true;
                     scale = 1.05;
                  }
                  return false;
               }
            }
         }
      }
      if (makeMove == 2)
      { // move down
         float new_x1 = coordLeftPawn.first - 0.001 * cos(angleOfPawn / 360.0 * 2 * M_PI);
         float new_y1 = coordLeftPawn.second - 0.001 * sin(angleOfPawn / 360.0 * 2 * M_PI);
         float new_x2 = coordRightPawn.first - 0.001 * cos(angleOfPawn / 360.0 * 2 * M_PI);
         float new_y2 = coordRightPawn.second - 0.001 * sin(angleOfPawn / 360.0 * 2 * M_PI);
         int minYCoordLine = std::min(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
         int maxYCoordLine = std::max(floor((1 + new_x1) * N / 2), floor((1 + new_x2) * N / 2));
         int minXCoordLine = std::min(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
         int maxXCoordLine = std::max(floor((1 - new_y1) * N / 2), floor((1 - new_y2) * N / 2));
         for (int i = minXCoordLine; i <= maxXCoordLine; i++)
         {
            for (int j = minYCoordLine; j <= maxYCoordLine; j++)
            {
               if ((i < N - 1 || j > 0) && IsIntersecting(std::make_pair(new_x1, new_y1), std::make_pair(new_x2, new_y2), countPoint(i, j, 0), countPoint(i, j, 1)))
               {
                  if (i == 0 && j == N - 1)
                  {
                     finish = true;
                     scale = 1.05;
                  }
                  return false;
               }
            }
         }
      }
      return true;
   }

   std::pair<float, float> countPoint(int xCoordFromTable, int yCoordFromTable, int leftOrRightCoord)
   {

      int angle = angles[xCoordFromTable * N + yCoordFromTable];
      float s1 = (vert[xCoordFromTable * 2 * N + 2 * yCoordFromTable + 1][0] + vert[xCoordFromTable * 2 * N + 2 * yCoordFromTable + 0][0]) / 2;
      float s2 = vert[xCoordFromTable * 2 * N + 2 * yCoordFromTable + 1][1];

      GLfloat a = vert[xCoordFromTable * 2 * N + 2 * yCoordFromTable + 1][0] - s1;
      GLfloat cosinus = std::cos((angle / 360.0) * 2.0 * M_PI);
      GLfloat sinus = std::sin((angle / 360.0) * 2.0 * M_PI);
      std::pair<float, float> coord;

      float new_x = a;
      float new_y = 0.0;

      if (leftOrRightCoord)
      { // right side
         new_x = s1 + (abs(a) * cosinus);
         if (angle <= 180.0)
            new_y = s2 + sqrt(a * a * (1 - cosinus * cosinus));
         else
            new_y = s2 - sqrt(a * a * (1 - cosinus * cosinus));
      }
      else
      {
         new_x = s1 - (abs(a) * cosinus);
         if (angle <= 180.0)
            new_y = s2 - sqrt(a * a * (1 - cosinus * cosinus));
         else
            new_y = s2 + sqrt(a * a * (1 - cosinus * cosinus));
      }
      coord = std::make_pair(new_x, new_y);
      return coord;
   }

   void draw(float aspect, std::pair<float, float> left, std::pair<float, float> right, std::pair<float, float> center, float angleOfPawn, int makeMove, int makeRotation)
   {

      setBuffers(center);
      bindProgram();
      bindBuffers();
      glUniform2f(3, center.first, center.second);
      glUniform1f(7, aspect);
      glUniform1f(8, scale);
      glUniform1f(9, scaleAngle);
      angles[N * (N - 1) + 0] = angleOfPawn;

      for (int i = 0; i < N; i++)
      {
         for (int j = 0; j < N; j++)
         {
            if (i == N - 1 && j == 0)
            {
               glUniform1i(4, 1);
            }
            else
            {
               glUniform1i(4, 0);
            }
            glUniform2f(5, vert[i * 2 * N + 2 * j + 0][0], vert[i * 2 * N + 2 * j + 0][1]);
            glUniform2f(6, vert[i * 2 * N + 2 * j + 1][0], vert[i * 2 * N + 2 * j + 1][1]);
            glUniform1f(0, angles[N * i + j]);
            glDrawArrays(GL_LINES, i * 2 * N + 2 * j, 2);
         }
      }
   }

   bool IsIntersecting(std::pair<float, float> a, std::pair<float, float> b, std::pair<float, float> c, std::pair<float, float> d)
   {

      float denominator = ((b.first - a.first) * (d.second - c.second)) - ((b.second - a.second) * (d.first - c.first));
      float numerator1 = ((a.second - c.second) * (d.first - c.first)) - ((a.first - c.first) * (d.second - c.second));
      float numerator2 = ((a.second - c.second) * (b.first - a.first)) - ((a.first - c.first) * (b.second - a.second));

      if (denominator == 0)
         return numerator1 == 0 && numerator2 == 0;

      float r = numerator1 / denominator;
      float s = numerator2 / denominator;

      return (r >= 0 && r <= 1) && (s >= 0 && s <= 1);
   }
};

class MyWin : public AGLWindow
{
public:
   MyWin(){};
   MyWin(int _wd, int _ht, const char *name, int vers, int fullscr = 0)
       : AGLWindow(_wd, _ht, name, vers, fullscr){};
   virtual void KeyCB(int key, int scancode, int action, int mods);
   void MainLoop();
};

void MyWin::KeyCB(int key, int scancode, int action, int mods)
{
   AGLWindow::KeyCB(key, scancode, action, mods);
   if ((key == GLFW_KEY_SPACE) && action == GLFW_PRESS)
   {
      ;
   }
   if (key == GLFW_KEY_HOME && (action == GLFW_PRESS))
   {
      ;
   }
}

void MyWin::MainLoop()
{
   ViewportOne(0, 0, wd, ht);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // white background

   MyLines lines;
   MySquare square;

   std::pair<float, float> coordLeftPawn = std::make_pair(-0.9 - (1.0 / N), -0.9);
   std::pair<float, float> coordRightPawn = std::make_pair(-0.9 + (1.0 / N), -0.9);
   std::pair<float, float> center = std::make_pair(-0.9, -0.9);
   float angleOfPawn = 0.0;
   bool writeTime = true;
   bool maximalize = true;
   double startTime = glfwGetTime();
   double currTime = startTime;
   double measuredTime = startTime;
   int makeMove = 0;
   int makeRotation = 0;

   do
   {
      glClear(GL_COLOR_BUFFER_BIT);

      AGLErrors("main-loopbegin");

      // Drawing
      square.draw(GetAspect());
      lines.draw(GetAspect(), coordLeftPawn, coordRightPawn, center, angleOfPawn, makeMove, makeRotation);
      makeMove = 0;
      makeRotation = 0;
      AGLErrors("main-afterdraw");

      glfwSwapBuffers(win());
      glfwPollEvents();

      if (finish)
      { 
         // End of the game
         finishTime = glfwGetTime();
         if (writeTime)
         {
            measuredTime = glfwGetTime();
            std::cout << "Your time for the game: " << finishTime - startTime << " s"
                      << "\n";
            writeTime = false;
            scaleAngle += 12.0;
         }
         if (scale < 0.0)
            break;
         if (scale > 1.0 && scale < 1.1 && finishTime - measuredTime > 0.05 && maximalize)
         {
            scale += 0.05;
            measuredTime = glfwGetTime();
            maximalize = false;
         }
         else if (scale < 1.1 && finishTime - measuredTime > 0.05)
         {
            scaleAngle += 12.0;
            scale -= 0.05;
            measuredTime = glfwGetTime();
         }
      }

      if (glfwGetKey(win(), GLFW_KEY_DOWN) == GLFW_PRESS)
      {
         currTime = glfwGetTime();
         makeMove = 2;
         if (lines.checkMove(center, angleOfPawn, makeMove) && (currTime - measuredTime > 0.004))
         {
            center.first -= 0.001 * cos(angleOfPawn / 360.0 * 2.0 * M_PI);
            center.second -= 0.001 * sin(angleOfPawn / 360.0 * 2.0 * M_PI);
            measuredTime = currTime;
         }
         else
            makeMove = 0;
      }
      else if (glfwGetKey(win(), GLFW_KEY_UP) == GLFW_PRESS)
      {
         currTime = glfwGetTime();
         makeMove = 1;
         if (lines.checkMove(center, angleOfPawn, makeMove) && (currTime - measuredTime > 0.004))
         {
            center.first += 0.001 * cos(angleOfPawn / 360.0 * 2.0 * M_PI);
            center.second += 0.001 * sin(angleOfPawn / 360.0 * 2.0 * M_PI);
            measuredTime = currTime;
         }
         else
            makeMove = 0;
      }
      else if (glfwGetKey(win(), GLFW_KEY_RIGHT) == GLFW_PRESS)
      {
         makeRotation = 2;
         if (lines.checkRotation(center, angleOfPawn, makeRotation))
         {
            angleOfPawn -= 1.0;
            if (angleOfPawn < 0)
               angleOfPawn = 359;
            angleOfPawn = (int)angleOfPawn % 360;
         }
         else
            makeRotation = 0;
      }
      else if (glfwGetKey(win(), GLFW_KEY_LEFT) == GLFW_PRESS)
      {
         makeRotation = 1;
         if (lines.checkRotation(center, angleOfPawn, makeRotation))
         {
            angleOfPawn += 1.0;
            angleOfPawn = (int)angleOfPawn % 360;
         }
         else
            makeRotation = 0;
      }
   } while (glfwGetKey(win(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
            glfwWindowShouldClose(win()) == 0);
}

int main(int argc, char *argv[])
{
   bean = 0;
   N = 10;
   scale = 1.0;
   scaleAngle = 0.0;
   finish = false;
   std::size_t pos1;
   std::string arg1;
   if (argc == 3)
   { 
      // two parameters
      std::string arg2 = argv[2];
      std::size_t pos2;
      N = std::stoi(arg2, &pos2);
      if (N < 2){
         N = 2;
      } 
      arg1 = argv[1];
      bean = std::stoi(arg1, &pos1);
   }
   if (argc == 2)
   { 
      // one parameter
      arg1 = argv[1];
      bean = std::stoi(arg1, &pos1);
   }
   MyWin win;
   win.Init(600, 600, "Pawn as a line game", 0, 33);
   win.MainLoop();
   return 0;
}
