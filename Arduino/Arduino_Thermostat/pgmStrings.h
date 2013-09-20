#if !defined(__PGMSTRINGS_H__)
#define __PGMSTRINGS_H__

#include <avr/pgmspace.h>

const char OSC_Error_str_str[] PROGMEM = "OSC did not start.";
const char *OSC_Error_str PROGMEM = OSC_Error_str_str;

const char Init_str_str[] PROGMEM = "Started! Type '?' for help.";
const char *Init_str PROGMEM = Init_str_str;

const char Uptime_str_str[] PROGMEM = "Uptime: ";
const char *Uptime_str PROGMEM = Uptime_str_str;

const char Uptime_Seconds_Elapsed_str_str[] PROGMEM = " seconds elapsed";
const char *Uptime_Seconds_Elapsed_str PROGMEM = Uptime_Seconds_Elapsed_str_str;

const char Uptime_Seconds_str_str[] PROGMEM = " seconds";
const char *Uptime_Seconds_str PROGMEM = Uptime_Seconds_str_str;

const char Relay_Statuses_str_str[] PROGMEM = "Relay Statuses:\r\n---------";
const char *Relay_Statuses_str PROGMEM = Relay_Statuses_str_str;

const char Colon_Space_str_str[] PROGMEM = ": ";
const char *Colon_Space_str PROGMEM = Colon_Space_str_str;

const char Equals_line_str_str[] PROGMEM = "=========";
const char *Equals_line_str PROGMEM = Equals_line_str_str;

const char Thermostat_States_str_str[] PROGMEM = "States:\r\n---------";
const char *Thermostat_States_str PROGMEM = Thermostat_States_str_str;

const char Thermostat_States_Curr_str_str[] PROGMEM = "curr: ";
const char *Thermostat_States_Curr_str PROGMEM = Thermostat_States_Curr_str_str;

const char Thermostat_States_Last_str_str[] PROGMEM = "last: ";
const char *Thermostat_States_Last_str PROGMEM = Thermostat_States_Last_str_str;

const char Thermostat_States_User_str_str[] PROGMEM = "user: ";
const char *Thermostat_States_User_str PROGMEM = Thermostat_States_User_str_str;

const char Thermostat_States_TooHot_str_str[] PROGMEM = "tooHot: ";
const char *Thermostat_States_TooHot_str PROGMEM = Thermostat_States_TooHot_str_str;

const char Thermostat_States_JustRight_str_str[] PROGMEM = "justRight: ";
const char *Thermostat_States_JustRight_str PROGMEM = Thermostat_States_JustRight_str_str;

const char Thermostat_States_TooCold_str_str[] PROGMEM = "tooCold: ";
const char *Thermostat_States_TooCold_str PROGMEM = Thermostat_States_TooCold_str_str;

const char Thermostat_States_Desired_str_str[] PROGMEM = "Desired Temp: ";
const char *Thermostat_States_Desired_str PROGMEM = Thermostat_States_Desired_str_str;

const char Thermostat_States_UserChanged_str_str[] PROGMEM = "userChanged: ";
const char *Thermostat_States_UserChanged_str PROGMEM = Thermostat_States_UserChanged_str_str;

const char Thermostat_States_HaveRestored_str_str[] PROGMEM = "haveRestored: ";
const char *Thermostat_States_HaveRestored_str PROGMEM = Thermostat_States_HaveRestored_str_str;

const char HelpMenu_Title_str_str[] PROGMEM = "Welcome to Help Menu for Arduino Thermostat";
const char *HelpMenu_Title_str PROGMEM = HelpMenu_Title_str_str;

const char HelpMenu_Question_str_str[] PROGMEM = "\t'?': This Help Menu";
const char *HelpMenu_Question_str PROGMEM = HelpMenu_Question_str_str;

const char HelpMenu_w_str_str[] PROGMEM = "\t'w': Show Welcome on LCD";
const char *HelpMenu_w_str PROGMEM = HelpMenu_w_str_str;

const char HelpMenu_l_str_str[] PROGMEM = "\t'l': Toggle LCD On/Off";
const char *HelpMenu_l_str PROGMEM = HelpMenu_l_str_str;

const char HelpMenu_u_str_str[] PROGMEM = "\t'u': Arduino's Uptime on LCD";
const char *HelpMenu_u_str PROGMEM = HelpMenu_u_str_str;

const char HelpMenu_t_str_str[] PROGMEM = "\t't': Get current Temperature on LCD";
const char *HelpMenu_t_str PROGMEM = HelpMenu_t_str_str;

