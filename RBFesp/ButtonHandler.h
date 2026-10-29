#pragma once
#include "RemoteCore.h"

void userSetup() {

}

void onButton(KeyEvent event) {
  if (event.key == 'W')
  {
    if(event.isPressed('W'))
    {
      monitor.println("Decolate");
    }
    else
    {
      monitor.println("Stopped");
    }
  }
}
