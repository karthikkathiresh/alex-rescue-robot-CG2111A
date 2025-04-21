

#include <serialize.h>
#include <math.h>
#include <stdarg.h>

#include "packet.h"
#include "constants.h"

//#include "ColourSensorMega.h"

#define ALEX_LENGTH 25.3
#define ALEX_BREADTH 15.3
#define NUDGE_DELAY_FB 200
#define NUDGE_DELAY_LR 200
//#define PI 3.141592654

volatile TDirection dir;

/*
 * Alex's configuration constants
 */

// Number of ticks per revolution from the 
// wheel encoder.

#define COUNTS_PER_REV 4

// Wheel circumference in cm.
// We will use this to calculate forward/backward distance traveled 
// by taking revs * WHEEL_CIRC

#define WHEEL_CIRC 20.4

/*
 *    Alex's State Variables
 */

// Store the ticks from Alex's left and
// right encoders.
volatile unsigned long leftForwardTicks; 
volatile unsigned long rightForwardTicks;
volatile unsigned long leftReverseTicks;
volatile unsigned long rightReverseTicks;

// Left and right encoder ticks for turning
volatile unsigned long leftForwardTicksTurns;
volatile unsigned long rightForwardTicksTurns;
volatile unsigned long leftReverseTicksTurns;
volatile unsigned long rightReverseTicksTurns;

// Store the revolutions on Alex's left
// and right wheels
volatile unsigned long leftRevs;
volatile unsigned long rightRevs;

// Forward and backward distance traveled
volatile unsigned long forwardDist;
volatile unsigned long reverseDist;

//Variables to keep track of whether we've moved a commanded distance

unsigned long deltaDist;
unsigned long newDist;

unsigned long deltaTicks;
unsigned long targetTicks;

float alexDiagonal = 0.0;
float alexCirc = 0.0;
/*
 * 
 * Alex Communication Routines.
 * 
 */

 unsigned long computeDeltaTicks(float ang) {
  unsigned long ticks = (unsigned long) ((ang * alexCirc * COUNTS_PER_REV) / (360.0 * WHEEL_CIRC));
  return ticks;
 }

 void left(float ang, float speed) {
  if(ang == 0) 
    deltaTicks=99999999; 
  else 
    deltaTicks=computeDeltaTicks(ang); 
    
  targetTicks = leftReverseTicksTurns + deltaTicks;   
  ccw(ang, speed);
  rotate(ang);
  stop();
 }

 void right(float ang, float speed) {
  if(ang == 0) 
    deltaTicks=99999999; 
  else 
    deltaTicks=computeDeltaTicks(ang); 
    
  targetTicks = rightReverseTicksTurns + deltaTicks;   
  cw(ang, speed);
  rotate(ang);
  stop();
 }
 
 
TResult readPacket(TPacket *packet)
{
    // Reads in data from the serial port and
    // deserializes it.Returns deserialized
    // data in "packet".
    
    char buffer[PACKET_SIZE];
    int len;

    len = readSerial(buffer);

    if(len == 0)
      return PACKET_INCOMPLETE;
    else
      return deserialize(buffer, len, packet);
    
}

void sendStatus()
{
  // Implement code to send back a packet containing key
  // information like leftTicks, rightTicks, leftRevs, rightRevs
  // forwardDist and reverseDist
  // Use the params array to store this information, and set the
  // packetType and command files accordingly, then use sendRespons
  // to send out the packet. See sendMessage on how to use sendResponse.
  TPacket statusPacket;
  statusPacket.packetType=PACKET_TYPE_RESPONSE;
  statusPacket.command = RESP_STATUS;
  statusPacket.params[0] = leftForwardTicks;
  statusPacket.params[1] = rightForwardTicks;
  statusPacket.params[2] = leftReverseTicks;
  statusPacket.params[3] = rightReverseTicks; 
  statusPacket.params[4] = leftForwardTicksTurns; 
  statusPacket.params[5] = rightForwardTicksTurns; 
  statusPacket.params[6] = leftReverseTicksTurns; 
  statusPacket.params[7] = rightReverseTicksTurns; 
  statusPacket.params[8] = forwardDist; 
  statusPacket.params[9] = reverseDist;
//  statusPacket.params[10] = colourLoop();
  sendResponse(&statusPacket);
}

