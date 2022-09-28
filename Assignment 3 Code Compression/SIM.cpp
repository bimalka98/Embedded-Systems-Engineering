/* On my honor, I have neither given nor received unauthorized aid on this assignment */

/*
*************************************************************************************
*                                Author Details                                     *
* Author: Thalagala B. P.   180631J                                                 *
* Github: https://github.com/bimalka98                                              *
* Email: bimalkapiyaruwan1998322@gmail.com                                          *
*===================================================================================*
*/


// include the standard C++ headers
#include <iostream>         // for standard I/O
#include <fstream>          // for file I/O: https://cplusplus.com/reference/fstream/fstream/
#include <bits/stdc++.h>    // for standard C++ headers


/*
*************************************************************************************
*              >>>         Start of Compressor Class         >>>                    *
*************************************************************************************
*/
class Compressor
{
    private:
        /*           Private Data Members     
        *******************************************************/
        std::string inputFileName;
        std::string outputFileName = "cout.txt";        

        /*         Private Member Functions      
        *******************************************************/

    public:
        /*            Public Data Members     
        *******************************************************/

        // default constructor
        Compressor(){
            std::cout << "[INFO] compressor object created." << std::endl;
        }

        // default destructor
        ~Compressor(){
            std::cout << "[INFO] compressor object destroyed." << std::endl;
        }

        /*         Public Member Fucntions     
        *******************************************************/
        void Compress(std::string file2compress){
            std::cout << "[INFO] compressing..." << std::endl;
            
            this->inputFileName = file2compress; // file with the original data
        }
};


/*
*************************************************************************************
*              >>>         Start of Decompressor Class         >>>                  *
*************************************************************************************
*/
class Decompressor
{
    private:
        /*           Private Data Members     
        *******************************************************/
        std::string inputFileName;
        std::fstream inputStream;
        std::string outputFileName = "dout.txt";       

        std::map<std::string, std::string> dictionary; // dictionary in the form [index:word]        
        std::map<std::string, int> compressingFormats = { // compressing formats and their bit lengths
            {"000", 2}, // RLE: run Length Encoding
            {"001", 12},// bit masked based compression
            {"010", 8}, // 1 bit mismatch
            {"011", 8}, // 2 bit mismatches (consecutive)
            {"100", 13},// 2 bit mismatches (anywhere)
            {"101", 3}, // direct matching
            {"110", 32} // original 32 bit binary
        };

        /*         Private Member Functions      
        *******************************************************/
        // function to retrieve the dictionary from the compressed data file
        void retrieveDictionary(){
            
            this->inputStream.open(inputFileName, std::ios::in); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be decompressed does not exist." << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::cout << "[INFO] retrieving the dictionary..." << std::endl;

            // loop over the file
            short _dictionaryentry = 0;
            bool _isdictionary = false; 
            std::string _currentline;

            while(std::getline(this->inputStream, _currentline)){
                
                // insert the word in the dictionary if it is an entry in it
                if(_isdictionary){
                    
                    // convert dictionary index to 3 bit string: https://stackoverflow.com/a/22746526/15939357
                    std::string _index = std::bitset<3>(_dictionaryentry).to_string();
                    
                    // insert the word into the dictionary
                    this->dictionary.insert(std::make_pair(_index, _currentline));
                    
                    // update the dictionary index
                    _dictionaryentry++;

                    // [DEBUG]
                    std::cout << "[INFO] " << _index << " : " << _currentline << std::endl;
                }

                // identifying the start of the dictionary
                if(_currentline == "xxxx"){                    
                    _isdictionary = true;
                }

            } 

            this->inputStream.close();
        }
        
        // fucntion to decode the binary stream
        void decodeStream(){
            
            this->inputStream.open(inputFileName, std::ios::in); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be decompressed can not be opened." << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "[INFO] decoding the stream..." << std::endl;

            std::string _currentline, _nextline; // variables to store the currenly decoding line and the next line
            std::string _compressingformat, compressedcode; // variables to store the compressed data and compression method

            int _cursorposition = 0; // position of the cursor in the stream
            int _compressionlength = 0; // length of the compressed code to be extracted            
 
            while(true){
                
                std::getline(this->inputStream, _currentline);
                if(_currentline == "xxxx") break; // found the end of the compressed data
                
                // std::string::substr: https://cplusplus.com/reference/string/string/substr/
                while(_cursorposition < 32){
                    
                    // check whether compressed code can be extracted from the current line itself
                    if((_cursorposition + 3) < 32){

                        _compressingformat = _currentline.substr(_cursorposition, 3); // format is always 3 bit long
                        _cursorposition+=3; // incrementing the cursor to get the compressed code

                    }else{
                        
                        // if the substring extends to the next line read it
                        std::getline(this->inputStream, _nextline);
                        
                        std::string _stringpart1 =
                        std::string _stringpart2 =
                        _compressingformat = _stringpart1 + _stringpart2;                        
                        
                        _currentline = _nextline;
                        _cursorposition = (_cursorposition + 3) % 32;
                    }
                    
                    // get the length of the compression fromat from the map
                    _compressionlength = this->compressingFormats[_compressingformat];

                    // check whether compressed code can be extracted from the current line itself
                    if((_cursorposition + _compressionlength) < 32){ 
                        
                        _compressedcode = _currentline.substr(_cursorposition, _compressionlength)
                        _cursorposition+=  _compressionlength;

                    }else{
                        
                        // if the substring extends to the next line read it                        
                        std::getline(this->inputStream, _nextline);

                        std::string _stringpart1 =
                        std::string _stringpart2 =
                        _compressedcode = _stringpart1 + _stringpart2;

                        _currentline = _nextline;
                        _cursorposition = (_cursorposition + _compressionlength) % 32;
                    }

                    // get the decompressed code and write it to the output stream

                    
                }
                
                

            }
            
        }

    public:
        /*            Public Data Members     
        *******************************************************/

        // default constructor
        Decompressor(){
            std::cout << "[INFO] decompressor object created." << std::endl;
        }

        // default destructor
        ~Decompressor(){
            std::cout << "[INFO] decompressor object destroyed." << std::endl;
        }

        /*         Public Member Fucntions     
        *******************************************************/
        void Decompress(std::string file2decompress){
            
            std::cout << "[INFO] decompressing..." << std::endl;

            this->inputFileName = file2decompress; // file with the compressed data

            retrieveDictionary(); // extract the dictionary from the compressed data

            decodeStream(); // decode the stream

        }
};


/*
*************************************************************************************
*              >>>          Start of Main Function           >>>                    *
*************************************************************************************
*/      

int main( int argc, char *argv[] ){

    // check whether the required argument available on the command line
    if(argc == 2){

        // check the argument to decide what to do
        if (argv[1][0] == '1'){

            // code compression
            Compressor compressor;
            compressor.Compress("original.txt");

        }else if (argv[1][0] == '2'){

            // code decompression
            Decompressor decompressor;
            decompressor.Decompress("compressed.txt");

        }else{

            // invalid argument
            return 1;
        }
    }else{
        return 1;
    }
}

