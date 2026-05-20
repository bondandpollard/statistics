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

// Clear terminal screen
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

// Prompt user to enter an option within specified range of numbers, return number
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

// After menu option processed ask user to press enter to return to the menu
void prompt_continue() {
  char response;
  
  std::cout << "\nPress enter to continue to the menu: ";
  std::cin.get(response);
  if (response == '\n')            // if previous input left a newline, read a real char
    std::cin.get(response);
}


// Confirm user is sure they want to quit
bool prompt_confirm_quit() {
  bool quit=false;
  char response;
  
  std::cout << "\a"; // beep
  std::cout << "\nARE YOU SURE YOU WANT TO QUIT? ANY UNSAVED DATA WILL BE LOST!" << std::endl << std::endl;
  std::cout << "Enter Q to quit and lose unsaved data, or any other key to return to the menu: ";
  std::cin.get(response);
  if (response == '\n')            // if previous input left a newline, read a real char
    std::cin.get(response);
  if (response=='Q' || response == 'q') quit=true; 
  return quit;
}


// Prompt user to enter stats data
std::vector<double> get_data(int p_count) { 
  std::vector<double> v;
  v.reserve(p_count);
  double val;
  for (int i = 0; i < p_count; ++i) {
    while (true) {
      std::cout << "Enter data [" << (i+1) << "]: ";
      if (std::cin >> val) { v.push_back(val); break; }
      std::cout << "Invalid number, try again.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
  return v;
}

// Display stats data entered by user
void display_data(const std::vector<double>& data) {
  std::cout << "\nYour data is:\n\n";
  for (size_t i = 0; i < data.size(); ++i)
    std::cout << "Data[" << (i+1) << "]=" << data[i] << '\n';
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
  std::cout << " 2. Display Data"                 << std::endl;
  std::cout << " 3. SAVE Data"                    << std::endl;
  std::cout << " 4. Load Data"                    << std::endl;
  std::cout << " 5. Calculate Statistics"         << std::endl;
  std::cout << "99. Exit"                         << std::endl;
  std::cout << "-------------------------------"  << std::endl << std::endl;
}

void action_one(std::vector<double>& stats_data) {
  int count_values=0;
  const int min=2, max=20;

  
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-        ENTER DATA           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  count_values=prompt_range(std::string("How many values do you want to enter?"),min,max);
  stats_data=get_data(count_values); //Prompt  user to enter data values
}

void action_two(std::vector<double>& stats_data) {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-       DISPLAY DATA          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  display_data(stats_data);
}

void action_three(std::vector<double>& stats_data) {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-          SAVE DATA          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_four(std::vector<double>& stats_data) {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-          LOAD DATA          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

void action_five(std::vector<double>& stats_data) {
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-         CALCULATE           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
}

bool action_quit(std::vector<double>& stats_data) {
  bool confirm_quit=false;    // check user sure they want to quit!
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-           EXIT              -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  // Double-check user really wants to quit program
  confirm_quit=prompt_confirm_quit();
  return confirm_quit;
}

// Check menu option selected and execute it
bool process_option(int p_option, std::vector<double>& stats_data) {
  bool quit=false;
  
  switch (p_option) {
    case 1:
      action_one(stats_data);
      break;
    case 2:
      action_two(stats_data);
      break;
    case 3:
      action_three(stats_data);
      break;
    case 4:
      action_four(stats_data);
      break;
    case 5:
      action_five(stats_data);
      break;
    case 99:                    // Quit, exit program
      quit=action_quit(stats_data);
      break;
    default:
      std::cout << "Invalid option." << std::endl;
    }
  // prompt user to continue unless they confirmed they want to quit the program
  if (!quit && p_option!=99) {
    prompt_continue();
  }
  return quit;
}

void exit_message() {
 std::cout << "Goodbye!" << std::endl; 
}

int main(int argc, char* argv[]) {
  const int min=1, max=99;
  std::vector<std::string> args(argv, argv + argc);   // easy to use: args as vector<string>
  std::vector<double> stats_data; // The data used to calculate the stats is stored here
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
    exit_menu=process_option(option, stats_data);
  } while (!exit_menu);

  exit_message();
  
  return EXIT_SUCCESS;
}

