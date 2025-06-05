// Libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_camera.h"
#include <WiFiClientSecure.h>

// WIFI connection
#define WIFI_SSID "BTB-PJARMX"
#define WIFI_PASSWORD "Pablosmellz64"

// MQTT connection
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT (1883)
#define MQTT_PUBLIC_TOPIC "uok/iot/eeo8/distance"
#define MQTT_SUBSCRIBE_TOPIC "uok/iot/eeo8/stream"

// Ultrasonic sensor pins
#define TRIG_PIN D1 
#define ECHO_PIN D0

// Camera pins
#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"

// Server certificate and private key
const char* server_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDmTCCAoGgAwIBAgIUG83T2ks1nBfZK4TM/ozzxAvUaUAwDQYJKoZIhvcNAQEL
BQAwXDELMAkGA1UEBhMCdWsxEzARBgNVBAgMCmNhbnRlcmJ1cnkxGzAZBgNVBAoM
EnVuaXZlcnNpdHkgb2Yga2VudDEbMBkGA1UEAwwSY2FyIHBhcmtpbmcgc2Vuc29y
MB4XDTI0MTIwOTIzMjkzOVoXDTI1MTIwOTIzMjkzOVowXDELMAkGA1UEBhMCdWsx
EzARBgNVBAgMCmNhbnRlcmJ1cnkxGzAZBgNVBAoMEnVuaXZlcnNpdHkgb2Yga2Vu
dDEbMBkGA1UEAwwSY2FyIHBhcmtpbmcgc2Vuc29yMIIBIjANBgkqhkiG9w0BAQEF
AAOCAQ8AMIIBCgKCAQEAooyviHkk7uHqAqfm83hWC6xDQ9zx85qQLLDwfGr3a2vO
Zemg0yZMHD9D2BPjQG9Cqw4N7AtHj4zZ0yvVuSeNpJEzU3+cmxIKqY2E7hBhaN09
Xe9CeVFRUJvGgzjRZIMmO2BeBDpeidQv/jlaqk/j6nuaOW9w20jCNXpXfY8/K6Wd
OglRMPJvGgsHXUZz7zbBsjFSIWnCku4ltPU7PLJUAlx4zb5dSsbBTThnKZ9ey6/J
B/ChodmLdh+iZLavx0Wn18l5zVPbciLHoB4DQaxhhkOyIj9Ky2Hu3M2dTxSjkfq8
6aOaK9su85CT7Jygt6zKXRxBchp/eLzHry7n3HfzvQIDAQABo1MwUTAdBgNVHQ4E
FgQUpXpqCma3g1B2Eq3O/CRYbMPN2g0wHwYDVR0jBBgwFoAUpXpqCma3g1B2Eq3O
/CRYbMPN2g0wDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAAG6I
LMrayEXApFu+IZses8s1EiKiBDUxRCpxhSEogiQV647iKgQvuvTczfqgXDUyjFoB
l5TQRU/hXzJLs+fRz6SkY0fpPvr46TdCCsPKk4NZXMSiBb2BheeMCMEh+RmZwB50
izGx2osXDX3DNbvawbxlvKc1uKwU8PZ+BrjjsBLdSGkMdRTEQ0KovDZ0Z0/er30X
N0dW/c8r8rM36ajFKYEs1Rg23jDDs+xrW1CQ0YMAv+88hjn7W34S2cNYRQAMrZ20
jTU9jP25imR0ui+Oov7aDEHP0JBtSJISoLe9vAbTuaqMAB2gkQ/b44P32bqgD15+
tbIJ4PqzIkerSabBSw==
-----END CERTIFICATE-----
)EOF";

