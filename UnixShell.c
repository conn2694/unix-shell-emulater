/*			Lab 2 Partial Example			*/



#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

main()

{

char *path, *argv[20], buf[80], n, *p;

int m, status, inword, continu, i;



while(1) {



inword = 0;

p = buf;

m = 0;

continu=0;



printf( "\nshhh> ");

while ( ( n = getchar() ) != '\n'  || continu ) {

	if ( n ==  ' ' ) { 

	    if ( inword ) {

		inword = 0;

		*p++ = 0;

	    } 

	}

	else if ( n == '\n' ) continu = 0;

	else if ( n == '\\' && !inword ) continu = 1;

	else {

	    if ( !inword ) {

		 inword = 1;

		 argv[m++] = p;

		 *p++ = n;

	     }

	     else 

		 *p++ = n;

	}

}	

*p++ = 0;

argv[m] = 0;



if ( strcmp(argv[0],"exit") == 0 ) exit (0);


if ( fork() == 0 )
	
	{
		pid_t pid;
		int r_fd[2];
		int l_fd[2];
	int pipeCount = 0;
	int start[10]; /* Max 10 processes */
	start[0] = 0;
	/* Parse command */
	for (i = 0; i < m; i++) {
		if (strcmp(argv[i], "|") == 0) {
			pipeCount++;
			start[pipeCount] = i+1;
			argv[i] = '\0'; /* turn this on and off to test */
			
		}

	}
	/* so we can count the end if there's no pipes for redirs */
	if (pipeCount == 0) {start[1] = m;}
	/* create processes */
	for (i = 0; i < pipeCount+1; i++) {
		if (pipeCount > 0 && i < pipeCount) { /* Not Last process */
			pipe(r_fd);	
		}
		pid = fork();
		if (pid < 0) {perror("fork failed");}
		
		else if (pid == 0) {
			/* Scan for redirection */
			
			int redirect = 0;
			int j;
			int fileName = 0;
			for (j = start[i]; j < start[i + 1]-1; j++) {
				
				if (strcmp(argv[j], "<") == 0) {
					redirect = 1;
					fileName = j + 1;
					argv[j] = '\0';
					break;
				}	
				else if (strcmp(argv[j], ">") == 0) {
					redirect = 2;
					fileName = j + 1;
					argv[j] = '\0';
					break;	
				}

			       

			}

			/* input redir */
			if (redirect == 1 && i == 0) {
				char n;
				n = open(argv[fileName], O_RDONLY);
				close(0);
				dup(n);
				close(n);
			}

			/* if there are pipes */
			if (pipeCount > 0) {

				/* first process */
				if (i == 0) {
					close(1);
					dup(r_fd[1]);
					close(r_fd[0]);
					close(r_fd[1]);
					
				}
				/* middle process */
				else if (i > 0 && i < pipeCount) {
					/* left pipe */		
					close(0);
					dup(l_fd[0]);
					close(l_fd[0]);
					close(l_fd[1]);
					/* right pipe */
					close(1);
					dup(r_fd[1]);
					close(r_fd[0]);
					close(r_fd[1]);
				} 
				/* last process */
				else {
					close(0);
					dup(l_fd[0]);
					close(l_fd[0]);
					close(l_fd[1]);
				
				}
			}
			/* output redir */
			if (redirect == 2 && i == pipeCount) {
				char n;
				n = creat(argv[fileName], 0755);
				close(1);
				dup(n);
				close(n);

			}
			/* perror("Executing \n"); */ /* for testing */
			execvp(argv[start[i]], &argv[start[i]]);
			perror( " didn't exec \n ");

		}
		else { 
			if (i > 0) {
				close(l_fd[0]);
				close(l_fd[1]);
			}	
			l_fd[0] = r_fd[0];
			l_fd[1] = r_fd[1];
			wait(&status);
		}
			
		
	}



	}



wait(&status);
}

}

