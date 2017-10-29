Aaron Gordon 0884023

Program is run by typing make and then mpiexec -n x ./a2 where x is the number of characters you wish
to max out with in the program. The program allows for you to go under this number which allows you to
only use some of the processes but still exit out of all of them when the program is finished for ease of
access. This way, if you put in an x value of say 10 then most strings you could come up with will work fine,
but smaller strings will use less processors for its runtime.

If the character number exceeds the number of processors given, then the program will prompt the user for a
new string and not process the given string. The program is used as soon as its opened, and any string inputted
at any time (typing anything will feed it into the process itself but there is no prompt asking for a string).
Spaces etc should stay consistent and the program should display every string that is found that contains only 
words (so stuff like cat asdjalskd will not return) and processes finishing are also displayed so the user
knows when each one is done.

Exit the program by simply typing lowercase q. This will finalize MPI and exit safely.

Tests in characters (no spaces for consistency of timing):
3 - ~0.8s
4 - ~2.9s
5 - ~10.4s
6 - ~40s
At 7 characters and beyond, it goes a bit beyond 1 minute.