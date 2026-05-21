/*
Program   : stats.cpp
System    : cpp-practice
Function  : Statistics application.
            Enter data and save as CSV file. Compatible with Oracle appsdemo stats app.
            Load data from CSV.
            Generate statistics from data.
            
            TO DO:
            SAVE Data 
            PRINT Results (save calculate stats as test)
            
            Also: Make this compatible with appsdemo by handling the CSV header row when loading and saving data!
            
            
Author    : Ian Bond
Date      : 20 May 2026

MODIFICATION HISTORY
Version   Log/Ref   Date        Author        Description
==========================================================================================================
1.00      <ref#>    DD/MM/YYYY  <name>        Created
*/

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>   // EXIT_SUCCESS / EXIT_FAILURE
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#endif

// Open file
std::ifstream file_open(std::string &filename) {
  std::ifstream file(filename);
  return file;
}

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


// Prompt for filename
std::string prompt_filename(const std::string& prompt = "\nEnter filename: ") {
    std::string filename;
    std::cout << prompt << std::flush;            // ensure prompt is shown
    if (!std::getline(std::cin, filename)) return std::string(); // return empty on EOF/error
    // optional trim of leading/trailing whitespace
    const auto start = filename.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return std::string();
    const auto end = filename.find_last_not_of(" \t\r\n");
    return filename.substr(start, end - start + 1);
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


// Confirm user is sure they want to quit and lose unsaved data
bool prompt_confirm_quit() {
  bool quit=false;
  char response;
  
  std::cout << "\a"; // beep
  std::cout << "\nARE YOU SURE YOU WANT TO QUIT? ANY UNSAVED DATA WILL BE LOST!" << std::endl << std::endl;
  std::cout << "\nEnter Q to quit and lose unsaved data, or any other key to return to the menu: ";
  std::cin.get(response);
  if (response == '\n')            // if previous input left a newline, read a real char
    std::cin.get(response);
  if (response=='Q' || response == 'q') quit=true; 
  return quit;
}


// Warn user existing data will be cleared, and ask if they want to continue
bool warn_clear_data(std::vector<double>& stats_data) {
  const std::size_t n = stats_data.size();   // number of entries in stats_data
  char response;
  bool clear_data=false;
  
  if (n>0) {
    std::cout << "\a\nWARNING! Existing data will be cleared if you continue." << std::endl;
    std::cout << "\nEnter Y to continue and lose existing data, or N to keep data and return to the menu :";
    std::cin.get(response);
    if (response == '\n')            // if previous input left a newline, read a real char
      std::cin.get(response);
    if (response=='Y' || response == 'y') clear_data=true;
  } else {
    clear_data=true;    // no data to lose
  }
  return clear_data;
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

// Calculate standard deviation (Sample) for the data set
double stddev(const std::vector<double>& data) {
  const std::size_t n = data.size();
  if (n < 2) throw std::domain_error("stddev requires at least two values");

  double sum = 0.0;
  for (double x : data) sum += x;
  double mean = sum / static_cast<double>(n);

  double sumsq = 0.0;
  for (double x : data) {
    double d = x - mean;
    sumsq += d * d;
  }

  double variance = sumsq / static_cast<double>(n - 1); // sample variance
  return std::sqrt(variance);
}

// Calculate the mean value of the data
double mean(const std::vector<double>& data) {
  const std::size_t n = data.size();
  if (n < 2) throw std::domain_error("stddev requires at least two values");

  double sum = 0.0;
  for (double x : data) sum += x;
  double mean = sum / static_cast<double>(n);

  return mean;
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
  std::cout << " 6. Print Results"                << std::endl;  
  std::cout << "99. Exit"                         << std::endl;
  std::cout << "-------------------------------"  << std::endl << std::endl;
}

void action_one(std::vector<double>& stats_data) {
  const int min=2, max=30;                   // determines how many data values may be entered
  int count_values=0;                        // number of values user chose to enter
  char response;
  bool ok_to_enter_data=true;                // Confirm with user before wiping data to enter new data

  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-        ENTER DATA           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  // Check if there is existing data. 
  ok_to_enter_data=warn_clear_data(stats_data);
  if (ok_to_enter_data) {
    // No existing data, or user confirmed they want to enter new data
    count_values=prompt_range(std::string("How many values do you want to enter?"),min,max);
    stats_data=get_data(count_values); //Prompt  user to enter data values
  } else {
    std::cout << "\nYour existing data has been kept in memory." << std::endl;
  }
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
  const std::size_t n = stats_data.size();   // number of entries in stats_data
  std::ifstream file;                        // file pointer
  std::string filename;                      // Name of file to open passed as command line arg
  std::string line;                          // Record read from CSV file
  bool ok_to_continue;                       // If there is existing data, confirm user want to load new data and replace it

  
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-          LOAD DATA          -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  
     
  if (n>0) {
    std::cout << "\nYou have existing data. There are " << n << " values in memory." << std::endl;
  }
  // Check if existing data, warn user
  ok_to_continue=warn_clear_data(stats_data);
  if (!ok_to_continue) {
    // User aborted load new data to keep existing
    std::cout << "Y\nour existing data will be kept in memory." << std::endl;
    return;
  }
  
  // Either no data, or user confirmed OK continue
  
  if (n>0) {
    // CLEAR DATA AND CONTINUE
    std::cout << "\nExisting data will be cleared." << std::endl;
    stats_data.clear();
  }
  
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  filename=prompt_filename();
  if (filename.empty()) {
    std::cout << "\nNo filename entered." << std::endl;
    return;
  } 
  
  // Open file
  file=file_open(filename);
  if (!file) {
    std::cerr << "Error: could not open file '" << filename << "'\n";
    return;
  }
  
  // Load CSV data
  while (std::getline(file, line)) {
    if (line.empty()) continue;

    std::istringstream row(line);
    std::vector<std::string> fields;
    std::string field;

    // split by comma
    while (std::getline(row, field, ',')) {
      // trim simple whitespace (optional)
      size_t start = field.find_first_not_of(" \t\r\n");
      size_t end   = field.find_last_not_of(" \t\r\n");
      if (start == std::string::npos) field = "";
      else field = field.substr(start, end - start + 1);

      fields.push_back(field);
    }

    // example: take second column (index 1) as numeric value
    if (fields.size() > 1) {
      try {
          double val = std::stod(fields[1]); // convert string to double
          stats_data.push_back(val);
      } catch (const std::invalid_argument&) {
          std::cerr << "Warning: non-numeric value '" << fields[1] << "' in line: " << line << '\n';
      } catch (const std::out_of_range&) {
          std::cerr << "Warning: numeric value out of range in line: " << line << '\n';
      }
    } else {
        std::cerr << "Warning: not enough fields in line: " << line << '\n';
    }
  }

  if (file.bad()) {
      std::cerr << "I/O error while reading.\n";
      return;
  }

  file.close();   // Close file
  
  std::cout << "Data loaded OK." << std::endl;
}

void action_five(std::vector<double>& stats_data) {
  const std::size_t n = stats_data.size();   // number of entries in stats_data
  double standard_deviation, mean_value;
  
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-         CALCULATE           -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  // Check if there are any values for the calculation
  if (n) {
    standard_deviation=stddev(stats_data);
    std::cout << "\n\nStandard Deviation (Sample) = " << standard_deviation << std::endl;
  
    mean_value=mean(stats_data);
    std::cout << "\n\nMean = " << mean_value << std::endl <<std::endl;
  } else {
    std::cout << "Please enter or load some data for the calculations!" << std::endl;
  }
}

void action_six(std::vector<double>& stats_data) {
  const std::size_t n = stats_data.size();   // number of entries in stats_data
  double standard_deviation, mean_value;
  
  clear_screen();
  std::cout << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  std::cout << "-     S T A T I S T I C S     -"  << std::endl;
  std::cout << "-                             -"  << std::endl;
  std::cout << "-        PRINT RESULTS        -"  << std::endl;
  std::cout << "-------------------------------"  << std::endl;
  
  // Check if there are any values for the calculation
  if (n) {
    standard_deviation=stddev(stats_data);
    std::cout << "\n\nStandard Deviation (Sample) = " << standard_deviation << std::endl;
  
    mean_value=mean(stats_data);
    std::cout << "\n\nMean = " << mean_value << std::endl <<std::endl;
  } else {
    std::cout << "Please enter or load some data for the calculations!" << std::endl;
  }
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
    case 6:
      action_six(stats_data);
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

