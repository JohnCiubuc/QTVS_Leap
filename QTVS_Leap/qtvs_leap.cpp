#include "qtvs_leap.h"
#include <QMessageBox>
#include <Windows.h>

QString debugDisplayString;


int ScreenResX;
int ScreenResY;
bool bDesktopSwipeMode = false;
bool bLeftClickToggle = false;
bool bLeftClickDragToggle = false;
float dMouseLerpValue = 1.0;
bool bSwipeThrottleTimer = false;
int debug_extendedFingerCounter = 0;

float debug_fingerIndexY = 0;
bool bDebug_FingerIndexYToggle = false;
float debug_fingerMiddleY = 0;
bool bDebug_FingerMiddleYToggle = false;

float debug_palmIndexY = 0;

float fFistPositionY = 0;

bool bDebug_HandRollDrag = true;

float debugLastX;

RECT debugWindowDrag_Left;
HWND debugWindowHWND_Left;
RECT debugWindowDrag_Right;
HWND debugWindowHWND_Right;

int debug_iReferencePointY_Left = 0;
int debug_iReferencePointY_Right = 0;
int debug_iReferencePointX_Left = 0;
int debug_iReferencePointX_Right = 0;

int iDebugWindowRestore_LeftHand = 0;

int iDebug_PreviousWindowWidth_LeftHand;
int iDebug_PreviousWindowHeight_LeftHand;



int iDebugWindowRestore_RightHand = 0;

int iDebug_PreviousWindowWidth_RightHand;
int iDebug_PreviousWindowHeight_RightHand;

Frame frame;
HandList hands;
Hand hand;
FingerList fingers;
GestureList gestures;

RECT rcMonitor;


const int iCursorLeapWidth = 460;
const int iCursorLeapHeight = 300;

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

// HWND GetRealParent(HWND hWnd)
// {
//   HWND hParent;

//   hParent = GetAncestor(hWnd, GA_PARENT);
//   if (hParent == NULL || hParent == GetDesktopWindow())
//   {
//     hParent = GetParent(hWnd);
//     if (hParent == NULL || hParent == GetDesktopWindow())
//     {
//       hParent = hWnd;
//     }

//   }

//   return hParent;
// }

// Better version of above one. 
// Stolen and butchered from AltDrag's code
HWND GetRealParent(HWND hWnd)
{
      HWND root = GetAncestor(hWnd, GA_ROOT);

        while (hWnd != root) {
          HWND parent = GetParent(hWnd);
          LONG_PTR exstyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
          if ((exstyle & WS_EX_MDICHILD)) {
            break;
          }
          hWnd = parent;
        }
      return hWnd;
}

void QTVS_Leap::LeapListener::SetParent(QTVS_Leap *tParent)
{
  Parent = tParent;
}

void QTVS_Leap::LeapListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void QTVS_Leap::LeapListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  /*
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
  */
}

void QTVS_Leap::LeapListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void QTVS_Leap::LeapListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void QTVS_Leap::LeapListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information

  frame = controller.frame();
  // std::cout << "Frame id: " << frame.id()
  //           << ", timestamp: " << frame.timestamp()
  //           << ", hands: " << frame.hands().count()
  //           << ", extended fingers: " << frame.fingers().extended().count()
  //           << ", tools: " << frame.tools().count()
  //           << ", gestures: " << frame.gestures().count() << std::endl;


  hands = frame.hands();

  Parent->HandLogic();

  // Get tools
  // const ToolList tools = frame.tools();
  // for (ToolList::const_iterator tl = tools.begin(); tl != tools.end(); ++tl) {
  //   const Tool tool = *tl;
  //   std::cout << std::string(2, ' ') <<  "Tool, id: " << tool.id()
  //             << ", position: " << tool.tipPosition()
  //             << ", direction: " << tool.direction() << std::endl;
  // }

  // Get gestures
  gestures = frame.gestures();

  if (Parent->ui.checkBox_gesturesLeap->isChecked())
    Parent->LeapGestureLogic();


}

void QTVS_Leap::LeapListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void QTVS_Leap::LeapListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void QTVS_Leap::LeapListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void QTVS_Leap::LeapListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void QTVS_Leap::LeapListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}


QTVS_Leap::QTVS_Leap(QWidget *parent
                    )  : QMainWindow(parent)
{
  ui.setupUi(this);


//  HDC hDC_Desktop = GetDC(0);

  /* Draw a simple blue rectangle on the desktop */
//  RECT rect = { 20, 20, 200, 200 };
//  HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
//  FillRect(hDC_Desktop, &rect, blueBrush);


  ScreenResX = GetSystemMetrics(SM_CXSCREEN);
  ScreenResY = GetSystemMetrics(SM_CYSCREEN);

  POINT pt;
  pt.x = 10;
  pt.y = 10;

  HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

  MONITORINFO mi = { sizeof(MONITORINFO) };
  GetMonitorInfo(monitor, &mi);
  rcMonitor = mi.rcMonitor;

  debugDisplayString = QString::number(rcMonitor.right - rcMonitor.left);

  Sleep(500);

  debugWindowDrag_Left.left = -1;
  debugWindowDrag_Right.left = -1;
//10 fingers
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  fingerTraces.append(new FingerTraceWindow());
  // thumbTrace = new FingerTraceWindow();
  // thumbTrace->show();
  // thumbTrace = new FingerTraceWindow();
  // indexTrace = new FingerTraceWindow();
  // indexTrace->show();
  // middleTrace = new FingerTraceWindow();
  // middleTrace->show();
  // ringTrace = new FingerTraceWindow();
  // ringTrace->show();
  // pinkieTrace = new FingerTraceWindow();
  // pinkieTrace->show();


  MouseFocusCenter.x = -1;



  QtConcurrent::run(this, &QTVS_Leap::Debug_ControllerListener);

  // QMediaPlayer* player = new QMediaPlayer;

  FingerSwipeThrottleTimer = new QTimer(this);
  connect(FingerSwipeThrottleTimer, SIGNAL(timeout()), this, SLOT(resetSwipeThrottle()));

//connect(this, SIGNAL(ParangusGestureDetected(int fingerIndex, leapSwipeDirection direction)), this, SLOT( ParangusGesture(int fingerIndex, leapSwipeDirection direction)));

  QTimer * debugTimer = new QTimer;
  connect(debugTimer, SIGNAL(timeout()), this, SLOT(DebugLooper()));
  debugTimer->start(5);

}

