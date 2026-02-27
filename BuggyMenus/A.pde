ArrayList<String> lines = new ArrayList<String>();
int maxLines = 10;

final int mode_TRACK = 0;
final int mode_MANUAL = 1;
final int mode_FOLLOW = 2;

int mode = mode_MANUAL;

Client sam;//arduino called sam

PFont font;

final int PORT = 80;

final int START = 0;
final int MENU = 1;

//final String SAM = "192.168.0.84";
final String SAM = "192.168.4.1";
//final String SAM = "10.255.199.10";

int state = START;

ControlP5 panel;
ArrayList<Controller<?>> menuButtons = new ArrayList<Controller<?>>();
Button downloadButton, manualModeButton, trackModeButton, followModeButton;
Slider slider, leftThreshold, rightThreshold, maxDistance;
Textlabel speedlabel, modelabel, leftIRlabel, rightIRlabel, uslabel;
Toggle stopToggle;
Textfield textBox;
