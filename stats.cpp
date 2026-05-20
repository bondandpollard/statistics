/*
Program   : stats.cpp
System    : cpp-practice
Function  : Statistics application.
            Enter data and save as CSV file. Compatible with Oracle appsdemo stats app.
            Load data from CSV.
            Generate statistics from data.
Author    : Ian Bond
Date      : 20 May 2026

MODIFICATION HISTORY
Version   Log/Ref   Date        Author        Description
==========================================================================================================
1.00      <ref#>    DD/MM/YYYY  <name>        Created
*/

#include <iostream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>   // EXIT_SUCCESS / EXIT_FAILURE
#ifdef _WIN32
#include <windows.h>
#endif


void clear_screen() {
  #ifdef _WIN32
    // Prefer Win32 API to avoid system()
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) { std::system("cls"); return; }
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(h, &csbi)) { std::system("cls"); return; }
    DWORD cells = csbi.dwSize.X * csbi.dwSize.Y, written;
    COORD home = {0,0};
    FillConsoleOutputCharacter(h, ' ', cells, home, &written);
    FillConsoleOutputAttribute(h, csbi.wAttributes, cells, home, &written);
    SetConsoleCursorPosition(h, home);
  #else
    // POSIX: send ANSI clear + move-home (works in bash, xterm, most terminals)
    std::cout << "\x1b[2J\x1b[H" << std::flush;
  #endif
}

void display_menu() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-          MAIN MENU          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << " 1. Enter Data"                   << std::endl;
  std::cout << " 2. Save"                         << std::endl;
  std::cout << " 3. Load Data"                    << std::endl;
  std::cout << " 4. Calculate Statistics"         << std::endl;
  std::cout << "99. Exit"                         << std::endl;
  std::cout << "-------------------------------"  << std::endl << std::endl;
}

void action_one() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-        ENTER DATA           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_two() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-         SAVE DATA           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_three() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-          LOAD DATA          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_four() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-         CALCULATE           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_quit() {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-           EXIT              -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  // Check if unsaved data, warn if data may be lost, prompt to save
  return true;
}


// Prompt user to enter an option, return number
int prompt_range(const std::string &p_prompt, int p_min, int p_max) {
  int value;
  std::cout << p_prompt << " (Enter a number between " << p_min << " and " << p_max << ") : ";
  while (!(std::cin >> value && (value >= p_min && value <= p_max))) {   // handle bad input   
    std::cout << "Error: You must enter a number between " << p_min << " and " << p_max << " : "; 
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  } 
  return value;
}

// After menu option processed ask user if they want to return to the menu or exit
bool prompt_continue() {
  bool quit=false;
  char response;
  
  std::cout << "\nEnter Q to quit, or any other key to return to the menu: ";
  std::cin.get(response);
  if (response == '\n')            // if previous input left a newline, read a real char
    std::cin.get(response);
  if (response=='Q' || response == 'q') quit=true; 
  return quit;
}
  
// Check menu option selected and execute it
bool process_option(int p_option) {
  bool quit=false;
  
  switch (p_option) {
    case 1:
      action_one();
      break;
    case 2:
      action_two();
      break;
    case 3:
      action_three();
      break;
    case 4:
      action_four();
      break;
    case 99:
      quit=action_quit();
      break;
    default:
      std::cout << "Invalid option." << std::endl;
    }
  // Unless user quit, prompt asking if they want to return to the menu or exit
  if (!quit) {
    quit=prompt_continue();
  }
  return quit;
}

void exit_message() {
 std::cout << "Goodbye!" << std::endl; 
}

int main(int argc, char* argv[]) {
  const int min=1, max=4;
  std::vector<std::string> args(argv, argv + argc);   // easy to use: args as vector<string>
  bool exit_menu=false;
  int option;

  // simple help
  if (argc > 1 && (args[1] == "-h" || args[1] == "--help")) {
    std::cout << "Usage: menu" << std::endl
              << "  [-h | --help]" << std::endl;
    return EXIT_SUCCESS;
  }

  do {
    display_menu();
    option=prompt_range(std::string("Please select an option."),min,max);
    exit_menu=process_option(option);
  } while (!exit_menu);

  exit_message();
  
  return EXIT_SUCCESS;
}

