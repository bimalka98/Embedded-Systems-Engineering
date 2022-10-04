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

        /*           User Defined Data Types     
        *******************************************************/
        struct DictionaryWord{
            unsigned long _word;
            unsigned long _priority;
            unsigned long _frequency;
        };

        struct FourBitMask{
            std::string _dictionaryword;
            std::bitset<4> _mask;
            int _mismatchlocation;
            int _mismatches;
        };

        /*
            Classes Having Pointers To Members
            // http://websites.umich.edu/~eecs381/handouts/Pointers_to_memberfuncs.pdf
            // https://opensource.com/article/21/2/ccc-method-pointers
            // http://www.icce.rug.nl/documents/cplusplus/cplusplus16.html            
        */        

        // to use member function pointers of the compression algorithms
        typedef void(Compressor::*CompressionAlgorithm)();  // https://stackoverflow.com/a/2402624/15939357

        /*           Private Data Members     
        *******************************************************/
        std::string inputFileName;
        std::fstream inputStream;
        std::string outputFileName = "cout.txt";        
        std::fstream outputStream;

        // vector to store frequency of words for sorting purposes when generating dictionary
        std::vector<DictionaryWord> frequencyStore;
        // map to dtore the final dictionary for compresing
        std::map<std::string, std::bitset<32>> dictionary;

        // vector to hold pointes of the compression algorithms in the order of bits per compression, ascending order
        std::vector<CompressionAlgorithm> compressionAlgorithms = {
            &Compressor::runLengthEncoding, // bits per compression: 2     
            &Compressor::directMatch,       // bits per compression: 3
            &Compressor::oneBitMismatch,    // bits per compression: 8 
            &Compressor::twoBitMismatchCon, // bits per compression: 8     
            &Compressor::fourBitMasked,     // bits per compression: 12 
            &Compressor::twoBitMismatchAny, // bits per compression: 13     
            &Compressor::originalBinary     // bits per compression: 32 
        };
        
        std::string previousWord = "xxxx";  // variable to hold previously compressed word (useful in RLE)
        bool isRLEUsed = false; // to check if the RLE is used if the similar occurences exceeds 5

        std::bitset<32> originalWord; // varibales to hold the original word
        std::string compressedCode; // varibales to hold the compressed word
        bool isCompressed = false; // flag to check if the word is compressed 
        
        

        /*         Private Member Functions      
        *******************************************************/
        void getWordFrequencies(){
            
            // https://cplusplus.com/reference/fstream/fstream/open/
            this->inputStream.open(inputFileName, std::ios::in | std::ios::binary); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be compressed does not exist." << std::endl;
                exit(EXIT_FAILURE);
            }
            
            std::cout << "[INFO] calculating word frequencies while preserving their order..." << std::endl;
            
            // an unordered set to hold distinct keys: https://cplusplus.com/reference/unordered_set/
            std::unordered_set<unsigned long> _distinctwords;  
            // an unordered map to hold frequency values of each word: https://cplusplus.com/reference/unordered_map/
            std::unordered_map<unsigned long, unsigned long> _wordfreqs; 
            // list to presrve the insertion order of the words : https://cplusplus.com/reference/list/list/
            std::list<unsigned long> _wordorder;
            

            // variable to store current line of iteration in the file
            std::string _currentline;
            unsigned long _word;


            while(std::getline(this->inputStream, _currentline)){
                
                _word = std::bitset<32>(_currentline).to_ulong(); // convert binary to decimal
                
                // check if this is found in the _distinctwords set
                // https://cplusplus.com/reference/unordered_set/unordered_set/count/
                if(_distinctwords.count(_word)){ // Average case: constant. Worst case: linear in container size.

                    _wordfreqs[_word] +=1; // incrementing the word count for that word - Average case: constant. Worst case: linear in container size)

                }else{

                    _distinctwords.insert(_word); // to keep track of the distinct words - insertion: Average case: constant. Worst case: linear in container size.   
                    _wordorder.push_back(_word); // to keep track of the order of the words- : Constant.
                    _wordfreqs.insert(std::make_pair(_word, 1)); // set the word count to 1, when initializing - Single element insertions: Average case: constant. Worst case: linear in container size.
                    
                }
            }
            
            DictionaryWord _dictionaryword;

            // inserting  words and their frequencies available in the map, to a vecotor for sorting
            unsigned long _index = (unsigned long) _wordorder.size(); // variable to track priority of a word

            for(auto &_it : _wordorder){
                
                _dictionaryword._word = _it; // word                  
                _dictionaryword._frequency = _wordfreqs[_it]; // frequency of the word
                _dictionaryword._priority = _index; // priority of the word                

                this->frequencyStore.push_back(_dictionaryword);                

                _index--; // decrement
                
            } 
            
            this->inputStream.close();

        }
        
        // sorting the frequency store words depending on the frequency of words 
        // static: https://stackoverflow.com/a/29287632/15939357
        static bool sortbyfrequency(const DictionaryWord &a, const DictionaryWord &b){
            
            return !(a._frequency < b._frequency);
        
        }
        
        // sorting the frequency store words depending on the priority of words 
        static bool sortbypriority(const DictionaryWord &a, const DictionaryWord &b){
            
            return !(a._priority < b._priority);
        
        }

        // generating the dictionary using the frequencies of the words
        void generateDictionary() {            
            
            // sorting the frequency store depending on the frequency of words 
            // https://www.geeksforgeeks.org/sorting-vector-of-pairs-in-c-set-1-sort-by-first-and-second/            

            // sorting the whole frequency store vector depending on the frequency of words
            std::cout << "[INFO] sort by frequency..." << std::endl;
            
            std::sort(this->frequencyStore.begin(), this->frequencyStore.end(), sortbyfrequency);

            // [DEBUG]
            // for(auto &_it : this->frequencyStore) {std::cout << "[INFO] w: " << _it._word << " f: " << _it._frequency << " p: " << _it._priority  << std::endl;}

            // resort the subsections of the sorted frequency store depending on the priority of words
            // if two words has the same frequency, sort them by priority
            std::cout << "[INFO] re-sort by priority for similar frequencies..." << std::endl;
            
            auto _it = this->frequencyStore.begin();    // iterator initialization to loop over the full vctor

            while( _it != this->frequencyStore.end()){  // loop over the vector till the end
                
                DictionaryWord _currentword = *_it;     // get the current word where the pointer is                
                
                // if word freqencies are repeated; sort the sub vectors using their priorities
                bool _subvectorsorted = false;  // flag indicating thether the subvector is sorted                                
                auto _subvectorstart = _it;     // start of the subvector to be sorted
                auto _subvectorend = _it;       // end of the subvector to be sorted 
                DictionaryWord _nextword;       // tempory variable to hold the next word after current pointer
                
                while(!_subvectorsorted){

                    _nextword = *(_subvectorend +1);

                    if(_nextword._frequency != _currentword._frequency){
                        
                        // sort the subvecotor by priority, in the range [first,last)
                        std::sort(_subvectorstart, _subvectorend + 1, sortbypriority); 
                        _subvectorsorted = true;

                    }else{
                        
                        // increment the subvector's end pointer
                        _subvectorend++;
                        _subvectorsorted = false;

                    }
                
                }

                _it = _subvectorend + 1; // point iterator to next word with a different frequency
                                
            }
      
            /*
            Dictionary ideally should have 8 indexes
            however, words count may be less than or equal to or greater than that.
            These three cases needs to be handled.
            */
            std::cout << "[INFO] generating the dictionary..." << std::endl;

            int _dictionaryentry = 0; // dictionary entry

            for(auto _it = this->frequencyStore.begin(); _it != this->frequencyStore.end(); _it++){

                // convert dictionary index to 3 bit string: https://stackoverflow.com/a/22746526/15939357
                std::string _index = std::bitset<3>(_dictionaryentry).to_string();
                
                // conver the word to 32bit bitset
                std::bitset<32> _word(_it->_word);

                // insert the word into the dictionary
                this->dictionary.insert(std::make_pair(_index, _word));

                // update the dictionary index
                _dictionaryentry++;

                // we need only the first 8 of the frequencyStore                    
                if (_dictionaryentry == 8) break;
            }

            

        }

        void writeDictionary(){

            // delimit the dictionary from the compressed code
            this->outputStream << "xxxx" << std::endl;

            // write the dictionary
            for(auto &_it: this->dictionary){
                this->outputStream << _it.second << std::endl;
            }
        }

        // compress the stream
        void compressStream(){
            
            this->inputStream.open(inputFileName, std::ios::in); // open the input stream

            if(!this->inputStream.is_open()){
                std::cout << "[ERROR] file to be compressed can not be opened." << std::endl;
                exit(EXIT_FAILURE);
            }

            this->outputStream.open(outputFileName, std::ios::out); // open the output stream

            if(!this->outputStream.is_open()){
                std::cout << "[ERROR] file to save compressed data can not be opened." << std::endl;
                exit(EXIT_FAILURE);
            }            
                        
            std::string _currentline; // variables to store the currenly encoding line
                        
            std::string _outputbuffer; // variables to store the line to write at a moment            

            while(std::getline(this->inputStream, _currentline)){
                
                // compression code
                this->originalWord = std::bitset<32>(_currentline);

                // iterate over the compression algorithms to find the optimal encoding
                this->isCompressed = false;                             

                // get a poinet to the first compression algorithm
                auto _pointertoalgorithm = this->compressionAlgorithms.begin(); 

                while(!this->isCompressed){

                    // execute the compression algorithm (in the form of pointer to member function)
                    (this->*(*_pointertoalgorithm))(); 
                    
                    // move to the next fucntion if compression was not success from previous algorithm
                    _pointertoalgorithm++;

                    // this->isCompressed is set to true if compression happens inside an algorithm
                }
    
                
                // WRITE TO THE FILE                
                // add the compressedcode to the buffer
                _outputbuffer += this->compressedCode;

                if(_outputbuffer.length() > 32){
                    
                    // separete the fisrt 32 bits to write to the file
                    std::string _line2write = _outputbuffer.substr(0, 32);

                    // save rest of the buffer to the same variable
                    _outputbuffer = _outputbuffer.substr(32);
                    
                    // write to the file
                    this->outputStream << _line2write << std::endl;

                }

                //update the previous line to make use at RLE 
                this->previousWord = _currentline;
    
            }

            // check anything is left in the _outputbuffer prior to include the dictionary
            if(_outputbuffer.length() == 32){
                
                // write to the file
                this->outputStream << _outputbuffer << std::endl;


            }else{

                while(_outputbuffer.length() < 32){
                    _outputbuffer+="1";
                }

                // write to the file
                this->outputStream << _outputbuffer << std::endl;

            }

            // write dictionary to the output stream
            writeDictionary();

            // closing the files
            this->inputStream.close();
            this->outputStream.close();
            
            // [DEBUG]
            std::cout << "[INFO] compression complete." << std::endl;
                      
        }

        /*  
            ----PRIORITY OF THE COMPRESSION ALGORITHM----
            runLengthEncoding, // bits per compression: 2     
            directMatch,       // bits per compression: 3
            oneBitMismatch,    // bits per compression: 8 
            twoBitMismatchCon, // bits per compression: 8     
            fourBitMasked,     // bits per compression: 12 
            twoBitMismatchAny, // bits per compression: 13     
            originalBinary     // bits per compression: 32 

            NOTE:
            Above algorithms are implemeted in the order given in the assignment. Not in the above order.
        */

        // {code: "000", # bits: 2,  index: 0} - RLE: run Length Encoding
        void runLengthEncoding(){            
            /*
            The two bits in the RLE indicates the number of occurrences 
            (00, 01, 10 and 11 imply 1, 2, 3 and 4 occurrences, respectively),
            */
            std::map<int, std::string> _occurrences2bits = {
                {1, "00"},
                {2, "01"},
                {3, "10"},
                {4, "11"}
            };
            if((this->originalWord.to_string() == this->previousWord) && (!this->isRLEUsed)){
                
                // RLE can be considered
                this->isRLEUsed = true;     // to avoid the use of RLE consecutively

                int _occurrences = 1;       // number of similar occurences max is 4               
                std::string _currentline;   // tempory vaiable to hold the current line
                bool _end = false;

                std::streampos _oldposition; // https://cplusplus.com/reference/ios/streampos/

                while(!_end){
                    
                    // required when we need to get back to the previous line
                    _oldposition = this->inputStream.tellg(); // https://cplusplus.com/reference/istream/istream/tellg/

                    // read the next line to check whether it is equal as well                    
                    std::getline(this->inputStream, _currentline);                                

                    // check whether newly got line is same and adding it will not exceed the RLE limits of 4 occurences
                    if((_currentline == this->previousWord) && (_occurrences < 4)){
                        
                        _occurrences++;

                    }else{
                       
                        _end = true; // exiting the loop

                        // get back to the previous position in the file: https://stackoverflow.com/a/27331411/15939357
                        // this line must be encoded usig another method in the next run: otherwise it will be missed 
                        this->inputStream.seekg(_oldposition); // https://cplusplus.com/reference/istream/istream/seekg/
                                                                    
                    }
                }

                // RLE encoding
                this->compressedCode = "000" + _occurrences2bits[_occurrences];
                this->isCompressed = true; // compression complete

                
            }            
        }

        // {code: "001", # bits: 12, index: 1} - a 4 bit masked, based compression
        void fourBitMasked(){            
            
            this->compressedCode = "001";       // add start ML, mask then dictionary entry index during the program
            
            bool _ismaskedfound = false;        // flag to check whether any useful mask is found
            std::vector<FourBitMask> _masks;    // vector to hold masks from different dictionary entries

            // iterate over the dictionary and check if the word matches with any of its entries            
            for(auto &_it: this->dictionary){                
                
                int _hammingdistance = hammingDistance(_it.second, this->originalWord);

                if( (1 < _hammingdistance ) && (_hammingdistance < 5)){ // capturing 2, 3, 4 mismatches                                        
                    
                    // get where the bits are different
                    std::list<int> _mismatchlocations;  _mismatchlocations.clear(); 

                    for(int _index =0; _index <32; _index++){
                        
                        if(_it.second[_index] ^ this->originalWord[_index]){ // xor is 1 where the bits are different
                            
                            // ML counted from MSB while bitset traverse from LSB:= Location from MSB = 31-index
                            _mismatchlocations.push_back(31-_index);

                        }
                    }

                    // test consecutivity: https://cplusplus.com/reference/cmath/abs/
                    // https://cplusplus.com/reference/list/list/front/                                          
                    int _displacement = abs(_mismatchlocations.front()-_mismatchlocations.back());                                 
                    
                    // consecutive two bit mismatches handled previous stage                    
                    // therefore only the consective bits with ML1 and ML2 has displacemets by 2 or 3 bits needs to be handled here
                    // * this method is benificial over twoBitMismatchAny(13): it can also be handled here if displacement <= 3   
                    if( (1 < _displacement) && (_displacement < 4)){ // 2, 3 displacement: (last ML - first ML = x)
                        
                        
                        _ismaskedfound = true; // set the flag to indicate a suitable mask found

                        FourBitMask _fourbitmask;

                        _fourbitmask._dictionaryword    = _it.first; // dictionary entry
                        _fourbitmask._mismatchlocation  = _mismatchlocations.back(); // closes to the MSB is the last elemet of the list
                        _fourbitmask._mismatches        = _mismatchlocations.size(); // number of mismatches                        
                        
                        // get the mask: rechecked take xor between the original word's and the dictionary word's required 4 bits starting from ML1 (Left most)
                        std::bitset<4> _bitsetfromdict(_it.second.to_string().substr(_mismatchlocations.back(), 4));
                        std::bitset<4> _bitsetfromword(this->originalWord.to_string().substr(_mismatchlocations.back(), 4));
                        _fourbitmask._mask =  _bitsetfromdict ^ _bitsetfromword;

                        // push to the vector
                        _masks.push_back(_fourbitmask);                        
                    }            
                            
                }
            }

            // select the best mask
            /*
                if there is a scenario where you have two possible
                ways of applying bitmasks to a 32-bit binary, always give
                preference to the scenario where the leftmost bit
                ‘1’ for the bitmask pattern (e.g., 11 is preferred over 01)
            */
            if(_ismaskedfound){
                
                // iterate to find the optimal mask
                unsigned long _maxmask = -1;    // (e.g., 11 is preferred over 01)-> just compare the int values of masks
                int _mismatches = -1;           // 4 > 3 > 2 prefered: number of mismatches 
                
                FourBitMask _selectedmask;      // iterate to find the optimal mask (highest mismatches + mask starts with 1s)

                for(auto _it = _masks.begin(); _it != _masks.end(); _it++){ 
                    
                    // check # of mismatches : hogher the number of mismatches higher the optmality
                    if((*_it)._mismatches > _mismatches){
                        
                        _selectedmask = *_it;
                        _mismatches = (*_it)._mismatches;
                        _maxmask = (int)(*_it)._mask.to_ulong();

                    }else if( ((*_it)._mismatches == _mismatches) && ((int)(*_it)._mask.to_ulong() > _maxmask)){
                        
                        _selectedmask = *_it;                        
                        _maxmask = (int)(*_it)._mask.to_ulong();
                    }
                }

                this->compressedCode += std::bitset<5>(_selectedmask._mismatchlocation).to_string(); // concat the first ML from MSB
                this->compressedCode += _selectedmask._mask.to_string();    // mask comes here
                this->compressedCode += _selectedmask._dictionaryword;      // concatenating the dictionary entry
                
                this->isCompressed = true;  // compression complete 
                this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                                       
                return; // return from the fucntion

            }

        }

        // {code: "010", # bits: 8,  index: 2} - 1 bit mismatch
        void oneBitMismatch(){
            
            // iterate over the dictionary and check if the word matches with any of its entries
            for(auto &_it: this->dictionary){
                
                this->compressedCode = "010"; // add ML then dictionary entry index during the program
                
                if(hammingDistance(_it.second, this->originalWord) == 1){
                    
                    // get where the bits are different                     
                    for(int _index =0; _index <32; _index++){
                        
                        if(_it.second[_index] ^ this->originalWord[_index]){ // xor is 1 where the bits are different
                            
                            // ML counted from MSB while bitset traverse from LSB:= Location from MSB = 31-index
                            this->compressedCode += std::bitset<5>(31-_index).to_string(); // concatenate the ML
                            this->compressedCode += _it.first;  // concatenating the dictionary entry
                            
                            this->isCompressed = true;  // compression complete      
                            this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                                  
                            return; // return from the fucntion since we have only one mismatch

                        }
                    }                    
                    
                }
            }

        }

        // {code: "011", # bits: 8,  index: 3} - 2 bit mismatches (consecutive)
        void twoBitMismatchCon(){

            // iterate over the dictionary and check if the word matches with any of its entries
            for(auto &_it: this->dictionary){
                
                this->compressedCode = "011"; // add start ML then dictionary entry index during the program
                
                if(hammingDistance(_it.second, this->originalWord) == 2){
                    
                    // get where the bits are different
                    std::list<int> _mismatchlocations;                   
                    for(int _index =0; _index <32; _index++){
                        
                        if(_it.second[_index] ^ this->originalWord[_index]){ // xor is 1 where the bits are different
                            
                            // ML counted from MSB while bitset traverse from LSB:= Location from MSB = 31-index
                            _mismatchlocations.push_back(31-_index);

                        }
                    }

                    // test consecutivity: https://cplusplus.com/reference/cmath/abs/
                    if(abs(_mismatchlocations.front()-_mismatchlocations.back())==1){

                        this->compressedCode += std::bitset<5>(_mismatchlocations.back()).to_string(); // concat the first ML from MSB
                        this->compressedCode += _it.first;  // concatenating the dictionary entry
                        
                        this->isCompressed = true;  // compression complete  
                        this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                                      
                        return; // return from the fucntion
                    }
                            
                }
            }
        }

        // {code: "100", # bits: 13, index: 4} - 2 bit mismatches (anywhere) ML: Mismatch Location
        void twoBitMismatchAny(){                       
            
            // iterate over the dictionary and check if the word matches with any of its entries
            for(auto &_it: this->dictionary){
                
                this->compressedCode = "100"; // ad ML 1 and ML2 then dictionary entry index during the program
                
                if(hammingDistance(_it.second, this->originalWord) == 2){
                    
                    // get where the bits are different
                    std::list<int> _mismatchlocations;  _mismatchlocations.clear(); 

                    for(int _index =0; _index <32; _index++){
                        
                        if(_it.second[_index] ^ this->originalWord[_index]){ // xor is 1 where the bits are different
                            
                            // ML counted from MSB while bitset traverse from LSB:= Location from MSB = 31-index
                            _mismatchlocations.push_back(31-_index);

                        }
                    }
                    
                    this->compressedCode += std::bitset<5>(_mismatchlocations.back()).to_string(); // first ML
                    this->compressedCode += std::bitset<5>(_mismatchlocations.front()).to_string(); // second ML
                    this->compressedCode += _it.first;  // concatenating the dictionary entry
                    
                    this->isCompressed = true;  // compression complete 
                    this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                                       
                    return; // return from the fucntion
                    
                }
            }
            
        }

        // {code: "101", # bits: 3,  index: 5} - direct matching
        void directMatch(){                      
            
            // iterate over the dictionary and check if the word matches with any of its entries
            // https://www.geeksforgeeks.org/search-by-value-in-a-map-in-c/
            for(auto &_it: this->dictionary){
                
                if(_it.second == this->originalWord){
                                        
                    this->compressedCode = "101" + _it.first;

                    this->isCompressed = true;  // compression complete
                    this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                    
                    return; // return from the fucntion
                    
                }
            }            
        }

        // {code: "110", # bits: 32, index: 6} - original 32 bit binary
        void originalBinary(){
            
            this->compressedCode = "110" + this->originalWord.to_string(); 
            
            this->isCompressed = true;  // compression complete
            this->isRLEUsed = false;    // RLE comes only after some other encoding method. This flag is reset, to indicate that.                    
        }

        // fucntion to get hamming distance between two bitsets
        int hammingDistance(std::bitset<32> a, std::bitset<32> b){
            /*
                the xor operation constructs a bit string that has one bits
                in positions where a and b differ. Then, the number of 1 bits is calculated using
                the __builtin_popcount function.
                From: Competitive Programmer’s Handbook by Antti Laaksonen, Draft July 3, 2018
            */
            unsigned int _a = (unsigned int)a.to_ulong();
            unsigned int _b = (unsigned int)b.to_ulong();

            return __builtin_popcount(_a^_b);

        }

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

            getWordFrequencies(); // get words and their frequencies from the input file

            generateDictionary(); // generate the dictionary depending on the frequencies

            compressStream();     // compressing using the compression algorithms
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
        std::fstream outputStream;

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

            this->outputStream.open(outputFileName, std::ios::out); // open the output stream

            if(!this->outputStream.is_open()){
                std::cout << "[ERROR] file to save decompressed data can not be opened." << std::endl;
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
                    
                    // get the decompressed code and write it to the output stream
                    this->compressionFormat = this->compressionFormats[_compressionformat].second; 
                    this->compressedWord = _compressedword;
                    
                    // decompress the found compressed code with the help of its format and write it to the output stream
                    decompressWord();                    

                }
                
            }
            
            // closing the files
            this->inputStream.close();
            this->outputStream.close();
            
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
                    /*
                    The two bits in the RLE indicates the number of occurrences 
                    (00, 01, 10 and 11 imply 1, 2, 3 and 4 occurrences, respectively),
                    */
                    std::map<std::string, int> _bits2occurrences = {
                        {"00", 1},
                        {"01", 2},
                        {"10", 3},
                        {"11", 4}
                    };

                    // nothing to decode in the RLE, just store the number of occurences
                    // https://en.cppreference.com/w/cpp/utility/bitset/to_ulong
                    this->occurencesOfWord = _bits2occurrences[_compressedword];
                    
                    for(int i=0; i < this->occurencesOfWord; i++) {
                        this->outputStream << this->decompressedWord << std::endl;
                    }
                    
                }break; 
                
                // bit masked based compression
                case 1:{                                                            
                    // https://www.geeksforgeeks.org/c-bitset-interesting-facts/
                    int _startlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong();     // start position to apply the mask from left                                 
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(9, 3)];  // dictionary entry
                    std::bitset<4> _mask(_compressedword, 5, 4);  // the 4 bit mask
                    
                    std::bitset<4> _string2applymask(_dictionaryentry, _startlocation, 4);          // the 4 bits of the dictionary entry 
                    std::string _result = std::bitset<4>(_mask ^ _string2applymask).to_string();    // considering the xor operation
                    
                    // https://cplusplus.com/reference/string/string/replace/
                    this->decompressedWord = _dictionaryentry.replace(_startlocation, 4, _result);  // saving in a private data member
                    this->outputStream << this->decompressedWord << std::endl;                    
                }break; 
                
                // 1 bit mismatch
                case 2:{
                    
                    int _mismatchlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // start position to apply the mask from left                                        
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(5, 3)]; // dictionary entry
                    
                    std::bitset<1> _mismatch(_dictionaryentry, _mismatchlocation, 1);              // extracting the mismatching bit
                    _mismatch.flip(); // https://en.cppreference.com/w/cpp/utility/bitset/flip     // flip the mismatching bit
                    
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation, 1, _mismatch.to_string());
                    this->outputStream << this->decompressedWord << std::endl;                   

                }break; 
                
                // 2 bit mismatches (consecutive)
                case 3:{                    

                    int _mismatchlocation = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // start position of mismatch                                        
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(5, 3)]; // dictionary entry
                    
                    std::bitset<2> _mismatches(_dictionaryentry, _mismatchlocation, 2);            // extracting the mismatching bit
                    _mismatches.flip();                                                            // flip the mismatching bit

                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation, 2, _mismatches.to_string());
                    this->outputStream << this->decompressedWord << std::endl;                    

                }break; 
                
                // 2 bit mismatches (anywhere)
                case 4:{
                    
                    int _mismatchlocation1 = (int)std::bitset<5>(_compressedword, 0, 5).to_ulong(); // mismatch location 1
                    int _mismatchlocation2 = (int)std::bitset<5>(_compressedword, 5, 5).to_ulong(); // mismatch location 2
                    std::string _dictionaryentry = this->dictionary[_compressedword.substr(10, 3)]; // dictionary entry

                    std::bitset<1> _mismatch1(_dictionaryentry, _mismatchlocation1, 1);             // extracting the mismatching bit
                    _mismatch1.flip();                                                              // flip the mismatching bit
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation1, 1, _mismatch1.to_string());

                    std::bitset<1> _mismatch2(_dictionaryentry, _mismatchlocation2, 1);             // extracting the mismatching bit                                        
                    _mismatch2.flip();                                                              // flip the mismatching bit
                    
                    this->decompressedWord = _dictionaryentry.replace(_mismatchlocation2, 1, _mismatch2.to_string());
                    this->outputStream << this->decompressedWord << std::endl;                    

                }break; 
                
                // direct matching
                case 5:{
                    
                    this->decompressedWord = this->dictionary[_compressedword]; // dictionary entry direct matching
                    this->outputStream << this->decompressedWord << std::endl;
                    
                }break; 
                
                // original 32 bit binary
                case 6:{                    
                    
                    this->decompressedWord = _compressedword;                   // no compression
                    this->outputStream << this->decompressedWord << std::endl;
                    
                }break;

            }
            
        }

    public:

        // default constructor
        Decompressor(){
            std::cout << "[INFO] decompressor object created." << std::endl;
        }

        // default destructor
        ~Decompressor(){
            std::cout << "[INFO] decompressor object destroyed." << std::endl;
        }

        /*         Public Member Fucntion    
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

// completed:  Wed Oct  5 00:19:00 +0530 2022