QTVS_Leap::~QTVS_Leap()
{
  killDetectionLoop = true;
}

void QTVS_Leap::DebugLooper()
{
  DebugTimerCount++;
  ui.label->setText(debugDisplayString);
  if (bSwipeThrottleTimer && !FingerSwipeThrottleTimer->isActive())
  {
    bSwipeThrottleTimer = false;
    FingerSwipeThrottleTimer->start(200);
  }

//    if(FingerSwipeThrottleTimer.elapsed() > 200)
//    {

//    }

}

void QTVS_Leap::Debug_ControllerListener()
{
  listener.SetParent(this);
  // Have the sample listener receive events from the controller
  controller.addListener(listener);
  controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  while (!killDetectionLoop)
  {
    Sleep(1000);
    if (!controller.isPolicySet(Leap::Controller::POLICY_BACKGROUND_FRAMES))
      controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
  }
}

void QTVS_Leap::HandLogic()
{
  //TODO: Fix this
  if (hands.count() == 1)
  {
    int iHandToFingerShift = hand.isLeft() ? 5 : 0;

    for (int iFingerCounter = iHandToFingerShift;
         iFingerCounter <= iHandToFingerShift + 4;
         iFingerCounter ++)
      QCoreApplication::postEvent(fingerTraces.at(iFingerCounter), new QHideEvent());

    iHandToFingerShift = hand.isLeft() ? 0 : 5;

if(ui.checkBox_ShowFingers->isChecked())
{
    for (int iFingerCounter = iHandToFingerShift;
         iFingerCounter <= iHandToFingerShift + 4;
         iFingerCounter ++)
      QCoreApplication::postEvent(fingerTraces.at(iFingerCounter), new QShowEvent());
}

  }
  else if (hands.isEmpty())
  {
    foreach (FingerTraceWindow * fTrace, fingerTraces)
      QCoreApplication::postEvent(fTrace, new QHideEvent());
    // QCoreApplication::postEvent(thumbTrace, new QHideEvent());
    // QCoreApplication::postEvent(indexTrace, new QHideEvent());
    // QCoreApplication::postEvent(middleTrace, new QHideEvent());
    // QCoreApplication::postEvent(ringTrace, new QHideEvent());
    // QCoreApplication::postEvent(pinkieTrace, new QHideEvent());
  }
  else
  {
    if(ui.checkBox_ShowFingers->isChecked())
    {
    foreach (FingerTraceWindow * fTrace, fingerTraces)
      QCoreApplication::postEvent(fTrace, new QShowEvent());      
    }
    // QCoreApplication::postEvent(thumbTrace, new QShowEvent());
    // QCoreApplication::postEvent(indexTrace, new QShowEvent());
    // QCoreApplication::postEvent(middleTrace, new QShowEvent());
    // QCoreApplication::postEvent(ringTrace, new QShowEvent());
    // QCoreApplication::postEvent(pinkieTrace, new QShowEvent());

  }

  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    hand = *hl;

//TODO: Perhaps move this to gestures?
    if (ui.checkBox_Crunch->isChecked())
    {
      if (hands.count() == 2)
      {
        // we check if one hand's dragging and the other's closed
        if (hand.isLeft())
        {
          // if this hand is left, and the other hand (right) is dragging something..
          if (debugWindowDrag_Right.left != -1)
          {
            debugDisplayString = QString::number(hand.grabStrength());
            //pretty much closed
            if (hand.grabStrength() >= 0.7)
            {
              SendMessage(debugWindowHWND_Right, WM_SYSCOMMAND, SC_CLOSE, 0);
              // DestroyWindow();
            }
          }
        }
        else
        {
          // if this hand is left, and the other hand (right) is dragging something..
          if (debugWindowDrag_Left.left != -1)
          {
            //pretty much closed
            if (hand.grabStrength() >= 0.7)
            {
              // DestroyWindow(debugWindowHWND_Left);
              SendMessage(debugWindowHWND_Left, WM_SYSCOMMAND, SC_CLOSE, 0);
            }
          }
        }
      }
    }
    // std::string handType = hand.isLeft() ? "Left hand" : "Right hand";

    // std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
    //           << ", palm position: " << hand.palmPosition() << std::endl;
    // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    // debugDisplayString = QString(", palm position: " + QString(hand.palmPosition().toString().data() ));

    // debugDisplayString = QString::number(hand.palmPosition().x); //20
    // debugDisplayString.append("\n");
    // debugDisplayString.append(QString::number(hand.palmPosition().y)); // 5
    // debugDisplayString.append("\n");
    // debugDisplayString.append(QString::number(hand.palmPosition().z));
    // debugDisplayString.append("\n");
    // debugDisplayString.append("roll:" + QString::number(normal.roll()));
///--------------------------------------------------

    if (debug_extendedFingerCounter != 0 && ui.checkBox_palmMouse->isChecked())
      HandCursorPosition(hand.stabilizedPalmPosition());


    if (ui.checkBox_HandRollDrag->isChecked())
    {
      if (bDebug_HandRollDrag)
      {
        if (normal.roll() > 0.7)
        {
          bDebug_HandRollDrag = false;

          if (debug_extendedFingerCounter != 0)
            MouseKeyboardEmulation::MouseLeftClickDown();
        }
      }
    }
    if (!bDebug_HandRollDrag)
    {
      if (normal.roll() < 0.5)
      {
        bDebug_HandRollDrag = true;
        MouseKeyboardEmulation::MouseLeftClickUp();
      }
    }

///-----------------------------------------------

    debug_extendedFingerCounter = 0;
    foreach (Finger finger, fingers)
    {
      if (finger.isExtended())
        debug_extendedFingerCounter++;
    }

    // Fist Scrolling

    if (debug_extendedFingerCounter == 0 && ui.checkBox_palmScroll->isChecked())
    {

      if (fFistPositionY == 0)
        fFistPositionY = hand.palmPosition().y;


      if (hand.palmPosition().y > fFistPositionY + 10)
      {
        float fDifference = hand.palmPosition().y - fFistPositionY + 10;
        fDifference /= 10;
        MouseKeyboardEmulation::MouseWheelUp(fDifference);
      }
      else if (hand.palmPosition().y < fFistPositionY - 10)
      {
        float fDifference = abs(hand.palmPosition().y - fFistPositionY - 10);
        fDifference /= 10;
        MouseKeyboardEmulation::MouseWheelUp(-1 * fDifference);
      }

      //              debugDisplayString = QString::number(ThumbMiddleDifference_X); //20
      //    debugDisplayString.append("\n");
      //       debugDisplayString.append(QString::number(abs(middleFinger.stabilizedTipPosition().y - indexFinger.stabilizedTipPosition().y))); // 5
      //    debugDisplayString.append("\n");
      //       debugDisplayString.append(QString::number(debug_extendedFingerCounter));

      //works
      // if(direction.pitch() * RAD_TO_DEG > 30)
      // MouseKeyboardEmulation::MouseWheelUp((direction.pitch() * RAD_TO_DEG)/10);
      // if(direction.pitch() * RAD_TO_DEG < 30)
      // MouseKeyboardEmulation::MouseWheelDown(5);

    }
    else
    {

      if (fFistPositionY != 0)
        fFistPositionY = 0;
    }
    // std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
    //           << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
    //           << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl;

    // Get the Arm bone
    Arm arm = hand.arm();
    // std::cout << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
    //           << " wrist position: " << arm.wristPosition()
    //           << " elbow position: " << arm.elbowPosition() << std::endl;

    // Get fingers
    fingers = hand.fingers();
    FingerLogic(hand.isLeft() ? handLeft : handRight);

  }

}

