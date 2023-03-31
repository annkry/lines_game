#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>

#include <array>

void AGLErrors(const char *comment);

class AGLWindow {
public:
   AGLWindow(){};
   AGLWindow(int _wd, int _ht, const char *name, int fullscr=0, int vers=33)
   {  Init(_wd,_ht,name,vers,fullscr);  };
   void Init(int _wd, int _ht, const char *name, int fullscr=0, int vers=33);
   ~AGLWindow();
   GLFWwindow* win() {  return window;  };

   int   wd, ht;              
   float aspect;                

   float Viewport(   int _vtx,int _vty,int _vwd,int _vht);
   float ViewportOne(int _vtx,int _vty,int _vwd,int _vht);
   void GetCursorNDC(float *x, float *y);         
   float GetAspect( void );

   virtual void Resize( int _wd, int _ht ); 
   virtual void KeyCB(int key, int scancode, int action, int mods);
   virtual void MouseButtonCB(int button,    int action, int mods);
   virtual void ScrollCB(double xp, double yp);
   virtual void MousePosCB(double xp, double yp);

   void MainLoop ( void );
   
   void WaitForFixedFPS(float frame_time=1./60.); 

private:
   bool IsFullScreen(  void );
   void SetFullScreen( bool fullscreen );

   int vtx, vty, vwd, vht;

   GLFWwindow  *        window         = nullptr;
   GLFWmonitor *        monitor        = nullptr;
   std::array< int, 2 > winPos         = {0, 0};
   std::array< int, 2 > winSize        = {0, 0};

   std::array< int, 2 > vwpSize        = {0, 0};
   bool                 updateViewport = true;
   std::array<GLFWmonitor *,2> monitors= {nullptr, nullptr};

   double prev_time=0.0;
   
   static void CallbackResize(GLFWwindow* window, int cx, int cy);
   static void CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
   static void CallbackMouseButton(GLFWwindow* window, int button, int action, int mods);
   static void CallbackScroll(GLFWwindow* window, double xp, double yp);
   static void CallbackMousePos(GLFWwindow* window, double xp, double yp);
};
