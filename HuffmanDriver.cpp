#include <iostream>
#include "Huffman.h"

using namespace std;

int main() {
    HuffmanTree *compressor = new HuffmanTree();
    int choice = 0;
    string in_file, out_file;

    cout << "Would you like to:" << endl
         << "1) Compress a file" << endl
         << "2) Decompress a file" << endl;

    cin >> choice;

    if (choice == 1) {
        cout << "Enter the path of the file to be compressed: ";
        cin >> in_file;
        cout << "Enter the path where you'd like the decompress file to be saved: ";
        cin >> out_file;
        compressor->huffmanCompress(in_file, out_file);

    } else if (choice == 2) {
        cout << "Enter the path of the file to be decompressed: ";
        cin >> in_file;
        cout << "Enter the path where you'd like the uncompressed file to be saved: ";
        cin >> out_file;
        compressor->huffmanDecompress(in_file, out_file);


    } else {
        cout << "That is not a valid choice." << endl;
    }
    return 0;
}