void QTVS_Leap::FingerLogic(handIndex hIndex)
{

  if (ui.checkBox_gesturesParangus->isChecked())
    ParangusGestureLogic();

  if (ui.checkBox_gesturesLeap->isChecked())
    LeapGestureLogic();

//DEBUG: Atm we cancel
  // return;

  // 0 -> 4 = left hand
  // 5 -> 9 = right hand
  // So lets use a shift and no redundant code
  int iHandToFingerShift = 0;
  if (hIndex == handRight)
    iHandToFingerShift += 5;

  for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
    const Finger finger = *fl;
    // std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
    //           << " finger, id: " << finger.id()
    //           << ", length: " << finger.length()
    //           << "mm, width: " << finger.width() << std::endl;
    Leap::Vector position = finger.stabilizedTipPosition();

    // Convert Leap::Vector position to Screen Coords
    QPoint screenPosition = FingerCursorPosition(position);


    // Lerp coords for smoothness if required
    int xCoord = lerp(fingerTraces.at(finger.type() + iHandToFingerShift)->geometry().left(), screenPosition.x(), dMouseLerpValue);
    int yCoord = lerp(fingerTraces.at(finger.type() + iHandToFingerShift)->geometry().top(), screenPosition.y(), dMouseLerpValue);

    // Qt Doesn't allow different threads to overwride gui locations.
    // Therefore, the usage of events are well, used.
    // This updates visual finger representation to correct location
    QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), fingerTraces.at(finger.type() + iHandToFingerShift)->pos());
    QCoreApplication::postEvent(fingerTraces.at(finger.type() + iHandToFingerShift), tEvent);

    // Z axis does NOT use stabilized position.
    // Stabilized position is generally used for X/Y movement (2D display)
    // Therefore Z is updated quite poorely. So we used unStabalized position instead
    // Setup position limits (manual testing for values
    position = finger.tipPosition();
    float zFinger = position.z < 0 ? 0 : position.z > 200 ? 200 : position.z;

    // Convert to percentages
    // trim lowerend of percentages
    zFinger /= 200;
    if (zFinger < 0.1) zFinger = 0.1;

    if (hIndex == handLeft)
    {
        if(ui.checkBox_FingerDragsWindows->isChecked())
        {
      // We're on index finger and its close to screen / center of Z-plane on leap motion
      if (finger.type() == leapIndex && zFinger < 0.12)
      {
        POINT pt;
        pt.x = screenPosition.x();
        pt.y = screenPosition.y();


        // if our rect was reset
        // Therefore, we just started to drag
        if (debugWindowDrag_Left.left == -1)
        {
          // Find window under point
          // Find window's dimmensions
          // Find difference between player's finger coords and window's coords
          debugWindowHWND_Left = GetRealParent(WindowFromPoint(pt));

          // Set it as active window if need be.
          if(debugWindowHWND_Left != GetForegroundWindow() && ui.checkBox_DragSetsActive->isChecked())
          {
            SetForegroundWindow(debugWindowHWND_Left);
            // Backup incase SetForegroundWindow fails
            // TODO: Perhaps replace in future? since foreground window DOES faile occasionally.
            // SetWindowPos(debugWindowHWND_Left,HWND_TOPMOST,0,0,0,0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
          }

          // Restore window if maximized Section
          WINDOWPLACEMENT wndpl = { sizeof(WINDOWPLACEMENT) };
          GetWindowPlacement(debugWindowHWND_Left, &wndpl);

          // Determine if window is maximized to begin with
          if (wndpl.showCmd == SW_MAXIMIZE)
          {
            // Setup the restore command and send it
            wndpl.showCmd = SW_RESTORE;
            SetWindowPlacement(debugWindowHWND_Left, &wndpl);

            // Center restored window around player's fingers
            int iTempWindowWidth = wndpl.rcNormalPosition.right - wndpl.rcNormalPosition.left;
            int iTempWindowHeight = wndpl.rcNormalPosition.bottom - wndpl.rcNormalPosition.top;
            MoveWindow(debugWindowHWND_Left, pt.x - iTempWindowWidth / 2, pt.y - iTempWindowHeight / 2,
                       iTempWindowWidth, iTempWindowHeight, true);
          }

          GetWindowRect(debugWindowHWND_Left, &debugWindowDrag_Left);
          debugWindowDrag_Left.left = pt.x - debugWindowDrag_Left.left;
          debugWindowDrag_Left.top = pt.y -  debugWindowDrag_Left.top;
          debugWindowDrag_Left.bottom = pt.y;//debugWindowDrag_Left.bottom - pt.y;
          debugWindowDrag_Left.right = pt.x;//debugWindowDrag_Left.bottom - pt.y;
        }

        // Setup temp rect for window's width/height
        RECT tRect;
        GetWindowRect(debugWindowHWND_Left, &tRect);

        debugDisplayString = QString::number(pt.y);

        // If left hand and right hand have different HWNDs on drag
        // Proceed with normal drag
        if (debugWindowHWND_Left != debugWindowHWND_Right)
        {
          WINDOWPLACEMENT wndpl = { sizeof(WINDOWPLACEMENT) };
          GetWindowPlacement(debugWindowHWND_Left, &wndpl);

          // AeroSnap time

          // Maximize window on top
          if (pt.y <= 30 && wndpl.showCmd != SW_MAXIMIZE)
          {
            iDebugWindowRestore_LeftHand = 1;
            iDebug_PreviousWindowWidth_LeftHand = tRect.right - tRect.left;
            iDebug_PreviousWindowHeight_LeftHand = tRect.bottom - tRect.top;
            wndpl.showCmd = SW_SHOWMAXIMIZED;
            SetWindowPlacement(debugWindowHWND_Left, &wndpl);
          }

          // AeroSnap Left
          else if (pt.x <= 30)
          {
            if (iDebugWindowRestore_LeftHand == 0)
            {
              iDebugWindowRestore_LeftHand = 1;

              iDebug_PreviousWindowWidth_LeftHand = tRect.right - tRect.left;
              iDebug_PreviousWindowHeight_LeftHand = tRect.bottom - tRect.top;

              int wndwidth = (rcMonitor.right - rcMonitor.left) / 2;
              int wndheight = rcMonitor.bottom - rcMonitor.top;//max(min((rcMonitor.bottom-rcMonitor.top), state.mmi.ptMaxTrackSize.y), state.mmi.ptMinTrackSize.y);
              int posx = rcMonitor.left;
              int posy = rcMonitor.top + (rcMonitor.bottom - rcMonitor.top) / 2 - wndheight / 2; // Center vertically (if window has a max height)
              MoveWindow(debugWindowHWND_Left, posx, posy,
                         wndwidth, wndheight, true);
            }
          }

          // AeroSnap Left
          else if (pt.x >= (rcMonitor.right - rcMonitor.left) - 30)
          {
            if (iDebugWindowRestore_LeftHand == 0)
            {
              iDebugWindowRestore_LeftHand = 1;

              iDebug_PreviousWindowWidth_LeftHand = tRect.right - tRect.left;
              iDebug_PreviousWindowHeight_LeftHand = tRect.bottom - tRect.top;

              int wndwidth = (rcMonitor.right - rcMonitor.left) / 2;
              int wndheight = rcMonitor.bottom - rcMonitor.top;//max(min((rcMonitor.bottom-rcMonitor.top), state.mmi.ptMaxTrackSize.y), state.mmi.ptMinTrackSize.y);
              int posx = rcMonitor.right - wndwidth;
              int posy = rcMonitor.top + (rcMonitor.bottom - rcMonitor.top) / 2 - wndheight / 2; // Center vertically (if window has a max height)
              MoveWindow(debugWindowHWND_Left, posx, posy,
                         wndwidth, wndheight, true);
            }
          }


          // Normal Drag
          else if ( pt.y > 30 && pt.x > 30  && (pt.x < (rcMonitor.right - rcMonitor.left) - 30 ))
          {

            // some other custom snap is in effect
            if (iDebugWindowRestore_LeftHand == 1)
            {
              iDebugWindowRestore_LeftHand = 0;

              if (wndpl.showCmd == SW_MAXIMIZE)
              {
                // Setup the restore command and send it
                wndpl.showCmd = SW_RESTORE;
                SetWindowPlacement(debugWindowHWND_Left, &wndpl);
              }
              MoveWindow(debugWindowHWND_Left, pt.x - iDebug_PreviousWindowWidth_LeftHand / 2, pt.y - iDebug_PreviousWindowHeight_LeftHand / 2,
                         iDebug_PreviousWindowWidth_LeftHand, iDebug_PreviousWindowHeight_LeftHand, true);
            }

            // Nope, just normal drag
            else if (wndpl.showCmd != SW_MAXIMIZE)
              MoveWindow(debugWindowHWND_Left, pt.x - debugWindowDrag_Left.left, pt.y - debugWindowDrag_Left.top, tRect.right - tRect.left, tRect.bottom - tRect.top, true);
          }

        }

        // Left and Right hand have same HWNDs
        // Resize mode engadged
        else if(ui.checkBox_ResizeWindows->isChecked()) {
          // Left hand's overview:
          // Horizontal movement = simultaneious change in window's coords + change in width.
          // Vertical movement = change in window's height only.

          // Left reference points:
          // X = hwnd.left
          // Y = hwnd.bottom

          RECT resizeRect;

          resizeRect.top = tRect.top;
          resizeRect.left = tRect.left + (pt.x - debugWindowDrag_Left.right);// / 5;
          resizeRect.right = (tRect.right - resizeRect.left);// - (pt.x - debugWindowDrag_Left.left) ;/// 5;
          resizeRect.bottom = (tRect.bottom - tRect.top) + (pt.y - debugWindowDrag_Left.bottom);// / 5;

          debugWindowDrag_Left.right = pt.x;
          debugWindowDrag_Left.bottom = pt.y;

          MoveWindow(debugWindowHWND_Left, resizeRect.left, resizeRect.top, resizeRect.right , resizeRect.bottom, true);

        }

      }
      else if (finger.type() == leapIndex && zFinger > 0.15)
      {
        debugWindowDrag_Left.left = -1;
        debugWindowHWND_Left = NULL;
      }
    }
    }

    if (hIndex == handRight)
    {

        if(ui.checkBox_FingerDragsWindows->isChecked())
        {
      if (finger.type() == leapIndex && zFinger < 0.12)
      {

        // debugDisplayString = "TRYIGN";
        POINT pt;
        pt.x = screenPosition.x() - 15;
        pt.y = screenPosition.y() - 15;


        if (debugWindowDrag_Right.left == -1)
        {
          debugWindowHWND_Right = GetRealParent(WindowFromPoint(pt));
        
          if(debugWindowHWND_Right != GetForegroundWindow() && ui.checkBox_DragSetsActive->isChecked())
          {
            SetForegroundWindow(debugWindowHWND_Right);
            // Backup incase SetForegroundWindow fails
            // TODO: Perhaps replace in future? since foreground window DOES faile occasionally.
            // SetWindowPos(debugWindowHWND_Left,HWND_TOPMOST,0,0,0,0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
          }

          GetWindowRect(debugWindowHWND_Right, &debugWindowDrag_Right);
          debugWindowDrag_Right.left = pt.x - debugWindowDrag_Right.left;
          debugWindowDrag_Right.top = pt.y -  debugWindowDrag_Right.top;
          debugWindowDrag_Right.bottom = pt.y;//debugWindowDrag_Left.bottom - pt.y;
          debugWindowDrag_Right.right = pt.x;//debugWindowDrag_Left.bottom - pt.y;
        }

        RECT tRect;
        GetWindowRect(debugWindowHWND_Right, &tRect);

        if (debugWindowHWND_Left != debugWindowHWND_Right)
        {
          WINDOWPLACEMENT wndpl = { sizeof(WINDOWPLACEMENT) };
          GetWindowPlacement(debugWindowHWND_Right, &wndpl);

          // AeroSnap time

          // Maximize window on top
          if (pt.y <= 30 && wndpl.showCmd != SW_MAXIMIZE)
          {
            iDebugWindowRestore_RightHand = 1;
            iDebug_PreviousWindowWidth_RightHand = tRect.right - tRect.left;
            iDebug_PreviousWindowHeight_RightHand = tRect.bottom - tRect.top;
            wndpl.showCmd = SW_SHOWMAXIMIZED;
            SetWindowPlacement(debugWindowHWND_Right, &wndpl);
          }

          // AeroSnap Left
          else if (pt.x <= 30)
          {
            if (iDebugWindowRestore_RightHand == 0)
            {
              iDebugWindowRestore_RightHand = 1;

              iDebug_PreviousWindowWidth_RightHand = tRect.right - tRect.left;
              iDebug_PreviousWindowHeight_RightHand = tRect.bottom - tRect.top;

              int wndwidth = (rcMonitor.right - rcMonitor.left) / 2;
              int wndheight = rcMonitor.bottom - rcMonitor.top;//max(min((rcMonitor.bottom-rcMonitor.top), state.mmi.ptMaxTrackSize.y), state.mmi.ptMinTrackSize.y);
              int posx = rcMonitor.left;
              int posy = rcMonitor.top + (rcMonitor.bottom - rcMonitor.top) / 2 - wndheight / 2; // Center vertically (if window has a max height)
              MoveWindow(debugWindowHWND_Right, posx, posy,
                         wndwidth, wndheight, true);
            }
          }

          // AeroSnap Left
          else if (pt.x >= (rcMonitor.right - rcMonitor.left) - 30)
          {
            if (iDebugWindowRestore_RightHand == 0)
            {
              iDebugWindowRestore_RightHand = 1;

              iDebug_PreviousWindowWidth_RightHand = tRect.right - tRect.left;
              iDebug_PreviousWindowHeight_RightHand = tRect.bottom - tRect.top;

              int wndwidth = (rcMonitor.right - rcMonitor.left) / 2;
              int wndheight = rcMonitor.bottom - rcMonitor.top;//max(min((rcMonitor.bottom-rcMonitor.top), state.mmi.ptMaxTrackSize.y), state.mmi.ptMinTrackSize.y);
              int posx = rcMonitor.right - wndwidth;
              int posy = rcMonitor.top + (rcMonitor.bottom - rcMonitor.top) / 2 - wndheight / 2; // Center vertically (if window has a max height)
              MoveWindow(debugWindowHWND_Right, posx, posy,
                         wndwidth, wndheight, true);
            }
          }


          // Normal Drag
          else if ( pt.y > 30 && pt.x > 30  && (pt.x < (rcMonitor.right - rcMonitor.left) - 30 ))
          {

            // some other custom snap is in effect
            if (iDebugWindowRestore_RightHand == 1)
            {
              iDebugWindowRestore_RightHand = 0;

              if (wndpl.showCmd == SW_MAXIMIZE)
              {
                // Setup the restore command and send it
                wndpl.showCmd = SW_RESTORE;
                SetWindowPlacement(debugWindowHWND_Right, &wndpl);
              }
              MoveWindow(debugWindowHWND_Right, pt.x - iDebug_PreviousWindowWidth_RightHand / 2, pt.y - iDebug_PreviousWindowHeight_RightHand / 2,
                         iDebug_PreviousWindowWidth_RightHand, iDebug_PreviousWindowHeight_RightHand, true);
            }

            // Nope, just normal drag
            else if (wndpl.showCmd != SW_MAXIMIZE)
              MoveWindow(debugWindowHWND_Right, pt.x - debugWindowDrag_Right.left, pt.y - debugWindowDrag_Right.top, tRect.right - tRect.left, tRect.bottom - tRect.top, true);
          }
        }
        else if (ui.checkBox_ResizeWindows->isChecked()){
          // Right hand's overview:
          // Horizontal movement = change in width.
          // Vertical movement = simultaneious change in window's coords + change in window's height only.

          // debug_iReferencePointX_Right =  tRect.left + debugWindowDrag_Right.left;// + debug_iReferencePointY_Left;
          // debug_iReferencePointY_Right =  tRect.top + debugWindowDrag_Right.top;// + debug_iReferencePointY_Left;



          // RECT resizeRect;

          // resizeRect.top = tRect.top;
          // resizeRect.left = tRect.left + (pt.x - debugWindowDrag_Left.right);// / 5;
          // resizeRect.right = (tRect.right - resizeRect.left);// - (pt.x - debugWindowDrag_Left.left) ;/// 5;
          // resizeRect.bottom = (tRect.bottom - tRect.top) + (pt.y - debugWindowDrag_Left.bottom);// / 5;

          // debugWindowDrag_Left.right = pt.x;
          // debugWindowDrag_Left.bottom = pt.y;



          RECT resizeRect;

          resizeRect.top = tRect.top + (pt.y - debugWindowDrag_Right.bottom);
          resizeRect.left = tRect.left;
          resizeRect.right = (tRect.right - tRect.left) + (pt.x - debugWindowDrag_Right.right);
          resizeRect.bottom = (tRect.bottom - resizeRect.top);// - (pt.y - debug_iReferencePointY_Right) / 5;


          // debugWindowDrag_Right.left += (pt.x - debug_iReferencePointX_Right) / 5;
          // debugWindowDrag_Right.top += (pt.y - debug_iReferencePointY_Right) / 5;
          debugWindowDrag_Right.right = pt.x;
          debugWindowDrag_Right.bottom = pt.y;


          MoveWindow(debugWindowHWND_Right, resizeRect.left, resizeRect.top, resizeRect.right , resizeRect.bottom, false);

        }
        // debugDisplayString = QString::number(finger.tipPosition().z);
      }
      else if (finger.type() == leapIndex && zFinger > 0.15)
      {
        debugWindowDrag_Right.left = -1;
        debugWindowHWND_Right = NULL;
      }
        }
    }
    zFinger *= 75.0;

    QResizeEvent *rEvent = new QResizeEvent(QSize(zFinger, zFinger), fingerTraces.at(finger.type() + iHandToFingerShift)->size());
    QCoreApplication::postEvent(fingerTraces.at(finger.type() + iHandToFingerShift), rEvent);


    // switch (finger.type())
    // {
    // case leapThumb:
    // {
    //   // if(finger.isExtended())
    //   // {
    //     // QShowEvent * sEvent = new QShowEvent();
    //     // QCoreApplication::postEvent(thumbTrace, sEvent);
    //   int xCoord = lerp(thumbTrace->geometry().left(), screenPosition.x(), dMouseLerpValue);
    //   int yCoord = lerp(thumbTrace->geometry().top(), screenPosition.y(), dMouseLerpValue);

    //   QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), thumbTrace->pos());
    //   QCoreApplication::postEvent(thumbTrace, tEvent);
    //   // }
    //   // else
    //   // {
    //     // QHideEvent * hEvent = new QHideEvent();
    //     // QCoreApplication::postEvent(thumbTrace, hEvent);
    //   // }
    //   // thumbTrace->move(xCoord, yCoord);
    // }
    //   // indexTrace;
    //   // middleTrace;
    //   // ringTrace;
    //   // pinkieTrace;

    // break;

    // case leapIndex:

    // {
    //   int xCoord = lerp(indexTrace->geometry().left(), screenPosition.x(), dMouseLerpValue);
    //   int yCoord = lerp(indexTrace->geometry().top(), screenPosition.y(), dMouseLerpValue);

    //   QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), indexTrace->pos());
    //   QCoreApplication::postEvent(indexTrace, tEvent);
    //   // thumbTrace->move(xCoord, yCoord);
    // }
    // break;

    // case leapMiddle:

    // {
    //   int xCoord = lerp(middleTrace->geometry().left(), screenPosition.x(), dMouseLerpValue);
    //   int yCoord = lerp(middleTrace->geometry().top(), screenPosition.y(), dMouseLerpValue);

    //   QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), middleTrace->pos());
    //   QCoreApplication::postEvent(middleTrace, tEvent);
    //   // thumbTrace->move(xCoord, yCoord);
    // }
    // break;

    // case leapRing:

    // {
    //   int xCoord = lerp(ringTrace->geometry().left(), screenPosition.x(), dMouseLerpValue);
    //   int yCoord = lerp(ringTrace->geometry().top(), screenPosition.y(), dMouseLerpValue);

    //   QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), ringTrace->pos());
    //   QCoreApplication::postEvent(ringTrace, tEvent);
    //   // thumbTrace->move(xCoord, yCoord);
    // }
    // break;

    // case leapPinkie:

    // {
    //   int xCoord = lerp(pinkieTrace->geometry().left(), screenPosition.x(), dMouseLerpValue);
    //   int yCoord = lerp(pinkieTrace->geometry().top(), screenPosition.y(), dMouseLerpValue);

    //   QMoveEvent *tEvent = new QMoveEvent(QPoint(xCoord, yCoord), pinkieTrace->pos());
    //   QCoreApplication::postEvent(pinkieTrace, tEvent);
    //   // thumbTrace->move(xCoord, yCoord);
    // }
    // break;
    // }
  }
}

