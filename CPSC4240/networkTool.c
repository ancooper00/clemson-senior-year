#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void checkHostName(int hostname) {
	if (hostname == -1) {
		perror("gethostname");
		exit(1);
	}
}

void checkHostEntry(struct hostent * hostentry) {
	if (hostentry == NULL) {
		perror("gethostbyname");
		exit(1);
	}
}

void checkIPbuffer(char *IPbuffer) {
	if (NULL == IPbuffer) {
		perror("inet_ntoa");
		exit(1);
	}
}

int main(void){

    /* GET USER IP ADDRESS */
    char hostbuffer[256];
	char *IPbuffer;
	struct hostent *host_entry;
	int hostname;

	hostname = gethostname(hostbuffer, sizeof(hostbuffer));
	checkHostName(hostname);

	host_entry = gethostbyname(hostbuffer);
	checkHostEntry(host_entry);

	IPbuffer = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

    /* MAIN PROGRAM */
    char userChoice; 
    char printChoice;
    char command[50];
    char commandWithPrint[75];
    
    printf("Welcome to our network device tracking tool.\n");
    printf("--------------------------------------------\n\n");

    while(userChoice != 'Q'){
        //ask user if they want to save to file 
        printf("Would you like to save the results of the network scan to a file? Please type 'Y' for yes and 'N' for no.\n");
        scanf(" %c", &printChoice);

        //print out choice options for users to select 
        printf("\nWhat type of summary report would you like to generate?\n");
        
        //description of modes
        printf("- Summarized: This scan will return a list of live IP addresses \n");
        printf("      on your network along with the manufacturer information for the device if available.\n");
        printf("      To use this mode, enter 'S'\n\n");

        printf("- Aggressive: This scan will try to aggressively get more information out of the device. \n");
        printf("      This scan will including operating system detection, version detection, script scanning,\n");
        printf("      and traceroute detection.\n");
        printf("      To use this mode, enter 'A'\n\n");

        printf("- ACK Scan: This scan will help differentiate between statefull and stateless firewalls.\n");
        printf("      The scan report will show open and closed ports with a lable of filtered or unfiltered, \n");
        printf("      with unfilitered meaning that the port was reachable by an ACK packet. If the port is labeled\n");
        printf("      as filtered, a firewall is preventing the reach to the port.\n");
        printf("      To use this mode, enter 'F'\n\n");

        printf("- Grep output: This scan will provide you with a grepable output. The format lists each host on one line\n");
        printf("      and can be searched with standard Unix tools such as grep, awk, cut, etc. \n");
        printf("      To use this mode, enter 'G'\n\n");

        printf("- Verbose: This scan will produce an output with a hgiher verbosity level. \n");
        printf("      It will print information on the scan process. \n");
        printf("      To use this mode, enter 'V'\n\n");

        printf("To quit the program, enter 'Q'\n\n");

        printf("Enter choice: \n");
        scanf(" %c", &userChoice);

        //do action of user choice
        switch(userChoice){
            case 'S':
                strcpy(command, "sudo nmap -sn ");
                strcat(command, IPbuffer);

                if(printChoice == 'Y'){
                    strcpy(commandWithPrint, "sudo nmap -sn ");
                    strcat(commandWithPrint, IPbuffer);
                    strcat(commandWithPrint, " > results.txt");

                    system(commandWithPrint);
                    printf("Printing results to terminal and file called 'results.txt'\n");
                    system(command);
                }
                else{
                    printf("Printing results:\n");
                    system(command);
                }

                break;
            case 'A':
                strcpy(command, "sudo nmap -A ");
                strcat(command, IPbuffer);

                if(printChoice == 'Y'){
                    strcpy(commandWithPrint, "sudo nmap -A ");
                    strcat(commandWithPrint, IPbuffer);
                    strcat(commandWithPrint, " > results.txt");

                    system(commandWithPrint);
                    printf("Printing results to terminal and file called 'results.txt'\n");
                    system(command);
                }
                else{
                    printf("Printing results:\n");
                    system(command);
                }
                break;
            case 'F':
                strcpy(command, "sudo nmap -sA ");
                strcat(command, IPbuffer);

                if(printChoice == 'Y'){
                    strcpy(commandWithPrint, "sudo nmap -sA ");
                    strcat(commandWithPrint, IPbuffer);
                    strcat(commandWithPrint, " > results.txt");

                    system(commandWithPrint);
                    printf("Printing results to terminal and file called 'results.txt'\n");
                    system(command);
                }
                else{
                    printf("Printing results:\n");
                    system(command);
                }
                break;
            case 'G':       
                strcpy(command, "sudo nmap -oG ");
                strcat(command, IPbuffer);

                if(printChoice == 'Y'){
                    strcpy(commandWithPrint, "sudo nmap -oG ");
                    strcat(commandWithPrint, IPbuffer);
                    strcat(commandWithPrint, " > results.txt");

                    system(commandWithPrint);
                    printf("Printing results to terminal and file called 'results.txt'\n");
                    system(command);
                }
                else{
                    printf("Printing results:\n");
                    system(command);
                }
                break;
            case 'V':
                strcpy(command, "sudo nmap -v ");
                strcat(command, IPbuffer);

                if(printChoice == 'Y'){
                    strcpy(commandWithPrint, "sudo nmap -v ");
                    strcat(commandWithPrint, IPbuffer);
                    strcat(commandWithPrint, " > results.txt");

                    system(commandWithPrint);
                    printf("Printing results to terminal and file called 'results.txt'\n");
                    system(command);
                }
                else{
                    printf("Printing results:\n");
                    system(command);
                }
                break;
            default:
                printf("Invalid option entered. Please enter the correct option.\n");
                scanf(" %c", &userChoice)
                break;
        }

        //prompt to ask user if they would like to make another report
        printf("Would you like to generate another report? If you wish to quit enter 'Q', other wise press any key to continue\n");
        scanf(" %c", &userChoice);
    }

    return 0;

}