const char* server_key = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQCijK+IeSTu4eoC
p+bzeFYLrEND3PHzmpAssPB8avdra85l6aDTJkwcP0PYE+NAb0KrDg3sC0ePjNnT
K9W5J42kkTNTf5ybEgqpjYTuEGFo3T1d70J5UVFQm8aDONFkgyY7YF4EOl6J1C/+
OVqqT+Pqe5o5b3DbSMI1eld9jz8rpZ06CVEw8m8aCwddRnPvNsGyMVIhacKS7iW0
9Ts8slQCXHjNvl1KxsFNOGcpn17Lr8kH8KGh2Yt2H6Jktq/HRafXyXnNU9tyIseg
HgNBrGGGQ7IiP0rLYe7czZ1PFKOR+rzpo5or2y7zkJPsnKC3rMpdHEFyGn94vMev
Lufcd/O9AgMBAAECggEAIcPilJFBnEOeVgXlog6NpSgX4ulq8wTYzQfGNLoNgiln
OuFAcIwEhFuZ7rEW5CQYb6rgNneTWlzSRJzW7Pqr4BLvW/lmoQmq4FSO19rcOSm3
3SLdppC/OOTa5Bgx2tSpZO0cKnmreKr0ezUTwcwNloAtZZ/iDBL9kJlEwka2wwxz
FhJJGu2kEvjC3gtIUS4XQfrRb2nA5ztL1xY+dl/dl1yl2Tz4ZR5ErVoa24WO5pYp
az7ZRCgG8M0wo7623FFM49psGlJqlBF8IjKpJQzQdWDsBu3ppY3JeJwzBc881a4X
XLUwEwfsQgwIzetQLkP3SmNdJvJccNs/zNs2A0MIcQKBgQDl0FO9zSfidjD4e5zi
3l5e4VaRPE9dE4ylfdCMms/6Xmsw/G3LKKiC0D+YRYIK0HkxcmiGlTnOg0TkhRrn
A0CGXDta19H9mLTb/wV48zmKYfnDaiaqcdMu9Lmm1BrsNN7zvxIBQcOS0ctZfUeo
CG8Vwhx4DM8D4k5VKaVSqSuUCQKBgQC1EkJ6LYjYWw+u57b3vbXvyQCGUD2CNBeD
Apdtsmq5QkP6+UThkDew4Dpi3Tli6N4wu485G9mzxmzuiqtyW1SRamsFwYLwus3x
k9xxYGrbidjB2BgDhv3NINi+aduuo8m64Ny1QjY80e3LC499gt5Cppbm3dCh+UG+
yGN1WtgXFQKBgEuRW1f4E/tlfejXTNlU4Zc1Za/QMxCaEw9/9Ymmhmj/jyPGfZvm
PLL3I1JA2OgRcSiPREus2htMEuVJ13TLHvaPcX0HpqdLBX2pt/TzlaIpYWWejCRT
1WuFUV7INCojoIRp+dOr+lFW5xUAGwXn8A8piw72t0PBsJxUW2Z49jlxAoGAMJw4
UIiM5lh7+16llhjOEROLGoPMH3pQnZFCieaOFDIgAe6NeWi4KrnnssblMxKFI9wW
dSRu635nnBqKMGcoaumA1ci/mFTuGN6zHZTe2xVEpbWXrMq+tCUPSv8sRNQfwhW/
6sFdwjIxfoMaAH3zoT0L4erGqdU6uzcczFKZFIkCgYAdW4U/TXtokjwUGm93fWC3
+IxpZI+BmsAXLkU2ETbfxmtqZ3G4WvBRzRwzvUBzWtp+2ZKHfQkYdY6DorELui4S
3Nte5xx4JEtgjqDjHIb0PdgWUHmRoBLL0V0gm2Q5xCQcswDSDLXKF3NVXYoOcK0d
EoawgjQ6Lh9m8S+4JY8Bhw==
-----END PRIVATE KEY-----
)EOF";

// Initialise WiFi server on port 80
WiFiClientSecure secureClient;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
WiFiServer server(443);

// Calculate the distance with ultrasonic sensor
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  if (duration == 0) {
    Serial.println("No echo received");
  }

  int distance = duration * 0.034 / 2;
  return distance;
}

// Initialise camera
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 20;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera initialisation failed", err);
  } else {
    Serial.println("Camera initialised successfully");
  }
}

