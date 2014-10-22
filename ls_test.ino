/************************** ls_test: LinnStrument debugging and testing ***************************
  Assorted debug functions. 
**************************************************************************************************/


void debugPrint(int level, char* msg) {
  if (Global.serialMode && (debugLevel >= level)) {
    Serial.print(msg);
  }
}

void debugPrintln(int level, char* msg) {
  if (Global.serialMode && (debugLevel >= level)) {
    Serial.println(msg);
  }
}

void debugPrint(int level, int val) {
  if (Global.serialMode && (debugLevel >= level)) {
    Serial.print(val);
  }
}

void debugPrintln(int level, int val) {
  if (Global.serialMode && (debugLevel >= level)) {
    Serial.println(val);
  }
}

// displayXFrame:
// For debug, displays an entire frame of raw X values in the Arduino serial monitor. Values are collected during each full read of the touch surface.
void displayXFrame()
{
  if (cell().touched == touchedCell)
  {
    cell().refreshX();
  }

  static unsigned long lastFrame = 0;
  unsigned long now = micros();
  if (sensorCol == 0 && sensorRow == 0 && calcTimeDelta(now, lastFrame) >= 500000) {
    lastFrame = now;

    Serial.println();
    for (byte x = 0; x < NUMCOLS; ++x)
    {
      Serial.print(x);
      Serial.print("\t");
    }
    Serial.println();
    for (byte y = NUMROWS; y > 0; --y)
    {
      for (byte x = 0; x < NUMCOLS; ++x)
      {
        if (cell(x, y-1).touched == touchedCell) {
          Serial.print(cell(x, y-1).currentRawX);
        }
        else {
          Serial.print("-");
        }
        Serial.print("\t");
      }
      Serial.println();
    }
  }
}

// displayYFrame:
// For debug, displays an entire frame of raw Z values in the Arduino serial monitor. Values are collected during each full read of the touch surface.
void displayYFrame()
{
  if (cell().touched == touchedCell)
  {
    cell().refreshY();
  }

  static unsigned long lastFrame = 0;
  unsigned long now = micros();
  if (sensorCol == 0 && sensorRow == 0 && calcTimeDelta(now, lastFrame) >= 500000) {
    lastFrame = now;
    
    Serial.println();
    for (byte x = 0; x < NUMCOLS; ++x)
    {
      Serial.print(x);
      Serial.print("\t");
    }
    Serial.println();
    for (byte y = NUMROWS; y > 0; --y)
    {
      for (byte x = 0; x < NUMCOLS; ++x)
      {
        if (cell(x, y-1).touched == touchedCell) {
          Serial.print(cell(x, y-1).currentRawY);
        }
        else {
          Serial.print("-");
        }
        Serial.print("\t");
      }
      Serial.println();
    }
  }
}

// displayZFrame:
// For debug, displays an entire frame of raw Z values in the Arduino serial monitor. Values are collected during each full read of the touch surface.
void displayZFrame()
{
  static unsigned long lastFrame = 0;
  unsigned long now = micros();
  if (sensorCol == 0 && sensorRow == 0 && calcTimeDelta(now, lastFrame) >= 500000) {
    lastFrame = now;
    
    Serial.println();
    for (byte x = 0; x < NUMCOLS; ++x)
    {
      Serial.print(x);
      Serial.print("\t");
    }
    Serial.println();
    for (byte y = NUMROWS; y > 0; --y)
    {
      for (byte x = 0; x < NUMCOLS; ++x)
      {
        Serial.print(cell(x, y-1).rawZ);
        Serial.print("\t");
      }
      Serial.println();
    }
  }
}

// For debug, displays an entire frame of raw Z values in the Arduino serial monitor. Values are collected during each full read of the touch surface.
void displaySurfaceScanTime()
{ 
  if (sensorCol == 0 && sensorRow == 0) {
    static int scanCount; 
    static unsigned long scanPeriod; 
    if (++scanCount > 255)
    { 
      Serial.print("Total surface scan time in microseconds: ");
      Serial.println((micros() - scanPeriod) / 256); 
      scanPeriod = micros(); 
      scanCount = 0;   
    }
  }
}

// displayCellTouchedFrame:
// For debug, displays an entire frame of raw Z values in the Arduino serial monitor. Values are collected during each full read of the touch surface.
void displayCellTouchedFrame()
{
  Serial.println();
  for (byte x = 0; x < NUMCOLS; ++x)
  {
    Serial.print(x);
    Serial.print("\t");
  }
  Serial.println();
  for (byte y = NUMROWS; y > 0; --y)
  {
    for (byte x = 0; x < NUMCOLS; ++x)
    {
      Serial.print(cell(x, y-1).touched);
      Serial.print("\t");
    }
    Serial.println();
  }
}

void modeLoopManufacturingTest() {

  TouchState previousTouch = cell().touched;

  byte z = readZ();
  boolean activeTouch = (z != Z_VAL_NONE && z != Z_VAL_FEATHER);

  // highlight the touches
  if (activeTouch) {
    cell().touched = touchedCell;

    byte color = COLOR_WHITE;
    switch (sensorRow)
    {
      case 0:
      case 1:
        color = COLOR_RED;
        break;
      case 2:
      case 3:
        color = COLOR_GREEN;
        break;
      case 4:
      case 5:
        color = COLOR_BLUE;
        break;
      case 6:
      case 7:
        color = COLOR_CYAN;
        break;
    }

    for (int row = sensorRow % 2; row < NUMROWS; row += 2) {
      setLed(sensorCol, row, color, 3);
    }
  }
  else if (!activeTouch && previousTouch != untouchedCell) {
    cell().touched = untouchedCell;

    for (int row = sensorRow % 2; row < NUMROWS; row += 2) {
      setLed(sensorCol, row, COLOR_BLACK, 0);
    }
  }

  unsigned long now = micros();
  
  // send out MIDI activity
  midiOutQueue.push((byte)MIDIActiveSensing);
  handlePendingMidi(now);
  handleMidiInput();

  checkRefreshLedColumn(now);
  nextSensorCell();
}

