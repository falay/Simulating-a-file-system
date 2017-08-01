#include "fileSystem.hpp"

#define FINDBLANK(s)(s.find_first_not_of(" ", s.find_first_of(" ")+1)) 	 


fileSystem::fileSystem()
{
	root = new fileSysNode( "/", directory ) ;	
	curNode = root ;
	root->parent = root ;
}



void fileSystem::dirLister(string cmd)
{
	fileSysNode* dirNode ;
	
	if( cmd.length() == 2 )
		dirNode = curNode ;
	else
	{
		NODE target = dirNodeSeacher( cmd.substr( FINDBLANK(cmd) ) ) ;
		if( (dirNode = target.first) == NULL )
		{
			cerr << "ls: " << target.second << ": No such file or directory\n" ;
			return ;
		}	
	}	

	if( dirNode->type == file )
		cout << dirNode->name << endl ;
	else
	{	
		if( dirNode->children.empty() )
			return ;
		
		for(auto it=dirNode->children.begin(); it!=dirNode->children.end(); ++it)
			cout << it->first << "\t" ;
		cout << endl ;		
	}	
}


NODE fileSystem::dirNodeSeacher(string path)
{
	stringstream SS(path) ;
	string dir ;

	fileSysNode* current = curNode ;
	while( getline(SS, dir, '/') )
	{
		if( dir == "." )
			continue ;

		else if( dir == ".."  )	
		{	
			if( current->parent )
				current = current->parent ;

			continue ;
		}

		else if( current->children.count( dir ) == 0 )
			return make_pair( (fileSysNode*)NULL, dir ) ;

		current = current->children[ dir ] ;
	}	

	return make_pair( current, dir ) ;
}


void fileSystem::dirMaker(string cmd) 
{
	string path2Dir = cmd.substr( FINDBLANK(cmd) ) ;
	string newDir ;
	fileSysNode* current ;
	size_t pos ;
	
	if( (pos = path2Dir.find_last_of("/")) == string::npos )
	{	
		newDir = path2Dir ;
		current = curNode ;
	}	
	else
	{	
		newDir = path2Dir.substr( pos+1 ) ;	
		NODE target = dirNodeSeacher( path2Dir.substr(0, pos) ) ;
		if( (current = target.first) == NULL )
		{
			cerr << "mkdir: " << target.second << ": No such file or directory\n" ;
			return ;
		}	
		if( current->type == file )
		{
			cerr << "mkdir: " << current->name << ": Not a directory\n" ;
			return ;
		}	
	}	

	current->children[ newDir ] = new fileSysNode( newDir, directory ) ;
	current->children[ newDir ]->parent = current ;
}


void fileSystem::changeDir(string cmd)
{
	string newDir = cmd.substr( FINDBLANK(cmd) ) ;

	NODE target = dirNodeSeacher( newDir ) ;
	if( target.first == NULL )
	{
		cerr << "cd: " << target.second << ": No such file or directory\n" ;
		return ;		
	}	
	if( target.first->type == file )
	{
		cerr << "cd: " << target.first->name << ": Not a directory\n" ;
		return ;		
	}		

	curNode = target.first ;
}


void fileSystem::fileWriter(string cmd)
{
	stringstream SS(cmd) ;
	string fileName, fileContent ;
	getline( SS, fileName, ' ' ) ;
	getline( SS, fileName, ' ' ) ;
	getline( SS, fileContent, '%' ) ;

	if( fileContent[0] != '\"' && fileContent.back() != '\"' )
	{
		cerr << "Usage: write [fileName] \"content\"\n" ;
		return ;
	}	
	fileContent = fileContent.substr( 1, fileContent.length()-2 ) ;

	string newFile ;
	fileSysNode* current ;
	size_t pos ;
	
	if( (pos = fileName.find_last_of("/")) == string::npos )
	{	
		newFile = fileName ;
		current = curNode ;
	}	
	else
	{	
		newFile = fileName.substr( pos+1 ) ;	
		NODE target = dirNodeSeacher( fileName.substr(0, pos) ) ;
		if( (current = target.first) == NULL )
		{
			cerr << "write: " << target.second << ": No such file or directory\n" ;
			return ;
		}	
		if( current->type == file )
		{
			cerr << "write: " << current->name << ": Not a directory\n" ;
			return ;
		}	
	}

	if( current->children.count(newFile) == 0 )
	{	
		current->children[ newFile ] = new fileSysNode( newFile, file ) ;
		current->children[ newFile ]->parent = current ;
	}

	else if( current->children[ newFile ]->type != file )
	{
		cerr << "write: " << newFile << ": Not a file\n" ;
		return ;
	}	

	current->children[ newFile ]->content += fileContent + "\n" ;
}