void setup() {
  // Initialise Serial communication
  Serial.begin(115200);

  // Initialise ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to WiFi
  Serial.print("Connecting to " WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialise MQTT
  client.setServer(MQTT_BROKER, MQTT_PORT);

  while (!client.connected()) {
    if (client.connect(("ESP32-" + String(random(0xffff), HEX)).c_str())) { 
      Serial.println("MQTT connected.");
      client.subscribe(MQTT_SUBSCRIBE_TOPIC);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_SUBSCRIBE_TOPIC);
    } else {
      Serial.printf("Failed, rc=%d, try again in 5 seconds", client.state());
      delay(5000); 
    }
  }

 // Load certificates
  secureClient.setCACert(server_cert);
  secureClient.setPrivateKey(server_key);

  // Start camera
  startCamera();

  // Start server
  server.begin();
  Serial.println("Secure server started on port 443.");
}

void loop() {
  client.loop();

  WiFiClient webClient = server.available();
  if (!webClient) return;

  Serial.println("New user connected");

  String request = webClient.readStringUntil('\r');
  webClient.flush();

  int distance = getDistance();

  // MQTT distance data
  char msg[50];
  snprintf(msg, sizeof(msg), "Distance: %d cm", distance);
  client.publish(MQTT_PUBLIC_TOPIC, msg);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Handle requests
  if (request.indexOf("GET / ") >= 0) {
    // main page
    webClient.println("HTTP/1.1 200 OK");
    webClient.println("Content-Type: text/html");
    webClient.println();

    // HTML and CSS styling
    webClient.println("<html lang=\"en\">");
    webClient.println("<head>");
    webClient.println("  <meta charset=\"UTF-8\">");
    webClient.println("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    webClient.println("  <title>My Car Parking Sensor</title>");
    webClient.println("  <style>");
    webClient.println("    body { font-family: Arial, sans-serif; text-align: center; padding: 20px; background-color: #e0f7fa; margin: 0; }");
    webClient.println("    h1 { font-size: 35px; margin-bottom: 20px; }");
    webClient.println("    p { font-size: 25px; margin-bottom: 10px; }");
    webClient.println("    button { padding: 10px 20px; background-color: #007bff; color: white; border: none; cursor: pointer; margin: 10px; border-radius: 5px; }");
    webClient.println("    button:hover { background-color: #0056b3; }");
    webClient.println("    #distanceWarning { color: white; font-size: 2.5em; background-color: red; margin-top: 20px; boarder-radius: 25px;}");
    webClient.println("  </style>");
    // Use distance data
    webClient.println("  <script>");
    webClient.println("    function fetchDistance() {");
    webClient.println("      fetch('/distance').then(response => response.text()).then(data => {");
    webClient.println("        const distance = parseInt(data, 10);");
    webClient.println("        document.getElementById('distanceValue').textContent = distance + ' cm';");
    webClient.println("        const warning = document.getElementById('distanceWarning');");
    webClient.println("        if (distance < 50) {");
    webClient.println("          warning.textContent = 'Warning: Object is getting too close!';");
    webClient.println("        } else {");
    webClient.println("          warning.textContent = '';");
    webClient.println("        }");
    webClient.println("        if (distance < 25) {");
    webClient.println("          window.location.href = '/stream';");
    webClient.println("        }");
    webClient.println("      });");
    webClient.println("    }");
    webClient.println("    setInterval(fetchDistance, 1000);");
    webClient.println("  </script>");
    webClient.println("</head>");
    webClient.println("<body>");
    webClient.println("  <h1>My Car Parking Sensor Web Server</h1>");
    webClient.println("  <p>Distance: <span id=\"distanceValue\">Loading...</span></p>");
    webClient.println("  <p id=\"distanceWarning\"></p>");
    webClient.println("  <button onclick=\"location.href='/distance'\">Get Distance</button>");
    webClient.println("  <button onclick=\"location.href='/stream'\">Start Video Streaming</button>");
    webClient.println("  <button onclick=\"location.href='/graph'\">View data Graph</button>");
    webClient.println("</body>");
    webClient.println("</html>");

   } else if (request.indexOf("/distance") != -1) {
    webClient.println("HTTP/1.1 200 OK");
    webClient.println("Content-Type: text/plain");
    webClient.println();
    webClient.print(distance);
  } else if (request.indexOf("/stream") != -1) {
    webClient.println("HTTP/1.1 200 OK");
    webClient.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
    webClient.println();

    while (webClient.connected()) {
      camera_fb_t* fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        break;
      }

      webClient.println("--frame");
      webClient.println("Content-Type: image/jpeg");
      webClient.println("Content-Length: " + String(fb->len));
      webClient.println();
      webClient.write(fb->buf, fb->len);
      webClient.println();
      esp_camera_fb_return(fb);

      delay(100);
    }
  } else if (request.indexOf("/graph") != -1) {
    webClient.println("HTTP/1.1 200 OK");
    webClient.println("Content-Type: text/html");
    webClient.println();

    // HTML and CSS for graph
    webClient.println("<html lang=\"en\">");
webClient.println("<head>");
webClient.println("  <meta charset=\"UTF-8\">");
webClient.println("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
webClient.println("  <title>Distance Graph</title>");
webClient.println("  <script src=\"https://cdn.jsdelivr.net/npm/chart.js\"></script>");
webClient.println("  <style>");
webClient.println("    body { font-family: Arial, sans-serif; text-align: center; padding: 10px; background-color: #f0f0f0; }");
webClient.println("    canvas { max-width: 100%; height: auto; margin: 10px auto; }");
webClient.println("    button { margin: 10px; padding: 10px 20px; background: #007bff; color: #fff; border: none; cursor: pointer; }");
webClient.println("    button:hover { background: #0056b3; }");
webClient.println("  </style>");
webClient.println("</head>");
webClient.println("<body>");
webClient.println("  <h1>Distance Data Graph</h1>");
webClient.println("  <button onclick=\"location.href='/'\">Go Back</button>");
webClient.println("  <canvas id=\"distanceGraph\"></canvas>");
webClient.println("  <script>");
webClient.println("    const ctx = document.getElementById('distanceGraph').getContext('2d');");
webClient.println("    const graphData = {");
webClient.println("      labels: [],");
webClient.println("      datasets: [{");
webClient.println("        label: 'Distance (cm)',");
webClient.println("        data: [],");
webClient.println("        borderColor: 'blue',");
webClient.println("        borderWidth: 1,");
webClient.println("        fill: false,");
webClient.println("        tension: 0.3");
webClient.println("      }]");
webClient.println("    };");
webClient.println("    const chart = new Chart(ctx, {");
webClient.println("      type: 'line',");
webClient.println("      data: graphData,");
webClient.println("      options: {");
webClient.println("        responsive: true,");
webClient.println("        scales: {");
webClient.println("          x: {");
webClient.println("            title: { display: true, text: 'Time' },");
webClient.println("            type: 'linear',");
webClient.println("            ticks: { callback: value => new Date(value).toLocaleTimeString() }");
webClient.println("          },");
webClient.println("          y: {");
webClient.println("            title: { display: true, text: 'Distance (cm)' },");
webClient.println("            beginAtZero: true");
webClient.println("          }");
webClient.println("        }");
webClient.println("      }");
webClient.println("    });");
webClient.println("    function updateGraph() {");
webClient.println("      fetch('/distance').then(response => response.text()).then(data => {");
webClient.println("        const distance = parseInt(data, 10);");
webClient.println("        const now = Date.now();");
webClient.println("        if (graphData.labels.length > 20) {");
webClient.println("          graphData.labels.shift();");
webClient.println("          graphData.datasets[0].data.shift();");
webClient.println("        }");
webClient.println("        graphData.labels.push(now);");
webClient.println("        graphData.datasets[0].data.push(distance);");
webClient.println("        chart.update();");
webClient.println("      });");
webClient.println("    }");
webClient.println("    setInterval(updateGraph, 1000);");
webClient.println("  </script>");
webClient.println("</body>");
webClient.println("</html>");
  }

  Serial.println("Request handled.");
}
