#include <M5Unified.h>
#include "wifi.h"

// Note: No need to include M5GFX.h separately or create 'M5GFX display;'
// We will use M5.Display which is already configured for the PaperS3.

// ---------- UI ELEMENT STRUCT ----------
struct Button {
  int x, y, w, h;
  const char* label;
  bool pressed = false;
  bool dirty = true; // New flag: only redraw if this is true
};

// ---------- UI ELEMENTS ----------
Button btnA = {20, 60, 200, 60, "Button A"};
Button btnB = {20, 140, 200, 60, "Button B"};

String statusText = "Touch a button";
bool textDirty = true; // Track if text needs redrawing

// ---------- DRAWING FUNCTIONS ----------
void drawButton(Button& b) {
  // Only draw if the state actually changed
  if (!b.dirty) return;

  uint16_t bg = b.pressed ? TFT_BLACK : TFT_WHITE;
  uint16_t fg = b.pressed ? TFT_WHITE : TFT_BLACK;

  // Draw ONLY the button area
  M5.Display.fillRoundRect(b.x, b.y, b.w, b.h, 12, bg);
  M5.Display.drawRoundRect(b.x, b.y, b.w, b.h, 12, TFT_BLACK);

  M5.Display.setTextColor(fg);
  M5.Display.setTextSize(2);

  int textWidth = M5.Display.textWidth(b.label);
  int textX = b.x + (b.w - textWidth) / 2;
  int textY = b.y + (b.h / 2) - 8;

  M5.Display.setCursor(textX, textY);
  M5.Display.print(b.label);

  b.dirty = false; // Reset dirty flag
}

void drawLabel() {
  if (!textDirty) return;

  // Clear ONLY the text area, not the whole screen
  M5.Display.fillRect(0, 0, 300, 40, TFT_WHITE);
  
  M5.Display.setTextColor(TFT_BLACK);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(10, 15);
  M5.Display.print(statusText);
  
  textDirty = false;
}

void drawUI() {
  // We do NOT call fillScreen here. 
  // We rely on the specific draw functions to clear their own areas.
  
  M5.Display.startWrite(); // Start transaction for smoother update
  drawLabel();
  drawButton(btnA);
  drawButton(btnB);
  M5.Display.endWrite(); // Commit the transaction
}

// ---------- TOUCH LOGIC ----------
bool isTouched(Button& b, int x, int y) {
  return (x > b.x && x < b.x + b.w &&
          y > b.y && y < b.y + b.h);
}

// ---------- SETUP ----------
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);

  // CRITICAL E-INK SETTINGS
  M5.Display.begin();
  
  // Set update mode to FAST (Low quality, but fast and no flash)
  // Options: epd_quality (slow/clean), epd_fast (fast/ghosting), epd_fastest
  M5.Display.setEpdMode(epd_mode_t::epd_fast); 

  M5.Display.fillScreen(TFT_WHITE); // Clear screen once at startup
  M5.Display.setTextSize(2);
  
  // Force initial draw
  drawUI();
}

// ---------- LOOP ----------
void loop() {
  M5.update();

  auto t = M5.Touch.getDetail();
  bool uiChanged = false;

  if (t.wasPressed()) {
    if (isTouched(btnA, t.x, t.y)) {
      btnA.pressed = true;
      btnA.dirty = true;
      statusText = "Button A pressed";
      textDirty = true;
      uiChanged = true;
    }
    if (isTouched(btnB, t.x, t.y)) {
      btnB.pressed = true;
      btnB.dirty = true;
      statusText = "Button B pressed";
      textDirty = true;
      uiChanged = true;
    }
  }

  if (t.wasReleased()) {
    if (btnA.pressed) {
      btnA.pressed = false;
      btnA.dirty = true;
      uiChanged = true;
    }
    if (btnB.pressed) {
      btnB.pressed = false;
      btnB.dirty = true;
      uiChanged = true;
    }
  }

  if (uiChanged) {
    drawUI();
  }

  delay(10);
}