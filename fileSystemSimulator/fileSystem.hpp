#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <utility>
using namespace std ;

struct fileSysNode ;
typedef pair<fileSysNode*,string> NODE ; 




typedef enum
{
	directory, file

} nodeType ;


typedef enum
{
	LS, MKDIR, WRITE, READ, RMDIR, RM, CD, EXIT, UNKNOWN 

} cmdType ;


struct fileSysNode
{
	string name ;
	nodeType type ;
	unordered_map<string, fileSysNode*> children ;
	fileSysNode* parent ;
	string content ;
	fileSysNode(string n, nodeType t): name(n), type(t), parent(NULL), content("") {}
} ;


class fileSystem
{
	public:

		fileSystem() ;

		cmdType commandTypeSelector(string) ;

		void cmdExecutor(string) ;

		void dirLister(string) ;

		NODE dirNodeSeacher(string) ;

		void dirMaker(string) ;

		void fileWriter(string) ;

		void fileReader(string) ;

		void changeDir(string) ;

		void fileRemover(string) ;

		void dirRemover(string) ;

		void shell() ;


	private:
		fileSysNode* root ;
		fileSysNode* curNode ;
			
} ;


#endif