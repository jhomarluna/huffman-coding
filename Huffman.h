#include <string>
#include <queue>
#include <fstream>
#include "Node.h"
#include "Storage/Storage.h"
#include <vector>
#include <sstream>

#ifndef HUFFMAN_H
#define HUFFMAN_H

class HuffmanTree {
private:
    Node *root;
    Storage huffmanStorage;
    std::priority_queue<Node*, std::vector<Node*>,compareWeights> queue;
    std::unordered_map<char, int> frequencyTable;

public:
    /**
     * Constructor for the HuffmanTree
     */
    HuffmanTree();

    /**
     * Creates leaf nodes needed to make a huffman tree
     * @param table the table to frequencies used to make the tree
     */
    void createLeafNodes(std::unordered_map<char, int> &table);

    /**
     * Builds a frequency table to the amount of time a character shows up in the text
     * @param input_string the text where the frequency table will be made from
     */
    void buildFrequencyTable(std::string &input_string);

    /**
     * Creates a huffman tree
     * @param input_string the text file that the huffman tree is based off of
     */
    void buildHuffmanTree(std::string &input_string);

    /**
     * Gets the binary huffman codes for each letter
     * @param tree the huffman tree to be traversed
     * @param path the path that leads to each letter
     * @param output the map that will contain the key/value pairs of char/codes.
     * @return
     */
    std::unordered_map<char, std::string> huffmanPathCodes(Node* tree, std::string path, std::unordered_map<char, std::string> &output);

    /**
     * Compresses a file and saves it to another file
     * @param input_file_path the file that will be compressed
     * @param output_file_path the file that the compressed data will be saved to
     */
    void huffmanCompress(std::string &input_file_path, std::string &output_file_path);

    /**
     * Decompresses a file and saves it to another file
     * @param input_file_path the file that will be compressed
     * @param output_file_path the file that the compressed data will be saved to
     */
    void huffmanDecompress(std::string &input_file_path, std::string &output_file_path);

    /**
     * Rebuilds the tree after for when we want to decompress a file
     */
    void rebuildTree();

    /**
     * Deletes nodes
     * @param node the tree that will be deleted
     */
    void deleteNodes(Node * node);

    /**
     * Makes the tree entirely empty
     */
    void makeEmpty();

};


#endif //HUFFMAN_H