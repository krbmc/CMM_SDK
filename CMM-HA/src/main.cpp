#include <iostream>
#include <thread>
#include "KETI_halmp.hpp"
#include "KETIHA.hpp"
#define ConfigurationFileDir "/redfish/CMMHA.conf" 
#define CONFIG_LINE_LEN 100
using namespace std;
using namespace KETIHA_NSP;
using namespace std::chrono;
#define PRIMEADDR "primeaddr"
#define SECONDADDR "secondaddr"
#define PRIMEPORT "primeport"
#define SECONDPORT "secondport"
#define NETWORKTIMEOUT "networktimeout"
#define HEARTBEAT "heartbeat"

char* primeaddr;
char* secondaddr;
int primeport,secondport,networktimeout,heartbeat;

/* used by ReadConfigurationFile, check the line if it's valuable*/
/* This file refer to the watchdog version 5.5*/
static int spool(char *line, int *i, int offset)
{
	for ((*i) += offset; line[*i] == ' ' || line[*i] == '\t'; (*i)++)
		;
	if (line[*i] == '=')
		(*i)++;
	for (; line[*i] == ' ' || line[*i] == '\t'; (*i)++)
		;
	if (line[*i] == '\0')
		return (1);
	else
		return (0);
}
static int ReadConfigurationFile(char *file)
{
	FILE *ReadConfigurationFile;

	/* Open the configuration file with readonly parameter*/
	printf("Trying the configuration file %s \n", ConfigurationFileDir);
	if ((ReadConfigurationFile = fopen(ConfigurationFileDir, "r")) == NULL)
	{
		printf("There is no configuration file, use default values for CMM HA \n");
		return (1);
	}

	/* Check to see the configuration has data or not*/
	while (!feof(ReadConfigurationFile))
	{
		char Configurationline[CONFIG_LINE_LEN];

		/* Read the line from configuration file */
		if (fgets(Configurationline, CONFIG_LINE_LEN, ReadConfigurationFile) == NULL)
		{
			if (!ferror(ReadConfigurationFile))
			{
				break;
			}
			else
			{
				return (1);
			}
		}
		else
		{
			int i, j;

			/* scan the actual line for an option , first remove the leading blanks*/
			for (i = 0; Configurationline[i] == ' ' || Configurationline[i] == '\t'; i++)
				;

			/* if the next sign is a '#' we have a comment , so we ignore the configuration line */
			if (Configurationline[i] == '#')
			{
				continue;
			}

			/* also remove the trailing blanks and the \n */
			for (j = strlen(Configurationline) - 1; Configurationline[j] == ' ' || Configurationline[j] == '\t' || Configurationline[j] == '\n'; j--)
				;

			Configurationline[j + 1] = '\0';

			/* if the line is empty now, we don't have to parse it */
			if (strlen(Configurationline + i) == 0)
			{
				continue;
			}

			/* now check for an option , interval first */

			/*PRIMEADDR */
			if (strncmp(Configurationline + i, PRIMEADDR, strlen(PRIMEADDR)) == 0)
			{
				if (spool(Configurationline, &i, strlen(PRIMEADDR)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
					primeaddr = strdup(Configurationline + i);

					{
						printf(" PRIMEADDR = %s \n", primeaddr);
					}
				}
			}

			/*SECONDADDR */
			else if (strncmp(Configurationline + i, SECONDADDR, strlen(SECONDADDR)) == 0)
			{
				if (spool(Configurationline, &i, strlen(SECONDADDR)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
                    secondaddr = strdup(Configurationline + i);

					{
						printf(" SECONDADDR = %s \n", secondaddr);
					}
				}
			}

			/*PRIMEPORT */
			else if (strncmp(Configurationline + i, PRIMEPORT, strlen(PRIMEPORT)) == 0)
			{
				if (spool(Configurationline, &i, strlen(PRIMEPORT)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
					primeport = atol(Configurationline + i);
					printf(" primeport = %d \n", primeport);
				}
			}

			/*SECONDPORT */
			else if (strncmp(Configurationline + i, SECONDPORT, strlen(SECONDPORT)) == 0)
			{
				if (spool(Configurationline, &i, strlen(SECONDPORT)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
					secondport = atol(Configurationline + i);
					printf(" secondport = %d \n", secondport);
				}
			}
            
            /*Network TimeOut */
			else if (strncmp(Configurationline + i, NETWORKTIMEOUT, strlen(NETWORKTIMEOUT)) == 0)
			{
				if (spool(Configurationline, &i, strlen(NETWORKTIMEOUT)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
					networktimeout = atol(Configurationline + i);
					printf(" networktimeout = %d \n", networktimeout);
				}
			}
            /*Heartbeat */
			else if (strncmp(Configurationline + i, HEARTBEAT, strlen(HEARTBEAT)) == 0)
			{
				if (spool(Configurationline, &i, strlen(HEARTBEAT)))
				{
					fprintf(stderr, "Ignoring invalid line in config file:\n%s\n", Configurationline);
				}
				else
				{
					heartbeat = atol(Configurationline + i);
					printf(" HEARTBEAT = %d \n", heartbeat);
				}
			}
			else
			{
				fprintf(stderr, "Ignoring config Configurationline: %s\n", Configurationline);
			}
		}
	}

	/* Close the configuration file */
	if (fclose(ReadConfigurationFile) != 0)
	{
		return (1);
	}
}

int main(void)
{
    
    auto ha = GetKetihaIf();
    ReadConfigurationFile(ConfigurationFileDir);
	string pa(primeaddr);
	string sa(secondaddr);
	cout <<"Primary Server IP Addr : "<<pa<<endl;
	cout <<"Secondary Server IP Addr : "<<sa<<endl;
    auto rc = ha->Initialize(pa, sa,primeport , secondport,
    chrono::milliseconds(networktimeout), chrono::seconds(heartbeat), chrono::seconds(networktimeout), false);
    // for (int i = 0; i < 3; i++)
    // {
    //     this_thread::sleep_for(chrono::seconds(1));
    // }
    //ha->Switch();
    
    //mainserver();
    while(true)
     pause();
    return 0;
}