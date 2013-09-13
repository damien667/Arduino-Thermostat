#if !defined(__PGMSTRINGS_H__)
#define __PGMSTRINGS_H__

const char OSC_Error_str[] = "OSC did not start.";
const char Init_str[] = "Started! Type '?' for help.";
const char Uptime_str[] = "Uptime: ";
const char Uptime_Seconds_Elapsed_str[] = " seconds elapsed";
const char Uptime_Seconds_str[] = " seconds";
const char Relay_Statuses_str[] = "Relay Statuses:\r\n---------";
const char Colon_Space_str[] = ": ";
const char Equals_line_str[] = "=========";
const char Thermostat_States_str[] = "States:\r\n---------";
const char Thermostat_States_Curr_str[] = "curr: ";
const char Thermostat_States_Last_str[] = "last: ";
const char Thermostat_States_User_str[] = "user: ";
const char Thermostat_States_TooHot_str[] = "tooHot: ";
const char Thermostat_States_JustRight_str[] = "justRight: ";
const char Thermostat_States_TooCold_str[] = "tooCold: ";
const char Thermostat_States_Desired_str[] = "Desired Temp: ";
const char Thermostat_States_UserChanged_str[] = "userChanged: ";
const char Thermostat_States_HaveRestored_str[] = "haveRestored: ";
const char HelpMenu_Title_str[] = "Welcome to Help Menu for Arduino Thermostat";
const char HelpMenu_Question_str[] = "\t'?': This Help Menu";
const char HelpMenu_w_str[] = "\t'w': Show Welcome on LCD";
const char HelpMenu_l_str[] = "\t'l': Toggle LCD On/Off";        
const char HelpMenu_u_str[] = "\t'u': Arduino's Uptime on LCD";
const char HelpMenu_t_str[] = "\t't': Get current Temperature on LCD";
const char HelpMenu_S_str[] = "\t'S': Get current HVAC status";
const char HelpMenu_Plus_str[] = "\t'+': Increase Desired Temperature by 0.5";
const char HelpMenu_Minus_str[] = "\t'-': Decrease Desired Temperature by 0.5";
const char HelpMenu_H_str[] = "\t'H': Turn the HEATER on (relays 1 and 3 on)";
const char HelpMenu_C_str[] = "\t'C': Turn the AC on (relays 2 and 3 on)";
const char HelpMenu_F_str[] = "\t'F': Turn the FAN on (relay 3 on)";
const char HelpMenu_O_str[] = "\t'O': Turn the HVAC off (all relays off)";
const char Show_Welcome_str[] = "Showing Welcome Banner";
const char LCD_On_str[] = "turning on LCD...";
const char LCD_Off_str[] = "turning off LCD...";
const char LCD_Welcome_str[] = "Welcome To:";
const char LCD_Welcome_PGM_NAME_str[] = "Arduino Thermostat";
const char ASK_CMD_str[] = "ask//";
const char OFF_CMD_str[] = "off//";
const char R1_ON_CMD_str[] = "01+//";
const char R1_OFF_CMD_str[] = "01-//";
const char R2_ON_CMD_str[] = "02+//";
const char R2_OFF_CMD_str[] = "02-//";
const char R3_ON_CMD_str[] = "03+//";
const char R3_OFF_CMD_str[] = "03-//";
const char LCD_Temperature_str[] = "Temperature: ";
const char LCD_Temperature_F_str[] = " F ";

#endif // __PGMSTRINGS_H__


