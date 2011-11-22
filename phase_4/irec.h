#ifndef IREC_H
#define IREC_H

class irec{
	public:
		irec(string ky, int blk): block(blk), key(ky){}
		string getKey(){return key;}
		int getBlock(){return block;}
		void setKey(string k){ key = k;}
		void setBlock(int blk){block = blk;}
	private:
		string key;
		int block;
};

#endif