void sendMessage(const char *message)
{
  // Sends text messages back to the Pi. Useful
  // for debugging.
  
  TPacket messagePacket;
  messagePacket.packetType=PACKET_TYPE_MESSAGE;
  strncpy(messagePacket.data, message, MAX_STR_LEN);
  sendResponse(&messagePacket);
}

void dbprintf(char *format, ...) {
va_list args;
char buffer[128];
va_start(args, format);
vsprintf(buffer, format, args);
sendMessage(buffer);
}

void sendBadPacket()
{
  // Tell the Pi that it sent us a packet with a bad
  // magic number.
  
  TPacket badPacket;
  badPacket.packetType = PACKET_TYPE_ERROR;
  badPacket.command = RESP_BAD_PACKET;
  sendResponse(&badPacket);
  
}

void sendBadChecksum()
{
  // Tell the Pi that it sent us a packet with a bada
  // checksum.
  
  TPacket badChecksum;
  badChecksum.packetType = PACKET_TYPE_ERROR;
  badChecksum.command = RESP_BAD_CHECKSUM;
  sendResponse(&badChecksum);  
}

void sendBadCommand()
{
  // Tell the Pi that we don't understand its
  // command sent to us.
  
  TPacket badCommand;
  badCommand.packetType=PACKET_TYPE_ERROR;
  badCommand.command=RESP_BAD_COMMAND;
  sendResponse(&badCommand);
}

void sendBadResponse()
{
  TPacket badResponse;
  badResponse.packetType = PACKET_TYPE_ERROR;
  badResponse.command = RESP_BAD_RESPONSE;
  sendResponse(&badResponse);
}

void sendOK()
{
  TPacket okPacket;
  okPacket.packetType = PACKET_TYPE_RESPONSE;
  okPacket.command = RESP_OK;
  sendResponse(&okPacket);  
}

void sendResponse(TPacket *packet)
{
  // Takes a packet, serializes it then sends it out
  // over the serial port.
  char buffer[PACKET_SIZE];
  int len;

  len = serialize(buffer, packet, sizeof(TPacket));
  writeSerial(buffer, len);
}


/*
 * Setup and start codes for external interrupts and 
 * pullup resistors.
 * 
 */
// Enable pull up resistors on pins 18 and 19
void enablePullups()
{
  DDRD &= 0b11110011;
  PORTD |= 0b00001100;
}

// Functions to be called by INT2 and INT3 ISRs.
void leftISR() {
    if (dir == FORWARD) leftForwardTicks++;
    if (dir == BACKWARD) leftReverseTicks++;
    if (dir == LEFT) leftReverseTicksTurns++;
    if (dir == RIGHT) leftForwardTicksTurns++;
    if (dir == FORWARD) forwardDist = (unsigned long)((float)leftForwardTicks / (COUNTS_PER_REV) * WHEEL_CIRC);
    if (dir == BACKWARD) reverseDist = (unsigned long)((float)leftReverseTicks / (COUNTS_PER_REV) * WHEEL_CIRC);
}

void rightISR() {
    if (dir == FORWARD) rightForwardTicks++;
    if (dir == BACKWARD) rightReverseTicks++;
    if (dir == RIGHT) rightReverseTicksTurns++;
    if (dir == LEFT) rightForwardTicksTurns++;
}

void setupEINT()
{
  EIMSK |= 0b00001100;
  EICRA |= 0b10100000; 

}

ISR(INT2_vect) {
  rightISR();
}

ISR(INT3_vect) {
  leftISR();
}

void setupSerial()
{ 
  Serial.begin(9600);
}

void startSerial()
{
  // Empty for now. To be replaced with bare-metal code
  // later on.
  
}

