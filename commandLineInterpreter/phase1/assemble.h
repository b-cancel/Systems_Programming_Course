#pragma once

//prototypes
void pass1(); //for pass1 func in pass1.c
void pass2(); //for pass2 func in pass1.c TODO place in pass2.c

void assembleCommand(char* filename) {

	printf("running assemble command\n");
	
	pass1();//seperate proc for pass 1 to create symbol table and intermediate file

	pass2();//seperate proc for pass 2 to create object file and listing file
}