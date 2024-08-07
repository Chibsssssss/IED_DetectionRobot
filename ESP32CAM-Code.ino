#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "esp_http_client.h"

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Define the Flask server URL
const char* serverUrl = "http://your-flask-server-ip:5000/upload";

// Initialize the web server
httpd_handle_t server = NULL;

// Function to capture and send photo to Flask server
void captureAndSendPhoto() {
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  esp_http_client_config_t config = {
    .url = serverUrl,
    .method = HTTP_METHOD_POST,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_http_client_set_header(client, "Content-Type", "image/jpeg");
  esp_http_client_set_post_field(client, (const char *)fb->buf, fb->len);

  esp_err_t res = esp_http_client_perform(client);
  if (res == ESP_OK) {
    Serial.println("Image sent successfully");
  } else {
    Serial.printf("Image send failed: %s\n", esp_err_to_name(res));
  }

  esp_http_client_cleanup(client);
  esp_camera_fb_return(fb);
}

// HTTP GET handler for capturing photo
esp_err_t capture_handler(httpd_req_t *req) {
  captureAndSendPhoto();
  const char resp[] = "Photo captured and sent to server";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

// Start the camera server
void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_uri_t capture_uri = {
    .uri = "/capture",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &capture_uri);
  }
}

void setup() {
  Serial.begin(115200);
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sccb_sda = 26;
  config.pin_sccb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected at IP: ");
  Serial.println(WiFi.localIP());

  startCameraServer();
}

void loop() {
  // No need to handle server in loop, handled by HTTP server task
}
