#include <iostream>
#include <string>
#include <vector>
#include <sstream>
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
	//Constructor for an instruction object. 
	Instruction(){
	}
};
class V_Instruction : public Instruction{
	public :
	int variable;
	V_Instruction(int variable) : Instruction(){
		this->variable = variable;
	}
	virtual void execute(URM *program) = 0;
};
//For instructions of the form
//L: xi <- xi + 1
class IncrementInstruction : public V_Instruction{
	public:
	IncrementInstruction(int variable) : V_Instruction(variable){

	}
	void execute(URM *program) override{
		++program->vars[variable];
	}
};

//For instructions of the form
//L: xi <- xi - 1
class DecrementInstruction : public V_Instruction{
	public:
	DecrementInstruction(int variable) : V_Instruction(variable){
	
	}
	void execute(URM *program) override{
		if(program->vars[variable] > 0){
			--program->vars[variable];
		}
	}

};

//For instructions of the form
//L: xi <- a 
class AssignmentInstruction : public V_Instruction{
	public:
	int value; //Value that the variable should be assigned to.
	AssignmentInstruction(int variable,int value) : V_Instruction(variable){
		this->value = value;
	}
	void execute(URM *program) override{
		program->vars[variable] = value;
	}
};

//For instructions of the form
//L: if xi = 0 goto X else goto Y
class ConditionalGotoInstruction : public V_Instruction{
	public:
	int X; /*The index of the instruction to jump to if the variable is 0.*/
	int Y; /*The index of the instruction to jump to if the variable is not 0.*/
	ConditionalGotoInstruction(int variable, int jump1, int jump2) : V_Instruction(variable){
		this->X = jump1;
		this->Y = jump2;
	}
	void execute(URM *program) override{
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
//Reads a URM variable from the given token.
int readVariable(string *token){
	int t_index = 1;
	//All urm variables must start with X.
	if(token->at(0) != 'X'){
		return -1;
	}
	//Read until we exceed the token length, or the current character element isn't a '1'.
	while(t_index < token->length() && token->at(t_index) == '1')++t_index;
	//If one of the characters after X wasn't a 1, we should return -1 because the variable is incorrectly formatted. 
	if(t_index < token->length())return -1;
	
	//Otherwise, return the number of 1s in the token, which is t_index - 2 
	return t_index - 2; 
}
//Function to read up to a specific character from the stream.
int readCharacter(ifstream *file,char c){
	int input = file->get();
	//While the character is not equal to the desired character,
	while(input != c){
		//Return an error if the character is not a whitespace character.
		if(!isspace(input)){
			return 0;
		}
		input = file->get();
	}
	return 1; //Return 1 since we read the character correctly. 
}
//Function to read an instruction from an input file stream.
//The label of the instruction must match the expected label.
//maxLabel is the value of the maximum goto label in our program.
Instruction *readInstruction(ifstream &file,int expectedLabel,int *maxGoto){
	int label;
	char input;
	string token;
	//If we can't read a label from the file or if it's not what we were expecting, return NULL.
	if(!(file >> label) || label != expectedLabel){
		cerr << "Error with instruction " << expectedLabel << ". Couldn't read the label.\n";
		return NULL;
	}
	//If we can't read a colon after the label or we can't read a token after the colon, this is not a valid instruction.
	if(readCharacter(&file,':') == false || !(file >> token)){
		cerr << "Error with instruction " << expectedLabel;
		return NULL;
	}
		
	Instruction *i;
	if(token == "stop"){ //Stop instruction.
		i = new Instruction();
		return i;
	}
	else if(token == "if"){ //Conditional goto instruction.
		//Read the variable for the conditional goto.
		if(!(file >> token))return NULL;
		
		int variable = readVariable(&token);
		int label1, label2; 
		//If the variable is not valid, or we can't read an '='
		if(variable == -1 || !readCharacter(&file,'='))return NULL;
		if(!(file >> token) || token != "goto")return NULL;
		//Read the first label from the conditional goto.
		
		//Ensure that 'else' is the next token.
		if(!(file >> label1) || !(file >> token) || token != "else") return NULL;
		
		//Ensure that 'goto' is the next token.
		if(!(file >> token) || token != "goto")return NULL;

		//Read the second label from the conditional goto. 
		if(!(file >> label2) )return NULL;
		
		//Ensure that the labels are valid!
		if(label1 < 1 || label2 < 1){
			return NULL;
		}
		//If our labels are the furthest gotos in the program, update the maxGoto 
		//(so that we can catch if the program tries to jump out of bounds).
		if(label1 > *maxGoto){
			*maxGoto = label1;
		}
		if(label2 > *maxGoto){
			*maxGoto = label2;
		}
		//The actual instruction object will hold label1-1, label2-1 
		//since our internal indices of instructions start from 0, not 1 like URMs.
		i = new ConditionalGotoInstruction(variable,label1-1,label2-1);
	}
	else{//Assignment, increment or decrement instruction. 
		//Read the variable for the instruction.
		int variable = readVariable(&token);
		if(variable == -1)return NULL;

		//Ensure that the next token is a <- 
		if(!readCharacter(&file,'<') || !readCharacter(&file,'-')) return NULL;
		//Read a token - it'll either be a variable (e.g. X111) or a constant (e.g. 5). 
		if(!(file >> token))return NULL;
		if(variable == readVariable(&token)){ //Increment or decrement instruction.
			if(!(file >> token))return NULL;
			if(token == "+"){ //Increment instruction
				i = new IncrementInstruction(variable);
			}
			else if(token == "-"){//Decrement instruction
				i = new DecrementInstruction(variable);
			}
			else{//Incorrect instruction.
				return NULL;
			}

			//Ensure that a 1 comes after the '+' or '-' though!
			if(!readCharacter(&file,'1'))return NULL;
		}
		else{//Assignment instruction.
			int constant;
			stringstream tokenStream(token);
			//Read a constant - ensure that we used the ENTIRE stringstream. 
			if(!(tokenStream >> constant) || tokenStream.rdbuf()->in_avail() != 0)return NULL; 
			
			//If the constant < 0, this is an illegal URM.
			if(constant < 0){
				return NULL;
			}//Otherwise, we have everything we need for the instruction!
			i = new AssignmentInstruction(variable,constant);
		}
	}
	return i;
}
int main(int argc,char **argv){
	if(argc != 2){
		cerr << "Incorrect number of arguments! Please specify 2 arguments.\n";
		return 1;
	}
	else{
		int maxGoto = 1;
		int currentLabel = 1;
		ifstream file(argv[1]);
		if(!file){
			cerr << "Cannot open URM source file.\n";
		}
		Instruction *i = readInstruction(file,currentLabel,&maxGoto);
		Instruction *dummy = new Instruction();
		cout << "Type dummy: " << typeid(dummy).name() << " type i: " << typeid(i).name() << '\n';
		while(i != NULL && typeid(i) != typeid(dummy)){
			cout << currentLabel << " ";
			++currentLabel;
			i = readInstruction(file,currentLabel,&maxGoto);
		}
		cout << "\n";
		if(i == NULL){
			cerr <<"Problem with URM stored in " << argv[1] << '\n';
			cerr << "Oops! One of your instructions doesn't seem to be in the right format.\n";
		}
		else{
			if(maxGoto > currentLabel){
				cerr << "URM has a goto jump that exceeds the stop instruction. Be careful, and please rewrite the URM.\n";
			}
			else{
				cout << "Looks like this is a syntactically correct URM!\n";
			}
		}
	}
	return 0;
}
