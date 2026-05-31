# statistics repository

# Purpose
An application to calculate statistics for sets of data entered by the user.

# Description of functions

## Enter data
Enter raw data for the statistics calculations.

## Save data as CSV
Save data to a CSV file.
The format is the same as used in the oracle-appsdemo statistics application.
You can exchange between this application and Oracle.
You will be able to export data from Oracle as a csv file and load it in this application,
then calculate stats.
You can also import CSV files created by this application into Oracle.

### CSV file format
PROJECT,<Description>
[<data-item-label>,<number>]

e.g.
PROJECT,PL/SQL Exam Results
Fred,90
Jim,93
Ann,97
Ian,98
Steve F,100
Bruce Scott,99
Tiger,95
Mary,91
Connor McD,100
Seyi,96
Harsh,98
Peter,89
Doris,85
Winston,92
Charlie,91
Morse,90

## Load data from CSV
Load statistics raw data from a CSV file.
The CSV files share the same format as in the Oracle stats app, so you can import data from Oracle,
or create your own CSV files either with this app, or using a text editor.

## Calculate statistics
Check that data has been loaded, display error message if no data.
