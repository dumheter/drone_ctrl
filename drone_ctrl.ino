/**
 * Drone ctrl
 */

// ============================================================ //
// Headers
// ============================================================ //

// Thirdparty
#include <ArduinoLog.h>

// Core
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <stdint.h>


// ============================================================ //
// Globals
// ============================================================ //

constexpr char NET_SSID[] = "WiFi-720P-C72A07";
constexpr char NET_PASSWD[] = "";

constexpr uint8_t DRONE_IP[] = {192,168,0,1};

// ============================================================ //
// Control
// ============================================================ //

WiFiUDP control;
constexpr uint16_t CONTROL_PORT = 50000;

constexpr uint8_t control_state_size = 8;
uint8_t control_state[control_state_size];

constexpr uint8_t CONTROL_INIT[] = {0x66, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x99};
constexpr uint8_t CONTROL_HOURGLASS[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99};
constexpr uint8_t CONTROL_UNLOCK[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x20, 0x20, 0x99};

void control_init () {
  control.begin(CONTROL_PORT); //we can listen on same port
  control_state_set(CONTROL_INIT);
}

void control_state_set (const uint8_t* state) {
  for (auto i=0; i<control_state_size; i++) {
    control_state[i] = state[i];
  }
}

void control_send_state () {
  auto res = control.beginPacket(DRONE_IP, CONTROL_PORT);
  if (!res) {
    Log.error("begin packet UDP 50000\n");
    return;
  }

  res = control.write(control_state, control_state_size);
  if (res != control_state_size) {
    Log.error("writing UDP 50000:%d\n", res);
    return;
  }

  res = control.endPacket();
  if (!res) {
    Log.error("sending UDP:50000\n");
    return;
  }
}

void control_update () {
  static unsigned long control_timer = millis();
  static constexpr unsigned long control_interval_ms = 50;

  if (millis() - control_timer > control_interval_ms) {
    control_timer = millis();
    control_send_state();
    Log.notice("state: ");
    for (auto i=0; i<8; i++) {
      if (control_state[i] < 16) Serial.print('0');
      Serial.print(control_state[i], HEX);
    }
  }
}

void control_handle_serial_input () {

  if (Serial.available() > 8 && Serial.read() == 'c') {
    for (auto i=0; i<8; i++) {
      control_state[i] = static_cast<uint8_t>(Serial.read());
    }

    //print_state();
    //Serial.println();
  }
}

void print_state () {
  Log.notice("state: ");
  for (auto i=0; i<8; i++) {
    if (control_state[i] < 16) Serial.print('0');
    Serial.print(control_state[i], HEX);
  }
}

// ============================================================ //
// Send Once Packet
// ============================================================ //

WiFiUDP send_once;

void send_once_init () {
  constexpr uint16_t SEND_ONCE_PORT = 40000;

  // listen to port 40 000
  //auto res = send_once.begin(SEND_ONCE_PORT); //we can listen on same port
  //if (!res) Log.error("send_once_init() failed to begin\n");

  res = send_once.beginPacket(DRONE_IP, SEND_ONCE_PORT);
  if (!res) {
    Log.error("send_once_init() failed to begin packet\n");
    return;
  }

  constexpr uint8_t data_size = 7;
  const uint8_t data[] = {0x63, 0x63, 0x02, 0x00, 0x00, 0x00, 0x00};
  res = send_once.write(data, data_size);
  if (res != data_size) {
    Log.error("send_once_init() failed to write into packet\n");
    return;
  }

  res = send_once.endPacket();
  if (!res) {
    Log.error("send_once_init() failed to end packet\n");
    return;
  }
}

void send_once_read () {
  if (send_once.available()) {
    constexpr uint16_t buffer_size = 256;
    uint8_t buffer[buffer_size];
    auto read = send_once.read(buffer, buffer_size);

    buffer[read] = 0; //null terminate so we can print

    Log.notice("send_once_read() read %d with msg %s\n", read, buffer);
  }
}

// ============================================================ //
// Camera Feed
// ============================================================ //

WiFiClient camera_feed;

/**
 * Call contineously in the loop to keep the tcp buffer open.
 */
void camera_feed_clear () {
  if (camera_feed.available()) {
    constexpr int NO_DATA_AVAILABLE = -1;
    while (camera_feed.read() != NO_DATA_AVAILABLE);
  }
}

void camera_feed_init () {
  constexpr uint16_t CAMERA_FEED_PORT = 7060;

  auto res = camera_feed.connect(DRONE_IP, CAMERA_FEED_PORT);
  Log.notice("Camera feed connect %s.\n", res ? "ok" : "fail");
}

// ============================================================ //
// Arduino Functions
// ============================================================ //

void setup () {
  //Serial.begin(74880);
  Serial.begin(115200);
  Serial.println();
  Serial.flush();
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.verbose("Setup\n");
  Serial.setDebugOutput(true);

  Log.verbose("Setting to WiFi station mode %s.\n", WiFi.mode(WIFI_STA) ? "ok" : "fail");

  Log.notice("Waiting for controlller interface\n");
  //while(Serial.read() != 'r');

  Log.verbose("Running, connecting to wifi..\n");
  WiFi.begin(NET_SSID, NET_PASSWD);
  WiFi.printDiag(Serial);

  bool not_connected = true;
  while(not_connected) {
    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      Log.verbose("idle\n");
      break;

    case WL_NO_SSID_AVAIL:
      Log.verbose("no ssid avail\n");
      break;

    case WL_CONNECTED:
      not_connected = false;
      break;

    case WL_CONNECT_FAILED:
      Log.fatal("connection failed - password wrong\n");
      break;

    case WL_DISCONNECTED:
      Log.verbose("disconnected - not configured in station mode\n");
      break;

    default:
      Log.verbose("unexpected value\n");
    }
    delay(500);
  }

  Log.notice("Connected to %s with ip %s\n", NET_SSID, WiFi.localIP().toString().c_str());
  WiFi.printDiag(Serial);

  //connect to camera feed
  camera_feed_init();

  //send_once_init();

  //connect to control channel
  control_init();
}

// ============================================================ //
void loop () {

  camera_feed_clear();

  //send_once_read();

  control_handle_serial_input();
  control_update();
}
