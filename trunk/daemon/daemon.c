#include "daemon.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

#include <signal.h>

daemonize_status do_daemonize(void)
{
   /*
    * Our process ID and Session ID.
    */
   pid_t pid = -1;
   pid_t sid = -1;

   /*
    * Fork off the parent process.
    */
   pid = fork();

   /*
    * If we got a good PID, then
    * we can exit the parent process.
    */

   if (pid > 0)
      {
         return PID_OK;
      }

   /* Change the file mode mask. */
   // Clear umask and check return value which is old umask
   mode_t mask = umask(0);
   if(mask == 0)
      {
         // Mask is 0, set to 022 instead... Will make dirs have 0755 and files 0644.
         umask(022);
      }
   else
      {
         // There was some mask already, set it again
         umask(mask);
      }

   /*
    * Create a new SID for the child process.
    */
   sid = setsid();
   if (sid < 0)
      {
         /* Log the failure. */
         return SETID_ERR;
      }

   /*
    * Change the current working directory.
    */
   if ((chdir("/")) < 0)
      {
         /* Log the failure. */
         return CHDIR_ERR;
      }

   /* Close out the standard file descriptors. */
   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);

   return PID_ERR;
}

extern int global_btg_run;
extern int global_btg_hup;

/* React on the signals from the outside world. */
void global_signal_handler(int signal_no)
{
   switch (signal_no)
      {
      case SIGINT:
         {
            global_btg_run = 0;
            // fprintf(stdout, "%s", "User interrupted.\n");
            break;
         }
      case SIGTERM:
         {
            global_btg_run = 0;
            // fprintf(stdout, "%s", "Killed.\n");
            break;
         }
      case SIGHUP:
         {
            global_btg_hup = 1;
            break;
         }
      }
}

