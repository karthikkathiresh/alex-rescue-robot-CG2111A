#ifndef __CONSTANTS_INC__
#define __CONSTANTS_INC__

/* 
 *  This file containts all the packet types, commands
 *  and status constants
 *  
 */

// Packet types
typedef enum
{
  PACKET_TYPE_COMMAND = 0,
  PACKET_TYPE_RESPONSE = 1,
  PACKET_TYPE_ERROR = 2,
  PACKET_TYPE_MESSAGE = 3,
  PACKET_TYPE_HELLO = 4
} TPacketType;

// Response types. This goes into the command field
typedef enum
{
  RESP_OK = 0,
  RESP_STATUS=1,
  RESP_BAD_PACKET = 2,
  RESP_BAD_CHECKSUM = 3,
  RESP_BAD_COMMAND = 4,
  RESP_BAD_RESPONSE = 5 
} TResponseType;


// Commands
// For direction commands, param[0] = distance in cm to move
// param[1] = speed
typedef enum
{
  COMMAND_FORWARD = 0,
  COMMAND_REVERSE = 1,
  COMMAND_TURN_LEFT = 2,
  COMMAND_TURN_RIGHT = 3,
  COMMAND_STOP = 4,
  COMMAND_GET_STATS = 5,
  COMMAND_CLEAR_STATS = 6,
  COMMAND_NUDGE_FORWARD = 7,
  COMMAND_NUDGE_BACKWARD = 8,
  COMMAND_NUDGE_LEFT = 9,
  COMMAND_NUDGE_RIGHT = 10,
  COMMAND_GET_COLOUR = 11,
  COMMAND_OPEN = 12,
  COMMAND_CLOSE = 13,
  COMMAND_NUDGE_FORWARD_VARIABLE = 14,
  COMMAND_NUDGE_BACKWARD_VARIABLE = 15,
  COMMAND_NUDGE_LEFT_VARIABLE = 16,
  COMMAND_NUDGE_RIGHT_VARIABLE = 17,
  COMMAND_RELEASE_MEDPACK = 18
} TCommandType;

typedef enum
{
  FORWARD = 1,
  BACKWARD=2,
  LEFT=3,
  RIGHT=4
} TDirection;

typedef enum Tdir
{
  STOP,
  GO,
  BACK,
  CCW,
  CW
} Tdir;

#endif
