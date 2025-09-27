
#include "Huffman.h"

HuffmanTree::HuffmanTree() {
    root = nullptr;
    huffmanStorage;
    frequencyTable;
}

void HuffmanTree::createLeafNodes(std::unordered_map<char, int> &table) {

    // create an iterator to iterate through our map 'frequencyTable'
    std::unordered_map<char, int>::iterator it;
    for (it = table.begin(); it != table.end(); it++) {

        // creating leaves with letters and their corresponding weights
        Node* leaf = new Node(it->first, it->second);
        // pushing the leaf onto the queue
        queue.push(leaf);
    }
}


void HuffmanTree::buildFrequencyTable(std::string &input_string) {

    // create a variable
    char current_char;

    // for loop to loop through the entire string
    for (int i = 0; i < input_string.length(); i++) {
        current_char = input_string[i];
        // if the current char is found in the map (!= frequencyTable.end()), then add one to the frequency
        if (frequencyTable.find(current_char) != frequencyTable.end()) {
            frequencyTable[current_char] = frequencyTable[current_char] + 1;
        }
        else // if it has not popped up yet, make its frequency 1
        {
            frequencyTable[current_char] = 1;
        }

    }
    // this will added to signify that we reached the end of the file
    frequencyTable['\x03'] = 1;
}

void HuffmanTree::buildHuffmanTree(std::string &input_string) {

    // build the frequency table
    buildFrequencyTable(input_string);

    // create the leaf nodes
    createLeafNodes(frequencyTable);

    // while there is still stuff in the queue, keep looping
    while (queue.size() != 1) {
        // create a parent node
        Node *parent = new Node(' ', 0, nullptr, nullptr);
        // make the right child of the parent equal to the top of the queue
        parent->one = queue.top();
        // remove the node from the queue
        queue.pop();
        // make the next node the left child of the parent
        parent->zero = queue.top();
        // remove the node from the queue
        queue.pop();

        // update parent weight
        parent->weight = parent->zero->weight + parent->one->weight;

        // enqueue the parent back into the queue
        queue.push(parent);
    }

    // At this point, the tree is built, so set the root of the tree to the top of the queue
    this->root = queue.top();

}

std::unordered_map<char, std::string> HuffmanTree::huffmanPathCodes(Node *tree, std::string path, std::unordered_map<char, std::string> &output) {
    // if the node is a leaf, set the key equal to its corresponding value
    if (tree->zero == nullptr && tree->one == nullptr) {
        output[tree->letter] = path;
    }
    else {
        // visit the furthest left nodes, add 0 to the path
        huffmanPathCodes(tree->zero, path + "0", output);
        // visit the furthest right nodes, add 1 to the path
        huffmanPathCodes(tree->one, path + "1", output);
    }

    return output;
}


void HuffmanTree::huffmanCompress(std::string &input_file_path, std::string &output_file_path) {

    // create a filehandler to open the input file path
    std::ifstream filehandler;
    filehandler.open(input_file_path);

    // creating two strings to get the lines from the input file
    std::string line;
    std::string input;

    // while there are still lines to get from the file, pass them into the other string
    while (std::getline(filehandler, line)) {
        input += line + '\n';
    }

    // close the file
    filehandler.close();

    // build the tree using the string
    buildHuffmanTree(input);

    // getting huffman codes
    std::unordered_map<char, std::string> huffman_codes;
    huffmanPathCodes(this->root, "", huffman_codes);


    // storing the keys to rebuild tree later by iterating through the map
    std::string header;
    for (const std::pair<const char, std::string>& pair : huffman_codes) {
        // first == char, second == binary code path
        // Format: (CHAR) (PATH) (\36)...
        header += pair.first + pair.second + '\36';
    }
    // opening the storage to write into the file
    if (!huffmanStorage.open(output_file_path, "write")) {
        throw std::runtime_error("Could not open the file for writing.");
    }
    // setting the header
    huffmanStorage.setHeader(header);

    // creating a string to insert into the storage that contains all of the path codes
    std::string compressed_data;
    // for each char in input, add its corresponding binary code to the compressed data
    for (char c : input) {
        compressed_data += huffman_codes[c];
    }
    // insert all of the binary code into the storage, once complete
    huffmanStorage.insert(compressed_data);

    // insert the special '\x03' so we can know later that we reached the end of the file
    huffmanStorage.insert(huffman_codes['\x03']);
    // close the storage
    huffmanStorage.close();

    // make the tree empty
    makeEmpty();
}

void HuffmanTree::huffmanDecompress(std::string &input_file_path, std::string &output_file_path) {

    // open the storage
    if (!huffmanStorage.open(input_file_path, "read")) {
        std::cerr << "Could not open the file for reading.";
    }

    // rebuilding tree
    rebuildTree();

    // Time to decompress the file
    // initialize variables to decode
    // variable to temporarily store bits of extracted binary data
    std::string binary_string;
    std::string file_bits;

    // extract 8-bits at a time from the encoded file
    while (huffmanStorage.extract(file_bits)) {
        binary_string += file_bits;
    }

    // close the file opened for reading
    huffmanStorage.close();

    // open the file to output in
    std::ofstream decoded_file;
    decoded_file.open(output_file_path);

    // throw an error if we cannot open the file
    if (!decoded_file.is_open()) {
        throw std::runtime_error("Failed to open output file.");
    }

    // start at the root of the tree
    Node* current_node = root;

    // iterate through each bit stored in the binary string
    for (char bit : binary_string) {
        // if the bit is 0, then go to the zero position
        if (bit == '0') {
            current_node = current_node->zero;
            // otherwise, if the bit is 1, then go to the one position
        } else if (bit == '1') {
            current_node = current_node->one;
        }

        // if it's a leaf node, then it must contain a char
        if (current_node->zero == nullptr && current_node->one == nullptr) {
            // if we reach our EOF char, break
            if (current_node->letter == '\x03') {
                break;
            }
            // pipe in the char into the decoded file
            decoded_file << current_node->letter;
            // reset to root after each outputted char so we can go through the tree again
            current_node = root;
        }
    }

    // close the files
    decoded_file.close();

    // make the tree empty
    makeEmpty();
    }

void HuffmanTree::rebuildTree() {
    // create a stringstream
    std::stringstream ss;

    // storing the header from storage in the stringstream
    ss << huffmanStorage.getHeader();

    std::string data;

    // set a new node to create new nodes for the tree
    Node *rebuilder = new Node(' ', 0, nullptr, nullptr);

    // set the root to a node with default values
    root = new Node(' ', 0);

    // Actually rebuilding the tree
    while (getline(ss, data, '\36')) {
        char letter = data[0];
        rebuilder = root;

        // checking the paths (starts at 1 to avoid the char)
        for (int i = 1; i < data.length(); i++) {
            if (data[i] == '0') {
                // if there is not already a node, then make one
                if (rebuilder->zero == nullptr) {
                    rebuilder->zero = new Node(' ', 0);
                }
                rebuilder = rebuilder->zero;
            }
            else { // data[i] = 1
                if (rebuilder->one == nullptr) {
                    rebuilder->one = new Node(' ', 0);
                }
                rebuilder = rebuilder->one;
            }
        }
        // after following the path, we need to store the value of the char at the leaf
        rebuilder->letter = letter;
        rebuilder = root;
    }
}


void HuffmanTree::deleteNodes(Node *node) {
    if (node == nullptr) {
        return;
    }
    deleteNodes(node->zero);
    deleteNodes(node->one);
    delete node;
}

void HuffmanTree::makeEmpty() {
    deleteNodes(root);
    root = nullptr;
}
