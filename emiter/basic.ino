//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"
#include <Servo.h>
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;
int pos = 90;
boolean increase = true;

// Create a servo object
Servo servo;
int minPos = 500;
int maxPos = 2000;
// User stub
void sendMessage(); // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage);
String serialSring = "";

void sendMessage() {
	String msg = "broadcast from emiter";
	msg += mesh.getNodeId();
	msg += "\r\n";
	mesh.sendBroadcast(msg);
	taskSendMessage.setInterval(random( TASK_SECOND * 1, TASK_SECOND * 5));
}

// Needed for painless library
void receivedCallback(uint32_t from, String &msg) {
	Serial.printf("startHere: emiter Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
	Serial.printf("--> emiter found new Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
	Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
	Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {

	Serial.begin(115200);

	// Open serial communications and wait for port to open:
	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}
	//mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE); // all types on
	mesh.setDebugMsgTypes(ERROR | STARTUP); // set before init() so that you can see startup messages

	mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
	mesh.onReceive(&receivedCallback);
	mesh.onNewConnection(&newConnectionCallback);
	mesh.onChangedConnections(&changedConnectionCallback);
	mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

	userScheduler.addTask(taskSendMessage);

	taskSendMessage.enable();


}

void loop() {

	userScheduler.execute(); // it will run mesh scheduler as well
	mesh.update();
}

