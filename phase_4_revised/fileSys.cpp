#include "fileSys.h"

fileSys::fileSys(Pdisk& dsk){
	disk = dsk;
	bool flag = false;
	char tmp;
	string dat = disk.getDiskName()+".dat";
	
	init();

	ifstream file;
	file.open(dat.c_str());
	file.seekg(disk.getBlockSize()-1);
	
	for(int i=disk.getBlockSize(); i < (2*disk.getBlockSize()); i++)
	{
		file.get(tmp);
		if(tmp != '#'){
			flag = true;
			break;
		}
	}

	if(flag == true){ // data was found load fs
		load_fs();
	}else{//data was not found
		make_fs();
		fsSynch();
	}
	
	file.close();
}

void fileSys::load_fs(){
	int blk_size = disk.getBlockSize()-1, i, first_blk, another_tmp;
	ostringstream output, fat_out;
	istringstream input, fat_in;
	string root_blk, file_name ;
	char tmp;

	fstream file((disk.getDiskName()+".dat").c_str());
	file >> fatSize >> rootSize; 

	//manually getting root block instead of getblock because
	//i dont want any extra ## in my string. 
	file.seekg(blk_size);
	for(i = blk_size; i < (2*blk_size); i++){
		file.get(tmp);
		if(tmp == '#')
			break;
		output << tmp ;
	}
	
	input.str(output.str());
	
	for(i=0; i < rootSize; i++){
		input >> file_name >> first_blk;
		fileName.push_back(file_name);
		firstBlock.push_back(first_blk);
	}

	//inporting fat vector
	file.seekg(2*blk_size+1);
	for(i = 2*blk_size+1; i< ((2+fatSize)*blk_size); i++){
		file.get(tmp);
		if(tmp == '#')
			break;
		fat_out << tmp;
	}
	
	fat_in.str( fat_out.str());
	for(i=0; i < disk.getNumOfBlocks(); i++){
		fat_in >> another_tmp;
		fat.push_back(another_tmp);
	}
}


void fileSys::init(){
	int num_blks = disk.getNumOfBlocks(), blk_size = disk.getBlockSize(), tmp;
	
	fatSize = ceil( (num_blks * blk_size)/ (8 * blk_size) );
	
	tmp = 8 + 1 + ceil( log10( disk.getNumOfBlocks() ) ) + 1 ; 
	rootSize = (disk.getBlockSize() / tmp) +2;

	dirty = 1;
}

void fileSys::make_fs(){
	//writing fat size to block 0 on disk since its not being used
	fstream file((disk.getDiskName()+".dat").c_str());
	file << fatSize << " " << rootSize;
	file.close();
		
	//still need to make fs
	fat.resize(disk.getNumOfBlocks(), 0);
	firstBlock.resize(rootSize, 0);
	fileName.resize(rootSize, "--------");	
	
	//first free block = fatSize + 1 block for root + 1 block reserved 
	int ffb = fatSize + 2; 
	
	fat[0] = ffb;

	for(int i = 0; i < fat.size()-ffb; i++)
		fat[ffb + i] = ffb + i + 1;
	
	fat[fat.size()-1] = 0;	
}

int fileSys::fsClose(){
	fsSynch();
	dirty = 0;
}

int fileSys::fsSynch(){
	ostringstream fat_out, root_out;
	int i;

	for(i=0; i < fileName.size(); i++)
		root_out << fileName[i] << " " << firstBlock[i] << " ";
		
	for(i=0; i < fat.size();i++)
		fat_out << fat[i] << " ";
	
	string buffer = fat_out.str();
	vector<string> blocks =  block(buffer, disk.getBlockSize());
		
	for(i =0; i < blocks.size();i++)
		disk.putblock(2+i, blocks[i]);

	buffer.clear(); 
	blocks.clear();
	
	buffer = root_out.str();
	blocks = block(buffer, disk.getBlockSize());

	for(i = 0; i < blocks.size(); i++)
		disk.putblock(1+i, blocks[i]);
	return 0;
}

vector<string> fileSys::block(string s, int b){
	vector<string> blocks;
	string temp_blks;
	
	int num_of_blks;
	
	if(s.size()%b == 0)
		num_of_blks =  s.size() / b;
	else
		num_of_blks = (s.size() / b)+1;

	for(int i=0; i < num_of_blks; i++){
		temp_blks = s.substr(b*i,b);
		blocks.push_back(temp_blks);
	}
		
	int last_blk = blocks.size()-1;

	for(int i = blocks[last_blk].size(); i < b; i++)
		blocks[last_blk] += "#";
	
	return blocks;
}