void fileSystem::fileReader(string cmd)
{
	fileSysNode* current ;	
	NODE target = dirNodeSeacher( cmd.substr( FINDBLANK(cmd) ) ) ;
	
	if( (current = target.first) == NULL )
	{
		cerr << "read: " << target.second << ": No such file or directory\n" ;
		return ;
	}	
	if( current->type != file )
	{
		cerr << "read: " << current->name << ": Not a file\n" ;
		return ;
	}	

	cout << current->content  ;
}


void fileSystem::fileRemover(string cmd)
{
	fileSysNode* current ;	
	NODE target = dirNodeSeacher( cmd.substr( FINDBLANK(cmd) ) ) ;
	
	if( (current = target.first) == NULL )
	{
		cerr << "rm: " << target.second << ": No such file or directory\n" ;
		return ;
	}	
	if( current->type != file )
	{
		cerr << "rm: " << current->name << ": Not a file\n" ;
		return ;
	}		

	current->parent->children.erase( target.second ) ;
	current->parent = NULL ;
	delete current ;
	current = NULL ;
}


void fileSystem::dirRemover(string cmd)
{
	fileSysNode* current ;	
	NODE target = dirNodeSeacher( cmd.substr( FINDBLANK(cmd) ) ) ;
	
	if( (current = target.first) == NULL )
	{
		cerr << "read: " << target.second << ": No such file or directory\n" ;
		return ;
	}	
	if( current->type != directory )
	{
		cerr << "read: " << current->name << ": Not a directory\n" ;
		return ;
	}		
	if( !current->children.empty() )
	{
		cerr <<"rmdir: " << current->name << ": Directory not empty\n" ;
		return ;
	}	

	current->parent->children.erase( target.second ) ;
	current->parent = NULL ;
	delete current ;
	current = NULL ;	
}



void fileSystem::cmdExecutor(string cmd)
{
	if( cmd.empty() || cmd == "\n" )
		return ;
			
	switch( commandTypeSelector(cmd) )
	{
		case LS:
			dirLister( cmd ) ;
			break ;

		case MKDIR:
			dirMaker( cmd ) ;
			break ;

		case WRITE:
			fileWriter( cmd ) ;
			break ;	

		case READ:
			fileReader( cmd ) ;
			break ;	

		case CD:
			changeDir( cmd ) ;	
			break ;

		case RM:
			fileRemover( cmd ) ;	
			break ;

		case RMDIR:
			dirRemover( cmd ) ;
			break ;

		case EXIT:
			exit(0) ;	

		case UNKNOWN:
			cout << cmd << ": command not found\n" ;
			break ;

		default:
			break ;	
	} ;
}





cmdType fileSystem::commandTypeSelector(string cmd)
{
	if( cmd.substr(0, 2) == "ls" )
		return LS ;

	if( cmd.substr(0, 5) == "mkdir" )
		return MKDIR ;

	if( cmd.substr(0, 5) == "write" )
		return WRITE ;

	if( cmd.substr(0, 4) == "read" )
		return READ ;

	if( cmd.substr(0, 5) == "rmdir" )
		return RMDIR ;

	if( cmd.substr(0, 2) == "rm" )
		return RM ;

	if( cmd.substr(0, 2) == "cd" )
		return CD ;

	if( cmd == "exit" )
		return EXIT ;

	return UNKNOWN ;
}


void fileSystem::shell()
{
	while( true )
	{
		string cmd ;
		cout << "$ " ;
		getline( cin, cmd ) ;
		cmdExecutor( cmd ) ;
	}	
}





