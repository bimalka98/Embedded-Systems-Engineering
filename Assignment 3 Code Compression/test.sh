#!/bin/sh -e
# 2022 Bimalka Piyaruwan Thalagala

################################################################################
#	A simple script,                                                           #
#       to test the fucntionality of the compression and decompression         #
################################################################################

# banner to inform the user that the dependencies are being installed
banner () {

    echo
    echo "==========================================================="
    echo "|      © Copyright 2022 Bimalka Piyaruwan Thalagala       |"
    echo "|                           ---                           |"
    echo "|         Github: https://github.com/bimalka98            |"                                                                       
    echo "==========================================================="
    echo

}

# compile the cpp file
compile () {

    echo "[TEST-INFO] compiling the source file..."
    
    # compilation
    g++ -std=c++0x SIM.cpp -o SIM

    echo "Done!"
    echo
}

# test if compression unit works properly
test_compression_unit () {
    
    echo "[TEST-INFO] testing compression unit..."

    # run
    ./SIM 1

    # test
    echo "[TEST-INFO] identifying differences between \"cout.txt\" & \"compressed.txt\"..."
    diff -w -B  cout.txt compressed.txt

    echo "Done!"
    echo
}

# test if compression unit works properly
test_decompression_unit () {
    
    echo "[TEST-INFO] testing decompression unit..."
    
    # run
    ./SIM 2

    # test
    echo "[TEST-INFO] identifying differences between \"dout.txt\" & \"original.txt\"..."
    diff -w -B  dout.txt original.txt

    echo "Done!"
    echo
}

# remove temporary files
remove_temp_files () {

    echo "[TEST-INFO] removing temporary files..."

    # remove generated compressed file
    rm cout.txt

    # remove generated decompressed file
    rm dout.txt

    # remove executable
    rm SIM

    echo "Done!"
    echo
}

# main function to setup the environment for the LE Object Detector
main () {

    # banner
    banner

    # compile the cpp file
    compile

    ## test compression unit
    test_compression_unit

    ## test decompression unit
    test_decompression_unit

    # remove temporary files
    remove_temp_files

}

main "$@"