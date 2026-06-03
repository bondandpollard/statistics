/*
Program   : stats.cpp
System    : statistics
Function  : Statistics application.
            Usage
            $./stats [<filename>]
            
            Optionally supply name of CSV file to load on command line.
            Load data from a CSV file.
            Enter data.
            Calculate statistics: Standard Deviation and Mean.
            Save data as CSV file in format compatible with Oracle appsdemo stats app.

            
            CSV File (input/output)
            ================
            Header record:
            PROJECT,[description text]
            
            Data records
            [label text], [9999.99]
            
            e.g.
            PROJECT,This is the project description
            Tom,23.45
            Dick,36.09
            Harry,42.01
            
            TO DO:
            Windows GUI version.
            Android Mobile version.
                 
Author    : Ian Bond
Date      : 20 May 2026

MODIFICATION HISTORY
Version   Log/Ref   Date        Author        Description
==========================================================================================================
1.00      <ref#>    31/05/2026  IAB           Make this compatible with Oracle appsdemo statistics.
                                              Pass the Project Description with the Stats Data to all functions.
                                              Display project desc on all screens.
                                              Enter data: Prompt for Project Desc, supply a default value.
                                              Load data: handle header - store project desc in passed variable. If no header set default value.
                                              Save data: Header row is created from project desc.
                                              Print Results: Save calculated stats as text file.
                                              load Data: Move file operation into a separate function.
                                              Provide a command line option: if csv file specified auto load data.
1.01                03/06/2026  IAB           Create function to display menu header and centre title text.
*/

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>   // EXIT_SUCCESS / EXIT_FAILURE
#include <cmath>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

// Struct holding a description and the numeric data vector
struct ProjectData {
  std::string description;
  std::vector<std::string> stats_label;
  std::vector<double> stats_data;
};

// Open file
std::ifstream file_open(std::string &filename) {
  std::ifstream file(filename);
  return file;
}

// Load data from CSV file
bool load_data(std::string &filename, ProjectData& pd) {
  std::ifstream file;                         // file pointer
  std::string line;                           // Record read from CSV file
  
  // Initialize project data structure
  pd.description="Project loaded from file. No description found.";
  pd.stats_label.clear();
  pd.stats_data.clear();
  
  // Open file
  file=file_open(filename);
  if (!file) {
    std::cerr << "Error: could not open file '" << filename << "'\n";
    return false;
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
    
    if (fields[0]=="PROJECT") {
      // Header row. Load description from second field.
      pd.description=fields[1];
    } else {
      // Data row. load data values from second field.
      if (fields.size() > 1) {
        pd.stats_label.push_back(fields[0]);    // Store data label in vector
        try {
            double val = std::stod(fields[1]);  // convert string to double
            pd.stats_data.push_back(val);       // Store data value in vector
        } catch (const std::invalid_argument&) {
            std::cerr << "Warning: non-numeric value '" << fields[1] << "' in line: " << line << '\n';
        } catch (const std::out_of_range&) {
            std::cerr << "Warning: numeric value out of range in line: " << line << '\n';
        }
      } else {
          std::cerr << "Warning: not enough fields in line: " << line << '\n';
      }
    }
  }

  if (file.bad()) {
      std::cerr << "I/O error while reading.\n";
      return false;
  }
  
  file.close();   // Close file
  return true;
}

