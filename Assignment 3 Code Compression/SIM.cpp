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
#include <iostream> // for standard I/O
#include <fstream>  // for file I/O


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
        void compress(){
            std::cout << "[INFO] compressing..." << std::endl;
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

        /*         Private Member Functions      
        *******************************************************/

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
        void decompress(){
            std::cout << "[INFO] decompressing..." << std::endl;
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
            compressor.compress();

        }else if (argv[1][0] == '2'){

            // code decompression
            Decompressor decompressor;
            decompressor.decompress();

        }else{

            // invalid argument
            return 1;
        }
    }else{
        return 1;
    }
}

