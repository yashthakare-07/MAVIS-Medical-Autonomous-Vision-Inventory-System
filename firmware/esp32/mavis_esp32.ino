#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
const char* API_TOKEN = "YOUR_API_TOKEN";
const char* SERVER_ARRIVAL_URL =
    "http://YOUR_FLASK_SERVER_IP:5000/api/robot/arrival";

WebServer server(80);

const int MOTOR_LEFT_IN1 = 26;
const int MOTOR_LEFT_IN2 = 27;
const int MOTOR_LEFT_EN = 25;

const int MOTOR_RIGHT_IN1 = 14;
const int MOTOR_RIGHT_IN2 = 12;
const int MOTOR_RIGHT_EN = 13;

const int IR_SENSOR_COUNT = 5;

const int IR_PINS[IR_SENSOR_COUNT] = {
    32,
    33,
    34,
    35,
    39
};

const bool LINE_ACTIVE_LOW = true;

const int PWM_FREQUENCY = 20000;
const int PWM_RESOLUTION = 8;

const int LEFT_PWM_CHANNEL = 0;
const int RIGHT_PWM_CHANNEL = 1;

const int BASE_SPEED = 150;
const int MAX_SPEED = 230;
const int SEARCH_SPEED = 120;

const float KP = 45.0f;

const unsigned long NODE_CONFIRM_TIME_MS = 300;

unsigned long nodeDetectionStart = 0;

enum RobotState {
    ROBOT_IDLE,
    ROBOT_NAVIGATING,
    ROBOT_ARRIVED,
    ROBOT_ERROR
};

RobotState robotState = ROBOT_IDLE;

String activeNode = "";
bool dispatchActive = false;
float previousError = 0.0f;

String getRobotStateString()
{
    switch (robotState) {
        case ROBOT_IDLE:
            return "IDLE";

        case ROBOT_NAVIGATING:
            return "NAVIGATING";

        case ROBOT_ARRIVED:
            return "ARRIVED";

        case ROBOT_ERROR:
            return "ERROR";

        default:
            return "UNKNOWN";
    }
}

void setMotor(
    int in1,
    int in2,
    int pwmChannel,
    int speed
)
{
    speed = constrain(speed, -255, 255);

    if (speed > 0) {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, speed);
    }
    else if (speed < 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        ledcWrite(pwmChannel, abs(speed));
    }
    else {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        ledcWrite(pwmChannel, 0);
    }
}

void driveMotors(
    int leftSpeed,
    int rightSpeed
)
{
    leftSpeed = constrain(
        leftSpeed,
        -MAX_SPEED,
        MAX_SPEED
    );

    rightSpeed = constrain(
        rightSpeed,
        -MAX_SPEED,
        MAX_SPEED
    );

    setMotor(
        MOTOR_LEFT_IN1,
        MOTOR_LEFT_IN2,
        LEFT_PWM_CHANNEL,
        leftSpeed
    );

    setMotor(
        MOTOR_RIGHT_IN1,
        MOTOR_RIGHT_IN2,
        RIGHT_PWM_CHANNEL,
        rightSpeed
    );
}

void stopRobot()
{
    driveMotors(0, 0);
}

bool readLineSensor(int index)
{
    int rawValue = digitalRead(IR_PINS[index]);

    if (LINE_ACTIVE_LOW) {
        return rawValue == LOW;
    }

    return rawValue == HIGH;
}

bool readIRSensors(bool sensorState[IR_SENSOR_COUNT])
{
    bool anyLineDetected = false;

    for (int i = 0; i < IR_SENSOR_COUNT; ++i) {
        sensorState[i] = readLineSensor(i);

        if (sensorState[i]) {
            anyLineDetected = true;
        }
    }

    return anyLineDetected;
}

float calculateLineError()
{
    bool sensors[IR_SENSOR_COUNT];

    bool lineDetected = readIRSensors(sensors);

    if (!lineDetected) {
        return previousError;
    }

    const float weights[IR_SENSOR_COUNT] = {
        -2.0f,
        -1.0f,
         0.0f,
         1.0f,
         2.0f
    };

    float weightedSum = 0.0f;
    float activeCount = 0.0f;

    for (int i = 0; i < IR_SENSOR_COUNT; ++i) {
        if (sensors[i]) {
            weightedSum += weights[i];
            activeCount += 1.0f;
        }
    }

    if (activeCount <= 0.0f) {
        return previousError;
    }

    float error = weightedSum / activeCount;

    previousError = error;

    return error;
}