int fileSys::newFile(string file){
	bool found_empty = false;
	int i;

	if(check_n_pad(file) == 0)
		return 0;
	for(i =0; i< fileName.size(); i++){
		if(fileName[i] == file){
			cout << "File (" << file << ") already exists!" << endl;
			return 1;
		}
	}
	for(i =0; i < fileName.size(); i++){

		if(fileName[i] == "--------"){
			found_empty = true;
			fileName[i] = file;
			firstBlock[i] = 0;
			break;
		}
	}

	if(found_empty == false){
		cout << "No empty slots in root directory.\n";
		return 0;
	}
	
	fsSynch();
	cout << "Successfully added file: " << file << endl;
	return 1;
}

//yet another helper
int fileSys::check_n_pad(string &file){
		
	if(file.size() > 8){
		cout << "check_n_pad(): Error, filename is too long.\n";
		return 0;
	}

	if(file.size() < 8)
		for(int i=file.size(); i < 8; i++)
			file += "-";

	return 1;
}
int fileSys::rmFile(string file){
   bool found_file = false;
	
	if(check_n_pad(file) == 0)
		return 0;

   for(int i =0; i < fileName.size(); i++){
      if(fileName[i] == file && firstBlock[i] == 0){
         found_file = true;
         fileName[i] = "--------";
         firstBlock[i] = 0;
         break;
      }
   }

   if(found_file == false){
      cout << "rmFile(): File was not found or file was not empty.\n";
      return 0;
   }

   fsSynch();
   return 1;
}
//returns -1 if not successfull
int fileSys::getFirstBlock(string file){
	bool found_file = false;
	int first_block;

	if(check_n_pad(file) == 0)
		return -1;
	
	for(int i = 0; i < fileName.size(); i++){
		if(fileName[i] == file){
			found_file = true;
			first_block = firstBlock[i];
			break;
		}
	}

	if(found_file == false){
		cout << "getFirstBlock(): File was not found!\n";
		return -1;
	}

	fsSynch();
	return first_block;
}

//returns -1 if disk is full and -2 if file wasnt found
int fileSys::addBlock(string file, string buffer){
	int first_blk = getFirstBlock(file), new_blk = fat[0];
	bool file_found = false;
	if(check_n_pad(file) == 0)
		return -1;
	
	if(new_blk == 0){
		cout << "Disk is full!\n";
		return -1;
	}
	
	if(first_blk == 0){//then empty file
		
		for(int i = 0; i < fileName.size(); i++){
			if(fileName[i] == file){
				firstBlock[i] = new_blk;
				fat[0] = fat[new_blk];
				fat[new_blk] = 0;
				file_found = true;
				break;
			}
		}
		if(file_found == false){
			cout << "addBlock(): File was not found!\n";
			return -2;
		}
	}else{
		int next_blk = first_blk;
		
		while(fat[next_blk] != 0)
			next_blk = fat[next_blk];
		fat[next_blk] = new_blk;
		fat[0] = fat[new_blk];
		fat[new_blk] = 0;
	}
	
	disk.putblock(new_blk, buffer);
	fsSynch();
	return new_blk;
}//addblock

int fileSys::delBlock(string file, int block_number){
	int i,first_blk = getFirstBlock(file);
	
	if(check_n_pad(file) == 0)
		return 0;

	if(check_block(block_number, file) == 0)
		return 0;
	
	//check if the block_number belongs to the file
	if(first_blk == block_number){
		for(i = 0; i < firstBlock.size(); i++){
			if(firstBlock[i] == block_number){
				firstBlock[i] = fat[block_number];
				fat[block_number] = fat[0];
				fat[0] = block_number;
				break; 
			}
		}
	}else{
		for(i = 0; i < fat.size();i++){
			if(fat[i] == block_number){
				fat[i] = fat[block_number];
				fat[block_number] = fat[0];
				fat[0] = block_number;
				break;
			}
		}
	}

	fsSynch();
	return 1;
}

int fileSys::check_block(int block, string file){
   
	int next_block, first_blk = getFirstBlock(file);
   bool found = false;

   next_block = first_blk;
   while(next_block != 0){
		if(next_block == block){
         found = true;
         break;
      }
      next_block = fat[next_block];
   }
   if(found == false){
      cout << "Hey! That block number does not belong to that file. (Block: "<< block << ")" << endl;
      return 0;
   }
	return 1;
}

int fileSys::getBlock(string file, int block_number,string &buffer){

	if(check_block(block_number, file) == 0)
		return 0;
	
	disk.getblock(block_number, buffer);
	return 1;
}

int fileSys::putBlock(string file, int block_number, string buffer){
	
	if(check_block(block_number, file) == 0)
		return 0;
	
	disk.putblock(block_number, buffer);
	return 1;
}

int fileSys::nextBlock(string file, int block_number){
	
	if(check_n_pad(file) == 0)
		return -1;
	
	if(check_block(block_number, file) == 0)
		return -1;
	
	return fat[block_number];
}
