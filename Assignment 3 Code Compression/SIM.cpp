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
*              >>>          Start of Main Function           >>>                    *
*************************************************************************************
*/      

int main( int argc, char *argv[] ){

    // check whether the required argument available on the command line
    if(argc == 2){

        // check the argument to decide what to do
        if (argv[1][0] == '1'){

            // code compression
            std::cout << "Compression" << std::endl;

        }else if (argv[1][0] == '2'){

            // code decompression
            std::cout << "Decompression" << std::endl;
        }else{

            // invalid argument
            return 1;
        }
    }else{
        return 1;
    }
}

/*
*************************************************************************************
*            <<<             End of Main Function               <<<                 *
*************************************************************************************
*/