const char HelpMenu_s_str_str[] PROGMEM = "\t's': Get current HVAC status(non-human)";
const char *HelpMenu_s_str PROGMEM = HelpMenu_s_str_str;

const char HelpMenu_S_str_str[] PROGMEM = "\t'S': Get current HVAC status";
const char *HelpMenu_S_str PROGMEM = HelpMenu_S_str_str;

const char HelpMenu_Plus_str_str[] PROGMEM = "\t'+': Increase Desired Temperature by 0.5";
const char *HelpMenu_Plus_str PROGMEM = HelpMenu_Plus_str_str;

const char HelpMenu_Minus_str_str[] PROGMEM = "\t'-': Decrease Desired Temperature by 0.5";
const char *HelpMenu_Minus_str PROGMEM = HelpMenu_Minus_str_str;

const char HelpMenu_H_str_str[] PROGMEM = "\t'H': Turn the HEATER on (relays 1 and 3 on)";
const char *HelpMenu_H_str PROGMEM = HelpMenu_H_str_str;

const char HelpMenu_C_str_str[] PROGMEM = "\t'C': Turn the AC on (relays 2 and 3 on)";
const char *HelpMenu_C_str PROGMEM = HelpMenu_C_str_str;

const char HelpMenu_F_str_str[] PROGMEM = "\t'F': Turn the FAN on (relay 3 on)";
const char *HelpMenu_F_str PROGMEM = HelpMenu_F_str_str;

const char HelpMenu_O_str_str[] PROGMEM = "\t'O': Turn the HVAC off (all relays off)";
const char *HelpMenu_O_str PROGMEM = HelpMenu_O_str_str;

const char Show_Welcome_str_str[] PROGMEM = "Showing Welcome Banner";
const char *Show_Welcome_str PROGMEM = Show_Welcome_str_str;

const char LCD_On_str_str[] PROGMEM = "turning on LCD...";
const char *LCD_On_str PROGMEM = LCD_On_str_str;

const char LCD_Off_str_str[] PROGMEM = "turning off LCD...";
const char *LCD_Off_str PROGMEM = LCD_Off_str_str;

const char LCD_Welcome_str_str[] PROGMEM = "Welcome To:";
const char *LCD_Welcome_str PROGMEM = LCD_Welcome_str_str;

const char LCD_Welcome_PGM_NAME_str_str[] PROGMEM = "Arduino Thermostat";
const char *LCD_Welcome_PGM_NAME_str PROGMEM = LCD_Welcome_PGM_NAME_str_str;

const char ASK_CMD_str_str[] PROGMEM = "ask//";
const char *ASK_CMD_str PROGMEM = ASK_CMD_str_str;

const char OFF_CMD_str_str[] PROGMEM = "off//";
const char *OFF_CMD_str PROGMEM = OFF_CMD_str_str;

const char R1_ON_CMD_str_str[] PROGMEM = "01+//";
const char *R1_ON_CMD_str PROGMEM = R1_ON_CMD_str_str;

const char R1_OFF_CMD_str_str[] PROGMEM = "01-//";
const char *R1_OFF_CMD_str PROGMEM = R1_OFF_CMD_str_str;

const char R2_ON_CMD_str_str[] PROGMEM = "02+//";
const char *R2_ON_CMD_str PROGMEM = R2_ON_CMD_str_str;

const char R2_OFF_CMD_str_str[] PROGMEM = "02-//";
const char *R2_OFF_CMD_str PROGMEM = R2_OFF_CMD_str_str;

const char R3_ON_CMD_str_str[] PROGMEM = "03+//";
const char *R3_ON_CMD_str PROGMEM = R3_ON_CMD_str_str;

const char R3_OFF_CMD_str_str[] PROGMEM = "03-//";
const char *R3_OFF_CMD_str PROGMEM = R3_OFF_CMD_str_str;

const char LCD_Temperature_str_str[] PROGMEM = "Temperature: ";
const char *LCD_Temperature_str PROGMEM = LCD_Temperature_str_str;

const char LCD_Temperature_F_str_str[] PROGMEM = " F ";
const char *LCD_Temperature_F_str PROGMEM = LCD_Temperature_F_str_str;

const char Serial_Comma_Separator_str_str[] PROGMEM = ",";
const char *Serial_Comma_Separator_str PROGMEM = Serial_Comma_Separator_str_str;

#endif // __PGMSTRINGS_H__


