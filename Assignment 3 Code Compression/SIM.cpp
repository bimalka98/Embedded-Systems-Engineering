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
        std::map<std::string, std::pair<int, int>> compressionFormats = { // compressing formats and their bit lengths
            {"000", {2,  0}}, // RLE: run Length Encoding
            {"001", {12, 1}},// bit masked based compression
            {"010", {8,  2}}, // 1 bit mismatch
            {"011", {8,  3}}, // 2 bit mismatches (consecutive)
            {"100", {13, 4}},// 2 bit mismatches (anywhere)
            {"101", {3,  5}}, // direct matching
            {"110", {32, 6}} // original 32 bit binary
        };

        int compressionFormat; // format of the compression to be used with the fucntions
        std::string compressedWord; // varibales to hold the compressed word
        std::string decompressedWord; // varibales to hold the decompressed word
        int occurencesOfWord; // number of occurrences, useful when RLE is associated

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
                    // std::cout << "[INFO] " << _index << " : " << _currentline << std::endl;
                }

                // identifying the start of the dictionary
                if(_currentline == "xxxx"){                    
                    _isdictionary = true;
                }

            } 

            this->inputStream.close();
        }
        
        // fucntion to decode the binary stream
        void decompressStream(){
            
            this->inputStream.open(inputFileName, std::ios::in); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be decompressed can not be opened." << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "[INFO] decoding the stream..." << std::endl;

            std::string _currentline, _nextline; // variables to store the currenly decoding line and the next line
            std::string _compressionformat, _compressedword; // variables to store the compressed data and compression method

            int _cursorposition = 0; // position of the cursor in the stream
            int _compressionlength = 0; // length of the compressed code to be extracted            
 
            while(true){
                
                std::getline(this->inputStream, _currentline);
                if(_currentline == "xxxx") break; // found the end of the compressed data
                
                // std::string::substr: https://cplusplus.com/reference/string/string/substr/
                while(_cursorposition < 32){
                    
                    // check whether compressed code can be extracted from the current line itself
                    if((_cursorposition + 3) < 32){

                        _compressionformat = _currentline.substr(_cursorposition, 3); // format is always 3 bit long
                        _cursorposition+=3; // incrementing the cursor to get the compressed code

                    }else{
                        
                        // if the substring extends to the next line read it
                        std::getline(this->inputStream, _nextline);
                        
                        // constructing the combined string to get the compression format
                        int _lengthpart1 = 32 -_cursorposition;
                        int _lengthpart2 = 3 - _lengthpart1;
                        std::string _stringpart1 = _currentline.substr(_cursorposition,  _lengthpart1);
                        std::string _stringpart2 = _nextline.substr(0, _lengthpart2);
                        _compressionformat = _stringpart1 + _stringpart2;                        
                        
                        _currentline = _nextline;
                        _cursorposition = (_cursorposition + 3) % 32;
                    }
                    
                    // compressed patterns arranged in a sequential manner 
                    // 32-bit in each line, last line padded with 1’s, if needed. This case is handled here
                    if(_compressionformat == "111" | _compressionformat == "11x" | _compressionformat == "1xx"){
                        break;
                    }
                    
                    // get the length of the compression fromat from the map
                    _compressionlength = this->compressionFormats[_compressionformat].first;

                    // check whether compressed code can be extracted from the current line itself
                    if((_cursorposition + _compressionlength) < 32){ 
                        
                        _compressedword = _currentline.substr(_cursorposition, _compressionlength);
                        _cursorposition+=  _compressionlength;

                    }else{
                        
                        // if the substring extends to the next line read it                        
                        std::getline(this->inputStream, _nextline);

                        // constructing the combined string to get the compressed code
                        int _lengthpart1 = 32 -_cursorposition;
                        int _lengthpart2 = _compressionlength - _lengthpart1;
                        std::string _stringpart1 = _currentline.substr(_cursorposition,  _lengthpart1);
                        std::string _stringpart2 = _nextline.substr(0, _lengthpart2);
                        _compressedword = _stringpart1 + _stringpart2;

                        _currentline = _nextline;
                        _cursorposition = (_cursorposition + _compressionlength) % 32;
                    }
                    
                    // [DEBUG]                    
                    // std::cout << "[INFO] format: " << _compressionformat << ", code: " << _compressedword << std::endl;

                    // get the decompressed code and write it to the output stream
                    this->compressionFormat = this->compressionFormats[_compressionformat].second; 
                    this->compressedWord = _compressedword;
                    
                    // decompress the found compressed code with the help of its format
                    decompressWord();

                    

                }
                
            }
            
            // [DEBUG]
            std::cout << "[INFO] decompression complete. " << _currentline << std::endl;
        }

        // fucntion to decompress a given compressed code
        void decompressWord(){
            
            int _compressionformat = this->compressionFormat;
            std::string _compressedword =  this->compressedWord;

            switch(_compressionformat){

                 // RLE: run Length Encoding
                case 0:{
                    std::cout << "[INFO] RLE decoding..." << std::endl;
                    // nothing to decode in the RLE, just store the number of occurences
                    // https://en.cppreference.com/w/cpp/utility/bitset/to_ulong
                    this->occurencesOfWord = (int)std::bitset<2>(_compressedword).to_ulong();
                    
                }break; 
                
                // bit masked based compression
                case 1:{                    
                    std::cout << "[INFO] 4 bit masked decoding..." << std::endl;
                    
                    // https://www.geeksforgeeks.org/c-bitset-interesting-facts/
                    int _startlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // start position to apply the mask from left                                 
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(9, 3)];  // dictionary entry

                    std::bitset<4> _mask(_compressedword, 5, 4);  // the 4 bit mask
                    std::bitset<4> _string2applymask(_dictionaryentry, _startlocation, 4);  // the 4 bits

                    std::string _result = std::bitset<4>(_mask ^ _string2applymask).to_string();

                    // https://cplusplus.com/reference/string/string/replace/
                    this->decompressedWord = _dictionaryentry.replace(_startlocation, 4, _result);
                    
                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;

                }break; 
                
                // 1 bit mismatch
                case 2:{
                    std::cout << "[INFO] 1 bit mismatch decoding..." << std::endl;
                    int _mismatchlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // start position to apply the mask from left                                        
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(5, 3)]; // dictionary entry

                    std::bitset<1> _mismatch(_dictionaryentry, _mismatchlocation, 1);                    
                    _mismatch.flip(); // https://en.cppreference.com/w/cpp/utility/bitset/flip
                    
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation, 1, _mismatch.to_string());

                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;

                }break; 
                
                // 2 bit mismatches (consecutive)
                case 3:{
                    std::cout << "[INFO] 2 consecutive bits mismatch decoding..." << std::endl;

                    int _mismatchlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // start position of mismatch                                        
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(5, 3)]; // dictionary entry

                    std::bitset<2> _mismatches(_dictionaryentry, _mismatchlocation, 2);                    
                    _mismatches.flip(); 

                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation, 2, _mismatches.to_string());

                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;


                }break; 
                
                // 2 bit mismatches (anywhere)
                case 4:{
                    std::cout << "[INFO] 2 bit mismatch anywhere decoding..." << std::endl;

                    int _mismatchlocation1 = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // mismatch location 1
                    int _mismatchlocation2 = (int)std::bitset<5>(_compressedword, 5, 5).to_ulong(); // mismatch location 2
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(10, 3)]; // dictionary entry

                    std::bitset<1> _mismatch1(_dictionaryentry, _mismatchlocation1, 1);                    
                    _mismatch1.flip(); 
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation1, 1, _mismatch1.to_string());

                    std::bitset<1> _mismatch2(_dictionaryentry, _mismatchlocation2, 1);                    
                    _mismatch2.flip(); 
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation2, 1, _mismatch2.to_string());

                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;

                }break; 
                
                // direct matching
                case 5:{
                    std::cout << "[INFO] direct matching decoding..." << std::endl;

                    this->decompressedWord = this->dictionary[_compressedword]; // dictionary entry direct matching

                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;
                }break; 
                
                // original 32 bit binary
                case 6:{
                    std::cout << "[INFO] no compression..." << std::endl;
                    
                    this->decompressedWord = _compressedword; // no compression

                    std::cout << "[INFO] decompressed word: " << this->decompressedWord << std::endl;

                }break;

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

            decompressStream(); // decode the stream

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