void QTVS_Leap::LeapGestureLogic()
{

  for (int g = 0; g < gestures.count(); ++g) {
    Gesture gesture = gestures[g];

    switch (gesture.type()) {
    case Gesture::TYPE_CIRCLE:
    {
//        emit Listener_Gesture(gesture, gesture.type());
      CircleGesture circle = gesture;
      std::string clockwiseness;

      if (circle.pointable().direction().angleTo(circle.normal()) <= PI / 2) {
        clockwiseness = "clockwise";

        // MouseKeyboardEmulation::MouseWheelDown(5);

      } else {
        clockwiseness = "counterclockwise";

        // MouseKeyboardEmulation::MouseWheelUp(5);
      }

      // Calculate angle swept since last frame
      float sweptAngle = 0;
      if (circle.state() != Gesture::STATE_START) {
        CircleGesture previousUpdate = CircleGesture(controller.frame(1).gesture(circle.id()));
        sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * PI;
      }
      std::cout << std::string(2, ' ')
                << "Circle id: " << gesture.id()
                << ", state: " << stateNames[gesture.state()]
                << ", progress: " << circle.progress()
                << ", radius: " << circle.radius()
                << ", angle " << sweptAngle * RAD_TO_DEG
                <<  ", " << clockwiseness << std::endl;
      break;
    }
    case Gesture::TYPE_SWIPE:
    {
      SwipeGesture swipe = gesture;
      //            debugDisplayString = "";
      // debugDisplayString.append("Swipe id: " + QString::number(gesture.id()));
      // debugDisplayString.append( ", state: " + QString(stateNames[gesture.state()].data()));
      // debugDisplayString.append( ", direction: " + QString(swipe.direction().toString().data()));
      // debugDisplayString.append( ", speed: " + QString::number(swipe.speed()));
      std::cout << std::string(2, ' ')
                << "Swipe id: " << gesture.id()
                << ", state: " << stateNames[gesture.state()]
                << ", direction: " << swipe.direction()
                << ", speed: " << swipe.speed() << std::endl;
      break;
    }
    case Gesture::TYPE_KEY_TAP:
    {
      KeyTapGesture tap = gesture;
//        std::cout << std::string(2, ' ')
      //debugDisplayString = "";
      // debugDisplayString.append("Key Tap id: " + QString::number(gesture.id()));
      // debugDisplayString.append( ", state: " + QString(stateNames[gesture.state()].data()));
      //debugDisplayString.append( ", position: " + QString(tap.position().toString().data()));
      // debugDisplayString.append( ", direction: " + QString(tap.direction().toString().data()));
      break;
    }
    case Gesture::TYPE_SCREEN_TAP:
    {
      ScreenTapGesture screentap = gesture;
      std::cout << std::string(2, ' ')
                << "Screen Tap id: " << gesture.id()
                << ", state: " << stateNames[gesture.state()]
                << ", position: " << screentap.position()
                << ", direction: " << screentap.direction() << std::endl;
      break;
    }
    default:
      std::cout << std::string(2, ' ')  << "Unknown gesture type." << std::endl;
      break;
    }
  }

  if (!frame.hands().isEmpty() || !gestures.isEmpty()) {
    std::cout << std::endl;
  }
}

