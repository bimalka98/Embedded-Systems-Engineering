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
#include <fstream>          // for file I/O
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
        std::string currentLine;
        std::string nextLine;

        std::map<std::string, std::string> dictionary;

        /*         Private Member Functions      
        *******************************************************/
        void retrievDictionary(){
            
            this->inputStream.open(inputFileName, std::ios::in); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be decompressed does not exist." << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::cout << "[INFO] retrieving the dictionary..." << std::endl;

            // loop over the file
            short _dictionaryentry = 0;
            bool _isdictionary = false; 
            while(std::getline(this->inputStream, this->currentLine)){
                
                // insert the word in the dictionary if it is an entry in it
                if(_isdictionary){
                    
                    // convert dictionary index to 3 bit string: https://stackoverflow.com/a/22746526/15939357
                    std::string _index = std::bitset<3>(_dictionaryentry).to_string();
                    
                    // insert the word into the dictionary
                    this->dictionary.insert(std::make_pair(_index, this->currentLine));
                    
                    // update the dictionary index
                    _dictionaryentry++;

                    // [DEBUG]
                    std::cout << "[INFO] " << _index << " : " << this->currentLine << std::endl;
                }

                // identifying the start of the dictionary
                if(this->currentLine == "xxxx"){                    
                    _isdictionary = true;
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

            retrievDictionary(); // extract the dictionary from the compressed data
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

