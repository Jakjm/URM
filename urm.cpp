#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "urm.h"

//C++ program written for practice.
//This program should - when complete - allow the execution of URM programs.
//The Unbounded Register Machine is a theorhetical programming language created by Sheperdson and Sturgis.
//Hopefully this project should allow students to try simple URM programs to understand the concept a little better. 
using namespace std;

//Abstract function class..
class Instruction{
	public :
	int variable; //The index of the variable.
	//Constructor for an instruction object. 
	Instruction(int variable){
		this->variable = variable;
	}
	virtual void execute(URM program) = 0;

};
//For instructions of the form
//L: xi <- xi + 1
class IncrementInstruction : Instruction{
	IncrementInstruction(int variable) : Instruction(variable){

	}
	void execute(URM *program){
		++program->vars[variable];
	}
};

//For instructions of the form
//L: xi <- xi - 1
class DecrementInstruction : Instruction{
	DecrementInstruction(int variable) : Instruction(variable){
	
	}
	void execute(URM *program){
		if(program->vars[variable] > 0){
			--program->vars[variable];
		}
	}

};

//For instructions of the form
//L: xi <- a 
class AssignmentInstruction : Instruction{
	int value; //Value that the variable should be assigned to.
	AssignmentInstruction(int variable,int value) : Instruction(variable){
		this->value = value;
	}
	void execute(URM *program){
		program->vars[variable] = value;
	}
};

//For instructions of the form
//L: if xi = 0 goto X else goto Y
class ConditionalGotoInstruction : Instruction{
	int X; /*The index of the instruction to jump to if the variable is 0.*/
	int Y; /*The index of the instruction to jump to if the variable is not 0.*/
	ConditionalGotoInstruction(int variable, int jump1, int jump2) : Instruction(variable){
		this->X = jump1;
		this->Y = jump2;
	}
	void execute(URM *program){
		if(program->vars[variable] == 0){
			program->programCounter = X;
		}
		else{
			program->programCounter = Y;
		}
	}
};
URM::URM(){
	programCounter = 0;
}
Instruction *readInstruction(ifstream file,int expectedLabel){
	int label;
	char input;
	file >> label;
}
int main(int argc,char **argv){
	if(argc != 2){
		cerr << "Incorrect number of arguments! Please specify 2 arguments.\n";
		return 1;
	}
	else{
		ifstream file(argv[1]);
		if(!file){
			cerr << "Cannot open URM source file.\n";
		}
		vector <Instruction> instructions;
		char ch;
		/*Outputting the file.*/
		//while(file.get(ch)){
		//	cout << ch;
		//}
		
	}
	return 0;
}