void QTVS_Leap::ParangusGestureLogic()
{
  if (ui.checkBox_PalmForSwipes->isChecked())
  {

    Leap::Vector palmPosition = controller.frame(3).hands()[0].palmPosition();

    float previousPalmYPosAndDifference = palmPosition.y;
    float previousPalmXPosAndDifference = palmPosition.x;

    previousPalmYPosAndDifference = hand.palmPosition().y - previousPalmYPosAndDifference;
    previousPalmXPosAndDifference = hand.palmPosition().x - previousPalmXPosAndDifference;

    // std::cout << previousFingerXPosAndDifference << "\n";

    if (previousPalmYPosAndDifference < -2.5 && handCache.bGestureToggle)
    {
      std::cout << "down \n";
      handCache.bGestureToggle = false;
      QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, NULL, swipe_Down);
    }

    if (previousPalmYPosAndDifference > 2.5 && handCache.bGestureToggle)
    {
      std::cout << "swipe_Up \n";
      handCache.bGestureToggle = false;
      QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, NULL, swipe_Up);
    }

    if (previousPalmXPosAndDifference < -2.5 && handCache.bGestureToggle)
    {
      std::cout << "swipe_Left \n";
      handCache.bGestureToggle = false;
      QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, NULL, swipe_Left);
    }

    if (previousPalmXPosAndDifference > 2.5 && handCache.bGestureToggle)
    {
      std::cout << "swipe_Right \n";
      handCache.bGestureToggle = false;
      QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, NULL, swipe_Right);
    }


    if (!handCache.bGestureToggle &&
        abs(previousPalmYPosAndDifference) <= 0.5 &&
        abs(previousPalmXPosAndDifference) <= 0.5 )
      handCache.bGestureToggle = true;
    return;
  }

  // if palm for swipes isn't checked, we go for fingers instead:
  for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl)
  {
    const Finger finger = *fl;

//To simplify things down the road
    int leapFingerIndex = finger.type();

    if (finger.isExtended())
    {
      Leap::Vector fingerPosition = controller.frame(3).fingers()[leapFingerIndex].stabilizedTipPosition();

      float previousFingerYPosAndDifference = fingerPosition.y;
      float previousFingerXPosAndDifference = fingerPosition.x;

      previousFingerYPosAndDifference = finger.stabilizedTipPosition().y - previousFingerYPosAndDifference;
      previousFingerXPosAndDifference = finger.stabilizedTipPosition().x - previousFingerXPosAndDifference;

      // std::cout << previousFingerXPosAndDifference << "\n";

      if (previousFingerYPosAndDifference < -10 && handCache.fingers_p[leapFingerIndex].bGestureToggle)
      {
        handCache.fingers_p[leapFingerIndex].bGestureToggle = false;
        QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, leapFingerIndex, swipe_Down);
      }

      if (previousFingerYPosAndDifference > 10 && handCache.fingers_p[leapFingerIndex].bGestureToggle)
      {
        handCache.fingers_p[leapFingerIndex].bGestureToggle = false;
        QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, leapFingerIndex, swipe_Up);
      }

      if (previousFingerXPosAndDifference < -10 && handCache.fingers_p[leapFingerIndex].bGestureToggle)
      {
        handCache.fingers_p[leapFingerIndex].bGestureToggle = false;
        QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, leapFingerIndex, swipe_Left);
      }

      if (previousFingerXPosAndDifference > 10 && handCache.fingers_p[leapFingerIndex].bGestureToggle)
      {
        handCache.fingers_p[leapFingerIndex].bGestureToggle = false;
        QtConcurrent::run(this, &QTVS_Leap::ParangusGesture, leapFingerIndex, swipe_Right);
      }


      if (!handCache.fingers_p[leapFingerIndex].bGestureToggle &&
          abs(previousFingerYPosAndDifference) <= 0.1 &&
          abs(previousFingerXPosAndDifference) <= 0.1 )
        handCache.fingers_p[leapFingerIndex].bGestureToggle = true;
    }
  }
}

