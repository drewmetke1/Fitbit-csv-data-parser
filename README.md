# Fitbit CSV Analyzer (C)

## Overview
A C program that reads Fitbit-style time-series data from a CSV file, filters for a target patient, removes duplicate records, computes summary metrics, and writes results to an output CSV.

## Features
- CSV parsing that handles empty fields
- Filters rows by patient ID
- De-duplicates records by (patient, minute)
- Stores up to 1440 minute-level samples
- Computes:
  - Total calories, total distance, total floors, total steps
  - Average heart rate (ignores missing values)
  - Max steps and the minute it occurred
  - Best sleep interval within a nightly window

## Concepts Practiced
- File I/O (`fopen`, `fgets`, `fprintf`)
- String handling (`strtok`, `strcmp`, `strcpy`)
- Struct arrays and indexing
- Defensive handling of missing data
- Basic algorithmic logic (deduping, max search, window tracking)

## How to Run
1. Place `FitbitData.csv` in the project folder.
2. Compile:
   - `gcc main.c -o fitbit`
3. Run:
   - `./fitbit`
4. Output will be written to `Results.csv`.