// Read the serial port. Returns the read character in
// ch if available. Also returns TRUE if ch is valid. 
// This will be replaced later with bare-metal code.

int readSerial(char *buffer)
{

  int count=0;
  while(Serial.available())
    buffer[count++] = Serial.read();
  return count;
}

void writeSerial(const char *buffer, int len)
{
  Serial.write(buffer, len);
}

/*
 * Alex's setup and run codes
 * 
 */

// Clears all our counters
void clearCounters()
{
  leftForwardTicks=0; 
  rightForwardTicks=0;
  leftReverseTicks=0;
  rightReverseTicks=0;

  leftForwardTicksTurns=0;
  rightForwardTicksTurns=0;
  leftReverseTicksTurns=0;
  rightReverseTicksTurns=0;
  
  leftRevs=0;
  rightRevs=0;
  forwardDist=0;
  reverseDist=0; 
}

// Clears one particular counter
void clearOneCounter(int which)
{
  clearCounters();
}
// Intialize Alex's internal states

void initializeState()
{
  clearCounters();
}

void handleCommand(TPacket *command)
{
          
  switch(command->command)
  {
    // For movement commands, param[0] = distance, param[1] = speed.
    case COMMAND_FORWARD:
        sendOK();
        forward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_REVERSE:
        sendOK();
        backward((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_LEFT:
        sendOK();
        left((double) command->params[0], (float) command->params[1]);
      break;
    case COMMAND_TURN_RIGHT:
        sendOK();
        right((double) command->params[0], (float) command->params[1]);
      break;
    
    case COMMAND_GET_COLOUR: {
        sendOK();
        int clrr = getColour();
        for (int i = 0; i < clrr; i++) {
          delay(150);
        }
        switch(clrr) {
          case 0:
          dbprintf("Red!");
          break;
          
          case 1:
          dbprintf("Green!");
          break;

          case 2:
          dbprintf("Neither!");
          break;
        }
      break;
    }
    

    case COMMAND_OPEN:
        sendOK();
        open();
      break;

    case COMMAND_CLOSE:
        sendOK();
        close();
      break;

    case COMMAND_RELEASE_MEDPACK:
        sendOK();
        open_back();
        break;
    
    case COMMAND_STOP:
        sendOK();
        stop();
      break;
    
    case COMMAND_GET_STATS:
        sendStatus();
      break;
    
    case COMMAND_CLEAR_STATS:
        clearOneCounter(command->params[0]);
        sendOK();
      break;
      
    case COMMAND_NUDGE_FORWARD:
        sendOK();
        forward(0, 100);
        delay(10);
        forward((double) command->params[0], (float) command->params[1]);
        delay(NUDGE_DELAY_FB);
        stop();
        break;
    case COMMAND_NUDGE_BACKWARD:
        sendOK();
        backward(0, 100);
        delay(10);
        backward((double) command->params[0], (float) command->params[1]);
        delay(NUDGE_DELAY_FB);
        stop();
        break;
    case COMMAND_NUDGE_LEFT:
        sendOK();
        //char str[3];
        //str[0] = (uint8_t)command->params[0] / 10 + '0';
        //str[1] = (uint8_t)command->params[0] % 10 + '0';
        //str[2] = '\0';
        //dbprintf(str);
        ccw(0, 100);
        delay(10);
        left((double) command->params[0], (float) command->params[1]);
        //delay(NUDGE_DELAY_LR);
        //stop();
        break;
    case COMMAND_NUDGE_RIGHT:
        sendOK();
        cw(0, 100);
        delay(10);
        right((double) command->params[0], (float) command->params[1]);
        //delay(NUDGE_DELAY_LR);
        //stop();
        break;

    case COMMAND_NUDGE_FORWARD_VARIABLE:
        sendOK();
        forward((double) command->params[0], (float) command->params[1]);
        delay(command->params[0]);
        stop();
        break;

    case COMMAND_NUDGE_BACKWARD_VARIABLE:
        sendOK();
        backward((double) command->params[0], (float) command->params[1]);
        delay(command->params[0]);
        stop();
        break;

    case COMMAND_NUDGE_LEFT_VARIABLE:
        sendOK();
        left((double) command->params[0], (float) command->params[1]);
        //delay(command->params[0]);
        //stop();
        break;

    case COMMAND_NUDGE_RIGHT_VARIABLE:
        sendOK();
        right((double) command->params[0], (float) command->params[1]);
        //delay(command->params[0]);
        //stop();
        break;
      
    /*
     * Implement code for other commands here.
     * 
     */
        
    default:  
      sendBadCommand();
  }
}

void waitForHello()
{
  int exit=0;

  while(!exit)
  {
    TPacket hello;
    TResult result;
    
    do
    {
      result = readPacket(&hello);
    } while (result == PACKET_INCOMPLETE);

    if(result == PACKET_OK)
    {
      if(hello.packetType == PACKET_TYPE_HELLO)
      {
     

        sendOK();
        exit=1;
      }
      else
        sendBadResponse();
    }
    else
      if(result == PACKET_BAD)
      {
        sendBadPacket();
      }
      else
        if(result == PACKET_CHECKSUM_BAD)
          sendBadChecksum();
  } // !exit
}

void setup() {
  // put your setup code here, to run once:

  alexDiagonal = sqrt((ALEX_LENGTH * ALEX_LENGTH) + (ALEX_BREADTH *  ALEX_BREADTH)); 
  alexCirc = PI  * alexDiagonal;
  
  cli();
  setupEINT();
  setupSerial();
  startSerial();
  enablePullups();
  initializeState();

  servoSetup(); 
  colourSetup();
 
  sei();
  IMUsetup(); // MUST BE DONE AFTER SEI()
//  Serial.begin(115200);/
//  colourSetup();
}

void handlePacket(TPacket *packet)
{
  switch(packet->packetType)
  {
    case PACKET_TYPE_COMMAND:

      handleCommand(packet);
      break;

    case PACKET_TYPE_RESPONSE:
      break;

    case PACKET_TYPE_ERROR:
      break;

    case PACKET_TYPE_MESSAGE:
      break;

    case PACKET_TYPE_HELLO:
      break;
  }
}

void loop() {
// Uncomment the code below for Step 2 of Activity 3 in Week 8 Studio 2

// Uncomment the code below for Week 9 Studio 2
//AF_DCMotor motorFL(4);
//
//motorFL.run(1);
  // put your main code here, to run repeatedly:
  TPacket recvPacket; // This holds commands from the Pi

  TResult result = readPacket(&recvPacket);
  
  if(result == PACKET_OK) {

    handlePacket(&recvPacket);
  }
  else
    if(result == PACKET_BAD)
    {
      sendBadPacket();
    }
    else
      if(result == PACKET_CHECKSUM_BAD)
      {
        sendBadChecksum();
      } 

 if(deltaDist > 0) 
 { 
  if(dir==FORWARD) 
  { 
   if(forwardDist > newDist) 
   { 
    deltaDist=0; 
    newDist=0; 
    stop(); 
   } 
  } 
  else if(dir == BACKWARD) {
    if(reverseDist > newDist) 
    { 
     deltaDist=0; 
     newDist=0; 
     stop(); 
    } 
   }
  else if (dir == (TDirection)STOP) {
    deltaDist = 0;
    newDist = 0;
    stop();
   } 
  }
  
  if (deltaTicks > 0) {
   if (dir == LEFT) {
    if (leftReverseTicksTurns >= targetTicks) {
      deltaTicks = 0;
      stop();
    }
  }
  else if (dir == RIGHT) {
    if (rightReverseTicksTurns >= targetTicks) {
      deltaTicks = 0;
      targetTicks = 0;
      stop();
    }
  }
  else if (dir == (TDirection)STOP) { 
     deltaDist=0; 
     targetTicks = 0;
     stop(); 
    } 
  } 
 }
