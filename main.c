#include "header.h"

int main(void)
{
	static FitbitData arrFitbit[1440] = { 0 }; // static and 0 initilization to help with large memory warnings ( !! looked this up !! )

	char targetPatient[10] = "12cx7";

	char buffer[100];
	int i = 0;

	FILE* inputStream = fopen("FitbitData.csv", "r");
	if (inputStream == NULL)
	{
		perror("error opening file");
		return 1;
	}

	fgets(buffer, sizeof(buffer), inputStream);
	char* tok = strtok(buffer, ",");
	tok = strtok(NULL, ",");
	strcpy(targetPatient, tok); // first line .. getting target patient
	targetPatient[strcspn(targetPatient, "\r\n")] = '\0'; // trim newline and csv newline off targ patient if needed

	fgets(buffer, sizeof(buffer), inputStream); // second line and discard 
	
	while (fgets(buffer, sizeof(buffer), inputStream) != NULL)
	{
		if (i >= 1440)break;

		char* fields[8]; // helps with strtok problem with empty fields
	
		splitter(buffer, fields, 8);

		if (strcmp(fields[0], targetPatient) != 0)
			continue; // skip patient, not our target

		int duplicate = 0; //de duping set to false
		for (int j = 0; j < i; j++)
		{
			if (strcmp(arrFitbit[j].patient, fields[0]) == 0 && strcmp(arrFitbit[j].minute, fields[1]) == 0)
			{
				duplicate = 1; // set to true
				break;
			}
		}

		if (duplicate == 1) continue; // skips duplicate data

		//store data
		strcpy(arrFitbit[i].patient, fields[0]); 
		strcpy(arrFitbit[i].minute, fields[1]);

		if (fields[2][0] == '\0') arrFitbit[i].calories = -1.0;
		else arrFitbit[i].calories = atof(fields[2]);

		if (fields[3][0] == '\0') arrFitbit[i].distance = -1.0;
		else arrFitbit[i].distance = atof(fields[3]);
		
		if (fields[4][0] == '\0') arrFitbit[i].floors = 999;
		else arrFitbit[i].floors = atoi(fields[4]);

		if (fields[5][0] == '\0') arrFitbit[i].heartRate = 999;
		else arrFitbit[i].heartRate = atoi(fields[5]);

		if (fields[6][0] == '\0') arrFitbit[i].steps = 999;
		else arrFitbit[i].steps = atoi(fields[6]);

		if (fields[7][0] == '\0') arrFitbit[i].sleepLevel = NONE;
		else arrFitbit[i].sleepLevel = (Sleep)atoi(fields[7]);
		
		i++;
	}
	 // calculations etc
	double totalCalories = 0.0;
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].calories != -1.0) totalCalories += arrFitbit[j].calories;
	}

	double totalDistance = 0.0;
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].distance != -1.0) totalDistance += arrFitbit[j].distance;
	}

	unsigned int totalFloors = 0;
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].floors != 999) totalFloors += arrFitbit[j].floors;
	}

	unsigned int totalSteps = 0;
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].steps != 999) totalSteps += arrFitbit[j].steps;
	}

	unsigned int totalHR = 0;
	int countHR = 0; // using a count function to find total number of REAL values (not 999 filler values)
	double avgHR = 0.0;
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].heartRate != 999)
		{
			totalHR += arrFitbit[j].heartRate;
			countHR++;
		}
	}
	if (countHR > 0) avgHR = (double)totalHR / (double)countHR;
	
	unsigned int maxSteps = 0;
	char maxMin[9] = "";
	for (int j = 0; j < i; j++)
	{
		if (arrFitbit[j].steps == 999) continue; //skipping 999 filler value

		if (arrFitbit[j].steps > maxSteps || (arrFitbit[j].steps == maxSteps && strcmp(arrFitbit[j].minute, maxMin) > 0))
		{
			maxSteps = arrFitbit[j].steps;
			strcpy(maxMin, arrFitbit[j].minute);
		}
	}

	int bestSum = 0;
	char bestStart[9] = "";
	char bestEnd[9] = "";

	int curSum = 0;
	char curStart[9] = "";

	for (int j = 0; j < i; j++)
	{
		int hour = getHour(arrFitbit[j].minute); // takes the atoi for the data from the csv, giving us the hour as a number
		//sleep window between hour 21 and hour 8 ...
		int sleepWindow = (hour >= 21 || hour < 8); // gives true or false

		if (sleepWindow && arrFitbit[j].sleepLevel > 1)
		{
			if (curSum == 0) 
			{
				strcpy(curStart, arrFitbit[j].minute);
			}
			curSum += arrFitbit[j].sleepLevel;
		}
		else
		{
			if (curSum > bestSum)
			{
				bestSum = curSum;
				strcpy(bestStart, curStart);
				strcpy(bestEnd, arrFitbit[j - 1].minute);
			}
			curSum = 0;
		}
	}
	
	if (curSum > bestSum)
	{
		bestSum = curSum;
		strcpy(bestStart, curStart);
		strcpy(bestEnd, arrFitbit[i - 1].minute);
	}

	printf("Total Calories,Total Distance,Total Floors,Total Steps,Avg Heartrate,Max Steps,Sleep\n");
	printf("%.2lf,%.2lf,%u,%u,%.2lf,%u,%s:%s\n", totalCalories, totalDistance, totalFloors, totalSteps, avgHR, maxSteps, bestStart, bestEnd);


	FILE* outputStream = fopen("Results.csv", "w");
	if (outputStream == NULL)
	{
		perror("error opening file");
		fclose(inputStream);
		return 1;
	}

	fprintf(outputStream, "Total Calories,Total Distance,Total Floors,Total Steps,Avg Heartrate,Max Steps,Sleep\n");
	fprintf(outputStream, "%.2lf,%.2lf,%u,%u,%.2lf,%u,%s:%s\n", totalCalories, totalDistance, totalFloors, totalSteps, avgHR, maxSteps, bestStart, bestEnd);

	for (int j = 0; j < i; j++)
	{
		fprintf(outputStream, "%s,%s,%lf,%lf,%u,%u,%u,%d\n", arrFitbit[j].patient, arrFitbit[j].minute, arrFitbit[j].calories, arrFitbit[j].distance, arrFitbit[j].floors, arrFitbit[j].heartRate, arrFitbit[j].steps, arrFitbit[j].sleepLevel);
	}

	fclose(inputStream);
	fclose(outputStream);


	return 0;
}