void QTVS_Leap::HandCursorPosition(Leap::Vector hPosition)
{

  debug_palmIndexY = hPosition.y - controller.frame(2).hands()[0].stabilizedPalmPosition().y;

  // Addition is half the total width tracking for leap
  float tempX = hPosition.x + iCursorLeapWidth / 2;
  // Subtraction is vertical translation (no sub = touching leap controller)
  float tempY = hPosition.y - 50;

  POINT CurrentMousePos;
  GetCursorPos(&CurrentMousePos);

  //Translations from leap to screen coordinates
  tempX = tempX > iCursorLeapWidth ? iCursorLeapWidth : tempX < 0 ? 0 : tempX;
  tempX = ScreenResX * (tempX / iCursorLeapWidth);

  tempY = tempY > iCursorLeapHeight ? iCursorLeapHeight : tempY < 0 ? 0 : tempY;
  tempY = ScreenResY - (ScreenResY * (tempY / iCursorLeapHeight));

  //DEBUG: test to keep stable at low movement
  if (abs(hand.palmPosition().x - debugLastX) > 0.03)
    tempX = lerp(CurrentMousePos.x, tempX, dMouseLerpValue);
  else
    tempX = CurrentMousePos.x;

  tempY = lerp(CurrentMousePos.y, tempY, dMouseLerpValue);

  //DEBUG: test
  debugLastX = hPosition.x;

  // if (ui.checkBox_indexMouse->isChecked())
  // Sets new cursor position
  SetCursorPos(tempX, tempY);
}