bool isNodeMarkerDetected()
{
    bool sensors[IR_SENSOR_COUNT];

    readIRSensors(sensors);

    for (int i = 0; i < IR_SENSOR_COUNT; ++i) {
        if (!sensors[i]) {
            return false;
        }
    }

    return true;
}

bool confirmedNodeMarker()
{
    if (!isNodeMarkerDetected()) {
        nodeDetectionStart = 0;
        return false;
    }

    if (nodeDetectionStart == 0) {
        nodeDetectionStart = millis();
    }

    if (
        millis() - nodeDetectionStart
        >= NODE_CONFIRM_TIME_MS
    ) {
        return true;
    }

    return false;
}

void executeLineFollowing()
{
    bool sensors[IR_SENSOR_COUNT];

    bool lineDetected = readIRSensors(sensors);

    if (!lineDetected) {
        if (previousError < 0) {
            driveMotors(
                -SEARCH_SPEED,
                SEARCH_SPEED
            );
        }
        else {
            driveMotors(
                SEARCH_SPEED,
                -SEARCH_SPEED
            );
        }

        return;
    }

    float error = calculateLineError();

    int correction = static_cast<int>(
        KP * error
    );

    int leftSpeed =
        BASE_SPEED + correction;

    int rightSpeed =
        BASE_SPEED - correction;

    leftSpeed = constrain(
        leftSpeed,
        -MAX_SPEED,
        MAX_SPEED
    );

    rightSpeed = constrain(
        rightSpeed,
        -MAX_SPEED,
        MAX_SPEED
    );

    driveMotors(
        leftSpeed,
        rightSpeed
    );
}

bool notifyServerArrival()
{
    if (activeNode.length() == 0) {
        return false;
    }

    HTTPClient http;

    if (!http.begin(SERVER_ARRIVAL_URL)) {
        Serial.println(
            "[ERROR] Could not initialize HTTP client."
        );

        return false;
    }

    http.addHeader(
        "Content-Type",
        "application/json"
    );

    String payload =
        "{\"node_id\":\"" +
        activeNode +
        "\",\"status\":\"ARRIVED\"}";

    int responseCode =
        http.POST(payload);

    if (responseCode > 0) {
        Serial.print(
            "[SERVER] Arrival response: "
        );

        Serial.println(responseCode);

        http.end();

        return responseCode >= 200 &&
               responseCode < 300;
    }

    Serial.print(
        "[ERROR] Arrival notification failed: "
    );

    Serial.println(
        http.errorToString(responseCode)
    );

    http.end();

    return false;
}

bool isValidNodeId(const String& nodeId)
{
    if (nodeId.length() == 0) {
        return false;
    }

    if (nodeId.length() > 64) {
        return false;
    }

    return true;
}

void handleCommand()
{
    if (!server.hasArg("token")) {
        server.send(
            401,
            "text/plain",
            "Missing authentication token"
        );

        return;
    }

    String receivedToken =
        server.arg("token");

    if (receivedToken != API_TOKEN) {
        server.send(
            403,
            "text/plain",
            "Invalid authentication token"
        );

        return;
    }

    String body =
        server.arg("plain");

    body.trim();

    Serial.print(
        "[HTTP] Command received: "
    );

    Serial.println(body);

    const String commandPrefix = "GOTO,";

    if (!body.startsWith(commandPrefix)) {
        server.send(
            400,
            "text/plain",
            "Invalid command format. Expected GOTO,<node_id>"
        );

        return;
    }

    String nodeId =
        body.substring(
            commandPrefix.length()
        );

    nodeId.trim();

    if (!isValidNodeId(nodeId)) {
        server.send(
            400,
            "text/plain",
            "Invalid node ID"
        );

        return;
    }

    activeNode = nodeId;

    dispatchActive = true;

    robotState = ROBOT_NAVIGATING;

    nodeDetectionStart = 0;

    Serial.print(
        "[DISPATCH] Robot navigating to node: "
    );

    Serial.println(activeNode);

    server.send(
        200,
        "text/plain",
        "COMMAND_ACCEPTED"
    );
}

void handleStatus()
{
    String response = "{";

    response += "\"state\":\"";
    response += getRobotStateString();
    response += "\",";

    response += "\"node\":\"";
    response += activeNode;
    response += "\",";

    response += "\"dispatch_active\":";
    response += dispatchActive ? "true" : "false";

    response += "}";

    server.send(
        200,
        "application/json",
        response
    );
}

void handleHealth()
{
    server.send(
        200,
        "application/json",
        "{\"service\":\"mavis-esp32\",\"status\":\"ok\"}"
    );
}

