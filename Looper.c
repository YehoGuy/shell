#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


void handler(int sig)
{
    printf("\nReceived Signal: %s (%d)\n", strsignal(sig), sig);

    if (sig == SIGTSTP)
    {
        // Reinstate the handler for SIGCONT before propagating SIGTSTP
        signal(SIGCONT, handler);
    }
    else if (sig == SIGCONT)
    {
        // Reinstate the handler for SIGTSTP after SIGCONT
        signal(SIGTSTP, handler);
    }

    // Propagate the signal to the default handler
    signal(sig, SIG_DFL);
    raise(sig);
}


int main() {
    
    signal(SIGTSTP, handler); // Handle SIGTSTP (Ctrl+Z)
    signal(SIGINT, handler);  // Handle SIGINT (Ctrl+C)
    signal(SIGCONT, handler); // Handle SIGCONT (fg or continue)

    // Infinite loop to simulate a looper program
    while (1) {
        sleep(1); // Sleep for 1 second to avoid busy-waiting
    }

    return 0; // Program should never reach this point
}