QPoint QTVS_Leap::FingerCursorPosition(Leap::Vector hPosition)
{

  // debug_palmIndexY = hPosition.y - controller.frame(2).hands()[0].stabilizedPalmPosition().y;

  // Addition is half the total width tracking for leap
  float tempX = hPosition.x + iCursorLeapWidth / 2;
  // Subtraction is vertical translation (no sub = touching leap controller)
  float tempY = hPosition.y - 100;

  // POINT CurrentMousePos;
  // GetCursorPos(&CurrentMousePos);

  //Translations from leap to screen coordinates
  tempX = tempX > iCursorLeapWidth ? iCursorLeapWidth : tempX < 0 ? 0 : tempX;
  tempX = ScreenResX * (tempX / iCursorLeapWidth);

  tempY = tempY > iCursorLeapHeight ? iCursorLeapHeight : tempY < 0 ? 0 : tempY;
  tempY = ScreenResY - (ScreenResY * (tempY / iCursorLeapHeight));

  return QPoint(tempX, tempY);
  //DEBUG: test to keep stable at low movement
  // if (abs(hand.palmPosition().x - debugLastX) > 0.03)
  // tempX = lerp(CurrentMousePos.x, tempX, dMouseLerpValue);
  // else
  // tempX = CurrentMousePos.x;

  // tempY = lerp(CurrentMousePos.y, tempY, dMouseLerpValue);

  //DEBUG: test
  // debugLastX = hPosition.x;

  // if (ui.checkBox_indexMouse->isChecked())
  // Sets new cursor position
  // SetCursorPos(tempX, tempY);
}