void handleNotFound()
{
    server.send(
        404,
        "text/plain",
        "Endpoint not found"
    );
}

void connectToWiFi()
{
    Serial.println();
    Serial.println(
        "[WIFI] Connecting..."
    );

    WiFi.mode(WIFI_STA);

    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );

    const unsigned long WIFI_TIMEOUT = 15000;

    unsigned long startTime =
        millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startTime < WIFI_TIMEOUT
    ) {
        delay(500);

        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(
            "[WIFI] Connected."
        );

        Serial.print(
            "[WIFI] ESP32 IP: "
        );

        Serial.println(
            WiFi.localIP()
        );
    }
    else {
        Serial.println(
            "[WIFI] Connection failed."
        );

        robotState = ROBOT_ERROR;
    }
}

void initializeMotors()
{
    pinMode(
        MOTOR_LEFT_IN1,
        OUTPUT
    );

    pinMode(
        MOTOR_LEFT_IN2,
        OUTPUT
    );

    pinMode(
        MOTOR_RIGHT_IN1,
        OUTPUT
    );

    pinMode(
        MOTOR_RIGHT_IN2,
        OUTPUT
    );

    ledcSetup(
        LEFT_PWM_CHANNEL,
        PWM_FREQUENCY,
        PWM_RESOLUTION
    );

    ledcSetup(
        RIGHT_PWM_CHANNEL,
        PWM_FREQUENCY,
        PWM_RESOLUTION
    );

    ledcAttachPin(
        MOTOR_LEFT_EN,
        LEFT_PWM_CHANNEL
    );

    ledcAttachPin(
        MOTOR_RIGHT_EN,
        RIGHT_PWM_CHANNEL
    );

    stopRobot();
}

void initializeIRSensors()
{
    for (int i = 0; i < IR_SENSOR_COUNT; ++i) {
        pinMode(
            IR_PINS[i],
            INPUT
        );
    }
}

void initializeHTTPServer()
{
    server.on(
        "/command",
        HTTP_POST,
        handleCommand
    );

    server.on(
        "/status",
        HTTP_GET,
        handleStatus
    );

    server.on(
        "/health",
        HTTP_GET,
        handleHealth
    );

    server.onNotFound(
        handleNotFound
    );

    server.begin();

    Serial.println(
        "[HTTP] Server started."
    );
}

void handleRobotArrival()
{
    stopRobot();

    robotState = ROBOT_ARRIVED;

    dispatchActive = false;

    Serial.print(
        "[NAVIGATION] Destination reached: "
    );

    Serial.println(activeNode);

    bool notificationSuccess =
        notifyServerArrival();

    if (notificationSuccess) {
        Serial.println(
            "[SERVER] Arrival successfully reported."
        );
    }
    else {
        Serial.println(
            "[SERVER] Arrival notification failed."
        );
    }
}

void navigationTask()
{
    if (!dispatchActive) {
        stopRobot();
        return;
    }

    if (robotState != ROBOT_NAVIGATING) {
        stopRobot();
        return;
    }

    if (confirmedNodeMarker()) {
        handleRobotArrival();
        return;
    }

    executeLineFollowing();
}

void setup()
{
    Serial.begin(115200);

    delay(500);

    Serial.println();
    Serial.println(
        "=============================================="
    );

    Serial.println(
        "M.A.V.I.S. ESP32 ROBOT CONTROLLER"
    );

    Serial.println(
        "Medical Autonomous Vision & Inventory System"
    );

    Serial.println(
        "=============================================="
    );

    initializeMotors();

    initializeIRSensors();

    connectToWiFi();

    if (WiFi.status() == WL_CONNECTED) {
        initializeHTTPServer();

        robotState = ROBOT_IDLE;
    }
    else {
        robotState = ROBOT_ERROR;

        stopRobot();
    }

    Serial.println(
        "[SYSTEM] Initialization complete."
    );
}

void loop()
{
    server.handleClient();

    if (
        WiFi.status() != WL_CONNECTED &&
        robotState != ROBOT_NAVIGATING
    ) {
        stopRobot();

        static unsigned long lastReconnectAttempt = 0;

        if (
            millis() - lastReconnectAttempt > 5000
        ) {
            lastReconnectAttempt = millis();

            Serial.println(
                "[WIFI] Attempting reconnection..."
            );

            WiFi.disconnect();

            WiFi.begin(
                WIFI_SSID,
                WIFI_PASSWORD
            );
        }
    }

    navigationTask();

    delay(5);
}
