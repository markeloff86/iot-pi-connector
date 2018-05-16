#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int PATHSIZE = 255;
int SIZE = 8;
char cputemploc[255] = "/sys/class/thermal/thermal_zone0/temp";
char cpuloadloc[255] = "/proc/loadavg";

float getCPUTemp();
float GetCPULoad();

float getCPUTemp() {
	FILE * cputemp = NULL;
	char buffer [SIZE];
	long tempinmillic;
	float tempinc;

	memset(buffer, 0, sizeof(buffer));
	cputemp = fopen(cputemploc, "r");

	fgets(buffer, SIZE, cputemp);

	tempinmillic = atol(buffer);
	tempinc = tempinmillic * 1.0 / 1000.0;

	fclose(cputemp);
	return tempinc;
}

float GetCPULoad() {
	
	FILE *f1;
        float load1,load5,load15;

        f1 = fopen(cpuloadloc, "r");
        fscanf(f1, "%f\t%f\t%f\t", &load1, &load5, &load15 );
	fclose(f1);
        return (load1);

}
