#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>


#define AP_SSID "admin"
#define AP_PASS "admin"

WebServer server(80);

void handleCapture() {
  auto img = esp32cam::capture();
  if (img == nullptr) {
    server.send(500, "", "");
    return;
  }

  server.setContentLength(img->size());
  server.send(200, "image/jpeg");
  WifiClient client = server.client();
  img->writeTo(client);
}

void setup() {
  auto res = esp32cam::Resolution::find(1024, 768);
  esp32cam::Config cfg;
  cfg.setPins(esp32cam::pins::AiThinker);
  cfg.setResolution(res);
  cfg.setJpeg(80);

  esp32cam::Camera.begin(cfg);
  WiFi.softAP(AP_SSID, AP_PASS);
  server.on("/capture.jpg", handleCapture);
  server.begin();
}

void loop() {
  server.handleClient();
}