// Save data to CSV file
void save_data(const std::string& filename, const ProjectData& pd) {
  std::ofstream outfile(filename);
  
  if (!outfile.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }
  
  // Write description
  outfile << "PROJECT," << pd.description << std::endl;
  
  // Write stats_data
  for (size_t i = 0; i < pd.stats_data.size(); ++i) {
    outfile << pd.stats_label[i] << "," << pd.stats_data[i] << std::endl;
  }
  outfile.close();
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

// Prompt for a string, provide prompt text. Trim whitespace from response.
std::string prompt_string_trim(const std::string& prompt) {
    std::string response;
    std::cout << prompt << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    if (!std::getline(std::cin, response)) {
      std::cerr << "Error reading input or EOF reached\n";
      return std::string(); // return empty on EOF/error
    }
    // optional trim of leading/trailing whitespace
    const auto start = response.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return std::string();
    const auto end = response.find_last_not_of(" \t\r\n");
    return response.substr(start, end - start + 1);
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
bool prompt_confirm_quit(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();
  bool quit=false;
  char response;
  
  if (!n==0) {
    // Data exists so prompt user to confirm quit
    std::cout << "\a"; // beep
    std::cout << "\nARE YOU SURE YOU WANT TO QUIT? ANY UNSAVED DATA WILL BE LOST!" << std::endl << std::endl;
    std::cout << "\nEnter Q to quit and lose unsaved data, or any other key to return to the menu: ";
    std::cin.get(response);
    if (response == '\n')            // if previous input left a newline, read a real char
      std::cin.get(response);
    if (response=='Q' || response == 'q') quit=true; 
  } else {
    // No data so do not prompt for confirmation to quit
    quit=true;
  }
  return quit;
}

// Warn user existing data will be cleared, and ask if they want to continue
bool warn_clear_data(ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();   // number of entries in stats_data
  char response;
  bool clear_data=false;
  
  if (n>0) {
    std::cout << "\a\nWARNING! Existing [" << n << "] data values will be cleared if you continue." << std::endl;
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
void get_data(ProjectData& pd, int p_count) { 
  double val;
  std::string label;
  
  // Initialize project data structure
  pd.stats_label.clear(); // data labels
  pd.stats_data.clear();  // data values
  pd.stats_data.reserve(p_count);

  for (int i = 0; i < p_count; ++i) {
    std::cout << "\nData [" << (i+1) << "]: " << std::endl;
    label=prompt_string_trim("Label: ");
    pd.stats_label.push_back(label);      // store label
    while (true) {
      std::cout << "Value: ";
      if (std::cin >> val) { pd.stats_data.push_back(val); break; }   // store data
      std::cout << "Invalid number, try again.\n";
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }
}

// Display stats data entered by user
void display_data(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();
  if (n==0) {
    std::cout << "Please load or enter some data first." << std::endl;
  } else {
    std::cout << "\nProject Description: " << pd.description << std::endl;
    std::cout << "\n\nYour data is:\n\n";
    std::cout << "\nCount of values: " << n << std::endl << std::endl;
    for (size_t i = 0; i < pd.stats_data.size(); ++i)
      std::cout << "[" << i+1 << "]\t" << pd.stats_label[i] << "\t: " << pd.stats_data[i] << '\n';
  }
}

// Calculate standard deviation (Sample) for the data set
double stddev(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();
  if (n < 2) throw std::domain_error("stddev requires at least two values");

  double sum = 0.0;
  for (double x : pd.stats_data) sum += x;
  double mean = sum / static_cast<double>(n);

  double sumsq = 0.0;
  for (double x : pd.stats_data) {
    double d = x - mean;
    sumsq += d * d;
  }

  double variance = sumsq / static_cast<double>(n - 1); // sample variance
  return std::sqrt(variance);
}

// Calculate the mean value of the data
double mean(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();
  if (n == 0) throw std::domain_error("mean requires at least one value");

  double sum = 0.0;
  for (double x : pd.stats_data) sum += x;
  return sum / static_cast<double>(n);
}

void display_project(const ProjectData& pd) {
  std::cout << std::endl << "Project: " << pd.description << std::endl << std::endl;
}

// Centre text within length of border, with edge characters at left and right ends of string.
// e.g. -          TITLE          -"
std::string centre_header_text(const int border_length, const std::string& border_edge, const std::string& text) {
  int left_pad = (border_length - (text.length()+2)) /2;
  int right_pad = border_length - text.length() - left_pad -2;

  std::string format_text = border_edge + std::string(left_pad,' ') + text + std::string(right_pad,' ') + border_edge;
  return format_text;
}

// Display the screen header
void display_header(const ProjectData& pd, const std::string& title, const std::string& subtitle) {
  std::string border_line = "-----------------------------------------------";
  std::string border_edge = "-";
  std::size_t len_border_line = border_line.length();
  std::string format_title = centre_header_text(len_border_line, border_edge, title);
  std::string format_subtitle = centre_header_text(len_border_line, border_edge, subtitle);
  
  clear_screen();
  std::cout << std::endl;
  std::cout << border_line << std::endl;
  std::cout << format_title << std::endl;
  std::cout << format_subtitle << std::endl;
  std::cout << border_line  << std::endl;
  display_project(pd);
}

void display_menu(ProjectData& pd) {
  display_header(pd, "S T A T I S T I C S","MAIN MENU");
  std::cout << " 1. Enter Data"                   << std::endl;
  std::cout << " 2. Display Data"                 << std::endl;
  std::cout << " 3. SAVE Data"                    << std::endl;
  std::cout << " 4. Load Data"                    << std::endl;
  std::cout << " 5. Calculate Statistics"         << std::endl;
  std::cout << " 6. Print Results"                << std::endl;  
  std::cout << "99. Exit"                         << std::endl;
}

void action_one(ProjectData& pd) {
  const int min=2, max=30;                   // determines how many data values may be entered
  int count_values=0;                        // number of values user chose to enter
  char response;
  bool ok_to_enter_data=true;                // Confirm with user before wiping data to enter new data

  display_header(pd,"S T A T I S T I C S","ENTER DATA");
  
  // Check if there is existing data. 
  ok_to_enter_data=warn_clear_data(pd);
  if (ok_to_enter_data) {
    // No existing data, or user confirmed they want to enter new data
    pd.description=prompt_string_trim("\nEnter a name for your project: ");
    if(pd.description.empty())
      pd.description="Unnamed Statistics Project";
    count_values=prompt_range(std::string("How many values do you want to enter?"),min,max);
    get_data(pd, count_values); //Prompt  user to enter data values
  } else {
    std::cout << "\nYour existing data has been kept in memory." << std::endl;
  }
}

void action_two(const ProjectData& pd) {
  display_header(pd, "S T A T I S T I C S","DISPLAY DATA");
  display_data(pd);
}

void action_three(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();
  std::string filename;
  
  display_header(pd,"S T A T I S T I C S","SAVE DATA");
  if (n==0) {
    std::cout << "There is nothing to save. Please enter or load some data." << std::endl;
  } else {
    std::cout << "There are [" << n << "] values in your project." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    filename=prompt_filename();
    std::cout << "Saving data to file: " << filename << std::endl;
    save_data(filename, pd);
  }
}

void action_four(ProjectData& pd) {
  const std::size_t n = pd.stats_data.size(); // number of entries in stats_data
  std::string filename;                       // Name of file to open
  bool ok_to_continue;                        // If there is existing data, confirm user want to load new data and replace it

  display_header(pd,"S T A T I S T I C S","LOAD DATA");
     
  if (n>0) {
    std::cout << "\nYou have existing data. There are " << n << " values in memory." << std::endl;
  }
  // Check if existing data, warn user
  ok_to_continue=warn_clear_data(pd);
  if (!ok_to_continue) {
    // User aborted load new data to keep existing
    std::cout << "\nYour existing data will be kept in memory." << std::endl;
    return;
  }
  
  // Either no data, or user confirmed OK continue
  
  if (n>0) {
    // Warn user existing data will be cleared
    std::cout << "\nExisting data will be cleared." << std::endl;
  }
  
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  filename=prompt_filename();
  if (filename.empty()) {
    std::cout << "\nNo filename entered." << std::endl;
    return;
  } 
  
  if (load_data(filename, pd)) {
    std::cout << "\nData loaded." << std::endl;
  } else {
    std::cerr << "\nError, no data loaded." << std::endl;
  }
}

void action_five(ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();   // number of entries in stats_data
  double standard_deviation, mean_value;
  
  display_header(pd,"S T A T I S T I C S","CALCULATE");
  
  // Check if there are any values for the calculation
  if (n) {
    std::cout << "\n\nCount of values = " << n << std::endl;
    standard_deviation=stddev(pd);
    std::cout << "\n\nStandard Deviation (Sample) = " << standard_deviation << std::endl;
    mean_value=mean(pd);
    std::cout << "\n\nMean = " << mean_value << std::endl <<std::endl;
  } else {
    std::cout << "Please enter or load some data for the calculations!" << std::endl;
  }
}

void action_six(const ProjectData& pd) {
  const std::size_t n = pd.stats_data.size();   // number of entries in stats_data
  double standard_deviation, mean_value;
  std::string filename;
  
  display_header(pd,"S T A T I S T I C S","PRINT RESULTS");
  
  // Check if there are any values to print
  if (n) {
    // Write results to a text file
    
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    filename=prompt_filename();
    
    std::ofstream outfile(filename);
  
    if (!outfile.is_open()) {
      throw std::runtime_error("Could not open file: " + filename);
    }
  
    // Write description
    outfile << "Statistics Project: " << pd.description << std::endl;
    outfile << "\nData:" << std::endl;
    
    // Write data
    for (size_t i = 0; i < pd.stats_data.size(); ++i) {
      outfile << "[" << i+1 << "]\t" << pd.stats_label[i] << "\t : " << "\t" << pd.stats_data[i] << std::endl;
    }
    outfile << std::endl;
    
    // Write statistics results
    outfile << "\nResults:" << std::endl;
    outfile << "Count of values = " << n << std::endl;
    standard_deviation=stddev(pd);
    outfile << "Standard Deviation (Sample) = " << standard_deviation << std::endl;
    mean_value=mean(pd);
    outfile << "Mean = " << mean_value << std::endl <<std::endl;
    outfile.close(); 
    
    std::cout << "\nYour results were printed to file: " << filename << std::endl;
    
  } else {
    std::cout << "Please enter or load some data for the calculations!" << std::endl;
  }
}

bool action_quit(const ProjectData& pd) {
  bool confirm_quit=false;    // check user sure they want to quit!
  display_header(pd,"S T A T I S T I C S","EXIT");
  
  // Double-check user really wants to quit program
  confirm_quit=prompt_confirm_quit(pd);
  return confirm_quit;
}

// Check menu option selected and execute it
bool process_option(int p_option, ProjectData& pd) {
  bool quit=false;
  
  switch (p_option) {
    case 1:
      action_one(pd);
      break;
    case 2:
      action_two(pd);
      break;
    case 3:
      action_three(pd);
      break;
    case 4:
      action_four(pd);
      break;
    case 5:
      action_five(pd);
      break;
    case 6:
      action_six(pd);
      break;
    case 99:                    // Quit, exit program
      quit=action_quit(pd);
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
  bool exit_menu=false;
  bool data_loaded=false;
  int option;

  // Create the struct and initialize
  ProjectData pd;
  pd.description = "No project data entered";     // header information
  pd.stats_label = {};                            // data labels initialized as empty   
  pd.stats_data = {};                             // data values initialized as empty
  
  // simple help
  if (argc > 1 && (args[1] == "-h" || args[1] == "--help")) {
    std::cout << "Usage: stats [<filename>]" << std::endl
              << "             [-h | --help]" << std::endl;
    return EXIT_SUCCESS;
  }

  // If filename provided as argument, load data
  if (argc > 1) data_loaded=load_data(args[1], pd);
  
  do {
    display_menu(pd);
    option=prompt_range(std::string("\nPlease select an option."),min,max);
    exit_menu=process_option(option, pd);
  } while (!exit_menu);

  exit_message();
  
  return EXIT_SUCCESS;
}