void QTVS_Leap::ParangusGesture(int fingerIndex, leapSwipeDirection direction)
{

  //Is palm stabilized?
  // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) > 2.5)
  // return;
  if (ui.checkBox_PalmForSwipes->isChecked())
  {
    switch (direction)
    {
    case swipe_Up:
      MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeUp->keyboardMods, ui.lineEdit_SwipeUp->keyCode);
      break;

    case swipe_Down:
      MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeDown->keyboardMods, ui.lineEdit_SwipeDown->keyCode);
      break;

    case swipe_Left:
      MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeLeft->keyboardMods, ui.lineEdit_SwipeLeft->keyCode);
      break;

    case swipe_Right:
      MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeRight->keyboardMods, ui.lineEdit_SwipeRight->keyCode);
      break;
    }
    return;
  }

  switch (fingerIndex)
  {
  case leapIndex:
    switch (direction)
    {
    case swipe_Up:
      // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) < 2)
      // {
      if (!ui.checkBox_GamerMode->isChecked())
      {
        // MouseKeyboardEmulation::MouseLeftClickDown();
        // Sleep(10);
        // MouseKeyboardEmulation::MouseLeftClickUp();
      }
      else
        MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeUp->keyboardMods, ui.lineEdit_SwipeUp->keyCode);
      // }

      break;

    case swipe_Down:
      // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) < 2)
      // {
      if (!ui.checkBox_GamerMode->isChecked())
      {
        MouseKeyboardEmulation::MouseLeftClickDown();
        Sleep(10);
        MouseKeyboardEmulation::MouseLeftClickUp();
      }
      else
        MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeDown->keyboardMods, ui.lineEdit_SwipeDown->keyCode);
      // }
      break;

    case swipe_Left:
      // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) < 2)
      // {
      if (!ui.checkBox_GamerMode->isChecked())
      {
        // MouseKeyboardEmulation::MouseLeftClickDown();
        // Sleep(10);
        // MouseKeyboardEmulation::MouseLeftClickUp();
      }
      else
        MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeLeft->keyboardMods, ui.lineEdit_SwipeLeft->keyCode);
      // }

      break;

    case swipe_Right:
      // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) < 2)
      // {
      if (!ui.checkBox_GamerMode->isChecked())
      {
        // MouseKeyboardEmulation::MouseLeftClickDown();
        // Sleep(10);
        // MouseKeyboardEmulation::MouseLeftClickUp();
      }
      else
        MouseKeyboardEmulation::HotkeyPress(ui.lineEdit_SwipeRight->keyboardMods, ui.lineEdit_SwipeRight->keyCode);
      // }

      break;
    }

    break;

  case leapMiddle:
    if (direction == swipe_Down)
    {
      if (!ui.checkBox_GamerMode->isChecked())
      {
        // if (abs(controller.frame(3).hands()[0].stabilizedPalmPosition().y - hand.stabilizedPalmPosition().y) < 2)
        // {
        MouseKeyboardEmulation::MouseRightClickDown();
        Sleep(10);
        MouseKeyboardEmulation::MouseRightClickUp();
        // }
      }
    }
    break;
  }
}

void QTVS_Leap::on_pushButton_clicked()
{
  bDesktopSwipeMode = !bDesktopSwipeMode;
  if (bDesktopSwipeMode)
    ui.pushButton->setText("Desktop Swipe Mode: Enabled");
  else
    ui.pushButton->setText("Desktop Swipe Mode: Disabled");
}

void QTVS_Leap::on_doubleSpinBox_valueChanged(double arg1)
{
  dMouseLerpValue = 1.0 - arg1 / 100.0;
}


void QTVS_Leap::resetSwipeThrottle()
{
  FingerSwipeThrottleTimer->stop();
}

void QTVS_Leap::on_checkBox_palmMouse_clicked()
{
  ui.checkBox_indexMouse->setChecked(false);
}

void QTVS_Leap::on_checkBox_indexMouse_clicked()
{
  ui.checkBox_palmMouse->setChecked(false);
}
