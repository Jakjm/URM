#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <map>
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
	virtual ~Instruction(){}
};
class V_Instruction : public Instruction{
	public :
	int variable;
	V_Instruction(int variable) : Instruction(){
		this->variable = variable;
	}
	virtual int execute(URM *program) = 0;
};
//For instructions of the form
//L: xi <- xi + 1
class IncrementInstruction : public V_Instruction{
	public:
	IncrementInstruction(int variable) : V_Instruction(variable){}
	int execute(URM *program) override{
		int maxValue = 1 << 30 - 1;
		maxValue += 1 << 30;
		//Ensure the variable cannot be increased above the max value.
		if(program->variables->operator[](variable) < maxValue){
			++program->variables->operator[](variable);
		}
		return -1;
	}
};

//For instructions of the form
//L: xi <- xi - 1
class DecrementInstruction : public V_Instruction{
	public:
	DecrementInstruction(int variable) : V_Instruction(variable){
	
	}
	int execute(URM *program) override{
		if(program->variables->operator[](variable) > 0){
			--program->variables->operator[](variable);
		}
		return -1;
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
	int execute(URM *program) override{
		program->variables->operator[](variable) = value;
		return -1;
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
	int execute(URM *program) override{
		if(program->variables->operator[](variable) == 0){
			return X;
		}
		else{
			return Y;
		}
	}
};

ostream& operator <<(ostream& os, URM* u){
	os << "Values of variables after running program:\n";
	map<string,int>::const_iterator it = u->varMap.begin();
	int varIndex,varValue;
	string actualVar;
	while(it != u->varMap.end()){
		actualVar = it->first;
		varIndex = it->second;
		varValue = u->variables->operator[](varIndex);
		os << actualVar << ": " << varValue  << '\n';
		it++;
	}
	return os;
}
URM::URM(vector<V_Instruction*> *instructions,vector <int> *vars,map <string,int> varMap){
	programCounter = 0 ;
	this->instructions = instructions;
	this->variables = vars;
	this->varMap = varMap;
}
void URM::run(){
	V_Instruction *currentInstruction;
	int nextPC;
	while(programCounter < instructions->size()){
		currentInstruction = instructions->operator[](programCounter);
		nextPC = currentInstruction->execute(this);
		if(nextPC >= 0){
			programCounter = nextPC;
		}
		else{
			++programCounter;
		}
	}
}
//Reads a URM variable from the given token.
int readVariable(string *token, map<string,int> &m, int metaURM){
	if(!metaURM){
		//Checking that URM variable syntax is followed. 
		//All urm variables must start with X.
		if(token->length() < 1 || token->at(0) != 'X'){
			return -1;
		}
		//Start from the first character and keep reading 1s. 
		int t_index = 1;
		//Read until we exceed the token length, or the current character element isn't a '1'.
		while(t_index < token->length() && token->at(t_index) == '1')++t_index;

		//We must ensure that the entirety of the token after the X consisted of 1s, and that the token had at least one '1'.
		if(t_index < token->length() || t_index <= 1)return -1;
	}
	//Adjust the variable of the instruction to a naturalized one.
	//If the variable hasn't been mapped to a HT element...
	string str = *token;
	if(m.count(str) == 0){
		m.insert(pair <string,int>(str,(int)m.size()));
		return m.size() - 1;
	}
	//If the variable has been mapped to a naturalized variable, replace the variable with a mapped element.
	else{
		return m[str];
	}
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
Instruction *readInstruction(ifstream &file,int expectedLabel,int *maxGoto, map<string,int> &m,int metaURM){
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
		int variable = readVariable(&token,m,metaURM);
		int label1, label2; 
		
		//If the variable is not valid, or we can't read an '=' then a '0', we should return null.
		if(variable == -1 || !readCharacter(&file,'=') || !readCharacter(&file,'0'))return NULL;
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
		int variable = readVariable(&token,m,metaURM);
		if(variable == -1){
			cerr << "Failed to read variable from token \'" << token << "\' from instruction " << expectedLabel << '\n';
			return NULL;
		}

		//Ensure that the next token is a <- 
		if(!readCharacter(&file,'<') || !readCharacter(&file,'-')){
			cerr << "Variable change instruction missing '<-' from instruction " << expectedLabel << '\n';
			return NULL;
		}
		//Read a token - it'll either be a variable (e.g. X111) or a constant (e.g. 5). 
		if(!(file >> token))return NULL;
		//If the token is stored by the map, and it's associated with the same naturalized variable....
		if(m.count(token) > 0 && m[token] == variable){ //Increment or decrement instruction.
			if(!(file >> token))return NULL;
			if(token == "+"){ //Increment instruction
				i = new IncrementInstruction(variable);
			}
			else if(token == "-"){//Decrement instruction
				i = new DecrementInstruction(variable);
			}
			else{//Incorrect instruction.
				cerr << "Incorrect token: \'" << token << "\' during increment or decrement instruction " << expectedLabel << '\n';
				return NULL;
			}

			//Ensure that a 1 comes after the '+' or '-' though!
			if(!readCharacter(&file,'1'))return NULL;
		}
		else{//Assignment instruction.
			int constant;
			stringstream tokenStream(token);
			//Read a constant - ensure that we used the ENTIRE stringstream. 
			if(!(tokenStream >> constant) || tokenStream.rdbuf()->in_avail() != 0){
				cerr << "Incorrect variable or constant \'" << token << "\' from instruction " << expectedLabel << '\n';
				return NULL;
			}	
			
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
	if(argc < 2){
		cerr << "Incorrect number of arguments! Please specify 2 arguments.\n";
		return 1;
	}
	else{
		int metaURM = 0, maxGoto = 1, currentLabel = 1;
		//Set up an input filestream using the 1st command line argument. 
		ifstream file(argv[1]);
		if(!file){
			cerr << "Cannot open URM source file.\n";
			return 1;
		}

		//Allow the URM to contain metavariables if the --meta or -m argument is used.
		if(argc > 2 && (strcmp(argv[2],"--meta") == 0 || strcmp(argv[2],"-m") == 0)){
			metaURM = 1;
		}

		vector <V_Instruction*> *instructions = new vector<V_Instruction*>();
		//Map used to enumerate URM variable numbers (3, 2, 5, ...) to integers (0, 1, 2, ...)
		map <string,int> m;
		Instruction *i = readInstruction(file,currentLabel,&maxGoto,m,metaURM);
		V_Instruction *variableInstruction = dynamic_cast<V_Instruction *>(i);

		/*Keep reading instructions until the stop instruction is reached.*/
		while(variableInstruction != NULL){
			instructions->push_back(variableInstruction);
			++currentLabel;
			i = readInstruction(file,currentLabel,&maxGoto,m,metaURM);
			variableInstruction = dynamic_cast<V_Instruction *>(i);
		}
		if(i == NULL){
			cerr <<"Problem with URM stored in " << argv[1] << '\n';
			cerr << "Oops! One of your instructions doesn't seem to be in the right format.\n";
			return 1;
		}
		else{
			if(maxGoto > currentLabel){
				cerr << "URM has a goto jump that exceeds the stop instruction. Be careful, and please rewrite the URM.\n";
				return 1;
			}
			else{
				vector <int> *variables = new vector<int>(m.size());
				cout << "Looks like this is a syntactically correct URM!\n";
				URM *program = new URM(instructions,variables,m);
				program->run();
				cout << program;
			}
		}
	}
	return 0;
}
